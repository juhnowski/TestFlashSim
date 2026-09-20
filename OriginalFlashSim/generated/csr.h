// /home/ilya/TestFlashSim/OriginalFlashSim/generated/csr.h
#ifndef GENERATED_CSR_H
#define GENERATED_CSR_H

#include <stdint.h>

// Глобальный указатель на структуру Verilator-модели контроллера.
// Будет проинициализирован внутри sim_main.cpp
extern void* verilator_top_model;

// Макросы LiteX теперь привязаны к физическим адресам портов С++ объекта модели Verilator
#define CSR_ZNS_TRIGGER_ADDR        ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->io_trigger))
#define CSR_ZNS_NVME_OPCODE_ADDR    ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->io_cmd))
#define CSR_ZNS_NVME_LBA_LO_ADDR     ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->validated_zone_id)) // Упрощенный маппинг для тестов портов
#define CSR_ZNS_NVME_LBA_HI_ADDR     ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->validated_target_page))
#define CSR_ZNS_NVME_BLOCKS_ADDR    ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->bram_wdata)) // Временные пробросы

#define CSR_ZNS_OUT_STATUS_ADDR     ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->out_status))
#define CSR_ZNS_ERR_CODE_ADDR       ((uintptr_t)&(((struct Vzns_fsm_validator_mock*)verilator_top_model)->out_err_code))

// Заглушки для Doorbell, так как в изолированном тесте FSM они не заведены на порты верхнего уровня
extern volatile uint32_t stub_sq_tail;
extern volatile uint32_t stub_cq_head;
#define CSR_ZNS_SQ_TAIL_DB_ADDR     ((uintptr_t)&stub_sq_tail)
#define CSR_ZNS_CQ_HEAD_DB_ADDR     ((uintptr_t)&stub_cq_head)

// Нам нужна структура-зеркало Verilator-модели для компилятора Си, чтобы он знал смещения полей ножек портов
struct Vzns_fsm_validator_mock {
    uint8_t  clk;
    uint8_t  rst;
    uint8_t  io_trigger;
    uint8_t  io_cmd;
    uint32_t validated_zone_id;
    uint32_t validated_target_page;
    uint8_t  addr_bound_error;
    uint8_t  thermal_shutdown_tripped;
    uint32_t bram_addr;
    uint32_t bram_rdata;
    uint32_t bram_wdata;
    uint8_t  bram_we;
    uint8_t  out_status;
    uint8_t  out_err_code;
};

#endif // GENERATED_CSR_H
