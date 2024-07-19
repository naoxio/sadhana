{ pkgs, lib, config, ... }:
let
  # Define the binary directory
  binDir = "bin";
in
{
  packages = [
    pkgs.nim
    pkgs.nimble
    pkgs.pcre
    pkgs.termbox
    pkgs.libzip
  ];
  languages.nim.enable = true;
  scripts = {
    build.exec = ''
      mkdir -p ${binDir}
      nim c -d:release -o:${binDir}/sadhana src/sadhana.nim
    '';
    build-cli.exec = ''
      mkdir -p ${binDir}
      nim c -d:release -o:${binDir}/sadhana-cli src/cli/cli_main.nim
    '';
    build-tui.exec = ''
      mkdir -p ${binDir}
      nim c -d:release -o:${binDir}/sadhana-tui src/tui/tui_main.nim
    '';
    build-gui.exec = ''
      mkdir -p ${binDir}
      nim c -d:release -o:${binDir}/sadhana-gui src/gui/gui_main.nim
    '';
    clean.exec = ''
      rm -rf ${binDir}
      find . -name "*.nim.o" -type f -delete
      find . -name "nimcache" -type d -exec rm -rf {} +
    '';
    sadhana.exec = ''
      ${binDir}/sadhana "$@"
    '';
    sadhana-cli.exec = ''
      ${binDir}/sadhana-cli "$@"
    '';
    sadhana-tui.exec = ''
      ${binDir}/sadhana-tui "$@"
    '';
    sadhana-gui.exec = ''
      ${binDir}/sadhana-gui "$@"
    '';
  };
  env = {
    NIMBLE_DIR = "${config.env.DEVENV_ROOT}/.nimble";
  };
  enterShell = ''
    echo "Nim development environment ready!"
    echo "Available commands:"
    echo "  build : Build all sadhana binaries"
    echo "  build-cli : Build sadhana CLI binary"
    echo "  build-tui : Build sadhana TUI binary"
    echo "  build-gui : Build sadhana GUI binary"
    echo "  clean : Remove built binaries and Nim cache files"
    echo "  sadhana [args...] : Run the main sadhana binary with optional arguments"
    echo "  sadhana-cli [args...] : Run the sadhana CLI binary with optional arguments"
    echo "  sadhana-tui [args...] : Run the sadhana TUI binary with optional arguments"
    echo "  sadhana-gui [args...] : Run the sadhana GUI binary with optional arguments"

    # Initialize Nimbnim c -d:release -d:nimDebugDlOpen -o:sadhana src/sadhana.nim
le
    nimble refresh

    # Function to check if a package is installed
    is_package_installed() {
      nimble list -i | grep -q "^$1 "
    }

    # Install required packages if not already installed
    for package in zip nimbox iup; do
      if ! is_package_installed $package; then
        echo "Installing $package..."
        nimble install -y $package
      else
        echo "$package is already installed."
      fi
    done
  '';
}