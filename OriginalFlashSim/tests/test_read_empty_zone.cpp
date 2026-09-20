// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_read_empty_zone.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_read_empty_zone_test(Controller &controller) {
    printf("   [ТЕСТ 5]: Проверка запрета чтения пустых ячеек (Beyond WPTR)...\n");

    // ИЗОЛЯЦИЯ: Гарантируем чистоту Зоны 10 (LBA = 640) перед тестом
    controller.direct_zns_gate(640, 2);

    // Запишем одну страницу на LBA 640
    if (controller.direct_zns_gate(640, 1) == FAILURE) return false;

    // Попытка прочитать записанную страницу 0 — должно быть успешно
    if (controller.direct_zns_gate(640, 0) == FAILURE) return false;

    // Попытка прочитать СЛЕДУЮЩУЮ пустую страницу 1 (LBA 641)
    printf("   [ТЕСТ 5]: Попытка чтения нераспределенной страницы 1 (ожидается отказ)...\n");
    if (controller.direct_zns_gate(641, 0) == FAILURE) {
        printf("   [ТЕСТ 5]: Контроллер успешно заблокировал некорректное чтение.\n");

        // ОЧИСТКА
        controller.direct_zns_gate(640, 2);
        return true;
    }

    controller.direct_zns_gate(640, 2);
    return false;
}
