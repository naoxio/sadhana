# Sadhana CLI Tool
The Sadhana CLI tool provides easy access to mind-body-spirit practices from various traditions and disciplines.

## Installation
To install the Sadhana CLI tool:

1. Clone the repository:
```git clone https://github.com/naoxio/sadhana.git ~/.local/share/sadhana```

2. Navigate to the CLI directory:
```cd ~/.local/share/sadhana/cli```

3. Run the installation command:
```./sadhana install```
This will set up the Sadhana directory, copy necessary scripts, and create a symlink in `/usr/local/bin`.

## Usage
After installation, you can use the following commands:
- List all practices:
```sadhana list```

- Run a specific practice (by number or name):
```sadhana run "Practice Name"```
or
```sadhana run 1```

- Update practices:
```sadhana update```

- Reinitialize Sadhana:
```sadhana init```

## Uninstallation

To uninstall the Sadhana CLI tool:
```sadhana uninstall```
This will remove the Sadhana directory and the symlink from `/usr/local/bin`.

## Troubleshooting

If you encounter any issues, please ensure that:

1. You have the necessary permissions to create directories in `~/.local/share/` and create symlinks in `/usr/local/bin`.
2. Lua is installed on your system.
3. The required Lua modules (lfs and yaml) are available.
