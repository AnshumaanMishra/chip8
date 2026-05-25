{
  description = "CHIP-8 Emulator - Vanilla Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          cmake
          ninja
          clang
          clang-tools
          sdl3
          sdl3-mixer
          lcov
          gtest
        ];

        shellHook = ''
          export CC=clang
          export CXX=clang++
          export CXXFLAGS="-isystem ${pkgs.libcxx}/include/c++/v1"
          export CFLAGS="-isystem ${pkgs.libcxx}/include/c++/v1"

          # Make clang-tidy use the wrapper's include paths
          export CLANG_TIDY_FLAGS="--extra-arg=-isystem${pkgs.libcxx}/include/c++/v1"
          exec fish
        '';
      };
    };
}
