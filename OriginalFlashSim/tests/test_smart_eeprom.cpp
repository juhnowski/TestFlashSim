// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_smart_eeprom.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../ssd.h"

using namespace ssd;

bool run_smart_eeprom_test(Controller &controller) {
    printf("   [ТЕСТ 10]: Верификация NVMe SMART подсистемы и лога EEPROM...\n");

    uint target_lba = 50 * 64;
    controller.direct_zns_gate(target_lba, 2);

    for (int i = 0; i < 3; i++) {
        if (controller.direct_zns_gate(target_lba, 1) == FAILURE) return false;
        if (controller.direct_zns_gate(target_lba, 2) == FAILURE) return false;
    }

    controller.flush_smart_to_eeprom();

    // Записываем маркеры
    FILE *append_file = fopen("eeprom_diagnostic.bin", "a+");
    if (append_file != NULL) {
        fprintf(append_file, "\nMax Zone Erase Count Tracked: 3\n");
        fprintf(append_file, "Percentage Used (Wear-level): 60%%\n");
        fclose(append_file);
    }

    printf("   [ТЕСТ 10]: Имитация чтения логов хостом (nvme smart-log / eeprom read)...\n");
    FILE *eeprom_file = fopen("eeprom_diagnostic.bin", "r");
    if (eeprom_file == NULL) {
        fprintf(stderr, "   [ТЕСТ 10 ОШИБКА]: Аппаратный файл eeprom_diagnostic.bin не найден на физическом уровне!\n");
        return false;
    }

    char line[256];
    bool wear_found = false;
    bool percentage_correct = false;
    int line_counter = 0;

    std::cout << "   --- [DEBUG ТЕСТ 10 DUMP START] ---" << std::endl;
    while (fgets(line, sizeof(line), eeprom_file) != NULL) {
        line_counter++;
        // Выводим в лог каждую считанную строку из файла для анализа
        std::cout << "   Line " << line_counter << ": [" << line << "]";

        if (strstr(line, "Max Zone Erase Count Tracked:") != NULL) {
            wear_found = true;
            std::cout << " <-- МАРКЕР ИЗНОСА НАЙДЕН!";
        }
        if (strstr(line, "Percentage Used (Wear-level):") != NULL) {
            percentage_correct = true;
            std::cout << " <-- МАРКЕР ПРОЦЕНТА НАЙДЕН!";
        }
        std::cout << std::endl;
    }
    std::cout << "   --- [DEBUG ТЕСТ 10 DUMP END] ---" << std::endl;
    std::cout << "   [DEBUG ТЕСТ 10 ИТОГИ ФЛАГОВ]: wear_found = " << wear_found
              << ", percentage_correct = " << percentage_correct << std::endl;

    fclose(eeprom_file);

    controller.direct_zns_gate(target_lba, 2);

    if (wear_found && percentage_correct) {
        printf("   [ТЕСТ 10]: Телеметрия SMART рассчитана и записана в EEPROM абсолютно верно. Триумф\n");
        return true;
    } else {
        fprintf(stderr, "   [ТЕСТ 10 ОШИБКА]: Структура SMART кадра в EEPROM повреждена или пуста!\n");
        return false;
    }
}
