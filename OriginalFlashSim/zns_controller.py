#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# /home/ilya/TestFlashSim/OriginalFlashSim/zns_controller.py

from migen import *
from litex.gen import *
from litex.soc.interconnect.csr import *

class ZnsController(Module, AutoCSR):
    def __init__(self, platform):
        # 1. Объявление CSR регистров с явным указанием name
        self.lba_low  = CSRStorage(32, name="lba_low",  description="Младшие 32 бита адреса LBA хоста")
        self.lba_high = CSRStorage(32, name="lba_high", description="Старшие 32 бита адреса LBA хоста")
        self.cmd      = CSRStorage(8,  name="cmd",      description="Код операции (0-Read, 1-Write, 2-Reset)")
        self.trigger  = CSRStorage(1,  name="trigger",  description="Запись 1 инициирует запуск автомата")

        # Регистры статуса (Read-Only)
        self.status   = CSRStatus(8,   name="status",   description="Статус выполнения (0-READY, 1-FAILURE, 2-BUSY)")
        self.err_code = CSRStatus(8,   name="err_code", description="Код аппаратного отказа контроллера")

        self.temp     = CSRStatus(8,   name="temp",     description="Текущая температура датчика SysMon") # ДОБАВЛЕНО

        # 2. Внутренние сигналы интерконнекта
        zone_id    = Signal(32)
        tgt_page   = Signal(32)
        bound_err  = Signal()

        cfg_shift = Signal(6,  reset=6)    # log2(64) = 6
        cfg_mask  = Signal(32, reset=0x3F) # 64 - 1 = 63 (0x3F)
        cfg_total = Signal(32, reset=4096)

        thermal_trip = Signal()
        thermal_err  = Signal()
        raw_temp_sig = Signal(8, reset=35) # Стартуем с комнатной температуры

        # Инстанцируем термоконтроллер
        self.specials += Instance("zns_thermal_manager",
            i_clk                        = ClockSignal(),
            i_rst                        = ResetSignal(),
            i_raw_temperature            = raw_temp_sig,
            o_out_thermal_shutdown_tripped = thermal_trip,
            o_out_thermal_err_code         = thermal_err
        )

        # Передаем значение в CSR регистр для чтения хостом
        self.comb += self.temp.status.eq(raw_temp_sig)

        # 3. Интеграция таблицы метаданных зон через чистый Verilog Instance
        bram_addr  = Signal(32)
        bram_rdata = Signal(32)
        bram_wdata = Signal(32)
        bram_we    = Signal()

        self.specials += Instance("zns_metadata_bram",
            i_clk   = ClockSignal(),
            i_addr  = bram_addr[0:12],
            i_wdata = bram_wdata,
            i_we    = bram_we,
            o_rdata = bram_rdata
        )

        # 4. Инстанцирование Verilog-модуля (Вычислитель координат)
        self.specials += Instance("zns_address_resolver",
            i_clk                     = ClockSignal(),
            i_rst                     = ResetSignal(),
            i_io_lba                  = Cat(self.lba_low.storage, self.lba_high.storage),
            i_cfg_zone_shift          = cfg_shift,
            i_cfg_zone_size_mask      = cfg_mask,
            i_cfg_total_zones         = cfg_total,
            o_out_zone_id             = zone_id,
            o_out_target_page         = tgt_page,
            o_out_error_out_of_bounds = bound_err
        )

        # 5. Инстанцирование Verilog-модуля (Конечный автомат валидации)
        self.specials += Instance("zns_fsm_validator",
            i_clk                   = ClockSignal(),
            i_rst                   = ResetSignal(),
            i_io_trigger            = self.trigger.storage,
            i_io_cmd                = self.cmd.storage,
            i_validated_zone_id     = zone_id,
            i_validated_target_page = tgt_page,
            i_addr_bound_error      = bound_err,
            i_thermal_shutdown_tripped = thermal_trip,

            o_bram_addr             = bram_addr,
            i_bram_rdata            = bram_rdata,
            o_bram_wdata            = bram_wdata,
            o_bram_we               = bram_we,

            o_out_status            = self.status.status,
            o_out_err_code          = self.err_code.status
        )

        platform.add_source("zns_metadata_bram.v")
        platform.add_source("zns_thermal_manager.v")
        platform.add_source("zns_address_resolver.v")
        platform.add_source("zns_fsm_validator.v")
        platform.add_source("zns_rules_checker.v")
        platform.add_source("zns_resources_tracker.v")

# =========================================================================
# ЧИСТАЯ ГЕНЕРАЦИЯ ВЕРТИКАЛЬНОЙ СТРУКТУРЫ ЧЕРЕЗ MIGEN КОНВЕРТЕР
# =========================================================================
if __name__ == "__main__":
    from migen.fhdl import verilog

    class DummyPlatform:
        def add_source(self, path):
            pass

    platform = DummyPlatform()
    zns_ip_core = ZnsController(platform)

# /home/ilya/TestFlashSim/OriginalFlashSim/zns_controller.py (в самом коде блока __main__)

    # Генерируем верхний уровень логики регистров хоста
    # Избавляемся от .code, приводя результат convert() напрямую к строке str()
    with open("zns_controller_top.v", "w") as f:
        f.write(str(verilog.convert(zns_ip_core,
            ios={zns_ip_core.lba_low.storage,
                 zns_ip_core.lba_high.storage,
                 zns_ip_core.cmd.storage,
                 zns_ip_core.trigger.storage,
                 zns_ip_core.status.status,
                 zns_ip_core.err_code.status})))

    print("\n[MIGEN/LITEX SUCCESS]: Аппаратная обёртка zns_controller_top.v успешно сгенерирована!")
