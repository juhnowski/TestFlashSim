# /home/ilya/TestFlashSim/flake.nix
{
  description = "Воспроизводимая среда разработки для FlashSim (C++), LiteX (Python) и RTL-эмуляции ZNS";

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
          gcc        # Компилятор GNU C/C++
          gnumake    # Утилита Make
          cmake      # Генератор сборок
          git        # Управление версиями
          boost      # Библиотека Boost для FlashSim

          # Окружение для LiteX и аппаратной симуляции Verilog:
          (python3.withPackages (ps: with ps; [
            setuptools
            pip        # ДОБАВЛЕНО: Теперь утилита pip гарантированно будет в PATH
          ]))
          iverilog   # Компилятор Icarus Verilog (содержит iverilog и vvp)

          # Инструменты для работы с ZNS и виртуализацией:
          qemu       # Эмулятор для запуска ядра с поддержкой ZNS
          qemu-utils # Утилита qemu-img для создания образов дисков
          nvme-cli   # Утилита управления NVMe накопителями
          wget       # Для скачивания Live-CD образов ядра
        ];

        shellHook = ''
          export LANG=ru_RU.UTF-8

          # Автоматически создаем и активируем виртуальное окружение во временной папке проекта,
          # чтобы litex и migen ставились изолированно
          if [ ! -d ".venv" ]; then
            echo "📦 Создание виртуального окружения Python .venv..."
            python3 -m venv .venv
          fi
          source .venv/bin/activate

          echo ""
          echo "======================================================="
          echo "🚀 Добро пожаловать во флейк-окружение для FlashSim, LiteX & ZNS!"
          echo "Доступны: g++, make, boost, python3, pip, iverilog, vvp, qemu, nvme-cli."
          echo "Перед первым запуском выполните: pip install migen litex"
          echo "Сборка C++ FlashSim:    make test"
          echo "Запуск RTL верификации: ./run_hardware_tests.sh"
          echo "======================================================="
          echo ""

          export PS1="[flashsim-zns-flake:\w]\$ "
        '';
      };
    };
}
