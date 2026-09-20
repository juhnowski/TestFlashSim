# ~/TestFlashSim/OriginalFlashSim/flake.nix
{
  description = "Воспроизводимая среда разработки для FlashSim (C++) и эмуляции ZNS";

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

          # Инструменты для работы с ZNS и виртуализацией:
          qemu       # Эмулятор для запуска ядра с поддержкой ZNS
          qemu-utils # Утилита qemu-img для создания образов дисков
          nvme-cli   # Утилита управления NVMe накопителями
          wget       # Для скачивания Live-CD образов ядра
        ];

        shellHook = ''
          export LANG=ru_RU.UTF-8
          echo ""
          echo "======================================================="
          echo "🚀 Добро пожаловать во флейк-окружение для FlashSim & ZNS!"
          echo "Доступны: g++, make, boost, qemu, nvme-cli."
          echo "Сборка FlashSim:   make test"
          echo "Создать ZNS образ:  qemu-img create -f raw /tmp/zns.img 10G"
          echo "======================================================="
          echo ""

          export PS1="[flashsim-zns-flake:\w]\$ "
        '';
      };
    };
}
