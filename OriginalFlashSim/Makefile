# /home/ilya/TestFlashSim/OriginalFlashSim/Makefile

# ====================================================================
# Настройки компиляции и путей Verilator
# ====================================================================
VERILATOR = verilator
VERILATOR_FLAGS = -Wall --cc --trace --exe --build

RTL_SOURCES = zns_fsm_validator.v zns_rules_checker.v
TOP_MODULE = zns_fsm_validator
OBJ_DIR = obj_dir

CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17

# Динамически извлекаем внутренний корневой путь рантайма Verilator под NixOS
VERILATOR_ROOT = $(shell verilator --getenv VERILATOR_ROOT)

# Формируем корректные флаги инклудов для GCC
VERILATOR_INCLUDE = -I$(OBJ_DIR) \
                    -I$(VERILATOR_ROOT)/include \
                    -I$(VERILATOR_ROOT)/include/vltstd

# ====================================================================
# Модульные правила линковки и сборки сервера
# ====================================================================

.PHONY: nbd-server clean-nbd

# 1. Главный таргет для сборки декомпозированного сервера (без flashsim_stubs.o)
nbd-server: $(OBJ_DIR)/V$(TOP_MODULE).h hardware_bridge.o nbd_negotiation.o nbd_server.o sim_main.o
	@echo "📦 Линковка чистого автономного Verilator ZNS NBD сервера..."
	$(CXX) $(CXXFLAGS) \
		sim_main.o \
		nbd_negotiation.o \
		nbd_server.o \
		hardware_bridge.o \
		$(OBJ_DIR)/V$(TOP_MODULE)__ALL.a \
		$(VERILATOR_ROOT)/include/verilated.cpp \
		$(VERILATOR_ROOT)/include/verilated_threads.cpp \
		$(VERILATOR_ROOT)/include/verilated_vcd_c.cpp \
		-o zns_nbd_server
	@echo "🔥 Триумф! Модульный бинарник 'zns_nbd_server' успешно собран."

# 2. Правило для генерации С++ кода из исходных Verilog-файлов
$(OBJ_DIR)/V$(TOP_MODULE).h: $(RTL_SOURCES)
	@echo "⚙️  Трансляция RTL схем через Verilator..."
	$(VERILATOR) $(VERILATOR_FLAGS) --top-module $(TOP_MODULE) $(RTL_SOURCES)

# 3. Компиляция аппаратного моста (mock_bram_storage)
hardware_bridge.o: hardware_bridge.cpp
	@echo "🛠️  Компиляция аппаратного моста..."
	$(CXX) $(CXXFLAGS) $(VERILATOR_INCLUDE) -c hardware_bridge.cpp -o hardware_bridge.o

# 4. Компиляция изолированного модуля хендшейка Newstyle
nbd_negotiation.o: nbd_negotiation.cpp nbd_protocol.h
	@echo "🛠️  Компиляция модуля хендшейка nbd_negotiation.cpp..."
	$(CXX) $(CXXFLAGS) $(VERILATOR_INCLUDE) -c nbd_negotiation.cpp -o nbd_negotiation.o

# 5. Компиляция модуля трансмиссии I/O команд (Read/Write/Trim)
nbd_server.o: nbd_server.cpp nbd_server.h nbd_protocol.h
	@echo "🛠️  Компиляция модуля трансмиссии nbd_server.cpp..."
	$(CXX) $(CXXFLAGS) $(VERILATOR_INCLUDE) -c nbd_server.cpp -o nbd_server.o

# 6. Компиляция оркестратора симуляции и точки входа
sim_main.o: sim_main.cpp nbd_server.h
	@echo "🛠️  Компиляция точки входа sim_main.cpp..."
	$(CXX) $(CXXFLAGS) $(VERILATOR_INCLUDE) -c sim_main.cpp -o sim_main.o

# Очистка всех артефактов декомпозированной сборки NBD-сервера
clean-nbd:
	rm -rf sim_main.o nbd_negotiation.o nbd_server.o hardware_bridge.o zns_nbd_server
