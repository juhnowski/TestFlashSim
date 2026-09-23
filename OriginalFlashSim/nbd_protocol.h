// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_protocol.h
#ifndef ZNS_NBD_PROTOCOL_H
#define ZNS_NBD_PROTOCOL_H

#include <cstdint>

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

struct nbd_request_t {
    uint32_t magic;  // ZNS_NBD_REQUEST_MAGIC
    uint32_t type;   // NBD_CMD_LOCAL_*
    uint64_t handle; // ИСПРАВЛЕНО: Строго u64 (8 байт) для уникального ID транзакции ядра
    uint64_t from;   // Смещение в байтах
    uint32_t len;    // Длина в байтах
} __attribute__((packed));

struct nbd_reply_t {
    uint32_t magic;  // ZNS_NBD_REPLY_MAGIC
    uint32_t error;  // 0 в случае успеха, errno при сбое
    uint64_t handle; // ИСПРАВЛЕНО: Строго u64 (8 байт) для возврата ядру
} __attribute__((packed));

bool run_nbd_negotiation(int client_fd);
void handle_nbd_transmission_loop(int client_fd);

#endif // ZNS_NBD_PROTOCOL_H
