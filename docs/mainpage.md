# A *Fast* Driver for LCD displays on the I2C bus for Pi Pico *and* Arduino {#mainpage}

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

Note that on all platforms, the complete documentation can be accessed with a web browser at /docs/html/index.html

### Getting Started on Arduino

#### Installing the Library

 - Once you have downloaded the repository, create a new folder called "LCD_I2C" under the folder named "libraries" in your 
 Arduino sketchbook folder. (Create the folder "libraries" if it does not already exist.)
 - Copy all the files from the "Arduino_Library" directory into the "LCD_I2C" folder. 

#### Installing the Examples

 Copy the contents of the "Arduino_Examples" directory to your Arduino sketchbook folder and try them out!

#### Usage 
To use the library in your own sketch:
 -  select LCD_I2C from *Sketch > Import Library*.
 - *Also* select Wire from *Sketch > Import Library*.

## Getting Started on Raspberry Pi Pico

On the Pi Pico, programming in C++ will make all of the features of this package available. This will also make some advanced cases like multiple displays very easy. However, if you prefer to work in C, a reasonably complete subset of the program interface is available with almost all of the functionality of the C++ version. There are examples included for both languages.

### Prerequisites

In your development environment (IDE) you need:
 - The Pi Pico SDK installed with the environmental PATH variables set.
 - CMAKE installed.
 - MAKE installed.

You also need a display connected to the default Pi Pico I2C bus pins. *Make sure that you use a level shifter chip if you are running the display on 5 volts!*

### Generating the Tests and Examples

Running CMAKE in the base directory of the repository will create the library and the tests and examples in a directory tree in the "build" folder. From there you can load them into the Pi Pico.

#### C++ Usage


  

## Built With

* VSCODE and the MinGW tools
* Raspberry Pi Pico SDK


## Author

* **Keith Standiford** - [KStandiford](https://github.com/KStandiford)


## License

This project is licensed under the MIT License - see the LICENSE.md file for details

## Acknowledgments

* [Adafruit](https://www.adafruit.com/), whose dedication to documenting their products made it easy to find the documentation to figure this stuff out.
* [Cristian Cristea](https://github.com/cristiancristea00), who's code taught me a lot about modern professional C++.
* [Stan Reynolds](https://www.linkedin.com/in/stanley-reynolds-4bb86040), an old mentor and friend who once said 
"If you're going to do something stupid, at least make sure that it's *fast*!


