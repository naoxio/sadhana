# cli_handler.py

import os
import sys
import subprocess
import git
import platform

from list_practices import get_practices

SADHANA_DIR = os.path.expanduser("~/.local/share/sadhana")
USER_CONFIG_DIR = os.path.expanduser("~/.config/sadhana")
REPO_URL = "https://github.com/naoxio/sadhana.git"

def install_sadhana_command():
    is_nixos = platform.system() == "Linux" and os.path.exists("/etc/nixos")
    
    install_type = input("Install system-wide or user-wide? (system/user): ").lower()

    if install_type == "system":
        if is_nixos:
            print("System-wide installation not supported on NixOS. Proceeding with user-wide installation.")
            target_dir = os.path.expanduser("~/bin")
        else:
            target_dir = "/usr/local/bin"
    else:
        target_dir = os.path.expanduser("~/bin")

    os.makedirs(target_dir, exist_ok=True)

    source_path = os.path.abspath(__file__)
    target_path = os.path.join(target_dir, "sadhana")

    try:
        os.symlink(source_path, target_path)
        print(f"Sadhana command installed in {target_path}")
    except PermissionError:
        print("Permission denied. Try running with sudo for system-wide installation.")
    except FileExistsError:
        print("Sadhana command is already installed.")
    except Exception as e:
        print(f"Error installing Sadhana command: {str(e)}")

def handle_cli_command(command, practice=None):
    if command == 'extra':
        print("Extra options:")
        print("1. Install Sadhana command")
        choice = input("Enter your choice (1 or q to quit): ")
        if choice == '1':
            install_sadhana_command()
        elif choice.lower() == 'q':
            print("Exiting extra options.")
        else:
            print("Invalid choice.")
    elif command == 'install':
        if os.path.exists(SADHANA_DIR) and os.path.exists(os.path.join(SADHANA_DIR, ".git")):
            print("Sadhana is already installed.")
        else:
            os.makedirs(SADHANA_DIR, exist_ok=True)
            git.Repo.clone_from(REPO_URL, SADHANA_DIR)
            print("Sadhana installed successfully.")
    elif command == 'uninstall':
        if os.path.exists(SADHANA_DIR):
            subprocess.run(["rm", "-rf", SADHANA_DIR])
        print("Sadhana uninstalled successfully.")
    elif command == 'list':
        practices = get_practices(SADHANA_DIR)
        print("Available practices:")
        for practice in practices:
            print(f"- {practice}")
    elif command == 'run':
        if not practice:
            print("Please specify a practice name to run.")
            sys.exit(1)
        print(f"Running practice: {practice}")
        # Here you would implement the logic to run the practice
    elif command == 'update':
        if not os.path.exists(SADHANA_DIR) or not os.path.exists(os.path.join(SADHANA_DIR, ".git")):
            os.makedirs(SADHANA_DIR, exist_ok=True)
            git.Repo.clone_from(REPO_URL, SADHANA_DIR)
        else:
            repo = git.Repo(SADHANA_DIR)
            repo.remotes.origin.pull()
        print("Practices updated successfully.")
    elif command == 'conf':
        if not practice:
            print("Please specify a practice to configure.")
            sys.exit(1)
        print(f"Configuring practice: {practice}")
        # Here you would implement the logic to configure the practice