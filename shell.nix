{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
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
}
