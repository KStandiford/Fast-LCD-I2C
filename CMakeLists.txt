# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(
    Fast_LCD_I2C
    VERSION 1.0
    DESCRIPTION "A Fast library for LCD's on the I2C bus"
    LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()


add_subdirectory(docs)
add_subdirectory(src)
add_subdirectory(Pi_Pico_Examples)

