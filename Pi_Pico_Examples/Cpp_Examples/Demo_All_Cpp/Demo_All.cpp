/**
 * @file Demo_All.cpp
 * @author Keith Standiford
 * @brief Demonstrate all display functions from the C++ interface
 * @version 1.0
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * 
 */
#include <memory>
#include <cstdlib>
#include <array>

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "LCD_I2C.hpp"


// we have a 16 char, 2 line display
// Make the obvious changes if you have a 2004
// with 4 lines of 20 characters each.
#define MAX_LINES 2
#define MAX_CHARS 16

/**
 * @brief Helper Function to write characters slowly so screen action can be seen
 * 
 * @param LCD pointer to the LCD opbjec
 * @param str string to write
 * @param msDelay mSec to wait between characters
 */
void slowWrite(LCD_I2C *LCD, const char str[], uint32_t msDelay ) {
    if (str != NULL) while(char c = *str++) {
        LCD->writeChar(c);
        if(msDelay != 0) sleep_ms(msDelay);
    }
}

int main()
{
    char output[21];

    stdio_init_all();

    sleep_ms(2000);

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)

    uint32_t i2cspeed = LCD_I2C_Setup(i2c_default,PICO_DEFAULT_I2C_SDA_PIN,PICO_DEFAULT_I2C_SCL_PIN,100 * 1000);

    constexpr auto LCD_ADDRESS = 0x27;
    constexpr auto LCD_COLUMNS = 20;
    constexpr auto LCD_ROWS = 4;


    LCD_I2C* lcd = new LCD_I2C(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default);

    lcd->setCursor(0,0);
    slowWrite(lcd,"Hello ",500);
    lcd->writeString("Pi World!");
    sleep_ms(3000);
    lcd->clear();



    absolute_time_t start;
    double T1, T2;
    uint16_t i,m;
    sprintf(output, "I2C clk %-3ld kbps", i2cspeed/1000);
    lcd->setCursor(0, 0);
    lcd->writeString(output);

    start = get_absolute_time();
    int loops = 1000;
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(0, 0);
        lcd->writeString(output);
    }
    T1 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "noBfr%7.3f ms", T1);
    lcd->setCursor(1, 0);
    lcd->writeString(output);
    //output[10] = 0;
    start = get_absolute_time();
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(1,0,true); // buffer the cursor command
        lcd->writeString(output,true); // flush after string
        lcd->show();
    }
    T2 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "Buffr%7.3f ms ", T2);
    lcd->setCursor(0, 0);
    lcd->writeString(output);

    sleep_ms(3000);
    lcd->clear();

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

    lcd->createChar(0,d0);
    lcd->createChar(1,d1);
    lcd->createChar(2,d2);
    lcd->createChar(3,d3);
    lcd->createChar(4,d4);
    lcd->createChar(5,d5);
    lcd->createChar(6,d6);
    lcd->load_custom_character(7,d7);


    while(1) {

        //  Display the messages
        static const char *message[] =
            {
                "FAST Driver for", "LCDs on I2C bus",
                "Copyright (C)", "Keith Standiford",
                "Works on Pi Pico", "and Arduino",
                "This C++ program", "demonstrates ALL",
                "LCD modes of", "operation!",
                "", "Enjoy!"};


        int i, m;
        int num_messages = sizeof(message) / sizeof(message[0]);
        for (i = 1; i--;)
        {
            for (m = 0; m < num_messages; m += MAX_LINES)
            {
                for (int line = 0; line < MAX_LINES && m + line < num_messages; line++)
                {
                    lcd->setCursor(line, (MAX_CHARS / 2) - strlen(message[m + line]) / 2);
                    lcd->writeString(message[m + line]);
                }
                sleep_ms(5000);
                lcd->clear();
            }
        }

        // test each of the function calls

        lcd->clear();
        lcd->noCursor();
        lcd->noBlink();

        // test special characters
        lcd->setCursor(0,0);
        lcd->writeString("Custom 01234567");
        lcd->setCursor(1,0);
        lcd->writeString("Chars=>");
        for(int i=0;i<8;i++) lcd->writeChar((char) i);
        sleep_ms(4000);

        // test buffer overflow
        lcd->clear();
        char bigoutput[50];
        for (int i=0;i<6;i++) {
            lcd->setCursor(0,0,true);
            sprintf(output, "Buffer Test   %2d", i);
            lcd->writeString(output, true); // eventually the buffer overflows 
            for (int j=0;j<20;j++) lcd->setCursor(1,0,true);
            sprintf(output, "#'s != is good%2d", i);
            lcd->writeString(output, true); // eventually the buffer overflows 
            // but the rest of the data isn't dumped yet ...
            sleep_ms(1000);
        }

        // Test the backlight
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Backlight OFF");
        sleep_ms(2000);
        lcd->setBacklight(0);
        lcd->setCursor(1,0,true);
        lcd->printstr("Backlight ON");
        sleep_ms(2000);
        lcd->setBacklight(1);
        sleep_ms(2000);

        // Test blanking the display
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("  Blank display");
        sleep_ms(2000);
        lcd->noDisplay();
        lcd->setCursor(1,0,true);
        lcd->writeString("unBlank display");
        sleep_ms(2000);
        lcd->display();
        sleep_ms(3000);


        // turn cursor on and off
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor On  *",true);
        lcd->setCursor(0,11,true);
        lcd->cursor();
        sleep_ms(3000);

        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor Off *",true);
        lcd->setCursor(0,11,true);
        lcd->noCursor();
        sleep_ms(3000);

        // Show blinking cursor
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor   Blink *",true);
        lcd->setCursor(0,15,true);
        lcd->blink();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor noBlink *",true);
        lcd->setCursor(0,15,true);
        lcd->noBlink();
        sleep_ms(3000);

        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor Both *",true);
        lcd->setCursor(0,12,true);
        lcd->blink();
        lcd->cursor();
        sleep_ms(3000);

        lcd->noBlink();
        lcd->noCursor();
        lcd->clear();


        // Test scroll display right and left
        lcd->setCursor(0,0,true);
        lcd->writeString("Scroll to Right",true);
        lcd->setCursor(0,10,true);
        lcd->blink();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->setCursor(0,0,true);
        lcd->writeString("Scroll to Left ",true);
        lcd->setCursor(0,10,true);
        lcd->blink();
        sleep_ms(1000);
        lcd->scrollDisplayLeft();
        sleep_ms(1000);
        lcd->scrollDisplayLeft();
        sleep_ms(1000);
        lcd->scrollDisplayLeft();
        sleep_ms(1000);
        lcd->scrollDisplayLeft();
        sleep_ms(1000);
        lcd->clear();
    
        // Autoscroll  and write left to right  and right to left
        // Note that autoscroll right to left is WIERD!

        lcd->clear();
        lcd->blink();
        lcd->setCursor(0,14);
        lcd->autoscroll();
        slowWrite(lcd,"autoScroll L=>R",500);
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(1,0);
        lcd->noAutoscroll();
        slowWrite(lcd,"Write L=>R",500);
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(1,0);
        lcd->autoscroll();
        lcd->rightToLeft();
        slowWrite(lcd," L>=R llorcSotua",500);
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(1,15);
        lcd->noAutoscroll();
        slowWrite(lcd,"L>=R etirW",500);
        sleep_ms(2000);
        lcd->leftToRight();


    


    }


    while (1);

    return 0;

}