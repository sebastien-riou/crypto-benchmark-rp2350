# crypto-benchmark-rp2350
Integration of [crypto-benchmark](https://github.com/sebastien-riou/crypto-benchmark) on [Raspberry Pi pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) board.

## Dependencies

### Cmake
````
sudo apt-get install cmake
````

### ARM & RISC-V Toolchains
The toolchains `arm-none-eabi-gcc` and `riscv-none-elf-gcc` are needed in the `PATH`.

This projected as been tested on Ubuntu 24.04 with: 
- https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/tag/v14.2.1-1.1 
- https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/tag/v15.2.0-1

The `buildit` script will download them if they are not found on the path (it searches for any `arm-none-eabi-gcc` and any `riscv-none-elf-gcc`).

### Other repositories
This repo is set up as an add-on of [crypto-benchmark](https://github.com/sebastien-riou/crypto-benchmark)'s
`initial-setup`, which clones+builds it (and `pico-sdk`, via `./setup-pico-sdk`)
along with `crypto-benchmark`'s own library for its targets:
````
cd ../crypto-benchmark
./initial-setup --level=custom --addons=crypto-benchmark-rp2350
````

## How to build
Choose which one to build.

### Build for RISC-V RV32IMC AKA Hazard3
Build benchmark lib, for example:
````
cd ../crypto-benchmark
python link_ext.py --goal=small
./buildit on/rv32imcb mldsa 44
cd ../crypto-benchmark-rp2350 
````

Then build the test firmware:
````
./buildit rv32imcb
````

### Build for ARM Cortex-M33
Build benchmark lib, for example:
````
cd ../crypto-benchmark
python link_ext.py --goal=small
./buildit on/cortex-m33 mldsa 44
cd ../crypto-benchmark-rp2350 
````

Then build the test firmware:
````
./buildit cortex-m33
````

## Flash the board
The flash the latest firmware you built
````
./flash
````

## How to run the benchmark
Print the USB serial device to connect to (the CDC interface the RP2350 itself serves):

````
./find-usb
````

`./find-usb` resolves a `/dev/serial/by-id` entry, whose name embeds the board's flash unique ID and
is therefore stable across reboots and enumeration order, so nothing has to hardcode `/dev/ttyACM0`.
With more than one board attached, pin the one you want by passing that serial number, for example
`./find-usb E6614C775B532F35` -- it is the same serial `picotool --ser` filters on. `RP2350_USB`
overrides the result entirely.

The RP2350 serves the CDC itself, so the device node only exists while the firmware is running and
servicing USB: run `./find-usb` after `./run`, not while the board is in BOOTSEL mode.

To launch the benchmark and get results:

````
./run
DEV=$(./find-usb)
(cd ../crypto-benchmark && ./get-results "$DEV")
````

Be patient, the benchmark takes about one minute to execute.
