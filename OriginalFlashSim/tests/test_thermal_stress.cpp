// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_thermal_stress.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_thermal_stress_test(Controller &controller) {
    printf("   [ТЕСТ 14]: Динамический стресс-тест термозащиты под высокой нагрузкой I/O...\n");

    // Используем Зону 80 (LBA = 80 * 64 = 5120)
    uint base_lba = 5120;
    controller.set_temperature(35.0); // Нормальная стартовая температура
    controller.direct_zns_gate(base_lba, 2); // Очистка

    printf("   [ТЕСТ 14]: Запуск интенсивной последовательной записи...\n");
    for (uint i = 0; i < 64; i++) {
        // Имитируем резкий перегрев кристалла прямо в середине конвейера записи (на 32-й странице)
        if (i == 32) {
            printf("   [ТЕСТ 14]: ВНИМАНИЕ! Термический пробой во время записи! Температура скачет до 89.5°C!\n");
            controller.set_temperature(89.5);
        }

        // Отправляем команду записи
        if (controller.direct_zns_gate(base_lba + i, 1) == FAILURE) {
            // Запись ДОЛЖНА упасть ровно на 32-й странице из-за сработавшей термозащиты
            if (i == 32) {
                printf("   [ТЕСТ 14]: Аппаратный автомат успешно оборвал транзакцию на 32-й странице. Термозащита на лету сработала штатно.\n");
                controller.set_temperature(35.0); // Возвращаем норму
                controller.direct_zns_gate(base_lba, 2); // Очищаем зону за собой
                return true;
            } else {
                printf("   [ТЕСТ 14 ОШИБКА]: Запись упала слишком рано, на странице %u\n", i);
                controller.set_temperature(35.0);
                return false;
            }
        }
    }

    controller.set_temperature(35.0);
    printf("   [ТЕСТ 14 ОШИБКА]: Контроллер пропустил всю запись и проигнорировал перегрев в 89.5°C!\n");
    return false;
}
