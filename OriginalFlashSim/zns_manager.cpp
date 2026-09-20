// /home/ilya/TestFlashSim/OriginalFlashSim/zns_manager.cpp
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

// Реализация внутренней логики валидации ZNS операций с динамической геометрией
bool Controller::validate_and_advance_zns(Event &event)
{
    if (FTL_IMPLEMENTATION != 5) return true;

    // ИСПРАВЛЕНИЕ: Динамический размер зоны из конфигурации диска
    ulong hard_block_size = (BLOCK_SIZE > 0) ? BLOCK_SIZE : 64;
    ulong zone_id = event.get_logical_address() / hard_block_size;

    if (zns_zones == nullptr || zone_id >= total_zns_zones) {
        return false;
    }

    ZnsZone &zone = zns_zones[zone_id];

    if (event.get_event_type() == WRITE) {
        // ТЕСТ 9: Жесткая блокировка, если зона уже заполнена
        if (zone.is_full) {
            fprintf(stderr, "ZNS CRITICAL ERROR: Попытка записи в полностью заполненную зону %lu!\n", zone_id);
            return false;
        }

        // Вычисляем динамическое смещение внутри зоны
        uint target_page = event.get_logical_address() % hard_block_size;

        if (target_page != zone.wptr) {
            fprintf(stderr, "ZNS ERROR: Внеочередная запись в зону %lu (ожидался wptr: %lu, получен: %u)\n",
                    zone_id, zone.wptr, target_page);
            return false;
        }

        // Проверка лимита одновременно открытых зон (Zone Resources)
        if (zone.wptr == 0 && open_zones_count_check() >= 4) {
            fprintf(stderr, "ZNS RESOURCE ERROR: Превышен лимит одновременно открытых зон (Макс: 4)!\n");
            return false;
        }

        zone.wptr++;
        if (zone.wptr >= hard_block_size) {
            zone.is_full = true;
        }
    }
    return true;
}

ulong Controller::open_zones_count_check() {
    ulong count = 0;
    for (ulong i = 0; i < total_zns_zones; i++) {
        if (zns_zones[i].wptr > 0 && !zns_zones[i].is_full) count++;
    }
    return count;
}
