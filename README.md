# A *Fast* Driver for LCD displays on the I2C bus for Pi Pico *and* Arduino 

This is a driver package for Liquid Crystal displays like the common 1602 and 2004 models using the I2C bus. It has been optimized for speed, yielding typically an order of magnitude improvement. It is compatible with *both* Arduino and Raspberry Pi Pico environments. For the Pi Pico, both C++ and C interfaces are available. (Arduino uses the C++ interface only)

## Introduction

Liquid Crystal (LCD) displays with 2 lines of 16 characters (model 1602) and 4 lines of 20 characters (model 2004) are inexpensive but versatile and fun displays to add to a micro-controller. Versions with an I2C bus adapter built in are also available and attractive because they only need two output pins for the I2c bus. There are numerous tutorials about these displays on-line, and multiple packages of software. 

Unfortunately, most of the common packages are quite slow when updating the display. No, you cannot notice how long it takes to print "Hello World", but if you are trying to update several fields on the screen fairly quickly, you may find the response to other user and peripheral inputs begins to suffer. That was the motivation for this project.

## Timing Results

These benchmarks are against popular and commonly used (anonymous) repositories. The benchmark is:
 - Update 4 fields of 6 characters each on the display. The fields are assumed to be in different locations, requiring the cursor to be positioned for each one.
 - The I2C bus speed is the typical default of 100,000 bits/second
 - Hardware is a Pi Pico.
 

 | Environment| Conventional Driver | Fast LCD I2C | Improvement |
 | --: | :----: | :---: | :---: |
 | Arduino | 39.55 mSec | 11.62 mSec | 3.4x |
 | Pi Pico | 136.97 mSec | 11.56 mSec | 11.8x |

## Getting Started

Download the repository. On Arduino, the easiest is to simply download the zip file and unzip it somewhere convenient. For Pi Pico, you more likely should clone the repository into a convenient place.

Note that on all platforms, the complete documentation can be accessed with a web browser at `/docs/html/index.html` or you can
view it online [here](https://kstandiford.github.io/Fast-LCD-I2C/).

### Getting Started on Arduino

#### Installing the Library

 - Once you have downloaded the repository, create a new folder called `LCD_I2C` under the folder named `libraries` in your 
 Arduino sketchbook folder. (Create the folder `libraries` if it does not already exist.)
 - Copy all the files from the `Arduino_Library` directory into the `LCD_I2C` folder. 

#### Installing the Examples

 Copy the contents of the `Arduino_Examples` directory to your Arduino sketchbook folder and try them out!

#### Usage 
To use the library in your own sketch:
 -  select *LCD_I2C* from *Sketch > Import Library*.
 - *Also* select *Wire* from *Sketch > Import Library*.

This will add the following to your sketch:

    #include <Wire.h>
    #include <LCD_I2C.h>


See the documentation sections on the **C++ Interface**, and  **Arduino Differences**.

## Getting Started on Raspberry Pi Pico

On the Pi Pico, programming in C++ will make all of the features of this package available. This will also make some advanced cases like multiple displays very easy. However, if you prefer to work in C, a reasonably complete subset of the program interface is available with almost all of the functionality of the C++ version. There are examples included for both languages.

### Prerequisites

In your development environment (IDE) you need:
 - The Pi Pico SDK installed with the environmental PATH variables set.
 - CMAKE installed.
 - MAKE installed.

You also need a display connected to the default Pi Pico I2C bus pins. *Make sure that you use a level shifter chip if you are running the display on 5 volts!*

### Quick Start 

The cleanest approach is to build the libraries on your system, and include them in
the CMakeLists files for your project as demonstrated in the next section. However, if
you want to be as simple as possible...

The fastest way to get started using the driver is to copy the sources and include 
files to your project directory and add the sources to your `add_executable` CMake command.
(The sources are in `/src` and the includes are in `/src/include`.) For C++ projects, you only need 
`LCD_I2C.cpp` and `LCD_I2C.hpp`. For C projects, you need *all* the source and header files, 
but you should include *only* `LCD_I2C-C.h` in your program.

### Generating the Library and Examples

Running CMAKE in the base directory of the repository will create the library and examples in a directory tree 
in the `build` folder. From there you can load the examples into the Pi Pico.

### Adding the Library to Your Project

You must add a library target of type `IMPORT` for LCD_I2C and tell CMake where to find it,
as well as the include files.  This is done by adding an `add_libraries` command to
your `CMakeLists` file. The following CMakeLists is for the HelloWorld example, but it has been moved *outside*
the Fast-LCD-I2C project. You must modify the `set(LCD_I2C_PROJECT_PATH ...)` line to match
your environment.

~~~~{.cmake}

# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(HelloWorld
    LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set the path to the LCD_I2C Project Directory
set(LCD_I2C_PROJECT_PATH "<Path to the repository location>")

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()
# Tell CMake where to find the executable source file
add_executable(HelloWorld HelloWorld.c )

#Add the imported library from LCD_I2C
add_library(LCD_I2C STATIC IMPORTED)
    # and set the path to find it
    set_target_properties(LCD_I2C PROPERTIES
        IMPORTED_LOCATION ${LCD_I2C_PROJECT_PATH}/build/src/libLCD_I2C.a
        INTERFACE_INCLUDE_DIRECTORIES ${LCD_I2C_PROJECT_PATH}/src/include)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(HelloWorld)

# Link to LCD_I2C library, pico_stdlib (gpio, time, etc. functions) and the hardware_i2c library
target_link_libraries(HelloWorld LCD_I2C pico_stdlib hardware_i2c)

# Enable usb output, disable uart output
pico_enable_stdio_usb(HelloWorld 1)
pico_enable_stdio_uart(HelloWorld 0)
~~~~

#### C++ Usage

See the documentation sections on the **C++ Interface**, and  **I2C Initialization**.

#### C Usage

If you prefer to write in C, there is an interface that "wraps" the C++ interface and makes
a subset of the functions available in C. See the documentation section for the **C Interface** for 
the limitations and detailed usage information. Also see the documentation section for **I2C Initialization**.


## Built With

* VSCODE and the MinGW tools
* Raspberry Pi Pico SDK


## Author

* **Keith Standiford** - [KStandiford](https://github.com/KStandiford)


## License

This project is licensed under the MIT License - see the LICENSE file for details

## Acknowledgments

* [Adafruit](https://www.adafruit.com/), whose dedication to documenting their products made it easy to find the documentation to figure this stuff out.
* [Cristian Cristea](https://github.com/cristiancristea00), who's code taught me a lot about modern professional C++.
* [Stan Reynolds](https://www.linkedin.com/in/stanley-reynolds-4bb86040), an old mentor and friend who once said 
"If you're going to do something stupid, at least make sure that it's *fast*!


