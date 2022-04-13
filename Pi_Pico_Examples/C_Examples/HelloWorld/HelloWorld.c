
/**
 * @file HelloWorld.c
 * @author Keith Standiford
 * @brief The classic bare bones example
 * @version 1.0
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * 
 * This is the traditional "bare minimum" example for using the 
 * C interfaces.
 * 
 */
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "LCD_I2C-C.h"


// we assume a 16 char, 2 line display (1602 model)
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define I2C_SPEED 100000

int main()
{
    sleep_ms(2000); //Wait after loading to let the user re-focus

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    // We use the supplied helper function to set up the Pi Pico IO pins and initialize the I2C bus.

    LCD_I2C_Setup(i2c_default,PICO_DEFAULT_I2C_SDA_PIN,PICO_DEFAULT_I2C_SCL_PIN,I2C_SPEED);

//  Initialize the display
    lcd_init(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default);

/*  After initialization, the display is clear, the cursor is on the first line and character, 
    and the backlight is on. We will output the message on two lines just so cursor contorl is used
    once and then loop forever...
    */

    lcd_writeString("Hello Pi Pico");
    lcd_setCursor(1,3); // line 1 is the second line, and position 3 is the fourth character
    lcd_writeString("World!");

    // And it's that easy!

    while(1) ;
}
