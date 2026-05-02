# Building Pain Engine

## Requirements

- CMake 3.20+
- C++20 compatible compiler
- [Vulkan SDK](https://github.com/dargmuesli) installed on your system
- Git

## Setup

Clone the repo and init submodules:

```bash
git clone https://github.com/Kritoxx1/painEngine
cd painEngine
git submodule update --init --recursive
```

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The binary will be output as `painEngine` (or `painEngine.exe` on Windows).

## Shaders

Make sure the shell script is executable:

```bash
chmod +x compile.sh
```

## Note

- **Linux/macOS**: Vulkan must be installed, either via packetmanager or the [Tarball](https://vulkan.lunarg.com/sdk/home#linux).
- **Windows**: Vulkan SDK must be installed and its `bin` directory added to `PATH`. You can find the installer [here](https://vulkan.lunarg.com/sdk/home)

> **Note** macOS support is planned but not yet implemented.

### Off-topic Note
README.md will come soon.