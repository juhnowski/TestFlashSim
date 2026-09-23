// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_server.cpp
#include "nbd_server.h"
#include "nbd_protocol.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <byteswap.h>

extern "C" {
    void verilator_tick_hardware(Vzns_fsm_validator* top);
    extern uint8_t mock_bram_storage[TOTAL_SIZE_BYTES];
}

#define ntohll(x) bswap_64(x)
#define htonll(x) bswap_64(x)

void handle_nbd_transmission_loop(int client_fd, Vzns_fsm_validator* top) {
    std::cout << "[NBD] Переход в фазу Transmission I/O..." << std::endl;

    struct nbd_request_t request;
    struct nbd_reply_t reply;

    while (true) {
        if (read(client_fd, &request, sizeof(request)) <= 0) break;

        uint32_t magic = ntohl(request.magic);
        uint32_t type  = ntohl(request.type);
        uint64_t from  = ntohll(request.from);
        uint32_t len   = ntohl(request.len);

        if (magic != ZNS_NBD_REQUEST_MAGIC) {
            std::cerr << "[NBD] Ошибка: Неверный Request Magic!" << std::endl;
            break;
        }

        reply.magic = htonl(ZNS_NBD_REPLY_MAGIC);
        reply.error = 0;
        std::memcpy(reply.handle, request.handle, 8);

        if (type == NBD_CMD_LOCAL_READ) {
            verilator_tick_hardware(top);
            write(client_fd, &reply, sizeof(reply));
            write(client_fd, &mock_bram_storage[from], len);

        } else if (type == NBD_CMD_LOCAL_WRITE) {
                    char* write_buf = new char[len];
                    uint32_t recved = 0;
                    while (recved < len) {
                        ssize_t n = read(client_fd, write_buf + recved, len - recved);
                        if (n <= 0) break;
                        recved += n;
                    }

                    // ИСПРАВЛЕНИЕ: Перехват Out-of-Band команды сброса зоны от Rust FTL
                    // Если длина запроса ровно 8 байт и он пришел на нулевой LBA (или любой невыровненный под 4КБ)
                    if (len == 8 && (from % PAGE_SIZE) != 0) {
                        uint64_t target_zone_idx = 0;
                        std::memcpy(&target_zone_idx, write_buf, 8);

                        std::cout << "[NBD-RTL Bridge] 🔄 Перехвачен Out-of-Band запрос Zone Reset для зоны: "
                                  << target_zone_idx << std::endl;

                        // Подаем команду сброса в твой Verilog-автомат
                        // top->cmd_register = 0x03;
                        // top->zone_idx_reg = target_zone_idx;

                        // Сбрасываем BRAM-регион этой зоны в памяти хоста
                        uint64_t zone_offset = target_zone_idx * (ZONE_SIZE_PAGES * PAGE_SIZE);
                        std::memset(&mock_bram_storage[zone_offset], 0, ZONE_SIZE_PAGES * PAGE_SIZE);

                        verilator_tick_hardware(top);

                        write(client_fd, &reply, sizeof(reply));
                        delete[] write_buf;
                        continue; // Завершаем итерацию, минуя обычную запись
                    }

                    // Обычная логика записи 4КБ страниц
                    std::memcpy(&mock_bram_storage[from], write_buf, len);
                    verilator_tick_hardware(top);

                    write(client_fd, &reply, sizeof(reply));
                    delete[] write_buf;
        } else if (type == NBD_CMD_LOCAL_TRIM) {
            uint32_t target_zone_idx = from / (ZONE_SIZE_PAGES * PAGE_SIZE);
            std::cout << "[NBD-RTL] Системный BLKRESETZONE для зоны: " << target_zone_idx << std::endl;

            std::memset(&mock_bram_storage[from], 0, len);
            verilator_tick_hardware(top);

            write(client_fd, &reply, sizeof(reply));

        } else if (type == NBD_CMD_LOCAL_DISCONNECT) {
            std::cout << "[NBD] Получена команда DISCONNECT. Завершаем сессию." << std::endl;
            break;
        } else if (type == NBD_CMD_LOCAL_FLUSH) {
            write(client_fd, &reply, sizeof(reply));
        } else {
            reply.error = htonl(EINVAL);
            write(client_fd, &reply, sizeof(reply));
        }
    }
}

void start_nbd_server(Vzns_fsm_validator* top, int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(server_fd, 1) < 0) {
        perror("[NBD-Server] Ошибка запуска сокета");
        close(server_fd);
        return;
    }

    std::cout << "[NBD-Server] Слушаем порт " << port << "..." << std::endl;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

    if (client_fd >= 0) {
        if (run_nbd_negotiation(client_fd)) {
            handle_nbd_transmission_loop(client_fd, top);
        }
        close(client_fd);
    }
    close(server_fd);
}
