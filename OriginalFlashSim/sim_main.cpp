// /home/ilya/TestFlashSim/OriginalFlashSim/sim_main.cpp
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <memory>
#include "verilated.h"
#include "Vzns_fsm_validator.h"
#include "nbd_server.h"
#include "nbd_protocol.h"

// СТРОГОЕ ОБЪЯВЛЕНИЕ UNIQUE_PTR ДЛЯ МАТЧИНГА С HARDWARE_BRIDGE.CPP
std::unique_ptr<Vzns_fsm_validator> top = nullptr;

extern "C" void _ZN3ssd10Controller22check_thermal_shutdownEv() {}

namespace ssd { class Controller; }
ssd::Controller* global_controller_ptr = nullptr;

// ИСПРАВЛЕНИЕ: Добавляем все функции управления из hardware_bridge.cpp в область видимости файла
extern "C" {
    void verilator_tick_hardware(void);
    extern uint32_t mock_bram_storage[];

    void sim_set_io_trigger(uint8_t val);
    void sim_set_io_cmd(uint8_t val);
    void sim_set_validated_target_page(uint32_t val);
    void sim_set_validated_zone_id(uint32_t val);
    uint8_t sim_get_out_status(void);
    uint8_t sim_get_out_err_code(void);
}

void management_socket_thread() {
    const char* sock_path = "/tmp/zns_mgmt.sock";
    unlink(sock_path);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    std::cout << "[Mgmt-Server] Канал управления активен: " << sock_path << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;

        uint64_t zone_id = 0;
        char* zone_ptr = reinterpret_cast<char*>(&zone_id);
        uint32_t bytes_received = 0;
        bool success = true;

        while (bytes_received < sizeof(zone_id)) {
            ssize_t n = read(client_fd, zone_ptr + bytes_received, sizeof(zone_id) - bytes_received);
            if (n <= 0) { success = false; break; }
            bytes_received += n;
        }

        if (success) {
            std::cout << "[Mgmt-Server] 🔄 Извлечен ID зоны для аппаратного очищения: " << zone_id << std::endl;

            // ИСПРАВЛЕНИЕ: Сначала подаем команду стирания (2), чтобы трекер ресурсов (zns_resources_tracker.v)
            // увидел старый wptr > 0 и честно декрементировал счетчик open_zones_count!
            sim_set_io_cmd(2);
            sim_set_validated_zone_id(zone_id);
            sim_set_io_trigger(1);

            // Прогоняем циклы валидации и исполнения стирания в RTL
            sim_get_out_status();
            sim_set_io_trigger(0);
            verilator_tick_hardware();

            // И только ПОСЛЕ этого дочищаем физическую память пользовательских секторов и метаданных на хосте
            std::memset(mock_bram_storage, 0, 1024 * sizeof(uint32_t));

            extern uint8_t user_disk_storage[];
            uint64_t zone_size = TOTAL_SIZE_BYTES / NUM_ZONES;
            std::memset(&user_disk_storage[zone_id * zone_size], 0, zone_size);

            // Отправляем подтверждение в Rust FTL
            uint8_t ack = 1;
            write(client_fd, &ack, 1);
        }
        close(client_fd);
    }
}

int main(int argc, char** argv) {
    std::cout << "=======================================================" << std::endl;
    std::cout << "🚀 Запуск Verilator ZNS RAM-эмулятора (Интеграция FSM)..." << std::endl;
    std::cout << "=======================================================" << std::endl;

    Verilated::commandArgs(argc, argv);

    top = std::make_unique<Vzns_fsm_validator>();

    // Начальный сброс схемы
    if (top) {
        top->rst = 1;
        verilator_tick_hardware();
        top->rst = 0;
        verilator_tick_hardware();
    }

    std::thread mgmt_thread(management_socket_thread);
    mgmt_thread.detach();

    extern void start_nbd_server_mod(int port);
    start_nbd_server_mod(10809);

    return EXIT_SUCCESS;
}
