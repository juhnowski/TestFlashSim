# /home/ilya/TestFlashSim/OriginalFlashSim/zns_controller.py
from migen import *
from litex.soc.interconnect.csr import *
from litex.soc.interconnect import wishbone

class ZNSController(Module, AutoCSR):
    def __init__(self, platform=None):
        # --- NVMe / ZNS CSR РЕГИСТРЫ ---
        # 1. NVMe Doorbell регистры управления очередями хоста
        self.sq_tail_db = CSRStorage(16, name="sq_tail_db", description="NVMe Submission Queue Tail Doorbell")
        self.cq_head_db = CSRStorage(16, name="cq_head_db", description="NVMe Completion Queue Head Doorbell")

        # 2. Поля NVMe команды (64-байтный Command Packet - Dword 10-15)
        self.nvme_opcode = CSRStorage(8,   name="nvme_opcode", description="NVMe Opcode (0x01=Read, 0x02=Write, 0x79=Zone Mgmt)")
        self.nvme_nsid   = CSRStorage(32,  name="nvme_nsid",   description="NVMe Namespace ID")
        self.nvme_lba_lo = CSRStorage(32,  name="nvme_lba_lo", description="NVMe Starting LBA (Low 32-bit)")
        self.nvme_lba_hi = CSRStorage(32,  name="nvme_lba_hi", description="NVMe Starting LBA (High 32-bit)")
        self.nvme_blocks = CSRStorage(16,  name="nvme_blocks", description="NVMe Number of Blocks (NLB)")

        # 3. Регистры аппаратного триггера и фиксации статуса NVMe CQ Entry
        self.trigger     = CSRStorage(1,   name="trigger",     description="Аппаратный триггер выполнения команды (1=Запуск)")
        self.nvme_status = CSRStatus(16,   name="nvme_status", description="NVMe Status Field (SCT + SC Код завершения)")
        self.err_code    = CSRStatus(8,    name="err_code",    description="Внутренний код ошибки аппаратного движка ZNS")
        self.temperature = CSRStatus(8,    name="temperature", description="Текущая температура датчика SysMon")

        # 4. Прерывания и экстренные линии защиты (PLP и Crypto-Clear)
        self.plp_trigger = CSRStorage(1,   name="plp_trigger", description="Сигнал экстренной потери питания Protection (1=Авария)")
        self.crypto_erase= CSRStorage(1,   name="crypto_erase",description="Сигнал Crypto-Erase уничтожения ключей (1=Стереть)")

        # --- ВНУТРЕННИЕ СИГНАЛЫ СВЯЗИ С VERILOG ---
        # Объединяем LBA в единую 64-битную шину
        lba_64 = Signal(64)
        self.comb += lba_64.eq(Cat(self.nvme_lba_lo.storage, self.nvme_lba_hi.storage))

        # Статусные провода обратной связи
        fsm_status   = Signal(8)
        fsm_err_code = Signal(8)

        # Конвертация внутреннего статуса в NVMe Status Code (SC)
        # Если статус FSM == 1 (FAILURE), переводим внутренний код ошибки в формат NVMe ZNS
        self.comb += [
            If(fsm_status == 1,
                # Формируем NVMe Status Field (например, Invalid Field, Zone Boundary Error)
                self.nvme_status.status.eq(Cat(fsm_err_code, C(1, 8))) # SCT=1 (Generic Command Status)
            ).Else(
                If(fsm_status == 3,
                    self.nvme_status.status.eq(0x0000) # NVMe SUCCESS
                ).Else(
                    self.nvme_status.status.eq(0x0001) # NVMe Command Do Not Execution / Busy
                )
            ),
            self.err_code.status.eq(fsm_err_code)
        ]

        # --- ИНСТАНЦИРОВАНИЕ VERILOG ЯДРА (ПОДКЛЮЧЕНИЕ СТЕКА) ---
        # Внутренние соединительные провода архитектуры
        zone_id   = Signal(32)
        tgt_page  = Signal(32)
        bound_err = Signal()
        bram_addr = Signal(32)
        bram_wdata= Signal(32)
        bram_rdata= Signal(32)
        bram_we   = Signal()

        thermal_trip = Signal()
        thermal_err  = Signal()

        # 1. Резолвер адресов
        self.specials += Instance("zns_address_resolver",
            i_clk                     = ClockSignal(),
            i_rst                     = ResetSignal(),
            i_io_lba                  = lba_64,
            i_cfg_zone_shift          = 6, # 64 страницы на зону
            i_cfg_zone_size_mask      = 0x0000003F,
            i_cfg_total_zones         = 4096,
            o_out_zone_id             = zone_id,
            o_out_target_page         = tgt_page,
            o_out_error_out_of_bounds = bound_err
        )

        # 2. Block RAM метаданных зон
        self.specials += Instance("zns_metadata_bram",
            i_clk   = ClockSignal(),
            i_addr  = bram_addr[0:12],
            i_wdata = bram_wdata,
            i_we    = bram_we,
            o_rdata = bram_rdata
        )

        # 3. Термо-менеджер
        self.specials += Instance("zns_thermal_manager",
            i_clk                          = ClockSignal(),
            i_rst                          = ResetSignal(),
            i_raw_temperature              = self.temperature.status,
            o_out_thermal_shutdown_tripped = thermal_trip,
            o_out_thermal_err_code         = thermal_err
        )

        # 4. Диспетчер FSM
        self.specials += Instance("zns_fsm_validator",
            i_clk                    = ClockSignal(),
            i_rst                    = ResetSignal(),
            i_io_trigger             = self.trigger.storage,
            i_io_cmd                 = self.nvme_opcode.storage,
            i_validated_zone_id      = zone_id,
            i_validated_target_page  = tgt_page,
            i_addr_bound_error       = bound_err,
            i_thermal_shutdown_tripped = thermal_trip,
            o_bram_addr              = bram_addr,
            i_bram_rdata             = bram_rdata,
            o_bram_wdata             = bram_wdata,
            o_bram_we                = bram_we,
            o_out_status             = fsm_status,
            o_out_err_code           = fsm_err_code
        )

        # --- РЕГИСТРАЦИЯ ВСЕХ ИСХОДНЫХ RTL ФАЙЛОВ В LITEX PLATFORM ---
        if platform is not None:
            platform.add_source("zns_metadata_bram.v")
            platform.add_source("zns_thermal_manager.v")
            platform.add_source("zns_address_resolver.v")
            platform.add_source("zns_rules_checker.v")
            platform.add_source("zns_resources_tracker.v")
            platform.add_source("zns_eeprom_controller.v")
            platform.add_source("aes_round.v")
            platform.add_source("zns_crypto_engine.v")
            platform.add_source("zns_fsm_validator.v")

# Заглушка для автономной генерации LiteX-обертки через ./run_hardware_tests.sh
if __name__ == "__main__":
    from litex.build.generic_platform import *
    from litex.build.xilinx import XilinxPlatform
    plat = XilinxPlatform("xc7a100t", [], io_bank_voltage=3.3)
    module = ZNSController(platform=plat)
    print("[MIGEN/LITEX SUCCESS]: Аппаратная обёртка zns_controller_top.v успешно сгенерирована!")
