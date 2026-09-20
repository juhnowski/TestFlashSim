// /home/ilya/TestFlashSim/OriginalFlashSim/sim_main.cpp
#include <iostream>
#include <memory>
#include <unistd.h>
#include <cstring>
#include "Vzns_fsm_validator.h"
#include "verilated.h"
#include "zns_driver.h"

std::unique_ptr<Vzns_fsm_validator> top;

// Виртуальный массив памяти BRAM для 64 зон контроллера
uint32_t mock_bram_storage[64] = {0};

// Аппаратный регистр-защёлка выходных данных BRAM (удерживает данные при смене адреса!)
uint32_t bram_rdata_latch = 0;

extern "C" {
    void* verilator_top_model = nullptr;
    volatile uint32_t stub_sq_tail = 0;
    volatile uint32_t stub_cq_head = 0;

    // Окончательный тактовый генератор симулятора с потактовой фиксацией шин
    void verilator_tick_hardware(void) {
        if (top) {
            // ЖЕСТКАЯ ПРИВЯЗКА ДАТЧИКОВ К ЗЕМЛЕ
            top->thermal_shutdown_tripped = 0;
            top->addr_bound_error         = 0;

            // Непрерывно подсовываем автомату защёлкнутые данные из BRAM
            top->bram_rdata = bram_rdata_latch;

            // --- ПОЛОЖИТЕЛЬНЫЙ ФРОНТ (CLOCK HIGH) ---
            top->clk = 1;
            top->eval();

            // По тактовому фронту эмулируем чтение/запись синхронной Block RAM FPGA
            uint32_t current_addr = top->bram_addr;
            if (current_addr < 64) {
                // Синхронное чтение: данные фиксируются в защёлке строго на тактовом фронте!
                bram_rdata_latch = mock_bram_storage[current_addr];
            } else {
                bram_rdata_latch = 0x00000000;
            }

            // Синхронная запись в массив памяти
            if (top->bram_we && (current_addr < 64)) {
                mock_bram_storage[current_addr] = top->bram_wdata;
            }

            // Обновляем шину модели защёлкнутым значением послеeval
            top->bram_rdata = bram_rdata_latch;

            // --- ОТРИЦАТЕЛЬНЫЙ ФРОНТ (CLOCK LOW) ---
            top->clk = 0;
            top->eval();
        }
    }

    void sim_set_io_trigger(uint8_t val) { if (top) top->io_trigger = val; }
    void sim_set_io_cmd(uint8_t val) { if (top) top->io_cmd = val; }
    void sim_set_validated_target_page(uint32_t val) { if (top) top->validated_target_page = val; }
    void sim_set_validated_zone_id(uint32_t val) { if (top) top->validated_zone_id = val; }

    uint8_t sim_get_out_status(void) {
        verilator_tick_hardware();
        return top ? top->out_status : 0;
    }
    uint8_t sim_get_out_err_code(void) {
        return top ? top->out_err_code : 0;
    }
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    top = std::make_unique<Vzns_fsm_validator>();
    verilator_top_model = top.get();

    // Инициализируем BRAM чистыми метаданными для всех 64 пользовательских зон
    for (int i = 0; i < 64; i++) {
        mock_bram_storage[i] = 0x00000000;
    }
    bram_rdata_latch = 0x00000000;

    top->thermal_shutdown_tripped = 0;
    top->addr_bound_error         = 0;
    top->io_trigger               = 0;
    top->io_cmd                   = 0;
    top->validated_target_page    = 0;
    top->validated_zone_id        = 0;

    // Стабилизационный сброс схемы (5 тактов)
    top->rst = 1;
    for (int i = 0; i < 5; i++) {
        verilator_tick_hardware();
    }
    top->rst = 0;
    verilator_tick_hardware();

    zns_controller_init();

    std::cout << "[CO-SIM] Начало симуляции ZNS валидатора..." << std::endl;

    nvme_sqe_t test_sqe;
    nvme_cqe_t test_cqe;
    std::memset(&test_sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&test_cqe, 0, sizeof(nvme_cqe_t));

    // Формируем чистую NVMe команду Последовательной Записи на первую страницу Зоны 1 (LBA 64)
    test_sqe.opcode = NVME_CMD_WRITE;
    test_sqe.slba = 64;
    test_sqe.nlb = 0;
    test_sqe.zsa = 0;
    test_sqe.cid = 777;

    std::cout << "[CO-SIM] Отправка тестовой команды WRITE на SLBA " << std::dec << test_sqe.slba << "..." << std::endl;
    zns_process_nvme_command(&test_sqe, &test_cqe);

    std::cout << "[CO-SIM] Команда обработана. Статус возврата NVMe: 0x" << std::hex << test_cqe.status << std::endl;

    // Выводим дамп BRAM Зоны 1, чтобы увидеть аппаратный инкремент wptr (ожидается 0x1 в младшем байте кадра!)
    std::cout << "[CO-SIM] Метаданные Зоны 1 в BRAM после транзакции: 0x" << std::hex << mock_bram_storage[1] << std::endl;

    std::cout << "[CO-SIM] Завершение работы симулятора без утечек памяти." << std::endl;
    std::cout.flush();
    _exit(0);
}
