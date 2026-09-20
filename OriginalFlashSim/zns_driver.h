// /home/ilya/TestFlashSim/OriginalFlashSim/zns_driver.h
#ifndef ZNS_DRIVER_H
#define ZNS_DRIVER_H

#include <stdint.h>

// Константы Opcodes для NVMe ZNS Command Set
#define NVME_CMD_READ          0x02
#define NVME_CMD_WRITE         0x01
#define NVME_CMD_ZONE_MGMT     0x79

// Константы Zone Management Actions (из Dword 13 команды NVMe)
#define NVME_ZONE_ACTION_CLOSE  0x01
#define NVME_ZONE_ACTION_FINISH 0x02
#define NVME_ZONE_ACTION_RESET  0x04 // Наш аппаратный io_cmd = 2 (ZONE_RESET)

// Структура 64-байтной команды NVMe (Submission Queue Entry - SQE)
typedef struct {
    uint8_t  opcode;      // Dword 0: Код операции
    uint8_t  flags;
    uint16_t cid;         // Command Identifier
    uint32_t nsid;        // Dword 1: Namespace ID
    uint32_t rsvd1[2];    // Dword 2-3
    uint64_t mptr;        // Dword 4-5: Metadata Pointer
    uint64_t prp1;        // Dword 6-7: Data Pointer (PRP1)
    uint64_t prp2;        // Dword 8-9: PRP2
    uint64_t slba;        // Dword 10-11: Starting LBA (64-bit)
    uint32_t nlb;         // Dword 12: Number of Blocks (Младшие 16 бит)
    uint32_t zsa;         // Dword 13: Zone Action / Поля управления ZNS
    uint32_t rsvd2[2];    // Dword 14-15
} __attribute__((packed)) nvme_sqe_t;

// Структура 16-байтного ответа NVMe (Completion Queue Entry - CQE)
typedef struct {
    uint32_t cdw0;        // Command Specific Data
    uint32_t rsvd;
    uint16_t sq_head;     // Текущий указатель головы SQ
    uint16_t sq_id;       // Submission Queue ID
    uint16_t cid;         // Command Identifier (из SQE)
    uint16_t status;      // Поле статуса NVMe: Bit 0=Phase, Bits 1-15=Status Code (SC + SCT)
} __attribute__((packed)) nvme_cqe_t;

// Интерфейсы драйвера
void zns_controller_init(void);
void zns_process_nvme_command(const nvme_sqe_t *sqe, nvme_cqe_t *cqe);

#endif // ZNS_DRIVER_H
