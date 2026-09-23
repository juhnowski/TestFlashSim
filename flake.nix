# /home/ilya/TestFlashSim/flake.nix
{
  description = "Воспроизводимая среда разработки для FlashSim (C++), LiteX (Python), RTL-эмуляции ZNS и Host-FTL (Rust)";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          # Инструменты сборки C++
          gcc
          gdb
          gnumake
          cmake
          git
          boost

          # Окружение для Rust (Host-FTL с tokio-uring)
          rustc      # Компилятор Rust
          cargo      # Менеджер пакетов и сборщик
          pkg-config # Поиск системных библиотек для крейтов
          liburing   # Нативная библиотека io_uring для хоста

          # Окружение для LiteX и аппаратной симуляции Verilog:
          (python3.withPackages (ps: with ps; [
            setuptools
            pip
          ]))
          iverilog
          verilator

          # Инструменты для работы с ZNS, NBD и виртуализацией:
          qemu
          qemu-utils
          nvme-cli
          wget
          nbd
          linuxHeaders
        ];

        shellHook = ''
          export LANG=ru_RU.UTF-8

          # Настройка путей для GCC, чтобы он видел заголовки ядра и io_uring
          export C_INCLUDE_PATH="${pkgs.linuxHeaders}/include:${pkgs.liburing.dev}/include:$C_INCLUDE_PATH"
          export CPLUS_INCLUDE_PATH="${pkgs.linuxHeaders}/include:${pkgs.liburing.dev}/include:$CPLUS_INCLUDE_PATH"
          export LIBRARY_PATH="${pkgs.liburing}/lib:$LIBRARY_PATH"

          # Исключаем texinfo из путей сборки для минимизации оверхеда окружения
          export NIX_DISABLE_TEXINFO=1

          # Автоматически создаем и активируем виртуальное окружение во временной папке проекта,
          # чтобы litex и migen ставились изолированно
          if [ ! -d ".venv" ]; then
            echo "📦 Создание виртуального окружения Python .venv..."
            python3 -m venv .venv
          fi
          source .venv/bin/activate

          echo ""
          echo "======================================================="
          echo "🚀 Добро пожаловать во флейк-окружение для FlashSim, LiteX, ZNS & Rust!"
          echo "Доступны: rustc, cargo, g++, make, python3, pip, verilator, nbd-client."
          echo "======================================================="

          # Проверяем, загружен ли ядерный модуль NBD на хосте
          if ! lsmod | grep -q "^nbd"; then
            echo "⚠️  Внимание: Ядерный модуль 'nbd' не загружен на хосте!"
            echo "Выполните на хосте: sudo modprobe nbd nbds_max=2"
          else
            echo "✅ Модуль ядра NBD активен. Можно мапить симуляцию в /dev/nbd0."
          fi
          echo "======================================================="
          echo ""

          export PS1="[flashsim-zns-flake:\w]\$ "
        '';
      };
    };
}
