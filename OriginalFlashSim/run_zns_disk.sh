#!/usr/bin/env bash
# /home/ilya/TestFlashSim/OriginalFlashSim/run_zns_disk.sh

set -euo pipefail

# Константы окружения
SERVER_BIN="./zns_nbd_server"
PORT=10809
NBD_DEV="/dev/nbd0"

echo "======================================================="
echo "⚙️  Инициализация Verilator ZNS блочного устройства..."
echo "======================================================="

# 1. Проверка наличия бинарника сервера
if [ ! -f "$SERVER_BIN" ]; then
    echo "❌ Ошибка: Сервер '$SERVER_BIN' не найден. Сначала выполните: make nbd-server"
    exit 1
fi

# 2. Проверка и автоматическая загрузка модуля ядра nbd
if ! lsmod | grep -q "^nbd"; then
    echo "🔄 Модуль ядра 'nbd' не загружен. Пробуем подгрузить..."
    if ! sudo modprobe nbd nbds_max=2; then
        echo "❌ Ошибка: Не удалось загрузить модуль ядра nbd."
        echo "Убедитесь, что вы находитесь на хосте NixOS и у вас есть права sudo."
        exit 1
    fi
    echo "✅ Модуль nbd успешно загружен."
fi

# 3. Очистка старых зависших подключений
if [ -b "$NBD_DEV" ] && sudo nbd-client -d "$NBD_DEV" &>/dev/null; then
    echo "🧹 Очищено предыдущее зависшее подключение на $NBD_DEV"
fi

# 4. Запуск Verilator NBD-сервера в фоновом режиме
echo "🚀 Запуск Verilator-сервера на порту $PORT..."
$SERVER_BIN &
SERVER_PID=$!

# Даем серверу секунду на инициализацию сокета и сброс RTL-схемы
sleep 1

# Проверяем, что сервер не упал сразу после старта
if ! kill -0 "$SERVER_PID" &>/dev/null; then
    echo "❌ Ошибка: Сервер упал при старте. Проверьте логи сокетов."
    exit 1
fi

# Ловушка (trap): если скрипт прервут или он упадет дальше, гасим фоновый сервер
trap 'echo "🛑 Завершение фонового сервера..."; kill "$SERVER_PID" 2>/dev/null || true' EXIT

# 5. Подключение блочного устройства ядра к нашему серверу
echo "🔗 Привязка сокета к системному блочному устройству $NBD_DEV..."
if ! sudo nbd-client 127.0.0.1 $PORT "$NBD_DEV" -b 512; then
    echo "❌ Ошибка: nbd-client не смог подключить устройство."
    exit 1
fi

echo "======================================================="
echo "✅ Успех! Схема работает как реальный диск в NixOS."
echo "Устройство доступно: $NBD_DEV"
echo "Для проверки геометрии выполните: sudo nvme zns report-zones $NBD_DEV"
echo "Чтобы отключить диск, выполните: sudo nbd-client -d $NBD_DEV"
echo "======================================================="

# Удерживаем скрипт активным, пока работает сервер
wait "$SERVER_PID"
