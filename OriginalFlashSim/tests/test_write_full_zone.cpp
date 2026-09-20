// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_write_full_zone.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_write_full_zone_test(Controller &controller) {
    printf("   [ТЕСТ 9]: Проверка запрета записи в полностью заполненную Зону 40...\n");

    // ИЗОЛЯЦИЯ: Сбрасываем Зону 40 перед тестом (LBA = 40 * 64 = 2560)
    uint base_lba = 2560;
    controller.direct_zns_gate(base_lba, 2);

    // 1. Полностью заполняем Зону 40 (все 64 страницы подряд)
    for (uint i = 0; i < 64; i++) {
        if (controller.direct_zns_gate(base_lba + i, 1) == FAILURE) {
            return false;
        }
    }

    // 2. Исправление: Пытаемся записать данные обратно в начало Зоны 40, которая уже FULL
    printf("   [ТЕСТ 9]: Попытка повторной записи в заполненную Зону 40 (LBA %u)...\n", base_lba);
    if (controller.direct_zns_gate(base_lba, 1) == FAILURE) {
        printf("   [ТЕСТ 9]: Контроллер аппаратно заблокировал запись в заполненную зону (Успех).\n");

        // ОЧИСТКА: Сбрасываем зону за собой
        controller.direct_zns_gate(base_lba, 2);
        return true;
    }

    controller.direct_zns_gate(base_lba, 2);
    return false;
}
