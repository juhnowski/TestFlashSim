// /home/ilya/TestFlashSim/OriginalFlashSim/hardware_bridge.cpp
#include <iostream>
#include <memory>
#include "Vzns_fsm_validator.h"
#include "verilated.h"
#include "ssd.h"

// Явные внешние ссылки на объекты, чья память выделена в sim_main.cpp
extern std::unique_ptr<Vzns_fsm_validator> top;
extern ssd::Controller* global_controller_ptr;

// Выделяем память под массив Block RAM накопителя
uint32_t mock_bram_storage[1024] = {0};
uint32_t bram_rdata_latch = 0;

extern "C" {
    #include "zns_driver.h"

    // Физическое выделение памяти под указатель интерфейса драйвера
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
