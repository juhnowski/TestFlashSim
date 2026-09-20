// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_zone_reset.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_zone_reset_test(Controller &controller) {
    printf("   [ТЕСТ 3]: Проверка Zone Reset (команда TRIM)...\n");
    if (controller.direct_zns_gate(0, 2) == FAILURE) return false;

    // После сброса проверяем, что запись на LBA 0 снова разрешена
    if (controller.direct_zns_gate(0, 1) == SUCCESS) {
        printf("   [ТЕСТ 3]: Повторная запись после сброса успешна. Триумф\n");
        return true;
    }
    return false;
}
