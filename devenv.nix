{ pkgs, lib, config, ... }:
let
  # Define the binary directory
  binDir = "bin";
in
{
  packages = [
    pkgs.clang
    pkgs.curl
    pkgs.termbox
    pkgs.pkg-config
  ];
  languages.c.enable = true;
  scripts = {
    build.exec = ''
      mkdir -p ${binDir}
      clang $CFLAGS $INCLUDES -o ${binDir}/sadhana src/*.c $LIBS
    '';
    clean.exec = ''
      rm -rf ${binDir}
      rm -f src/*.o
    '';
    run.exec = ''
      ${binDir}/sadhana "$@"
    '';
  };
  env = {
    CFLAGS = "-Wall -Wextra -pedantic -std=c11";
    INCLUDES = builtins.concatStringsSep " " [
      "-I${pkgs.termbox}/include"
      "-I${pkgs.curl.dev}/include"
    ];
    LIBS = builtins.concatStringsSep " " [
      "-L${pkgs.termbox}/lib"
      "-L${pkgs.curl.out}/lib"
      "-ltermbox"
      "-lcurl"
    ];
  };
  enterShell = ''
    echo "C development environment ready!"
    echo "Available commands:"
    echo "  build : Build sadhana binary"
    echo "  clean : Remove built binaries and object files"
    echo "  run [args...] : Run the sadhana binary with optional arguments"
  '';
}
