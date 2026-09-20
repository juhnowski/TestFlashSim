// /home/ilya/TestFlashSim/OriginalFlashSim/FTLs/zns_ftl.cpp

#include <stdio.h>
#include "../ssd.h"

using namespace ssd;

FtlImpl_Zns::FtlImpl_Zns(Controller &controller) : FtlParent(controller)
{
    printf("ZNS FTL: Модуль успешно инициализирован. Трансляция LBA->Zone включена.\n");
}

FtlImpl_Zns::~FtlImpl_Zns(void)
{
    // Деструктор
}

enum status FtlImpl_Zns::read(Event &event)
{
    uint lba = event.get_logical_address();

    // ЗАЩИТА ОТ ДЕЛЕНИЯ НА 0: если BLOCK_SIZE равен 0, принудительно берем 64
    uint hard_block_size = (BLOCK_SIZE > 0) ? BLOCK_SIZE : 64;

    uint zone_id = lba / hard_block_size;
    uint page_offset = lba % hard_block_size;

    Address phyAddress;
    phyAddress.package = 0;
    phyAddress.die = 0;
    phyAddress.plane = 0;
    phyAddress.block = zone_id;
    phyAddress.page = page_offset;
    phyAddress.valid = PAGE;

    event.set_address(phyAddress);
    return FtlParent::controller.issue(event);
}

enum status FtlImpl_Zns::write(Event &event)
{
    uint lba = event.get_logical_address();

    // ЗАЩИТА ОТ ДЕЛЕНИЯ НА 0
    uint hard_block_size = (BLOCK_SIZE > 0) ? BLOCK_SIZE : 64;

    uint zone_id = lba / hard_block_size;
    uint page_offset = lba % hard_block_size;

    Address phyAddress;
    phyAddress.package = 0;
    phyAddress.die = 0;
    phyAddress.plane = 0;
    phyAddress.block = zone_id;
    phyAddress.page = page_offset;
    phyAddress.valid = PAGE;

    event.set_address(phyAddress);
    return FtlParent::controller.issue(event);
}

enum status FtlImpl_Zns::trim(Event &event)
{
    uint lba = event.get_logical_address();

    // ЗАЩИТА ОТ ДЕЛЕНИЯ НА 0
    uint hard_block_size = (BLOCK_SIZE > 0) ? BLOCK_SIZE : 64;
    uint zone_id = lba / hard_block_size;

    return FtlParent::controller.reset_zns_zone(zone_id);
}
