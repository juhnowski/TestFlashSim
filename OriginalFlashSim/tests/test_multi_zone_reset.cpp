// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_multi_zone_reset.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_multi_zone_reset_test(Controller &controller) {
    printf("   [ТЕСТ 13]: Проверка многозонного параллельного сброса метаданных...\n");

    // Открываем и записываем по одной странице в Зону 70, Зону 71 и Зону 72
    uint zone70_lba = 70 * 64;
    uint zone71_lba = 71 * 64;
    uint zone72_lba = 72 * 64;

    if (controller.direct_zns_gate(zone70_lba, 1) == FAILURE) return false;
    if (controller.direct_zns_gate(zone71_lba, 1) == FAILURE) return false;
    if (controller.direct_zns_gate(zone72_lba, 1) == FAILURE) return false;

    // Имитируем команду группового сброса пула зон хостом
    printf("   [ТЕСТ 13]: Отправка команд Zone Reset для пула зон 70, 71, 72...\n");
    if (controller.direct_zns_gate(zone70_lba, 2) == FAILURE) return false;
    if (controller.direct_zns_gate(zone71_lba, 2) == FAILURE) return false;
    if (controller.direct_zns_gate(zone72_lba, 2) == FAILURE) return false;

    // Верифицируем, что метаданные в BRAM обнулились для каждой зоны независимо
    // Пробуем записать снова в начало каждой зоны — операции должны пройти успешно (wptr сброшен в 0)
    if (controller.direct_zns_gate(zone70_lba, 1) == SUCCESS &&
        controller.direct_zns_gate(zone71_lba, 1) == SUCCESS &&
        controller.direct_zns_gate(zone72_lba, 1) == SUCCESS) {
        printf("   [ТЕСТ 13]: Пул зон успешно очищен, метаданные BRAM не повреждены. Успех.\n");

        // Очистка за собой
        controller.direct_zns_gate(zone70_lba, 2);
        controller.direct_zns_gate(zone71_lba, 2);
        controller.direct_zns_gate(zone72_lba, 2);
        return true;
    }

    return false;
}
