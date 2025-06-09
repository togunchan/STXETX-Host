# STXETX-Host

**A C++17 implementation of an STX/ETX-based serial communication host**

## Table of Contents

1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Architecture and Modules](#architecture-and-modules)
4. [Threaded Serial Reception](#threaded-serial-reception)
5. [Prerequisites](#prerequisites)
6. [Build and Installation](#build-and-installation)
7. [Usage](#usage)
8. [Running Tests](#running-tests)
9. [Directory Structure](#directory-structure)
10. [Contributing](#contributing)
11. [Future Improvements](#future-improvements)
12. [License](#license)

---

## Project Overview

STXETX-Host is a host-side application written in modern C++ (C++17) for reliable, frame-based serial communication with embedded devices using the STX/ETX protocol. It supports encoding, decoding, and CRC verification of frames, non-blocking serial I/O via a background thread, and comprehensive unit testing.

## Features

* **Frame Encoder/Parser**: Implements STX (0x02) and ETX (0x03) delimitation, escaping of control bytes, and CRC-16 (XMODEM/CCITT) integrity checks.
* **CRC Module**: Provides a bitwise CRC-16 implementation to verify data integrity.
* **Ring Buffer**: A fixed-size circular buffer for thread-safe storage of incoming bytes.
* **Logger**: Integrates [spdlog](https://github.com/gabime/spdlog) for efficient console and file logging.
* **Threaded Reception**: `ReceiverThread` class for reading serial data in a dedicated background thread, ensuring the main application loop remains responsive.
* **Unit Tests**: Uses [Catch2](https://github.com/catchorg/Catch2) for automated testing of each module.

## Architecture and Modules

* **CRC** (`include/CRC.hpp`, `src/CRC.cpp`)

  * Computes 16-bit CRC using polynomial 0x1021.
* **RingBuffer** (`include/RingBuffer.hpp`, `src/RingBuffer.cpp`)

  * Fixed-capacity buffer with `push`, `pop`, and status queries (`empty`, `full`).
* **FrameEncoder** (`include/FrameEncoder.hpp`, `src/FrameEncoder.cpp`)

  * Encodes raw payloads into protocol-compliant frames with STX/ETX, escapes, and CRC.
* **FrameParser** (`include/FrameParser.hpp`, `src/FrameParser.cpp`)

  * Decodes incoming byte streams, handles escaping, verifies CRC, and yields payloads.
* **Logger** (`include/Logger.hpp`, `src/Logger.cpp`)

  * Wraps spdlog for standardized logging patterns and severity levels.

## Threaded Serial Reception

* **ReceiverThread** (`src/ReceiverThread.cpp`, `include/ReceiverThread.hpp`)

  * Implements `start()` and `stop()` methods for a background thread that continuously reads from the serial port.
  * Uses a mutex-protected `RingBuffer` to safely transfer data between threads.

## Prerequisites

* C++17-compatible compiler (GCC, Clang, MSVC)
* [CMake](https://cmake.org/) ≥ 3.14
* Linux/macOS: `<termios.h>` for serial port control; Windows support requires adaptation.
* Dependencies (included as submodules in `third_party/`):

  * [spdlog](https://github.com/gabime/spdlog)
  * [Catch2](https://github.com/catchorg/Catch2)

## Build and Installation

```bash
# Clone the repository and initialize submodules
git clone --recurse-submodules https://github.com/<togunchan>/STXETX-Host.git
cd STXETX-Host

# Create build directory and compile
mkdir build && cd build
cmake ..
make
```

## Usage

You can simulate serial communication locally using `socat` and test payload frames with a Python script. Follow these steps:

1. **Open three terminals:**

   * **Terminal A:** Start a `socat` proxy creating two virtual serial ports:

     ```bash
     socat -d -d \
       pty,raw,echo=0,link=/tmp/ttyV0,ispeed=115200,ospeed=115200 \
       pty,raw,echo=0,link=/tmp/ttyV1,ispeed=115200,ospeed=115200
     ```

     This creates `/tmp/ttyV0` and `/tmp/ttyV1` linked pair.

   * **Terminal B:** Run the host application on one end of the pair:

     ```bash
     ./SerialCommHostApp /tmp/ttyV0
     ```

   * **Terminal C:** Send a test frame to `/tmp/ttyV0` using Python:

     ```bash
     #!/usr/bin/env python3
     import sys

     def compute_crc16(data: bytes, poly=0x1021, init=0xFFFF) -> int:
         crc = init
         for b in data:
             crc ^= (b << 8)
             for _ in range(8):
                 if crc & 0x8000:
                     crc = ((crc << 1) & 0xFFFF) ^ poly
                 else:
                     crc = (crc << 1) & 0xFFFF
         return crc

     # 1) Define the payload
     payload = bytes([0x44, 0x55])  # example payload

     # 2) Compute CRC16 over the payload only
     crc = compute_crc16(payload)

     # 3) Build the frame: STX + payload + CRC_hi + CRC_lo + ETX
     frame = bytes([0x02]) + payload + bytes([crc >> 8, crc & 0xFF]) + bytes([0x03])

     # Write the complete frame: STX, payload, CRC bytes, then ETX
     sys.stdout.buffer.write(frame)
     ```

When the frame is sent, the host application (running in Terminal B) will parse it, verify the CRC, and respond (e.g., send an ACK frame) back on `/tmp/ttyV1`.

## Running Tests

```bash
cd build
ctest --verbose
```

## Directory Structure

```
SerialCommHost/
├── include/                # Public headers
│   ├── CRC.hpp
│   ├── FrameEncoder.hpp
│   ├── FrameParser.hpp
│   ├── RingBuffer.hpp
│   ├── Logger.hpp
│   └── ReceiverThread.hpp
├── src/                    # Source files
│   ├── CRC.cpp
│   ├── FrameEncoder.cpp
│   ├── FrameParser.cpp
│   ├── RingBuffer.cpp
│   ├── Logger.cpp
│   └── ReceiverThread.cpp
├── tests/                  # Unit tests (Catch2)
│   ├── test_CRC.cpp
│   ├── test_RingBuffer.cpp
│   ├── test_FrameEncoder.cpp
│   └── test_FrameParser.cpp
├── third_party/            # External dependencies
│   ├── spdlog/
│   └── catch2/
├── CMakeLists.txt          # Root CMake configuration
├── tests/CMakeLists.txt    # Test-specific CMake configuration
└── README.md               # Project documentation (this file)
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository and create a feature branch (`feature/xyz`).
2. Write clear, concise commit messages in Conventional Commits style.
3. Add unit tests for new functionality.
4. Ensure all tests pass and code is formatted.
5. Open a Pull Request for review.

## Future Improvements

* Windows serial port support via Win32 API.
* Command-line argument parser (e.g., [cxxopts](https://github.com/jarro2783/cxxopts)).
* Multiple-port handling with thread pools.
* CI/CD pipeline with GitHub Actions for automated builds and tests.
* Detailed protocol documentation and example Python/PySerial client.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

Developed and maintained by [Murat Togunçhan Düzgün](https://www.linkedin.com/in/togunchan/)
