# crypto-benchmark-rp2350
Integration of [crypto-benchmark](https://github.com/sebastien-riou/crypto-benchmark) on [Raspberry Pi pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) board.

## Dependencies

### Pico tools & SDK
If you followed https://pip-assets.raspberrypi.com/categories/610-raspberry-pi-pico/documents/RP-008276-DS-1-getting-started-with-pico.pdf you are covered.

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
