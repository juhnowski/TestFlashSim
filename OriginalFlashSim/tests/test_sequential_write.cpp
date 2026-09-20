// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_sequential_write.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_sequential_write_test(Controller &controller) {
    printf("   [ТЕСТ 1]: Запуск последовательной записи в Зону 0...\n");
    for (uint i = 0; i < 4; i++) {
        if (controller.direct_zns_gate(i, 1) == FAILURE) {
            return false;
        }
    }
    return true;
}
