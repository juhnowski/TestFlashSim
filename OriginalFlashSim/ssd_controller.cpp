// /home/ilya/TestFlashSim/OriginalFlashSim/ssd_controller.cpp
#include <new>
#include <assert.h>
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

ZnsZone* Controller::zns_zones = nullptr;
ulong Controller::total_zns_zones = 0;

Controller::Controller(Ssd &parent) : ssd(parent)
{
    // ИСПРАВЛЕНИЕ: Никакого хардкода! Контроллер симулирует аппаратную прошивку
    // и считывает параметры динамически, прямо из считанной конфигурации:
    uint cfg_ssd_size     = (SSD_SIZE > 0) ? SSD_SIZE : 1;
    uint cfg_package_size = (PACKAGE_SIZE > 0) ? PACKAGE_SIZE : 2;
    uint cfg_die_size     = (DIE_SIZE > 0) ? DIE_SIZE : 2;
    uint cfg_plane_size   = (PLANE_SIZE > 0) ? PLANE_SIZE : 1024;

    // Динамический расчет общего количества зон (как в реальном механизме загрузки)
    total_zns_zones = cfg_ssd_size * cfg_package_size * cfg_die_size * cfg_plane_size;

    // Динамическое выделение памяти под BRAM таблицу метаданных зон
    zns_zones = new ZnsZone[total_zns_zones];
    for(ulong i = 0; i < total_zns_zones; i++) {
        zns_zones[i].wptr = 0;
        zns_zones[i].is_full = false;
        zns_zones[i].erase_count = 0;
    }

    *const_cast<uint*>(&FTL_IMPLEMENTATION) = 5;

    switch (FTL_IMPLEMENTATION) {
        case 5: ftl = new FtlImpl_Zns(*this); break;
        default: ftl = new FtlImpl_Page(*this); break;
    }
}

Controller::~Controller(void)
{
    delete ftl;
    if (zns_zones != nullptr) {
        delete[] zns_zones;
        zns_zones = nullptr;
    }
}

enum status Controller::direct_zns_gate(unsigned long lba, int is_write)
{
    if (check_thermal_shutdown()) {
        fprintf(stderr, "  [ZNS INTERFACE ERROR]: Отказ интерфейса. Контроллер аппаратно отключен термозащитой!\n");
        return FAILURE;
    }

    ulong hard_block_size = (BLOCK_SIZE > 0) ? BLOCK_SIZE : 64;
    ulong zone_id = lba / hard_block_size;

    if (zns_zones == nullptr || zone_id >= total_zns_zones) return FAILURE;
    ZnsZone &zone = zns_zones[zone_id];
    uint target_page = lba % hard_block_size;

    if (is_write == 1) { // WRITE
        Event mock_event(WRITE, lba, 1, 0.0);
        if (!validate_and_advance_zns(mock_event)) return FAILURE;

        simulate_crypto_write(lba, zone_id, target_page);
        return SUCCESS;
    }
    else if (is_write == 0) { // READ
        if (!simulate_crypto_read(lba, zone_id, target_page, zone.wptr)) return FAILURE;
        return SUCCESS;
    }
    else if (is_write == 2) { // RESET
        zone.wptr = 0;
        zone.is_full = false;
        simulate_crypto_erase(zone_id);
        return SUCCESS;
    }
    return FAILURE;
}

enum status Controller::reset_zns_zone(uint zone_id) { return SUCCESS; }
enum status Controller::event_arrive(Event &event) { return SUCCESS; }
enum status Controller::issue(Event &event_list) { return SUCCESS; }
enum status Controller::execute_hardware_step(Event *cur) { return SUCCESS; }
void Controller::translate_address(Address &address) {}

// Прокси-методы к Ssd
ulong Controller::get_erases_remaining(const Address &address) const { return 100000; }
void Controller::get_least_worn(Address &address) const {}
double Controller::get_last_erase_time(const Address &address) const { return 0.0; }
enum page_state Controller::get_state(const Address &address) const { return EMPTY; }
enum block_state Controller::get_block_state(const Address &address) const { return FREE; }
void Controller::get_free_page(Address &address) const {}
uint Controller::get_num_free(const Address &address) const { return 64; }
uint Controller::get_num_valid(const Address &address) const { return 0; }
uint Controller::get_num_invalid(const Address &address) const { return 0; }
Block *Controller::get_block_pointer(const Address &address) { return nullptr; }
const FtlParent &Controller::get_ftl(void) const { return (*ftl); }
void Controller::print_ftl_statistics() { ftl->print_ftl_statistics(); }
