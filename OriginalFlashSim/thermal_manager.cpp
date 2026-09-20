// /home/ilya/TestFlashSim/OriginalFlashSim/thermal_manager.cpp
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

double Controller::current_temperature = 35.0;

void Controller::set_temperature(double temp) {
    current_temperature = temp;
}

bool Controller::check_thermal_shutdown() {
    if (current_temperature >= 85.0) { // Проверяем порог 85°C
        fprintf(stderr, "\n  [THERMAL IP-CORE CRITICAL]: ОБНАРУЖЕН ПЕРЕГРЕВ КРИСТАЛЛА: %.1f°C! (Порог: 85.0°C)\n", current_temperature);
        return true; // ЖЕСТКИЙ ТРИГГЕР ОТКАЗА
    }
    return false;
}
