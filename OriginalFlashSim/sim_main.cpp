// /home/ilya/TestFlashSim/OriginalFlashSim/sim_main.cpp
#include <iostream>
#include <memory>
#include <unistd.h>
#include <cstring>
#include "Vzns_fsm_validator.h"
#include "verilated.h"
#include "ssd.h"
#include "tests/tests.h"

// ИСПРАВЛЕНО: Объявляем глобальный указатель до его использования в методах и функции main
namespace ssd {
    ulong Controller::total_zns_zones = 1024;
    ZnsZone* Controller::zns_zones = nullptr;
}

ssd::Controller* global_controller_ptr = nullptr;

namespace ssd {
    Controller::Controller(Ssd& parent) : ssd(parent) {
        if (total_zns_zones == 0) total_zns_zones = 1024;
        zns_zones = new ZnsZone[total_zns_zones];
        std::memset(zns_zones, 0, sizeof(ZnsZone) * total_zns_zones);
    }

    Controller::~Controller() {
        if (zns_zones) {
            delete[] zns_zones;
            zns_zones = nullptr;
        }
    }

    enum status Controller::issue(Event &event_list) { return SUCCESS; }
    Block* Controller::get_block_pointer(const Address & address) { return nullptr; }
    enum status Controller::event_arrive(Event &event) { return SUCCESS; }
    void Controller::print_ftl_statistics() {}
    ulong Controller::get_erases_remaining(const Address &address) const { return 100000; }
    void Controller::get_least_worn(Address &address) const {}
    enum page_state Controller::get_state(const Address &address) const { return (enum page_state)0; }
    enum block_state Controller::get_block_state(const Address &address) const { return (enum block_state)0; }
}

std::unique_ptr<Vzns_fsm_validator> top;

// Выделяем Block RAM на 1024 зоны накопителя
uint32_t mock_bram_storage[1024] = {0};
uint32_t bram_rdata_latch = 0;

extern "C" {
    #include "zns_driver.h"

    void* verilator_top_model = nullptr;
    volatile uint32_t stub_sq_tail = 0;
    volatile uint32_t stub_cq_head = 0;

    void verilator_tick_hardware(void) {
        if (top) {
            uint32_t current_addr = top->bram_addr;
            if (current_addr < 1024) {
                top->bram_rdata = bram_rdata_latch;
            } else {
                top->bram_rdata = 0x00000000;
            }

            if (global_controller_ptr != nullptr) {
                top->thermal_shutdown_tripped = global_controller_ptr->check_thermal_shutdown() ? 1 : 0;
            } else {
                top->thermal_shutdown_tripped = 0;
            }
            top->addr_bound_error = 0;

            top->clk = 1;
            top->eval();

            if (current_addr < 1024) {
                bram_rdata_latch = mock_bram_storage[current_addr];
            }
            if (top->bram_we && (current_addr < 1024)) {
                mock_bram_storage[current_addr] = top->bram_wdata;
            }
            top->bram_rdata = bram_rdata_latch;

            top->clk = 0;
            top->eval();
        }
    }

    void sim_set_io_trigger(uint8_t val) { if (top) top->io_trigger = val; }
    void sim_set_io_cmd(uint8_t val) { if (top) top->io_cmd = val; }
    void sim_set_validated_target_page(uint32_t val) { if (top) top->validated_target_page = val; }
    void sim_set_validated_zone_id(uint32_t val) { if (top) top->validated_zone_id = val; }

    uint8_t sim_get_out_status(void) { verilator_tick_hardware(); return top ? top->out_status : 0; }
    uint8_t sim_get_out_err_code(void) { return top ? top->out_err_code : 0; }
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

void execute_co_sim_command(uint8_t opcode, uint64_t slba, uint32_t nlb, uint32_t zsa, uint16_t cid, uint16_t *out_status) {
    nvme_sqe_t sqe;
    nvme_cqe_t cqe;
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));

    sqe.opcode = opcode;
    sqe.slba   = slba;
    sqe.nlb    = nlb;
    sqe.zsa    = zsa;
    sqe.cid    = cid;

    zns_process_nvme_command(&sqe, &cqe);
    if (out_status) {
        *out_status = cqe.status;
    }
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = std::make_unique<Vzns_fsm_validator>();
    verilator_top_model = top.get();

    std::cout << "=======================================================" << std::endl;
    std::cout << "🚀 ЗАПУСК СКВОЗНОЙ CO-SIMULATION СЮИТЫ ТЕСТОВ (1-14) 🚀" << std::endl;
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
    if (run_thermal_stress_test(my_controller)) { std::cout << "👉 ТЕСТ 14: [УСПЕШНО]" << std::endl; } else { std::cout << "👉 ТEСТ 14: [ПРОВАЛ]" << std::endl; }

    std::cout << "\n=======================================================" << std::endl;
    std::cout << "🏆 ИТОГИ CO-SIMULATION ВЕРИФИКАЦИИ ЖЕЛЕЗА ZNS 🏆" << std::endl;
    std::cout << "Выполнение сюиты завершено." << std::endl;
    std::cout << "=======================================================" << std::endl;

    std::cout.flush();
    _exit(0);
}
