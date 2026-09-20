// /home/ilya/TestFlashSim/OriginalFlashSim/zns_driver.c
#include "zns_driver.h"
#include "generated/csr.h" // Подключаем LiteX макросы регистров

// Вспомогательный макрос для записи в 32-битные CSR-регистры LiteX
#define csr_write32(val, addr) (*((volatile uint32_t *)(addr)) = (val))
// Вспомогательный макрос для чтения из CSR-регистров LiteX
#define csr_read32(addr)       (*((volatile uint32_t *)(addr)))

/**
 * Инициализация ZNS подсистемы
 */
void zns_controller_init(void) {
    // Принудительно сбрасываем триггер запуска в 0
    csr_write32(0, CSR_ZNS_TRIGGER_ADDR);
    // Инициализируем Doorbell указатели очередей NVMe
    csr_write32(0, CSR_ZNS_SQ_TAIL_DB_ADDR);
    csr_write32(0, CSR_ZNS_CQ_HEAD_DB_ADDR);
}

/**
 * Парсер кадра NVMe команды и трансляция в транзисторную CSR-шину
 */
void zns_process_nvme_command(const nvme_sqe_t *sqe, nvme_cqe_t *cqe) {
    uint8_t hardware_cmd = 0;
    
    // Заполняем базовые поля ответа (Completion Entry)
    cqe->cid = sqe->cid;
    cqe->cdw0 = 0;
    
    // Транслируем высокоуровневые коды операций NVMe ZNS в битового уровня CSR-шину
    switch (sqe->opcode) {
        case NVME_CMD_READ:
            hardware_cmd = 0; // Наш аппаратный код: 0 = READ
            break;
            
        case NVME_CMD_WRITE:
            hardware_cmd = 1; // Наш аппаратный код: 1 = WRITE
            break;
            
        case NVME_CMD_ZONE_MGMT: {
            // Извлекаем Zone Management Action (ZSA) из Dword 13
            uint8_t action = (sqe->zsa) & 0xFF;
            if (action == NVME_ZONE_ACTION_RESET) {
                hardware_cmd = 2; // Наш аппаратный код: 2 = ZONE_RESET (TRIM)
            } else {
                // ИСПРАВЛЕНО: убран Verilog-овский end
                // Если прилетела неподдерживаемая команда (например, Zone Finish)
                // Возвращаем NVMe ошибку: Invalid Field in Command (SCT=1, SC=0x02)
                cqe->status = (1 << 9) | (0x02 << 1); 
                return;
            }
            break;
        }
            
        default:
            // Неизвестный Opcode: возвращаем NVMe статус Invalid Command Opcode (SCT=1, SC=0x01)
            cqe->status = (1 << 9) | (0x01 << 1);
            return;
    }

    // --- КВИТИРОВАНИЕ (HANDSHAKE) С АППАРАТНЫМ ДВИЖКОМ ---
    
    // 1. Раскладываем 64-битный Starting LBA из NVMe SQE по 32-битным регистрам LiteX
    csr_write32((uint32_t)(sqe->slba & 0xFFFFFFFF), CSR_ZNS_NVME_LBA_LO_ADDR);
    csr_write32((uint32_t)((sqe->slba >> 32) & 0xFFFFFFFF), CSR_ZNS_NVME_LBA_HI_ADDR);
    
    // 2. Записываем код команды и количество блоков
    csr_write32(hardware_cmd, CSR_ZNS_NVME_OPCODE_ADDR);
    csr_write32((uint32_t)(sqe->nlb & 0xFFFF), CSR_ZNS_NVME_BLOCKS_ADDR);
    
    // 3. Импульс запуска: взводим триггер в 1, запуская конечный автомат Verilog (ST_BRAM_READ)
    csr_write32(1, CSR_ZNS_TRIGGER_ADDR);

    // 4. ПОЛЛИНГ (POLLING): Опрашиваем состояние аппаратного автомата, пока статус BUSY (2)
    uint32_t hw_status;
    do {
        hw_status = csr_read32(CSR_ZNS_OUT_STATUS_ADDR);
    } while (hw_status == 2); // ИСПРАВЛЕНО: убран Verilog-овский end while

    // 5. Анализируем финальный аппаратный статус выполнения
    uint32_t hw_error = csr_read32(CSR_ZNS_ERR_CODE_ADDR);
    
    if (hw_status == 3) { // 3 = SUCCESS_DONE в нашем Handshake-автомате
        cqe->status = 0x0000; // Чистый NVMe SUCCESS!
    } else {
        // Если FSM ушла в состояние ST_ERROR (hw_status == 1),
        // транслируем наши внутренние коды ошибок в официальные статусы NVMe ZNS
        uint16_t nvme_sc = 0;
        
        switch (hw_error) {
            case 0x01: // ZNS_ERR_UNALIGNED_WRITE (Тест 2)
                nvme_sc = 0x82; // NVMe SC: Zone Write Pointer Error (0x82)
                break;
            case 0x02: // ZNS_ERR_RESOURCE_EXCEEDED (Тест 4/6)
                nvme_sc = 0x81; // NVMe SC: Zone Resource Non-active Exceeded (0x81)
                break;
            case 0x03: // ZNS_ERR_READ_EMPTY_ZONE (Тест 3)
                nvme_sc = 0x80; // NVMe SC: Zone Read Boundary Error (0x80)
                break;
            case 0x05: // ZNS_WARN_WEAR_LIMIT (Тест 7)
                nvme_sc = 0x24; // NVMe SC: Hardware Device Error (0x24)
                break;
            case 0x07: // ZNS_ERR_WRITE_TO_FULL (Тест 8/9)
                nvme_sc = 0x83; // NVMe SC: Zone Is Full (0x83)
                break;
            case 0x08: // ZNS_ERR_THERMAL_SHUTDOWN (Тест 5/11)
                nvme_sc = 0x24; // NVMe SC: Hardware Device Error (0x24)
                break;
            default:
                nvme_sc = 0x06; // NVMe SC: Internal Device Error (0x06)
                break;
        }
        
        // Пакуем в NVMe Status Field: SCT = 1 (Generic Command Status)
        cqe->status = (1 << 9) | (nvme_sc << 1); 
    }

    // 6. Сбрасываем триггер в 0, возвращая аппаратную FSM обратно в состояние ST_IDLE
    csr_write32(0, CSR_ZNS_TRIGGER_ADDR);
}
