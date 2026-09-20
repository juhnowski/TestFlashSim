// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_zone_resources.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_zone_resources_test(Controller &controller) {
    printf("   [ТЕСТ 4]: Проверка ограничения на количество одновременно открытых зон (Макс: 4)...\n");

    // ИЗОЛЯЦИЯ: Сбрасываем Зону 0, которую открыли прошлые тесты
    controller.direct_zns_gate(0, 2);

    // Открываем 4 зоны последовательно (зоны 1, 2, 3, 4)
    for (uint z = 1; z <= 4; z++) {
        uint lba = z * 64;
        if (controller.direct_zns_gate(lba, 1) == FAILURE) {
            printf("   [ТЕСТ 4 ОШИБКА]: Не удалось открыть допустимую зону %u\n", z);
            return false;
        }
    }

    // Пытаемся открыть 5-ю зону (Зону 5, LBA = 5 * 64 = 320)
    printf("   [ТЕСТ 4]: Попытка открыть 5-ю зону (ожидается аппаратный отказ)...\n");
    if (controller.direct_zns_gate(320, 1) == FAILURE) {
        printf("   [ТЕСТ 4]: Аппаратный лимит ресурсов зон сработал корректно.\n");

        // ОЧИСТКА: Сбрасываем зоны назад, чтобы не мешать Тесту 5
        for (uint z = 1; z <= 4; z++) {
            controller.direct_zns_gate(z * 64, 2);
        }
        return true;
    }

    // Если пропустил 5-ю зону — сбрасываем и её для порядка
    controller.direct_zns_gate(320, 2);
    return false;
}
