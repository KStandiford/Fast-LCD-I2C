# Tell CMake where to find the executable source file
add_executable(Demo_All_C 
    Demo_All.c
    
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(Demo_All_C)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(Demo_All_C 
    LCD_I2C_lib
	
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(Demo_All_C 1)
pico_enable_stdio_uart(Demo_All_C 0)
