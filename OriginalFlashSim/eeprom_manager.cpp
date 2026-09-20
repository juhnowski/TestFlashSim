// /home/ilya/TestFlashSim/OriginalFlashSim/eeprom_manager.cpp
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

void Controller::flush_smart_to_eeprom() {
    printf("\n  [EEPROM HW CONTROLLER]: Перехват линии PLP (Power-Loss Protection).\n");
    printf("  [EEPROM HW CONTROLLER]: Формирование 512-байтной структуры NVMe SMART Log Page 02h...\n");

    ulong max_erase = 0;
    for (ulong i = 0; i < total_zns_zones; i++) {
        if (zns_zones[i].erase_count > max_erase) {
            max_erase = zns_zones[i].erase_count;
        }
    }

    // Исправление: Безопасный расчет процента (3 из 5 = 60%)
    ulong percentage_used = (max_erase * 100) / 5;

    FILE *eeprom = fopen("eeprom_diagnostic.bin", "w");
    if (eeprom != NULL) {
        fprintf(eeprom, "=== NVMe ZNS SSD SMART LOG PAGE (EEPROM DUMP) ===\n");
        fprintf(eeprom, "Critical Warning Flags: %s\n", (max_erase >= 5) ? "0x01 (WEAR_CRITICAL)" : "0x00 (OK)");
        fprintf(eeprom, "Percentage Used (Wear-level): %lu%%\n", percentage_used);
        fprintf(eeprom, "Total Addressable Zones: %lu\n", total_zns_zones);
        fprintf(eeprom, "Max Zone Erase Count Tracked: %lu\n", max_erase);
        fclose(eeprom);
        printf("  [EEPROM HW CONTROLLER]: Дамп успешно сохранен в eeprom_diagnostic.bin за 120 мкс (Имитация шины SPI).\n");
    } else {
        fprintf(stderr, "  [EEPROM HW ERROR]: Не удалось открыть аппаратный интерфейс внешней памяти Flash!\n");
    }
}
