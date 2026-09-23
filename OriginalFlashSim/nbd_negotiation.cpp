// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_negotiation.cpp
#include "nbd_protocol.h"
#include "nbd_server.h" // ДОБАВЛЕНО: Теперь TOTAL_SIZE_BYTES прозрачно виден здесь
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

bool run_nbd_negotiation(int client_fd) {
    std::cout << "[NBD] Начинаем Newstyle Хендшейк..." << std::endl;

    struct {
        uint64_t magic;
        uint64_t opts_magic;
        uint16_t global_flags;
    } __attribute__((packed)) greeting;

    greeting.magic = __builtin_bswap64(ZNS_NBD_INIT_MAGIC);
    greeting.opts_magic = __builtin_bswap64(0x444d41474943ULL); // "IHAVEOPT"
    greeting.global_flags = __builtin_bswap16(1);               // NBD_FLAG_FIXED_NEWSTYLE

    if (write(client_fd, &greeting, sizeof(greeting)) != sizeof(greeting)) {
        std::cerr << "[NBD] Ошибка отправки приветствия." << std::endl;
        return false;
    }

    uint32_t client_flags;
    if (read(client_fd, &client_flags, sizeof(client_flags)) < 0) return false;

    while (true) {
        struct {
            uint64_t magic;
            uint32_t option;
            uint32_t opt_len;
        } __attribute__((packed)) opt_header;

        if (read(client_fd, &opt_header, sizeof(opt_header)) <= 0) return false;

        uint32_t option = ntohl(opt_header.option);
        uint32_t opt_len = ntohl(opt_header.opt_len);

        char* opt_data = new char[opt_len + 1]();
        if (opt_len > 0) {
            uint32_t bytes_read = 0;
            while (bytes_read < opt_len) {
                ssize_t n = read(client_fd, opt_data + bytes_read, opt_len - bytes_read);
                if (n <= 0) break;
                bytes_read += n;
            }
        }

        if (option == 1) { // NBD_OPT_EXPORT_NAME
            struct {
                uint64_t disk_size;
                uint16_t transmission_flags;
            } __attribute__((packed)) export_reply;

            // Константа теперь успешно подтягивается из nbd_server.h
            export_reply.disk_size = __builtin_bswap64(TOTAL_SIZE_BYTES);
            export_reply.transmission_flags = __builtin_bswap16(1); // NBD_FLAG_HAS_FLAGS

            write(client_fd, &export_reply, sizeof(export_reply));
            std::cout << "[NBD] Успешно согласован размер диска: " << TOTAL_SIZE_BYTES / 1024 / 1024 << " МБ." << std::endl;
            delete[] opt_data;
            break;
        } else {
            struct {
                uint64_t reply_magic;
                uint32_t option;
                uint32_t reply_type;
                uint32_t length;
            } __attribute__((packed)) opt_reply;

            opt_reply.reply_magic = __builtin_bswap64(0x3e889045565a9ULL); // NBD_REP_MAGIC
            opt_reply.option = __builtin_bswap32(option);
            opt_reply.reply_type = __builtin_bswap32(0x80000001U);         // NBD_REP_ERR_UNSUP
            opt_reply.length = 0;

            write(client_fd, &opt_reply, sizeof(opt_reply));
            delete[] opt_data;
        }
    }
    return true;
}
