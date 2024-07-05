{
  description = "Sadhana - A CLI tool for managing Sadhana practices";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "sadhana";
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [ pkg-config ];
          buildInputs = with pkgs; [ tinycc termbox ];

          buildPhase = ''
            mkdir -p bin
            tcc -Wall -Wextra -pedantic -std=c99 \
                -I${pkgs.termbox}/include \
                -L${pkgs.termbox}/lib \
                -o bin/sadhana \
                src/*.c \
                -ltermbox
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp bin/sadhana $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "A CLI tool for managing Sadhana practices";
            homepage = "https://github.com/yourusername/sadhana";
            license = licenses.mit;  # Adjust as needed
            platforms = platforms.all;
          };
        };

        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            tinycc
            pkg-config
            termbox
          ];

          shellHook = ''
            export PKG_CONFIG_PATH="${pkgs.termbox}/lib/pkgconfig:$PKG_CONFIG_PATH"
            export C_INCLUDE_PATH="${pkgs.termbox}/include:$C_INCLUDE_PATH"
            export LIBRARY_PATH="${pkgs.termbox}/lib:$LIBRARY_PATH"
            export LD_LIBRARY_PATH="${pkgs.termbox}/lib:$LD_LIBRARY_PATH"
          '';
        };
      }
    );
}
