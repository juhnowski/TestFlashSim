// /home/ilya/TestFlashSim/OriginalFlashSim/zns_driver.c
#include "zns_driver.h"
#include "generated/csr.h"
#include <string.h>

extern void sim_set_io_trigger(uint8_t val);
extern void sim_set_io_cmd(uint8_t val);
extern void sim_set_validated_target_page(uint32_t val);
extern void sim_set_validated_zone_id(uint32_t val);
extern uint8_t sim_get_out_status(void);
extern uint8_t sim_get_out_err_code(void);
extern void verilator_tick_hardware(void);

static uint64_t current_lba_accumulator = 0;

void csr_write32(uint32_t val, uintptr_t addr) {
    if (addr == CSR_ZNS_TRIGGER_ADDR) {
        sim_set_io_trigger((uint8_t)(val & 0x1));
    }
    else if (addr == CSR_ZNS_NVME_LBA_LO_ADDR) {
        current_lba_accumulator = (current_lba_accumulator & 0xFFFFFFFF00000000ULL) | val;
        uint32_t local_page = (uint32_t)(current_lba_accumulator & 0x3F);
        uint32_t zone_id    = (uint32_t)((current_lba_accumulator >> 6) & 0xFFFFFFFF);
        sim_set_validated_target_page(local_page);
        sim_set_validated_zone_id(zone_id);
    }
    else if (addr == CSR_ZNS_NVME_LBA_HI_ADDR) {
        current_lba_accumulator = (current_lba_accumulator & 0x00000000FFFFFFFFULL) | ((uint64_t)val << 32);
        uint32_t local_page = (uint32_t)(current_lba_accumulator & 0x3F);
        uint32_t zone_id    = (uint32_t)((current_lba_accumulator >> 6) & 0xFFFFFFFF);
        sim_set_validated_target_page(local_page);
        sim_set_validated_zone_id(zone_id);
    }
    else if (addr == CSR_ZNS_NVME_OPCODE_ADDR) {
        sim_set_io_cmd((uint8_t)(val & 0xFF));
    }
    else if (addr == CSR_ZNS_SQ_TAIL_DB_ADDR) {
        stub_sq_tail = val;
    }
    else if (addr == CSR_ZNS_CQ_HEAD_DB_ADDR) {
        stub_cq_head = val;
    }
}

uint32_t csr_read32(uintptr_t addr) {
    if (addr == CSR_ZNS_OUT_STATUS_ADDR) {
        return (uint32_t)sim_get_out_status();
    }
    else if (addr == CSR_ZNS_ERR_CODE_ADDR) {
        return (uint32_t)sim_get_out_err_code();
    }
    return 0;
}

void zns_controller_init(void) {
    current_lba_accumulator = 0;
    sim_set_io_trigger(0);
    sim_set_io_cmd(0);
    sim_set_validated_target_page(0);
    sim_set_validated_zone_id(0);
}

void zns_process_nvme_command(const nvme_sqe_t *sqe, nvme_cqe_t *cqe) {
    uint8_t hardware_cmd = 0;
    cqe->cid = sqe->cid;
    cqe->cdw0 = 0;

    switch (sqe->opcode) {
        case NVME_CMD_READ:  hardware_cmd = 0; break;
        case NVME_CMD_WRITE: hardware_cmd = 1; break;
        case NVME_CMD_ZONE_MGMT: {
            if (((sqe->zsa) & 0xFF) == NVME_ZONE_ACTION_RESET) {
                hardware_cmd = 2;
            } else {
                cqe->status = (1 << 9) | (0x02 << 1);
                return;
            }
            break;
        }
        default:
            cqe->status = (1 << 9) | (0x01 << 1);
            return;
    }

    csr_write32((uint32_t)((sqe->slba >> 32) & 0xFFFFFFFF), CSR_ZNS_NVME_LBA_HI_ADDR);
    csr_write32((uint32_t)(sqe->slba & 0xFFFFFFFF), CSR_ZNS_NVME_LBA_LO_ADDR);
    csr_write32(hardware_cmd, CSR_ZNS_NVME_OPCODE_ADDR);
    csr_write32(0, CSR_ZNS_NVME_BLOCKS_ADDR);

    // Запускаем конечный автомат
    csr_write32(1, CSR_ZNS_TRIGGER_ADDR);

    uint32_t hw_status;
    do {
        hw_status = csr_read32(CSR_ZNS_OUT_STATUS_ADDR);
    } while (hw_status == 2 || hw_status == 0);

    uint32_t hw_error = csr_read32(CSR_ZNS_ERR_CODE_ADDR);

    if (hw_status == 3) {
        cqe->status = 0x0000;
    } else {
        uint16_t nvme_sc = 0;
        switch (hw_error) {
            case 0x01: nvme_sc = 0x82; break;
            case 0x02: nvme_sc = 0x81; break;
            case 0x03: nvme_sc = 0x80; break;
            case 0x05: nvme_sc = 0x24; break;
            case 0x07: nvme_sc = 0x83; break;
            case 0x08: nvme_sc = 0x24; break;
            default:   nvme_sc = 0x06; break;
        }
        cqe->status = (1 << 9) | (nvme_sc << 1);
    }

    csr_write32(0, CSR_ZNS_TRIGGER_ADDR);
    verilator_tick_hardware();
    verilator_tick_hardware();
}
