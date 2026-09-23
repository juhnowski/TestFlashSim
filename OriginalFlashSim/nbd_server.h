// /home/ilya/TestFlashSim/OriginalFlashSim/nbd_server.h
#ifndef ZNS_NBD_SERVER_H
#define ZNS_NBD_SERVER_H

#include <cstdint>

// Оригинальная геометрия FlashSim (1024 зоны по 64 страницы = 256 МБ)
constexpr uint32_t NUM_ZONES = 1024;
constexpr uint32_t ZONE_SIZE_PAGES = 64;
constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t BLOCK_SIZE = 512;
constexpr uint64_t TOTAL_SIZE_BYTES = (uint64_t)NUM_ZONES * ZONE_SIZE_PAGES * PAGE_SIZE;

#endif // ZNS_NBD_SERVER_H
