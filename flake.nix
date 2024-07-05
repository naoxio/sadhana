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
        pkgsCross = pkgs.pkgsCross.mingwW64;

        buildSadhana = { pkgs, static ? false }: pkgs.stdenv.mkDerivation {
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
                -ltermbox \
                ${if static then "-static" else ""}
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp bin/sadhana $out/bin/
          '';
          meta = with pkgs.lib; {
            description = "A CLI tool for managing Sadhana practices";
            homepage = "https://github.com/naoxio/sadhana";
            license = licenses.mit;
            platforms = platforms.all;
          };
        };
      in
      {
        packages = {
          default = buildSadhana { inherit pkgs; };
          static = buildSadhana { inherit pkgs; static = true; };
          windows = buildSadhana { pkgs = pkgsCross; static = true; };
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
    nixConfig = {
      extra-substituters = [ "https://naoxio.cachix.org" ];
      extra-trusted-public-keys = [ "naoxio.cachix.org-1:9+TVHBawUiQoy+GaxQOQzLYSfGnPdkjQqElBmGat2qo=" ];
    };
}
