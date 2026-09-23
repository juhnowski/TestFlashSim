// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_protocol.h
#ifndef ZNS_NBD_PROTOCOL_H
#define ZNS_NBD_PROTOCOL_H

#include <cstdint>
#include "Vzns_fsm_validator.h"
#include "nbd_server.h" // Импортируем геометрию (NUM_ZONES, TOTAL_SIZE_BYTES и т.д.) отсюда

// Уникальные магические числа Newstyle протокола
#define ZNS_NBD_INIT_MAGIC 0x4e42444d41474943ULL
#define ZNS_NBD_REQUEST_MAGIC 0x25609513U
#define ZNS_NBD_REPLY_MAGIC 0x67446698U

// Команды трансмиссии
constexpr uint32_t NBD_CMD_LOCAL_READ       = 0;
constexpr uint32_t NBD_CMD_LOCAL_WRITE      = 1;
constexpr uint32_t NBD_CMD_LOCAL_DISCONNECT = 2;
constexpr uint32_t NBD_CMD_LOCAL_FLUSH      = 3;
constexpr uint32_t NBD_CMD_LOCAL_TRIM       = 5;

// Жесткое и явное определение упакованных структур протокола NBD для полной независимости от хедеров ядра
struct nbd_request_t {
    uint32_t magic;  // ZNS_NBD_REQUEST_MAGIC
    uint32_t type;   // NBD_CMD_LOCAL_*
    char     handle[8];
    uint64_t from;   // Смещение на диске в байтах
    uint32_t len;    // Длина операции в байтах
} __attribute__((packed));

struct nbd_reply_t {
    uint32_t magic;  // ZNS_NBD_REPLY_MAGIC
    uint32_t error;  // 0 в случае успеха, или код ошибки (EIO, EINVAL)
    char     handle[8];
} __attribute__((packed));

// Декларация функций фаз протокола
bool run_nbd_negotiation(int client_fd);
void handle_nbd_transmission_loop(int client_fd, Vzns_fsm_validator* top);

#endif // ZNS_NBD_PROTOCOL_H
