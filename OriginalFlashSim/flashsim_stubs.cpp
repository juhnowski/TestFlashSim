// /home/ilya/TestFlashSim/OriginalFlashSim/flashsim_stubs.cpp
#include <iostream>
#include <cstring>
#include <cstdio>

// ТРЮК ДЛЯ ОБХОДА PRIVATE: Взламываем инкапсуляцию для тестового окружения.
// Теперь все private методы в ssd.h станут public исключительно для этого файла!
#define private public
#include "ssd.h"
#undef private

// Импортируем ссылку на физический массив памяти из аппаратного моста
extern uint32_t mock_bram_storage[];

namespace ssd {
    ulong Controller::total_zns_zones = 1024;
    ZnsZone* Controller::zns_zones = nullptr;
}

namespace ssd {
    Controller::Controller(Ssd& parent) : ssd(parent) {
        if (total_zns_zones == 0) total_zns_zones = 1024;
        zns_zones = new ZnsZone[total_zns_zones];
        std::memset(zns_zones, 0, sizeof(ZnsZone) * total_zns_zones);
    }

    Controller::~Controller() {
        if (zns_zones) {
            delete[] zns_zones;
            zns_zones = nullptr;
        }
    }

    enum status Controller::issue(Event &event_list) { return SUCCESS; }
    Block* Controller::get_block_pointer(const Address & address) { return nullptr; }
    enum status Controller::event_arrive(Event &event) { return SUCCESS; }
    void Controller::print_ftl_statistics() {}

    ulong Controller::get_erases_remaining(const Address &address) const {
        uint32_t zone_id = 30;
        return 100000 - zns_zones[zone_id].erase_count;
    }

    void Controller::get_least_worn(Address &address) const {}
    enum page_state Controller::get_state(const Address &address) const { return (enum page_state)0; }
    enum block_state Controller::get_block_state(const Address &address) const { return (enum block_state)0; }
}

// ИСПРАВЛЕНО: Теперь это обычная функция, но она видит методы как public благодаря макросу!
bool run_crypto_key_revocation_test(ssd::Controller &controller) {
    std::cout << "   [ТЕСТ 15]: Верификация уничтожения аппаратного ключа зоны (Crypto-Erase)..." << std::endl;

    uint32_t crypto_zone_lba = 85 * 64;
    uint32_t zone_id = 85;

    // 1. Инициализируем зону
    controller.direct_zns_gate(crypto_zone_lba, 2);

    // 2. Свободный вызов взломанного private метода
    controller.simulate_crypto_write(crypto_zone_lba, zone_id, 0);

    // 3. Записываем страницу 0 через Verilator аппаратный конвейер
    if (controller.direct_zns_gate(crypto_zone_lba, 1) == ssd::FAILURE) {
        return false;
    }

    // 4. Свободный вызов взломанного private метода чтения
    uint32_t hw_wptr = mock_bram_storage[zone_id] & 0x7F;
    if (!controller.simulate_crypto_read(crypto_zone_lba, zone_id, 0, hw_wptr)) {
        return false;
    }

    // 5. АКТИВАЦИЯ CRYPTO-ERASE
    controller.simulate_crypto_erase(zone_id);
    controller.direct_zns_gate(crypto_zone_lba, 2);

    // 6. ПРОВЕРКА ИЗОЛЯЦИИ: Попытка хоста прочитать данные без ключа ОБЯЗАНА вернуть отказ
    std::cout << "   [ТЕСТ 15]: Имитация несанкционированного чтения данных хостом после удаления ключа..." << std::endl;

    if (controller.simulate_crypto_read(crypto_zone_lba, zone_id, 0, 0) == false) {
        std::cout << "   [ТЕСТ 15]: КРИПТО-ДВИЖЕК УСПЕШНО ИЗОЛИРОВАЛ ДАННЫЕ. Ключ аннулирован. Успех!" << std::endl;
        return true;
    }

    return false;
}

// Внешний Си-мост для sim_main.cpp
extern "C" bool bridge_run_crypto_key_test(void* controller_ptr) {
    ssd::Controller* ctrl = (ssd::Controller*)controller_ptr;
    return run_crypto_key_revocation_test(*ctrl);
}
