# Tell CMake where to find the executable source file
add_executable(TimingTestCpp 
    main.cpp xLCD_I2C.cpp
    
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(TimingTestCpp )

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(TimingTestCpp  
    LCD_I2C_lib 
	
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(TimingTestCpp  1)
pico_enable_stdio_uart(TimingTestCpp  0)
