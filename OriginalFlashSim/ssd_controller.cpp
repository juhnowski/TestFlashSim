// Добавьте этот инклуд и блок в начало или внутрь ssd_controller.cpp
#include "ssd.h"

extern "C" {
    #include "zns_driver.h" // Подключаем наш Си-драйвер NVMe ZNS
}

namespace ssd {

enum status Controller::direct_zns_gate(unsigned long lba, int is_write)
{
    nvme_sqe_t sqe;
    nvme_cqe_t cqe;

    // Обнуляем кадры перед отправкой в железо
    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));

    // 1. Упаковываем высокоуровневые параметры теста во фрейм NVMe команды
    if (is_write == 1) {
        sqe.opcode = NVME_CMD_WRITE; // 0x01
    } else if (is_write == 0) {
        sqe.opcode = NVME_CMD_READ;  // 0x02
    } else if (is_write == 2) {
        sqe.opcode = NVME_CMD_ZONE_MGMT;
        sqe.zsa = NVME_ZONE_ACTION_RESET; // Сброс зоны (io_cmd = 2)
    }

    sqe.slba = lba;
    sqe.nlb  = 0;   // 1 логический блок
    sqe.cid  = 42;  // Идентификатор команды

    // 2. Вызываем Си-драйвер прошивки.
    // Он зашевелит порты Verilator и продвинет время симуляции через verilator_tick_hardware()
    zns_process_nvme_command(&sqe, &cqe);

    // 3. Анализируем статус возврата NVMe
    if (cqe.status == 0x0000) {
        return SUCCESS; // Транзисторы Verilog одобрили операцию!
    } else {
        // Извлекаем код ошибки NVMe для вывода отладки
        uint16_t nvme_sc = (cqe.status >> 1) & 0xFF;
        fprintf(stderr, "   [VERILATOR CO-SIM REFUSAL]: Аппаратный отказ NVMe SC: 0x%02X\n", nvme_sc);
        return FAILURE;
    }
}

} // namespace ssd
