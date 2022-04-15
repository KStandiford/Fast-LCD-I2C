/**
 * @file Demo_All.c
 * @author Keith Standiford
 * @brief Demonstrate all display functions from the C interface
 * @version 1.0
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <LCD_I2C-C.h>



// we assume a 16 char, 2 line display (1602 model)
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

/**
 * @brief Helper Function to write characters slowly so screen action can be seen
 * 
 * @param LCD pointer to the LCD object
 * @param str string to write
 * @param msDelay mSec to wait between characters
 */
void slowWrite(const char str[], uint32_t msDelay ) {
    char c;
    if (str != NULL) while( c = *str++) {
        lcd_writeChar(c);
        if(msDelay != 0) sleep_ms(msDelay);
    }
}

int main()
{
    char output[21];

    stdio_init_all();

    sleep_ms(2000);

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    // We use a helper function to set up the Pi Pico IO pins and initialize the I2C bus.

    uint32_t i2cspeed = LCD_I2C_Setup(i2c_default,PICO_DEFAULT_I2C_SDA_PIN,PICO_DEFAULT_I2C_SCL_PIN,100 * 1000);



// First test default object creation. THIS IS NOT BEST PRACTICE
// on a 1602, you may should not see anything
// This is here ONLY as a software test. Your program should start with the 
// call to lcd_init() below ...

    lcd_setCursor(2,0);
    slowWrite("Hello ",500);
    lcd_writeString("Pi World!");
    sleep_ms(3000);
    lcd_clear();

    

//  Initialize again, destroying the first instance and creating another
//  set to 2 lines, 16 characters each.
//  ******* This is the PROPER way to start out! ********
    lcd_init(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default);

    lcd_setCursor(0,0);
    slowWrite("Hello Again",500);
    lcd_setCursor(2,0); // Should end up on second line
    lcd_writeString("Pi World!");
    sleep_ms(3000);
    lcd_clear();

    uint16_t i,m;
    sprintf(output, "I2C clk %-3ld kbps", i2cspeed/1000);
    lcd_setCursor(0, 0);
    lcd_writeString(output);

    sleep_ms(3000);
    lcd_clear();

    // create special characters

    uint8_t  d0[] = {
        0b10001,
        0b01110,
        0b01100,
        0b01010,
        0b00110,
        0b01110,
        0b10001,
        0
    };
    uint8_t d1[] = {
        0b11011,
        0b10011,
        0b11011,
        0b11011,
        0b11011,
        0b11011,
        0b10001,
        0
    };
    uint8_t d2[] = {
        0b10001,
        0b01110,
        0b11110,
        0b11101,
        0b11011,
        0b10111,
        0b00000,
        0
    };
    uint8_t d3[] = {
        0b00000,
        0b11101,
        0b11011,
        0b11101,
        0b11110,
        0b01110,
        0b10001,
        0
    };
    uint8_t d4[] = {
        0b11101,
        0b11001,
        0b10101,
        0b01101,
        0b00000,
        0b11101,
        0b11101,
        0
    };
    uint8_t d5[] = {
        0b00000,
        0b01111,
        0b00001,
        0b11110,
        0b11110,
        0b01110,
        0b10001,
        0,

    };
    uint8_t d6[] = {
        0b11001,
        0b10111,
        0b01111,
        0b00001,
        0b01110,
        0b01110,
        0b10001,
        0
    };
    uint8_t d7[] = {
        0b00000,
        0b11110,
        0b11101,
        0b11011,
        0b10111,
        0b10111,
        0b10111,
        0
    };

    lcd_createChar(0,d0);
    lcd_createChar(1,d1);
    lcd_createChar(2,d2);
    lcd_createChar(3,d3);
    lcd_createChar(4,d4);
    lcd_createChar(5,d5);
    lcd_createChar(6,d6);
    lcd_createChar(7,d7);


    while(1) {

        //  Display the messages
        static const char *message[] =
            {
                "FAST Driver for", "LCDs on I2C bus",
                "Copyright (C)", "Keith Standiford",
                "This C program", "demonstrates ALL",
                "LCD modes of", "operation!",
                "", "Enjoy!"};


        int i, m;
        int num_messages = sizeof(message) / sizeof(message[0]);
        for (i = 1; i--;)
        {
            for (m = 0; m < num_messages; m += LCD_ROWS)
            {
                for (int line = 0; line < LCD_ROWS && m + line < num_messages; line++)
                {
                    lcd_setCursor(line, (LCD_COLUMNS / 2) - strlen(message[m + line]) / 2);
                    lcd_writeString(message[m + line]);
                }
                sleep_ms(5000);
                lcd_clear();
            }
        }

        // test each of the function calls

        lcd_clear();
        lcd_noCursor();
        lcd_noBlink();

        // test special characters
        lcd_setCursor(0,0);
        lcd_writeString("Custom 01234567");
        lcd_setCursor(1,0);
        lcd_writeString("Chars=>");
        for(int i=0;i<8;i++) lcd_writeChar((char) i);
        sleep_ms(4000);


        // Test the backlight
        lcd_clear();
        lcd_setCursor(0,0);
        lcd_writeString("Backlight OFF");
        sleep_ms(2000);
        lcd_noBacklight();
        lcd_setCursor(1,0);
        lcd_writeString("Backlight ON");
        sleep_ms(2000);
        lcd_backlight();
        sleep_ms(2000);

        // Test blanking the display
        lcd_clear();
        lcd_setCursor(0,0);
        lcd_writeString("  Blank display");
        sleep_ms(2000);
        lcd_noDisplay();
        lcd_setCursor(1,0);
        lcd_writeString("unBlank display");
        sleep_ms(2000);
        lcd_display();
        sleep_ms(3000);


        // turn cursor on and off
        lcd_clear();
        lcd_setCursor(0,0);
        lcd_writeString("Cursor On  *");
        lcd_setCursor(0,11);
        lcd_cursor();
        sleep_ms(3000);

        lcd_setCursor(0,0);
        lcd_writeString("Cursor Off *");
        lcd_setCursor(0,11);
        lcd_noCursor();
        sleep_ms(3000);

        // Show blinking cursor
        lcd_clear();
        lcd_setCursor(0,0);
        lcd_writeString("Cursor   Blink *");
        lcd_setCursor(0,15);
        lcd_blink();
        sleep_ms(3000);

        lcd_setCursor(1,0);
        lcd_writeString("Cursor noBlink *");
        lcd_setCursor(0,15);
        lcd_noBlink();
        sleep_ms(3000);

        lcd_clear();
        lcd_setCursor(0,0);
        lcd_writeString("Cursor Both *");
        lcd_setCursor(0,12);
        lcd_blink();
        lcd_cursor();
        sleep_ms(3000);

        lcd_noBlink();
        lcd_noCursor();
        lcd_clear();


        // Test scroll display right and left
        lcd_setCursor(0,0);
        lcd_writeString("Scroll to Right");
        lcd_setCursor(0,10);
        lcd_blink();
        sleep_ms(1000);
        lcd_scrollDisplayRight();
        sleep_ms(1000);
        lcd_scrollDisplayRight();
        sleep_ms(1000);
        lcd_scrollDisplayRight();
        sleep_ms(1000);
        lcd_setCursor(0,0);
        lcd_writeString("Scroll to Left ");
        lcd_setCursor(0,10);
        lcd_blink();
        sleep_ms(1000);
        lcd_scrollDisplayLeft();
        sleep_ms(1000);
        lcd_scrollDisplayLeft();
        sleep_ms(1000);
        lcd_scrollDisplayLeft();
        sleep_ms(1000);
        lcd_scrollDisplayLeft();
        sleep_ms(1000);
        lcd_clear();
    
        // Autoscroll  and write left to right  and right to left
        // Note that autoscroll right to left is WIERD!

        lcd_clear();
        lcd_blink();
        lcd_setCursor(0,14);
        lcd_autoscroll();
        slowWrite("autoScroll L=>R",500);
        sleep_ms(2000);
        lcd_clear();
        lcd_setCursor(1,0);
        lcd_noAutoscroll();
        slowWrite("Write L=>R",500);
        sleep_ms(2000);
        lcd_clear();
        lcd_setCursor(1,0);
        lcd_autoscroll();
        lcd_rightToLeft();
        slowWrite(" L>=R llorcSotua",500);
        sleep_ms(2000);
        lcd_clear();
        lcd_setCursor(1,15);
        lcd_noAutoscroll();
        slowWrite("L>=R etirW",500);
        sleep_ms(2000);
        lcd_leftToRight();


    


    }


    while (1);

    return 0;

}