# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.h")

# Make an automatic library 
add_library(LCD_I2C STATIC LCD_I2C.cpp LCD_I2C-C.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(LCD_I2C PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# This library needs pico_stdlib and hardware_i2C from the Pico SDK so
# they are automatically included in this step. This allows us to leave them out
# of the target_link_libraries commands within the project, but they must be
# added back in if the library is imported.
target_link_libraries(LCD_I2C  PUBLIC pico_stdlib hardware_i2c)


# IDEs should put the headers in a nice place
#source_group(
 # TREE "${PROJECT_SOURCE_DIR}/include"
  #PREFIX "Header Files"
  #FILES ${HEADER_LIST})

# Put the driver sources in the Arduino_Library folder too, so it is always current.
configure_file(LCD_I2C.cpp ${PROJECT_SOURCE_DIR}/Arduino_Library/LCD_I2C.cpp COPYONLY)
configure_file(LCD_I2C.hpp ${PROJECT_SOURCE_DIR}/Arduino_Library/LCD_I2C.h COPYONLY)

