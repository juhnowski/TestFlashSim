// /home/ilya/TestFlashSim/OriginalFlashSim/ssd_ftl.cpp

#include <new>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "ssd.h"

using namespace ssd;

// ============================================================================
// Реализация базового класса Ftl (Базовые прокси-методы симулятора)
// ============================================================================

Ftl::Ftl(Controller &controller):
	controller(controller),
	garbage(*this),
	wear(*this)
{
	currentPage = 0;
	uint numCells = SSD_SIZE * PACKAGE_SIZE * DIE_SIZE * PLANE_SIZE * BLOCK_SIZE;
	map = new long[numCells];
	for (int i=0;i<numCells;i++)
		map[i] = -1;
	return;
}

Ftl::~Ftl(void)
{
	delete[] map; // Исправлено: delete[] для массивов
	return;
}

enum status Ftl::read(Event &event)
{
	if (map[event.get_logical_address()] == -1)
	{
		fprintf(stderr, "Page not written! Logical Address: %i\n", event.get_logical_address());
		return FAILURE;
	}

	event.set_address(resolve_logical_address(map[event.get_logical_address()]));
	page_state s = controller.get_state(event.get_address());

	if (s == VALID)
	{
		controller.issue(event);
	}
	else
	{
		fprintf(stderr, "Page warning: Not able to read page as it has not been written or is invalid.");
		return FAILURE;
	}
	return SUCCESS;
}

enum status Ftl::write(Event &event)
{
	Address address = resolve_logical_address(currentPage);
	event.set_address(address);
	map[event.get_logical_address()] = currentPage;
	currentPage++;

	controller.issue(event);
	return SUCCESS;
}

inline Address Ftl::resolve_logical_address(uint logicalAddress)
{
	uint numCells = SSD_SIZE * PACKAGE_SIZE * DIE_SIZE * PLANE_SIZE * BLOCK_SIZE;

	Address phyAddress;
	phyAddress.package = floor(logicalAddress / (numCells / SSD_SIZE));
	phyAddress.die = floor(logicalAddress / (numCells / SSD_SIZE / PACKAGE_SIZE));
	phyAddress.plane = floor(logicalAddress / (numCells / SSD_SIZE / PACKAGE_SIZE / DIE_SIZE));
	phyAddress.block = floor(logicalAddress / (numCells / SSD_SIZE / PACKAGE_SIZE / DIE_SIZE / PLANE_SIZE));
	phyAddress.page = logicalAddress % BLOCK_SIZE;
	phyAddress.valid = PAGE;

	return phyAddress;
}

enum status Ftl::erase(Event &event) { return SUCCESS; }
enum status Ftl::merge(Event &event) { return SUCCESS; }
void Ftl::garbage_collect(Event &event) { (void) garbage.clean(event.get_address()); }
ulong Ftl::get_erases_remaining(const Address &address) const { return controller.get_erases_remaining(address); }
void Ftl::get_least_worn(Address &address) const { controller.get_least_worn(address); return; }
enum page_state Ftl::get_state(const Address &address) const { return controller.get_state(address); }
