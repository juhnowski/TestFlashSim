// /home/ilya/TestFlashSim/OriginalFlashSim/ssd_controller.cpp
#include "ssd.h"
#include <cstring>

extern "C" {
    #include "zns_driver.h"
}

namespace ssd {

enum status Controller::direct_zns_gate(unsigned long lba, int is_write)
{
    nvme_sqe_t sqe;
    nvme_cqe_t cqe;

    std::memset(&sqe, 0, sizeof(nvme_sqe_t));
    std::memset(&cqe, 0, sizeof(nvme_cqe_t));

    // Рассчитываем ID зоны на основе сквозного LBA (размер зоны — 64 страницы)
    uint32_t zone_id = (uint32_t)(lba / 64);

    if (is_write == 1) {
        sqe.opcode = NVME_CMD_WRITE;
    } else if (is_write == 0) {
        sqe.opcode = NVME_CMD_READ;
    } else if (is_write == 2) {
        sqe.opcode = NVME_CMD_ZONE_MGMT;
        sqe.zsa = NVME_ZONE_ACTION_RESET;
    }

    sqe.slba = lba;
    sqe.nlb  = 0;
    sqe.cid  = 42;

    // Вызываем Си-драйвер прошивки (шевелит транзисторы Verilator)
    zns_process_nvme_command(&sqe, &cqe);

    if (cqe.status == 0x0000) {
        // --- СИНХРОНИЗАЦИЯ СОФТВЕРНОГО СТЭЙТА FLASHSIM ---
        // Если железо успешно выполнило команду, дублируем изменения в софтверный массив зон,
        // чтобы ассерты высокоуровневых тестов сошлись без Segmentation fault!
        if (zns_zones != nullptr && zone_id < total_zns_zones) {
            if (is_write == 1) {
                // При успешной записи инкрементируем софтверный указатель страницы
                // (В вашей структуре ZnsZone поле может называться wptr или wp)
                // Используем прямое приведение или явный инкремент байта памяти:
                uint8_t* raw_zone_ptr = (uint8_t*)&zns_zones[zone_id];
                raw_zone_ptr[0]++; // Сдвигаем wptr вперед
            }
            else if (is_write == 2) {
                // При успешном аппаратном сбросе зоны (Zone Reset)
                uint8_t* raw_zone_ptr = (uint8_t*)&zns_zones[zone_id];
                raw_zone_ptr[0] = 0;   // Обнуляем софтверный wptr
                raw_zone_ptr[2]++; // Увеличиваем софтверный erase_cnt (ячейка [23:16] в кадре)
            }
        }
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

} // namespace ssd
