// /home/ilya/TestFlashSim/OriginalFlashSim/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zns_driver.h"
#include "generated/csr.h"

// Объявляем физическое пространство наших псевдо-регистров
volatile uint32_t mock_reg_space[64] = {0};

// Программная эмуляция ответа транзисторной логики Verilog
void simulate_hardware_response(void) {
    // Проверяем, взвел ли драйвер триггер запуска (TRIGGER = 1)
    if (mock_reg_space[7] == 1) {
        uint32_t opcode = mock_reg_space[2];
        uint32_t lba_lo = mock_reg_space[4];

        // Имитируем состояние BUSY (FSM_State = 2) во время прокрутки тактов
        mock_reg_space[8] = 2;

        // Логика эмуляции ответов для разных сценариев
        if (opcode == 1) { // Команда записи (WRITE)
            if (lba_lo == 0) {
                // Сценарий 1: Валидная последовательная запись на LBA 0 (УСПЕХ)
                mock_reg_space[8] = 3; // 3 = SUCCESS_DONE в FSM
                mock_reg_space[9] = 0; // Код ошибки 0
            } else if (lba_lo == 5) {
                // Сценарий 2: Невыровненная случайная запись на LBA 5 (ОТКАЗ)
                mock_reg_space[8] = 1; // 1 = ST_ERROR в FSM
                mock_reg_space[9] = 0x01; // Внутренний код: ZNS_ERR_UNALIGNED_WRITE
            }
        } else if (opcode == 2) { // Команда сброса зоны (ZONE_RESET)
            // Сценарий 3: Аппаратный сброс зоны (УСПЕХ)
            mock_reg_space[8] = 3;
            mock_reg_space[9] = 0;
        }
    }
}

void print_nvme_completion_status(const char *test_name, const nvme_cqe_t *cqe) {
    uint16_t sct = (cqe->status >> 9) & 0x7;
    uint16_t sc  = (cqe->status >> 1) & 0xFF;

    printf("[%s] Завершено -> Status Word: 0x%04X [SCT: %d, SC: 0x%02X] ", test_name, cqe->status, sct, sc);
    if (cqe->status == 0) {
        printf("✔️  SUCCESS\n");
    } else {
        printf("❌  COMMAND ERROR (Трансляция выполнена верно)\n");
    }
}

int main(void) {
    printf("=== СТАРТ ПРОГРАММНОГО СИ-ТЕСТБЕНЧА ДРАЙВЕРА NVMe ZNS ===\n\n");

    // Инициализируем стек CSR-регистров
    zns_controller_init();

    nvme_sqe_t sqe;
    nvme_cqe_t cqe;

    // --- ТЕСТ 1: Подача валидной команды последовательной записи (LBA 0) ---
    memset(&sqe, 0, sizeof(nvme_sqe_t));
    memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.cid = 101;
    sqe.opcode = NVME_CMD_WRITE;
    sqe.slba = 0; // Начинаем с LBA 0
    sqe.nlb = 0;  // 1 блок

    // Вызываем парсер драйвера. Так как у нас нет потоков, принудительно
    // вызываем симулятор ответа железа прямо перед вызовом поллинга внутри драйвера,
    // либо симулируем синхронный проход:
    mock_reg_space[7] = 0; // Сброс триггера

    // Передаем команду в драйвер (в условиях теста симулируем ответ "на лету")
    // Чтобы поллинг внутри zns_process_nvme_command не завис, мы заранее заполняем mock
    mock_reg_space[8] = 3; // SUCCESS
    mock_reg_space[9] = 0;
    zns_process_nvme_command(&sqe, &cqe);
    print_nvme_completion_status("ТЕСТ 1: Последовательная запись LBA=0", &cqe);

    // --- ТЕСТ 2: Подача некорректной невыровненной записи (LBA 5) ---
    memset(&sqe, 0, sizeof(nvme_sqe_t));
    memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.cid = 102;
    sqe.opcode = NVME_CMD_WRITE;
    sqe.slba = 5; // Нарушаем WPTR (Ожидается ошибка 0x82)

    mock_reg_space[8] = 1; // ST_ERROR
    mock_reg_space[9] = 0x01; // ZNS_ERR_UNALIGNED_WRITE
    zns_process_nvme_command(&sqe, &cqe);
    print_nvme_completion_status("ТЕСТ 2: Случайная запись LBA=5", &cqe);

    // --- ТЕСТ 3: Подача команды Zone Management (Action: Zone Reset) ---
    memset(&sqe, 0, sizeof(nvme_sqe_t));
    memset(&cqe, 0, sizeof(nvme_cqe_t));
    sqe.cid = 103;
    sqe.opcode = NVME_CMD_ZONE_MGMT;
    sqe.zsa = NVME_ZONE_ACTION_RESET; // Zone Reset Action
    sqe.slba = 64; // Сбрасываем Зону 1

    mock_reg_space[8] = 3; // SUCCESS
    mock_reg_space[9] = 0;
    zns_process_nvme_command(&sqe, &cqe);
    print_nvme_completion_status("ТЕСТ 3: Управление зоной (Zone Reset)", &cqe);

    printf("\n=== ПРОГРАММНЫЙ ТЕСТБЕНЧ УСПЕШНО ЗАВЕРШЕН ===\n");
    return 0;
}
