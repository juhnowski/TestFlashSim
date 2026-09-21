# /home/ilya/TestFlashSim/OriginalFlashSim/Makefile

.PHONY: all co-sim clean help

all: co-sim

help:
	@echo "Доступные команды для верификации ZNS-контроллера:"
	@echo "  make co-sim  - Скомпилировать RTL и C++ мосты, запустить сюиту из 17 тестов"
	@echo "  make clean   - Удалить временные объектные файлы и папку сборки Verilator"

co-sim: clean
	@echo "======================================================="
	@echo "🛠️  ЗАПУСК АВТОМАТИЧЕСКОЙ СБОРКИ CO-SIMULATION КОНВЕЙЕРА"
	@echo "======================================================="

	# 1. Трансляция Verilog-схемы и генерация С++ обвязки через Verilator
	verilator -Wall --cc zns_fsm_validator.v \
	          --exe sim_main.cpp hardware_bridge.cpp flashsim_stubs.cpp zns_driver.c \
	          ssd_ssd.cpp ssd_config.cpp ssd_controller.cpp thermal_manager.cpp \
	          crypto_engine.cpp eeprom_manager.cpp zns_manager.cpp ssd_address.cpp \
	          ssd_block.cpp ssd_bm.cpp ssd_bus.cpp ssd_channel.cpp ssd_die.cpp \
	          ssd_event.cpp ssd_gc.cpp ssd_package.cpp ssd_page.cpp ssd_plane.cpp \
	          ssd_raidssd.cpp ssd_ram.cpp ssd_stats.cpp ssd_wl.cpp ssd_ftlparent.cpp \
	          tests/test_*.cpp -I.

	@echo "======================================================="
	@echo "📦  КОМПИЛЯЦИЯ И ЛИНКОВКА ИСПОЛНЯЕМОГО МОНОЛИТА G++"
	@echo "======================================================="

	# 2. Нативная сборка бинарника внутри сгенерированной папки
	cd obj_dir && $(MAKE) -j -f Vzns_fsm_validator.mk Vzns_fsm_validator

	@echo "======================================================="
	@echo "🚀  ЗАПУСК СКВОЗНОЙ ВЕРИФИКАЦИИ КРЕМНИЯ (ТЕСТЫ 1-17)"
	@echo "======================================================="

	# 3. Выполнение тестов из корня проекта, где лежит конфигурационный ssd.conf
	./obj_dir/Vzns_fsm_validator

clean:
	@echo "🧹 Очистка временных файлов и кэша тестов..."
	@rm -rf obj_dir
	@rm -f tests/*.o
	@rm -f eeprom_diagnostic.bin
	@echo "✨ Очистка завершена."
