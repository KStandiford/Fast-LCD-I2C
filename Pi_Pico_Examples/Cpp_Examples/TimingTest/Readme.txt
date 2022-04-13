**NOTE:**

This directory will generate errors because the anonymous LCD drivers are *NOT* uploaded
to the repository. You must find your own candidate to test. Do the following:

 - Find a candidate driver for LCDs on the I2C bus for Pi Pico
 - Place the source and header files in this directory.
 - Modify the CMakeLists.txt file to include the source files along with main.cpp
 - Modify main.cpp to include the proper include files and call the appropriate interfaces

BOTH drivers will be talking to the same hardware! Most drivers will be remembering the 
state of the backlight control, so it may be necessary to turn the backlight on
for both drivers. Everything else probably works OK. But make sure that the I2C bus interfaces
on the same I2C instance and pins for each device.
