# Windows-Keymap

A small Windows utility that allows you to use the Caps Lock key to simulate a left mouse click, helping reduce strain on the right wrist. It provides simple command-line options to start, stop, and manage the behavior of `km_main.exe`.

## Features

- **Simulate Left Click** using the Caps Lock key
- **Start and Stop** the background utility (`km_main.exe`)
- **Enable or Disable Auto-Start** at login via registry
- **Check Running Status** of the utility
- **Display Version Info**

## Commands

The utility supports the following commands:

```bash
Usage:
  on                    - Start km_main.exe
  off                   - Terminate km_main.exe
  start --auto          - Register km_main.exe to run on startup (registry)
  start --manual        - Remove km_main.exe from startup registry
  status                - Check if km_main.exe is running
  --version             - Show version info
  --help                - Show this help message
```

## Example Usage

```bash
km on               # Start km_main.exe
km off              # Terminate km_main.exe
km start --auto     # Register km_main.exe to run at user login (via registry)
km start --manual   # Remove km_main.exe from startup
km status           # Check if km_main.exe is currently running
km --version        # Show version information
km --help           # Display this help message
```

## Installation

To compile and run this project, you will need to have **GCC** installed on your system.

```bash
git clone https://github.com/Hyeonnam-J/windows-keymap.git
cd windows-keymap
gcc -o km km.c
gcc -o km_main km_main.c
```

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.