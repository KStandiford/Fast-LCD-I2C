# Tell CMake where to find the executable source file
add_executable(HelloWorld 
    HelloWorld.c
    
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(HelloWorld)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(HelloWorld 
    LCD_I2C_lib
	
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(HelloWorld 1)
pico_enable_stdio_uart(HelloWorld 0)
