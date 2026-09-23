// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_server.h
#ifndef ZNS_NBD_SERVER_H
#define ZNS_NBD_SERVER_H

#include <cstdint>
#include "Vzns_fsm_validator.h"

// Константы геометрии ZNS
constexpr uint32_t NUM_ZONES = 1024;
constexpr uint32_t ZONE_SIZE_PAGES = 64;
constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t BLOCK_SIZE = 512;
constexpr uint64_t TOTAL_SIZE_BYTES = (uint64_t)NUM_ZONES * ZONE_SIZE_PAGES * PAGE_SIZE;

// Запуск бесконечного цикла прослушивания порта и обработки команд ядра
void start_nbd_server(Vzns_fsm_validator* top, int port = 10809);

#endif // ZNS_NBD_SERVER_H
