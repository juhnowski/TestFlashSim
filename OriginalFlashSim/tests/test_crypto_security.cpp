// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_crypto_security.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_crypto_security_test(Controller &controller) {
    printf("   [ТЕСТ 6]: Тестирование защиты от компрометации хоста (Ring -2) через Crypto-Erase...\n");

    // Используем Зону 20 для теста безопасности (LBA = 20 * 64 = 1280)
    if (controller.direct_zns_gate(1280, 1) == FAILURE) return false;

    printf("   [ТЕСТ 6]: Внимание! Обнаружена угроза Ring -2. Активация Crypto-Erase...\n");
    if (controller.direct_zns_gate(1280, 2) == FAILURE) return false;

    printf("   [ТЕСТ 6]: Попытка скомпрометированного хоста считать данные без ключа...\n");
    if (controller.direct_zns_gate(1280, 0) == FAILURE) {
        printf("   [ТЕСТ 6]: Данные защищены. Попытка чтения заблокирована крипто-движком.\n");
        return true;
    }

    return false;
}
