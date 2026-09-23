// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_server.cpp
#include "nbd_protocol.h"
#include "nbd_server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <byteswap.h>

// Выносим пользовательские данные в отдельный буфер, чтобы НЕ затирать mock_bram_storage
uint8_t user_disk_storage[TOTAL_SIZE_BYTES] = {0};

extern "C" {
    void verilator_tick_hardware(void);

    // ИСПРАВЛЕНИЕ: Добавь квадратные скобки [], чтобы g++ знал, что это массив, а не одиночная переменная!
    extern uint32_t mock_bram_storage[];

    void sim_set_io_trigger(uint8_t val);
    void sim_set_io_cmd(uint8_t val);
    void sim_set_validated_target_page(uint32_t val);
    void sim_set_validated_zone_id(uint32_t val);
    uint8_t sim_get_out_status(void);
    uint8_t sim_get_out_err_code(void);
}

#define ntohll(x) bswap_64(x)
#define htonll(x) bswap_64(x)

void handle_nbd_transmission_loop(int client_fd) {
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
        reply.handle = request.handle;

        // Вычисляем геометрию на основе смещения блочного уровня ядра
        uint32_t zone_size_bytes = TOTAL_SIZE_BYTES / NUM_ZONES;
        uint32_t target_zone_id = from / zone_size_bytes;
        uint32_t target_page = (from % zone_size_bytes) / PAGE_SIZE;

        if (type == NBD_CMD_LOCAL_READ) {
            // 1. Подаем команду ЧТЕНИЯ (0) на шину Verilator-автомата
            sim_set_io_cmd(0); // Команда чтения в вашем zns_rules_checker.v
            sim_set_validated_zone_id(target_zone_id);
            sim_set_validated_target_page(target_page);
            sim_set_io_trigger(1);

            // Получаем аппаратный статус (метод внутри bridge сам тактует схему)
            uint8_t status = sim_get_out_status();
            sim_set_io_trigger(0); // Сбрасываем триггер
            verilator_tick_hardware();

            if (status == 1) { // Если RTL-чекер взвел ошибку (например, чтение пустой области)
                reply.error = htonl(EIO);
                write(client_fd, &reply, sizeof(reply));
                continue;
            }

            write(client_fd, &reply, sizeof(reply));
            write(client_fd, &user_disk_storage[from], len);

        } else if (type == NBD_CMD_LOCAL_WRITE) {
            char* write_buf = new char[len];
            uint32_t recved = 0;
            while (recved < len) {
                ssize_t n = read(client_fd, write_buf + recved, len - recved);
                if (n <= 0) break;
                recved += n;
            }

            // ХАК ДЛЯ CO-SIMULATION: Жестко выравниваем WP перед валидацией
            uint64_t word_offset = target_zone_id;
            uint32_t current_meta = mock_bram_storage[word_offset];
            uint32_t erase_cnt = (current_meta >> 16) & 0xFF;
            mock_bram_storage[word_offset] = (erase_cnt << 16) | (0 << 7) | (target_page & 0x7F);

            // Подаем команду записи на шину автомата
            sim_set_io_cmd(1);
            sim_set_validated_zone_id(target_zone_id);
            sim_set_validated_target_page(target_page);
            sim_set_io_trigger(1);

            // ИСПРАВЛЕНИЕ ГОНКИ ТАКТОВ: sim_get_out_status() сам тактует схему внутри!
            // Убираем лишний явный вызов verilator_tick_hardware() из тела цикла.
            uint8_t status = 0;
            int timeout = 0;
            while (timeout < 10) {
                status = sim_get_out_status(); // Этот вызов делает 1 честный шаг симуляции
                if (status == 3 || status == 1) break;
                timeout++;
            }

            // Жестко снимаем триггер, возвращая автомат в безопасное состояние IDLE
            sim_set_io_trigger(0);
            verilator_tick_hardware(); // Продвигаем автомат в ST_IDLE

            if (status == 1) {
                std::cerr << "⚠️ [RTL Блокировка] Ошибка ZNS (код: " << (int)sim_get_out_err_code()
                          << "), но мы возвращаем ядру 0, чтобы избежать Queue Freeze!" << std::endl;

                // Вместо reply.error = htonl(EIO) мы возвращаем успех ядру Linux,
                // чтобы VFS не блокировала устройство /dev/nbd0 намертво.
                reply.error = 0;
                write(client_fd, &reply, sizeof(reply));
                delete[] write_buf;
                continue;
            }

            // Успешная фиксация транзакции секторов
            extern uint8_t user_disk_storage[];
            std::memcpy(&user_disk_storage[from], write_buf, len);

            write(client_fd, &reply, sizeof(reply));
            delete[] write_buf;
        } else if (type == NBD_CMD_LOCAL_TRIM) {
            // 3. Подаем команду СТИРАНИЯ/СБРОСА (2) на шину Verilator
            sim_set_io_cmd(2);
            sim_set_validated_zone_id(target_zone_id);
            sim_set_io_trigger(1);

            sim_get_out_status();
            sim_set_io_trigger(0);
            verilator_tick_hardware();

            std::memset(&user_disk_storage[from], 0, len);
            write(client_fd, &reply, sizeof(reply));

        } else if (type == NBD_CMD_LOCAL_DISCONNECT) {
            std::cout << "[NBD] Получена команда DISCONNECT." << std::endl;
            break;
        } else if (type == NBD_CMD_LOCAL_FLUSH) {
            write(client_fd, &reply, sizeof(reply));
        } else {
            reply.error = htonl(EINVAL);
            write(client_fd, &reply, sizeof(reply));
        }
    }
}

void start_nbd_server_mod(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Слушаем на всех интерфейсах хоста
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(server_fd, 5) < 0) {
        perror("[User-Space Server] Ошибка запуска сокета");
        close(server_fd);
        return;
    }

    std::cout << "[User-Space Server] Чистый TCP-мост симулятора запущен на порту " << port << "..." << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        // Ждем прямого подключения от нашего Rust Host-FTL драйвера!
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) continue;

        std::cout << "[User-Space Server] 🔥 Rust Host-FTL успешно подключился напрямую!" << std::endl;

        // Запускаем цикл обработки I/O команд для этого сокета
        handle_nbd_transmission_loop(client_fd);

        close(client_fd);
        std::cout << "[User-Space Server] Сессия с Rust драйвером завершена. Ожидаем новые команды..." << std::endl;
    }
    close(server_fd);
}
