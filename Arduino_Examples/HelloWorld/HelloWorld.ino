/**
 * @file HelloWorld.ino
 * @author Keith Standiford
 * @brief The classic "bare minimum" sketch
 * @version 1.0
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved.
 * 
 * This sketch just says hello!.
 */


#include <Wire.h>
#include <LCD_I2C.h>


// we have a 16 char, 2 line display at address 027 hex
#define MAX_LINES 2
#define MAX_CHARS 16
#define LCD_ADDRESS 0x27

// The I2C bus speed. 100000 is the default, but if your display supports it,
// You can try 400000 for faster response. (Though your eye won't notice.)
#define WIRESPEED 100000

// Declare and initialize the LCD object

LCD_I2C lcd = LCD_I2C(LCD_ADDRESS, MAX_CHARS, MAX_LINES);


void setup()
{

  delay(2000);  // give the user a chance to focus ... (optional)

  // Set the I2C bus speed. This is optional, but if you want to change the
  // default speed of 100000, here is where to do it!

  Wire.setClock(WIRESPEED);

  // Initialize the display. This also initializes the I2C bus.

  lcd.begin();

  // Display is initialized to cleared, backlight on and cursor at 0,0 (upper left edge).
  // Note that all the print methods work, but the print line will not. You must 
  // reposition the cursor yourself. See below...

  lcd.print("Hello Arduino");   // Say hello!

  // Demonstrate cursor positioning.

  lcd.setCursor(3,1);      // line 1 is second line, 3 is fourth character

  // Finish the classic message with a call to writeString, rather than print.
  // In practice, this is usually substantially faster than print.

  lcd.writeString("World!");     // Finish the classic message!

}

void loop()
{
  while(1); // All done!
}
