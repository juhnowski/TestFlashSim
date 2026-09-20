// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_random_write_protection.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_random_write_protection_test(Controller &controller) {
    printf("   [ТЕСТ 2]: Проверка блокировки внеочередной записи...\n");
    // Прыгаем мимо текущего wptr (wptr сейчас на 4, мы пишем в 10)
    if (controller.direct_zns_gate(10, 1) == FAILURE) {
        printf("   [ТЕСТ 2]: Защита ZNS отработала штатно (запись отклонена).\n");
        return true; // Тест пройден, так как запись ДОЛЖНА БЫЛА упасть
    }
    return false; // Тест провален, если симулятор пропустил случайную запись
}
