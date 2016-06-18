cmake_minimum_required(VERSION 2.8)
project(msp430-fm-receiver)

include(CMake/MSP430.cmake)

set(BOARD_NAME "msp430g2553")

# Set the C flags. Only the compiler is set by the platform file. The -mmcu flag is REQUIRED 
# by mspgcc to find the correct include files and enable preprocessor defs. this definition should
# be equal to the target board.
set(MSP_CPU_FLAG "-mmcu=${BOARD_NAME}")
set(CMAKE_C_FLAGS "${MSP_CPU_FLAG} -Os")
set(CMAKE_CXX_FLAGS "${MSP_CPU_FLAG} --std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${MSP_CPU_FLAG}")

# Create the executable
add_executable(msp430-fm-receiver
    Source/main.cpp
    Source/uart.c
    Source/uart.h
)

include_directories(../Protocol)

# Create a flash target if the toolchain file was selected correctly
if (MSP_TOOLCHAIN)
    setup_flash_target(msp430-fm-receiver rf2500)
endif () 