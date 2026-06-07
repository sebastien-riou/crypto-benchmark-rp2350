# crypto-benchmark-rp2350
Integration of [crypto-benchmark](https://github.com/sebastien-riou/crypto-benchmark) on [Raspberry Pi pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) board.

## Dependencies

### Cmake and ninja
````
sudo apt-get install cmake ninja
````


### Pico tools & SDK
If you followed https://pip-assets.raspberrypi.com/categories/610-raspberry-pi-pico/documents/RP-008276-DS-1-getting-started-with-pico.pdf you are covered.

### ARM & RISC-V Toolchains
The toolchains `arm-none-eabi-gcc` and `riscv-none-elf-gcc` are needed in the `PATH`. 
This repo use PICO SDK but the `initial-setup` script is cloning other repositories that need them.

This projected as been tested on Ubuntu 24.04 with: 
- https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/tag/v14.2.1-1.1 
- https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/tag/v15.2.0-1

You can get them using the following script:
````
./get_toolchains
````


### Other repositories
Install and build them using the initial setup script:
````
./initial-setup
````


## How to build
Choose which one to build.

### Build for RISC-V RV32IMC AKA Hazard3
Build benchmark lib, for example:
````
cd ../crypto-benchmark
python link_ext.py --goal=small
./buildit on/rv32imc mldsa 44
cd ../crypto-benchmark-rp2350 
````

Then build the test firmware:
````
./buildit rv32imc
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
To launch the benchmark and get results, go to `../crypto-benchmark` and run `./get-results /dev/ttyACM0`. 
Be patient, the benchmark takes about one minute to execute.
