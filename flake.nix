{
  description = "RSIBreak devShell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          clang-tools
          qt6.wrapQtAppsHook 
          kdePackages.extra-cmake-modules
          wayland-protocols
          wayland
        ];

        buildInputs = with pkgs; [
          wayland
        ] ++ (with pkgs.kdePackages; [
          qtbase
          kcoreaddons
          kconfig
          kcrash
          kdbusaddons
          kdoctools
          ki18n
          kidletime
          knotifications
          knotifyconfig
          ktextwidgets
          kxmlgui
          kwindowsystem
          kstatusnotifieritem
          kcolorscheme
        ]);

        shellHook = ''
          export CMAKE_EXPORT_COMPILE_COMMANDS=ON
        '';
      };
    };
}
