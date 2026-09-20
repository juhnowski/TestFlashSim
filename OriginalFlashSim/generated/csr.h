// /home/ilya/TestFlashSim/OriginalFlashSim/generated/csr.h
#ifndef GENERATED_CSR_H
#define GENERATED_CSR_H

#include <stdint.h>

// Виртуальная карта памяти регистров LiteX для тестирования
// Выделяем псевдо-адреса, смещенные относительно виртуального массива mock_reg_space
extern volatile uint32_t mock_reg_space[64];

#define CSR_ZNS_BASE                ((uintptr_t)&mock_reg_space[0])

#define CSR_ZNS_SQ_TAIL_DB_ADDR     ((uintptr_t)&mock_reg_space[0])
#define CSR_ZNS_CQ_HEAD_DB_ADDR     ((uintptr_t)&mock_reg_space[1])
#define CSR_ZNS_NVME_OPCODE_ADDR    ((uintptr_t)&mock_reg_space[2])
#define CSR_ZNS_NVME_NSID_ADDR      ((uintptr_t)&mock_reg_space[3])
#define CSR_ZNS_NVME_LBA_LO_ADDR     ((uintptr_t)&mock_reg_space[4])
#define CSR_ZNS_NVME_LBA_HI_ADDR     ((uintptr_t)&mock_reg_space[5])
#define CSR_ZNS_NVME_BLOCKS_ADDR    ((uintptr_t)&mock_reg_space[6])
#define CSR_ZNS_TRIGGER_ADDR        ((uintptr_t)&mock_reg_space[7])
#define CSR_ZNS_OUT_STATUS_ADDR     ((uintptr_t)&mock_reg_space[8])
#define CSR_ZNS_ERR_CODE_ADDR       ((uintptr_t)&mock_reg_space[9])
#define CSR_ZNS_TEMPERATURE_ADDR    ((uintptr_t)&mock_reg_space[10])

#endif // GENERATED_CSR_H
