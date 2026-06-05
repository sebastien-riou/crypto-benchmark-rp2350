# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SKIP_PICO_MALLOC 1)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
message("PICO_PLATFORM=${PICO_PLATFORM}")
if(${PICO_PLATFORM} STREQUAL "rp2350-riscv")
    set(toolchainVersion RISCV_ZCB_RPI_2_2_0_3)
else()
    set(toolchainVersion 14_3_Rel1)
endif()
message("toolchainVersion=${toolchainVersion}")
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

set(TARGET crypto-benchmark-rp2350)
project(${TARGET} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${TARGET} crypto-benchmark-rp2350.c )

pico_set_program_name(${TARGET} "crypto-benchmark-rp2350")
pico_set_program_version(${TARGET} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${TARGET} 0)
pico_enable_stdio_usb(${TARGET} 1)

#custom linker script to increase stack
pico_set_linker_script(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
# add paths for crypto-benchmark lib 
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../crypto-benchmark)
if(${PICO_PLATFORM} STREQUAL "rp2350-riscv")
    target_link_libraries(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/../crypto-benchmark/build/rv32imc/libcrypto-benchmark.a)
else()
    target_link_libraries(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/../crypto-benchmark/build/cortex-m33/libcrypto-benchmark.a)
endif()

target_link_options(${TARGET} PUBLIC "LINKER:--wrap=malloc")
target_link_options(${TARGET} PUBLIC "LINKER:--wrap=calloc")
target_link_options(${TARGET} PUBLIC "LINKER:--wrap=realloc")
target_link_options(${TARGET} PUBLIC "LINKER:--wrap=free")

# Add the standard library to the build
target_link_libraries(${TARGET}
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(${TARGET})

