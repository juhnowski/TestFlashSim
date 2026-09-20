# /home/ilya/TestFlashSim/OriginalFlashSim/base_soc.py
#!/usr/bin/env python3
import os
import json
from migen import *
from litex.soc.integration.soc_core import SoCCore
from litex.soc.integration.export import get_csr_json, get_csr_header

class ZNSSoC(SoCCore):
    def __init__(self):
        sys_clk_freq = int(100e6)

        # Используем встроенную пустую платформу LiteX для софтверных тестов
        from litex.build.generic_platform import GenericPlatform
        platform = GenericPlatform("abstract_virtual", [])
        platform.device = "sim_virtual"

        # Инициализируем ядро LiteX SoC без процессора и авто-периферии
        SoCCore.__init__(self,
            platform             = platform,
            clk_freq             = sys_clk_freq,
            cpu_type             = None,
            integrated_rom_size  = 0,
            integrated_sram_size = 0,
            csr_data_width       = 32,
            with_ctrl            = False,
            with_uart            = False,
            with_timer           = False
        )

        # Инстанцируем наш NVMe ZNS контроллер
        from zns_controller import ZNSController
        zns_ctrl = ZNSController(platform=platform)
        self.submodules.zns = zns_ctrl
        self.add_csr("zns")

def main():
    soc = ZNSSoC()

    # 1. Принудительно финализируем разметку Wishbone и CSR шин
    soc.finalize()

    # 2. Создаем директории сборки для софта
    os.makedirs("build/software/include/generated", exist_ok=True)

    # 3. Передаем словарь регистров сока (.csr_regions) в экспортер
    # Это решает проблему AttributeError: ZNSSoC object has no attribute items
    csr_json_content = get_csr_json(soc.csr_regions)
    with open("build/csr.json", "w") as f:
        f.write(csr_json_content)

    # 4. Генерируем Си-заголовки
    csr_header_content = get_csr_header(soc.csr_regions, soc.constants)
    with open("build/software/include/generated/csr.h", "w") as f:
        f.write(csr_header_content)

    print("\n🎉 [LITEX SOC BUILD SUCCESS]: Система на кристалле построена!")
    print("   Карта регистров в формате NVMe сохранена в: build/csr.json")
    print("   Заголовочные файлы регистров ZNS сгенерированы в: build/software/include/generated/csr.h")

if __name__ == "__main__":
    main()
