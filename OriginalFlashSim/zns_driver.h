// /home/ilya/TestFlashSim/OriginalFlashSim/zns_driver.h
#ifndef ZNS_DRIVER_H
#define ZNS_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Константы Opcodes для NVMe ZNS Command Set
#define NVME_CMD_READ          0x02
#define NVME_CMD_WRITE         0x01
#define NVME_CMD_ZONE_MGMT     0x79

// Константы Zone Management Actions
#define NVME_ZONE_ACTION_CLOSE  0x01
#define NVME_ZONE_ACTION_FINISH 0x02
#define NVME_ZONE_ACTION_RESET  0x04

// Структура 64-байтной команды NVMe (Submission Queue Entry - SQE)
typedef struct {
    uint8_t  opcode;      // Dword 0: Код операции
    uint8_t  flags;
    uint16_t cid;         // Command Identifier
    uint32_t nsid;        // Dword 1: Namespace ID
    uint32_t rsvd1;
    uint64_t mptr;
    uint64_t prp1;
    uint64_t prp2;
    uint64_t slba;        // Dword 10-11: Starting LBA (64-bit)
    uint32_t nlb;         // Dword 12: Number of Blocks
    uint32_t zsa;         // Dword 13: Zone Action
    uint32_t rsvd2;
} __attribute__((packed)) nvme_sqe_t;

// Структура 16-байтного ответа NVMe (Completion Queue Entry - CQE)
typedef struct {
    uint32_t cdw0;
    uint32_t rsvd;
    uint16_t sq_head;
    uint16_t sq_id;
    uint16_t cid;
    uint16_t status;
} __attribute__((packed)) nvme_cqe_t;

// Интерфейсы драйвера прошивки
void zns_controller_init(void);
void zns_process_nvme_command(const nvme_sqe_t *sqe, nvme_cqe_t *cqe);

// АППАРАТНЫЕ ИНТЕРФЕЙСЫ-МОСТЫ ДЛЯ СВЯЗИ C++ И СИ (Исключают name mangling)
void verilator_tick_hardware(void);
void sim_set_io_trigger(uint8_t val);
void sim_set_io_cmd(uint8_t val);
void sim_set_validated_target_page(uint32_t val);
void sim_set_validated_zone_id(uint32_t val);
uint8_t sim_get_out_status(void);
uint8_t sim_get_out_err_code(void);

#ifdef __cplusplus
}
#endif

#endif // ZNS_DRIVER_H
