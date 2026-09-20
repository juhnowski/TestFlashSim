// /home/ilya/TestFlashSim/OriginalFlashSim/tests/tests.h
#ifndef ZNS_MODULAR_TESTS_H
#define ZNS_MODULAR_TESTS_H

#include "../ssd.h"

// Базовые ZNS-тесты (1-5)
bool run_sequential_write_test(ssd::Controller &controller);
bool run_random_write_protection_test(ssd::Controller &controller);
bool run_zone_reset_test(ssd::Controller &controller);
bool run_zone_resources_test(ssd::Controller &controller);
bool run_read_empty_zone_test(ssd::Controller &controller);

// Аппаратная безопасность и диагностика (6-11)
bool run_crypto_security_test(ssd::Controller &controller);
bool run_wear_limit_test(ssd::Controller &controller);
bool run_unaligned_read_test(ssd::Controller &controller);
bool run_write_full_zone_test(ssd::Controller &controller);
bool run_smart_eeprom_test(ssd::Controller &controller);
bool run_thermal_shutdown_test(ssd::Controller &controller); // ТЕСТ 11 НАДО ВОССТАНОВИТЬ ТУТ

// Продвинутые аппаратные тесты (12-14)
bool run_read_split_zone_test(ssd::Controller &controller);
bool run_multi_zone_reset_test(ssd::Controller &controller);
bool run_thermal_stress_test(ssd::Controller &controller);

#endif // ZNS_MODULAR_TESTS_H
