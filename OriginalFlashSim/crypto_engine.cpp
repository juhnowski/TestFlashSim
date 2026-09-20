// /home/ilya/TestFlashSim/OriginalFlashSim/crypto_engine.cpp
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

// Имитация работы аппаратного крипто-блока контроллера
void Controller::simulate_crypto_write(unsigned long lba, ulong zone_id, uint target_page) {
    printf("  [CRYPTO-ENGINE]: Данные LBA %lu зашифрованы аппаратным ключом AES-XTS-256.\n", lba);
    printf("  [ZNS GATE]: Запись зашифрованного блока в зону %lu (страница %u) — УСПЕШНО.\n", zone_id, target_page);
}

// Исправлено: wptr имеет тип uint для точного соответствия спецификаторам формата %u
bool Controller::simulate_crypto_read(unsigned long lba, ulong zone_id, uint target_page, uint wptr) {
    if (target_page >= wptr) {
        fprintf(stderr, "ZNS READ ERROR: Попытка чтения нераспределенных данных в зоне %lu (wptr: %u, запрос к странице: %u)\n",
                zone_id, wptr, target_page);
        return false;
    }
    printf("  [CRYPTO-ENGINE]: Проверка подписи целостности данных для LBA %lu — OK.\n", lba);
    printf("  [CRYPTO-ENGINE]: Данные успешно расшифрованы на стороне контроллера.\n");
    printf("  [ZNS GATE]: Чтение LBA %lu из зоны %lu (страница %u) — УСПЕШНО.\n", lba, zone_id, target_page);
    return true;
}

void Controller::simulate_crypto_erase(ulong zone_id) {
    ZnsZone &zone = zns_zones[zone_id];
    zone.erase_count++;

    printf("  [CRYPTO-ENGINE]: Ключ шифрования зоны %lu уничтожен. Сброс метаданных.\n", zone_id);
    printf("  [ZNS WEAR-LEVEL]: Зона %lu была стёрта %lu раз(а).\n", zone_id, zone.erase_count);

    // Имитируем лимит износа (ресурс в 5 стираний для демонстрационных тестов)
    if (zone.erase_count >= 5) {
        fprintf(stderr, "  [ZNS WEAR-LEVEL CRITICAL]: Внимание! Зона %lu исчерпала аппаратный ресурс износа ячеек памяти!\n", zone_id);
    }
}
