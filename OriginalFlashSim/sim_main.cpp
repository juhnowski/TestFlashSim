// /home/ilya/TestFlashSim/OriginalFlashSim/sim_main.cpp
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include "verilated.h"
#include "Vzns_fsm_validator.h"
#include "nbd_server.h"
#include "nbd_protocol.h"

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ ЛИНКОВКИ С HARDWARE_BRIDGE.CPP
Vzns_fsm_validator* top = nullptr;

// Объявляем пространство имен и класс в строгом соответствии с заголовочными файлами проекта
namespace ssd {
    class Controller {
    public:
        void check_thermal_shutdown();
    };

    void Controller::check_thermal_shutdown() {
        // Пустая заглушка термического контроля для RAM-эмулятора
    }
}

// Выделяем указатель для линковщика
ssd::Controller* global_controller_ptr = nullptr;

extern "C" {
    void verilator_tick_hardware(Vzns_fsm_validator* top);
}
extern uint8_t mock_bram_storage[TOTAL_SIZE_BYTES];

// Поток управления: слушает Unix Domain Socket для сброса зон напрямую из Rust, минуя ядро
void management_socket_thread() {
    const char* sock_path = "/tmp/zns_mgmt.sock";
    unlink(sock_path);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[Mgmt-Server] Критическая ошибка: socket() failed");
        return;
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(server_fd, 5) < 0) {
        perror("[Mgmt-Server] Критическая ошибка: bind() или listen() failed");
        close(server_fd);
        return;
    }

    std::cout << "[Mgmt-Server] Канал управления активен: " << sock_path << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("[Mgmt-Server] Предупреждение: accept() failed");
            continue;
        }

        std::cout << "[Mgmt-Server] Rust-клиент подключился к сокету управления." << std::endl;

        uint64_t zone_id = 0;
        char* zone_ptr = reinterpret_cast<char*>(&zone_id);
        uint32_t bytes_received = 0;
        bool success = true;

        while (bytes_received < sizeof(zone_id)) {
            ssize_t n = read(client_fd, zone_ptr + bytes_received, sizeof(zone_id) - bytes_received);
            if (n < 0) {
                std::cerr << "[Mgmt-Server] Ошибка read(): " << strerror(errno) << " (код: " << errno << ")" << std::endl;
                success = false;
                break;
            }
            if (n == 0) {
                std::cerr << "[Mgmt-Server] Преждевременный EOF от клиента! Получено всего " << bytes_received << " из 8 байт." << std::endl;
                success = false;
                break;
            }
            bytes_received += n;
            std::cout << "[Mgmt-Server] Считано " << n << " байт. Всего: " << bytes_received << " / 8" << std::endl;
        }

        if (success) {
            std::cout << "[Mgmt-Server] 🔄 Успешно извлечен ID зоны для сброса: " << zone_id << std::endl;

            uint64_t zone_size = TOTAL_SIZE_BYTES / NUM_ZONES;
            uint64_t zone_offset = zone_id * zone_size;

            std::memset(&mock_bram_storage[zone_offset], 0, zone_size);

            if (top) {
                verilator_tick_hardware(top);
            }

            uint8_t ack = 1;
            ssize_t w_res = write(client_fd, &ack, 1);
            if (w_res != 1) {
                std::cerr << "[Mgmt-Server] Ошибка отправки ACK: " << (w_res < 0 ? strerror(errno) : "частичная запись") << std::endl;
            } else {
                std::cout << "[Mgmt-Server] Байт ACK (1) успешно отправлен в Rust-драйвер." << std::endl;
            }
        } else {
            std::cerr << "[Mgmt-Server] Обработка команды сброса отклонена из-за ошибки чтения данных." << std::endl;
        }

        // Даем ядру время протолкнуть буфер ACK перед жестким закрытием дескриптора сокета
        shutdown(client_fd, SHUT_WR);
        char dummy[10];
        while (read(client_fd, dummy, sizeof(dummy)) > 0); // Корректное закрытие сокета (TCP/Unix linger)
        close(client_fd);
        std::cout << "[Mgmt-Server] Сессия закрыта, сокет освобожден." << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cout << "=======================================================" << std::endl;
    std::cout << "🚀 Запуск Verilator ZNS RAM-эмулятора с Mgmt-сокетом..." << std::endl;
    std::cout << "=======================================================" << std::endl;

    Verilated::commandArgs(argc, argv);

    top = new Vzns_fsm_validator;
    global_controller_ptr = reinterpret_cast<ssd::Controller*>(new char[sizeof(ssd::Controller)]);

    // Запускаем независимый поток управления
    std::thread mgmt_thread(management_socket_thread);
    mgmt_thread.detach();

    // Запускаем основной NBD сервер диска (блокирующий вызов)
    start_nbd_server(top, 10809);

    top->final();
    delete top;
    delete[] reinterpret_cast<char*>(global_controller_ptr);
    return EXIT_SUCCESS;
}
