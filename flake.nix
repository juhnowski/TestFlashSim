{
  description = "Воспроизводимая среда разработки для FlashSim (C++)";

  inputs = {
    # Используем стабильный срез пакетов nixpkgs
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      # Указываем архитектуру вашего процессора (для ПК это x86_64-linux)
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        # Пакеты, которые будут доступны внутри среды разработки
        buildInputs = with pkgs; [
          gcc        # Компилятор GNU C/C++ (включая g++)
          gnumake    # Утилита Make для запуска файлов Makefile
          cmake      # Дополнительный генератор сборок, если потребуется
          git        # Утилита управления версиями
	  boost
        ];

        # Скрипт, который автоматически выполнится при входе в nix develop
        shellHook = ''
          export LANG=ru_RU.UTF-8
          echo ""
          echo "======================================================="
          echo "🚀 Добро пожаловать во флейк-окружение для FlashSim!"
          echo "Доступны: g++, make, git."
          echo "Для сборки симулятора введите: make test"
          echo "======================================================="
          echo ""

          # Красивый кастомный prompt, чтобы вы видели, что находитесь внутри флейка
          export PS1="[flashsim-flake:\w]\$ "
        '';
      };
    };
}
