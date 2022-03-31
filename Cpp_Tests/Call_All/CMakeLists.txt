# Tell CMake where to find the executable source file
add_executable(Call_All 
    main.cpp
    
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(Call_All)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(Call_All 
    LCD_I2C_lib
	
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(Call_All 1)
pico_enable_stdio_uart(Call_All 0)
