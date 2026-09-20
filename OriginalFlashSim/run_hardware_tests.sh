#!/usr/bin/env bash

# /home/ilya/TestFlashSim/OriginalFlashSim/run_hardware_tests.sh

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== 1. ЗАПУСК ГЕНЕРАТОРА СТРУКТУРЫ LITEX/MIGEN ===${NC}"
if [ -f zns_controller.py ]; then
    python3 zns_controller.py
    echo -e "${GREEN}[LITEX OK]: Топология портов успешно построена.${NC}"
else
    echo -e "${RED}[ОШИБКА]: Файл zns_controller.py не найден!${NC}"
    exit 1
fi

echo -e "\n${GREEN}=== 2. СВЯЗЫВАНИЕ И КОМПИЛЯЦИЯ RTL КОДА (ICARUS VERILOG) ===${NC}"
# Компилируем аппаратную память, резолвер, валидатор и тестбенч
iverilog -o zns_hardware_sim aes_round.v zns_crypto_engine.v zns_metadata_bram.v zns_address_resolver.v zns_fsm_validator.v tb_zns_controller.v
echo -e "${GREEN}[IVERILOG OK]: Все RTL-модели успешно скомпилированы.${NC}"

echo -e "\n${GREEN}=== 3. ЗАПУСК АППАРАТНОЙ RTL СИМУЛЯЦИИ КОНТРОЛЛЕРА ===${NC}"
vvp zns_hardware_sim

echo -e "\n${GREEN}🎉 [УСПЕХ]: Сквозная верификация RTL-автомата ZNS завершена!${NC}"

rm -f zns_hardware_sim
