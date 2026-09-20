// /home/ilya/TestFlashSim/OriginalFlashSim/ssd_gc.cpp

#include <new>
#include <assert.h>
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

Garbage_collector::Garbage_collector(FtlParent &ftl)
{
    // Оставляем пустым, так как базовый симулятор не требует сохранения ссылки
}

Garbage_collector::~Garbage_collector(void)
{
    // Деструктор
}

void Garbage_collector::clean(Address &address)
{
    // Классическая фоновая сборка мусора не используется в режиме ZNS
}
