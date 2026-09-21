// /home/ilya/TestFlashSim/OriginalFlashSim/sim_main.cpp
#include <iostream>
#include <memory>
#include <unistd.h>
#include <cstring>
#include "Vzns_fsm_validator.h"
#include "verilated.h"
#include "ssd.h"
#include "tests/tests.h"

std::unique_ptr<Vzns_fsm_validator> top = nullptr;
ssd::Controller* global_controller_ptr = nullptr;

extern uint32_t mock_bram_storage[];
extern uint32_t bram_rdata_latch;

extern "C" {
    #include "zns_driver.h"
    extern void* verilator_top_model;
    bool bridge_run_crypto_key_test(void* controller_ptr);
}

// ДОБАВЛЕНО: Сценарий ТЕСТА 16 для верификации аппаратного Zone Append
bool run_zone_append_pipeline_test(ssd::Controller &controller) {
    std::cout << "   [ТЕСТ 16]: Тестирование аппаратного конвейера Zone Append на базовый LBA..." << std::endl;

    // Базовый LBA начала Зоны 90 (90 * 64 = 5760). Хост всегда шлет запросы строго на этот адрес!
    uint64_t base_lba = 90 * 64;

    nvme_sqe_t sqe;
    nvme_cqe_t cqe;

    // 1. Сбрасываем Зону 90 перед тестом
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.opcode = NVME_CMD_ZONE_MGMT;
    sqe.zsa = NVME_ZONE_ACTION_RESET;
    sqe.slba = base_lba;
    zns_process_nvme_command(&sqe, &cqe);
    if (cqe.status != 0x0000) return false;

    // 2. Отправляем ПЕРВЫЙ Append на базовый LBA. Ожидаем выделение страницы 0 (LBA 5760)
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.opcode = 0x7D; // Zone Append
    sqe.slba = base_lba;
    zns_process_nvme_command(&sqe, &cqe);
    if (cqe.status != 0x0000 || cqe.cdw0 != base_lba + 0) {
        std::cout << "   [ТЕСТ 16 ОШИБКА]: Первое смещение неверно: " << cqe.cdw0 << std::endl;
        return false;
    }
    std::cout << "   [ТЕСТ 15 ПОДТВЕРЖДЕНИЕ]: Размещение 1-го блока -> LBA " << cqe.cdw0 << " [OK]" << std::endl;

    // 3. Отправляем ВТОРОЙ Append снова на БАЗОВЫЙ LBA. Автомат обязан сам продвинуть wptr и вернуть LBA 5761!
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.opcode = 0x7D;
    sqe.slba = base_lba;
    zns_process_nvme_command(&sqe, &cqe);
    if (cqe.status != 0x0000 || cqe.cdw0 != base_lba + 1) {
        std::cout << "   [ТЕСТ 16 ОШИБКА]: Автоматическое продвижение wptr сломалось: " << cqe.cdw0 << std::endl;
        return false;
    }
    std::cout << "   [ТЕСТ 15 ПОДТВЕРЖДЕНИЕ]: Размещение 2-го блока -> LBA " << cqe.cdw0 << " [OK]" << std::endl;

    // 4. Отправляем ТРЕТИЙ Append снова на БАЗОВЫЙ LBA. Ожидаем LBA 5762
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.opcode = 0x7D;
    sqe.slba = base_lba;
    zns_process_nvme_command(&sqe, &cqe);
    if (cqe.status != 0x0000 || cqe.cdw0 != base_lba + 2) {
        return false;
    }
    std::cout << "   [ТЕСТ 15 ПОДТВЕРЖДЕНИЕ]: Размещение 3-го блока -> LBA " << cqe.cdw0 << " [OK]" << std::endl;

    std::cout << "   [ТЕСТ 16]: Аппаратный Zone Append контроллер распределяет адреса абсолютно верно!" << std::endl;
    return true;
}

void reset_hardware_state(void) {
    for (int i = 0; i < 1024; i++) mock_bram_storage[i] = 0x00000000;
    bram_rdata_latch = 0x00000000;

    if (top) {
        top->rst = 1;
        for (int i = 0; i < 10; i++) {
            top->clk = 1; top->eval();
            top->clk = 0; top->eval();
        }
        top->rst = 0;
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    }
    zns_controller_init();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = std::make_unique<Vzns_fsm_validator>();
    verilator_top_model = top.get();

    std::cout << "=======================================================" << std::endl;
    std::cout << "🚀 ЗАПУСК СКВОЗНОЙ CO-SIMULATION СЮИТЫ ТЕСТОВ (1-16) 🚀" << std::endl;
    std::cout << "=======================================================" << std::endl;

    reset_hardware_state();
    ssd::load_config();
    ssd::Ssd my_ssd;
    ssd::Controller my_controller(my_ssd);
    global_controller_ptr = &my_controller;

    std::cout << "\n[СИСТЕМА]: Запуск тестирования аппаратного Verilog-ядра..." << std::endl;

    reset_hardware_state();
    if (run_sequential_write_test(my_controller)) { std::cout << "👉 ТЕСТ 1: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 1: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_random_write_protection_test(my_controller)) { std::cout << "👉 ТЕСТ 2: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 2: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_zone_reset_test(my_controller)) { std::cout << "👉 ТЕСТ 3: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 3: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_zone_resources_test(my_controller)) { std::cout << "👉 ТЕСТ 4: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 4: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_read_empty_zone_test(my_controller)) { std::cout << "👉 ТЕСТ 5: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 5: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_crypto_security_test(my_controller)) { std::cout << "👉 ТЕСТ 6: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 6: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_wear_limit_test(my_controller)) { std::cout << "👉 ТЕСТ 7: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 7: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_unaligned_read_test(my_controller)) { std::cout << "👉 ТЕСТ 8: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 8: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_write_full_zone_test(my_controller)) { std::cout << "👉 ТЕСТ 9: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 9: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_smart_eeprom_test(my_controller)) { std::cout << "👉 ТЕСТ 10: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 10: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_thermal_shutdown_test(my_controller)) { std::cout << "👉 ТЕСТ 11: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 11: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_read_split_zone_test(my_controller)) { std::cout << "👉 ТЕСТ 12: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 12: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_multi_zone_reset_test(my_controller)) { std::cout << "👉 ТЕСТ 13: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 13: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (run_thermal_stress_test(my_controller)) { std::cout << "👉 ТЕСТ 14: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 14: [ПРОВАЛ]" << std::endl; }

    reset_hardware_state();
    if (bridge_run_crypto_key_test(&my_controller)) { std::cout << "👉 ТЕСТ 15: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 15: [ПРОВАЛ]" << std::endl; }

    // ЗАПУСК НАШЕГО НОВОГО ТЕСТА 16 ZONE APPEND
    reset_hardware_state();
    if (run_zone_append_pipeline_test(my_controller)) { std::cout << "👉 ТЕСТ 16: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТЕСТ 16: [ПРОВАЛ]" << std::endl; }

    std::cout << "\n=======================================================" << std::endl;
    std::cout << "🏆 ИТОГИ CO-SIMULATION ВЕРИФИКАЦИИ ЖЕЛЕЗА ZNS 🏆" << std::endl;
    std::cout << "Выполнение сюиты завершено." << std::endl;
    std::cout << "=======================================================" << std::endl;

    std::cout.flush();
    _exit(0);
}
