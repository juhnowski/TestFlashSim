// /home/ilya/TestFlashSim/OriginalFlashSim/run_test.cpp
#include <stdio.h>
#include "ssd.h"
#include "tests/tests.h"

using namespace ssd;

int main() {
    printf("=== ОРКЕСТРАТОР ТЕСТОВ СИМУЛЯТОРА ZNS ===\n");
    load_config();
    print_config(NULL);

    Ssd my_ssd;
    Controller my_controller(my_ssd);
    printf("[СИСТЕМА]: Инициализация успешна. Запуск тестовой сюиты...\n\n");

    int passed = 0;
    int total = 14; // ЖЕСТКО ВЫСТАВЛЯЕМ 9 ТЕСТОВ

    // Базовые ZNS тесты
    if (run_sequential_write_test(my_controller)) { printf("👉 ТЕСТ 1: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 1: [ПРОВАЛ]\n\n"); }
    if (run_random_write_protection_test(my_controller)) { printf("👉 ТЕСТ 2: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 2: [ПРОВАЛ]\n\n"); }
    if (run_zone_reset_test(my_controller)) { printf("👉 ТЕСТ 3: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 3: [ПРОВАЛ]\n\n"); }
    if (run_zone_resources_test(my_controller)) { printf("👉 ТЕСТ 4: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 4: [ПРОВАЛ]\n\n"); }
    if (run_read_empty_zone_test(my_controller)) { printf("👉 ТЕСТ 5: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 5: [ПРОВАЛ]\n\n"); }

    // Аппаратная безопасность и крипто-движок (Ring -2)
    if (run_crypto_security_test(my_controller)) { printf("👉 ТЕСТ 6: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 6: [ПРОВАЛ]\n\n"); }

    // Граничные аппаратные лимиты
    if (run_wear_limit_test(my_controller)) { printf("👉 ТЕСТ 7: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 7: [ПРОВАЛ]\n\n"); }
    if (run_unaligned_read_test(my_controller)) { printf("👉 ТЕСТ 8: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 8: [ПРОВАЛ]\n\n"); }
    if (run_write_full_zone_test(my_controller)) { printf("👉 ТЕСТ 9: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 9: [ПРОВАЛ]\n\n"); }

    if (run_smart_eeprom_test(my_controller)) { printf("👉 ТЕСТ 10: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 10: [ПРОВАЛ]\n\n"); }

    if (run_thermal_shutdown_test(my_controller)) { printf("👉 ТЕСТ 11: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 11: [ПРОВАЛ]\n\n"); }

    if (run_read_split_zone_test(my_controller)) { printf("👉 ТЕСТ 12: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 12: [ПРОВАЛ]\n\n"); }
    if (run_multi_zone_reset_test(my_controller)) { printf("👉 ТЕСТ 13: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 13: [ПРОВАЛ]\n\n"); }
    if (run_thermal_stress_test(my_controller)) { printf("👉 ТЕСТ 14: [УСПЕШНО]\n\n"); passed++; } else { printf("👉 ТЕСТ 14: [ПРОВАЛ]\n\n"); }

    my_controller.flush_smart_to_eeprom();
    printf("=== ИТОГИ СИМУЛЯЦИИ ===\n");
    printf("Успешно пройдено: %d из %d тестов.\n", passed, total);

    return (passed == total) ? 0 : 1;
}
