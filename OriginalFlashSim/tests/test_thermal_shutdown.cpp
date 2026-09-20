// /home/ilya/TestFlashSim/OriginalFlashSim/tests/test_thermal_shutdown.cpp
#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

bool run_thermal_shutdown_test(Controller &controller) {
    printf("   [ТЕСТ 11]: Верификация системы аварийного отключения по перегреву...\n");
    
    controller.set_temperature(40.0);
    if (controller.direct_zns_gate(60 * 64, 1) == FAILURE) {
        return false; 
    }
    
    printf("   [ТЕСТ 11]: Симуляция критического термического разгона до 90.0°C...\n");
    controller.set_temperature(90.0);
    
    if (controller.direct_zns_gate(60 * 64, 1) == FAILURE) {
        printf("   [ТЕСТ 11]: Термозащита успешно изолировала физический уровень диска. Данные спасены.\n");
        controller.set_temperature(35.0);
        controller.direct_zns_gate(60 * 64, 2); 
        return true; 
    }
    
    controller.set_temperature(35.0);
    return false; 
}
