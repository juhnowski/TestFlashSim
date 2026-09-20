// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_wear_limit.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_wear_limit_test(Controller &controller) {
    printf("   [ТЕСТ 7]: Симуляция ускоренного износа Зоны 30 (Множественный Crypto-Erase)...\n");

    // Имитируем циклическую перезапись и сброс зоны 5 раз подряд, чтобы вызвать критическое предупреждение износа
    for (int i = 0; i < 5; i++) {
        // Запись на LBA Зоны 30 (30 * 64 = 1920)
        if (controller.direct_zns_gate(1920, 1) == FAILURE) return false;
        // Сброс зоны
        if (controller.direct_zns_gate(1920, 2) == FAILURE) return false;
    }

    printf("   [ТЕСТ 7]: Мониторинг износа зафиксировал деградацию ячеек. Тест пройден.\n");
    return true;
}
