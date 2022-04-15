# Tell CMake where to find the executable source file
add_executable(Demo_All_Cpp Demo_All.cpp)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(Demo_All_Cpp)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(Demo_All_Cpp LCD_I2C)

# Enable usb output, disable uart output
pico_enable_stdio_usb(Demo_All_Cpp 1)
pico_enable_stdio_uart(Demo_All_Cpp 0)
