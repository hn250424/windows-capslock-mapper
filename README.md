# Windows-Keymap

A small Windows utility that maps the Caps Lock key to simulate a left mouse click, helping reduce strain on your right wrist. It offers fast and simple command-line options, making it especially useful for developers who are comfortable with the CLI.

## Features

- **Simulate left mouse clicks using the Caps Lock key**
Helps reduce on the right wrist strain by allowing you to click without using the mouse button.

- **Caps Lock function is overridden while the program is running**
To type capital letters, use the Shift key instead of Caps Lock during operation.

- **Quickly enable or disable with simple CLI commands**
In cases where holding Shift isn't practical—like writing in all caps—you can easily pause and resume the program using km off and km on.

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