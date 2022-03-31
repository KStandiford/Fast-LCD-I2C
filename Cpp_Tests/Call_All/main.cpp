/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

#define TRIGGER 14

// we have a 20 char, 4 line display
#define MAX_LINES 4
#define MAX_CHARS 20

int main()
{
    stdio_init_all();

    sleep_ms(2000);

    gpio_init(TRIGGER);
    gpio_set_dir(TRIGGER, GPIO_OUT);
    gpio_pull_up(TRIGGER);

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    int i2cspeed = i2c_init(i2c_default, 400 * 1000);
    //    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    constexpr auto LCD_ADDRESS = 0x27;
    constexpr auto LCD_COLUMNS = 20;
    constexpr auto LCD_ROWS = 4;


    auto lcd = std::make_unique<LCD_I2C>(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default);

    lcd->setCursor(0,0);
    lcd->writeString("Hello World!");
    sleep_ms(5000);


    static const char *message[] =
        {
            "RP2040 by", "Raspberry Pi",
            "A brand new", "microcontroller",
            "Twin core M0", "Full C SDK",
            "More power in", "your product",
            "More beans", "than Heinz!",
            "1   5   10   15   20", "This line is - full!",
            "  75 RPM  Level   26", "10.2 Mile   MPH 33.2",
            " 267 kCal C/Min 10.2", " 6.2 METs  00:10:59 "};
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
            sleep_ms(2000);
            lcd->clear();
        }
    }
    //while(1);
    char output[21];
    absolute_time_t start;
    double T1, T2;
    sprintf(output, "I2C speed %-10ld", i2cspeed);
    lcd->setCursor(0, 0);
    lcd->writeString(output);

    start = get_absolute_time();
    int loops = 1000;
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(1, 0);
        lcd->writeString(output);
    }
    T1 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "Set Cur,Wr%7.3f ms", T1);
    lcd->setCursor(1, 0);
    lcd->writeString(output);
    //output[10] = 0;
    start = get_absolute_time();
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(2,0,true); // buffer the cursor command
        lcd->writeString(output,true); // flush after string
        lcd->show();
    }
    T2 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "Write wPos%7.3f ms", T2);
    lcd->setCursor(2, 0);
    lcd->writeString(output);

        sleep_ms(5000);

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
        // test each of the function calls

        lcd->clear();

        // test special characters
        lcd->setCursor(0,0);
        lcd->writeString("01234567");
        lcd->setCursor(1,0);
        for(int i=0;i<16;i++) lcd->writeChar((char) i);
        output[0] = 0xb1;
        for(i=1;i<20;i++) output[i] = i;
        output[20] = 0;
        lcd ->setCursor(2,0);
        lcd->writeString(output); // chars 0x8 t0 0xf mirror special 0-7
        sleep_ms(30000);

        // test buffer overflow
        lcd->clear();
        lcd->setCursor(3,0);
        lcd->writeString("  (Means it works!)");
        char bigoutput[50];
        for (int i=0;i<5;i++) {
            sprintf(bigoutput, "                    %2d Different #'s? %2d", i,i);
            lcd->setCursor(1,0,true);
            lcd->setCursor(2,0,true);
            lcd->writeString(bigoutput, true); // eventually the buffer overflows 
            // but the rest of the data isn't dumped yet ...
            sleep_ms(1500);
        }

        lcd->clear();
        lcd->setCursor(1,0,true);
        lcd->writeString("Turn backlight OFF");
        sleep_ms(1000);
        lcd->setBacklight(0);
        lcd->setCursor(2,0,true);
        lcd->printstr("And then back ON");
        sleep_ms(1000);
        lcd->setBacklight(1);
        sleep_ms(2000);

        lcd->clear();
        lcd->setCursor(1,0,true);
        lcd->blink();
        lcd->writeString("Blank the display");
        sleep_ms(1000);
        lcd->noDisplay();
        lcd->clear();
        lcd->setCursor(1,0,true);
        lcd->writeString("And we're BACK!!!");
        sleep_ms(1000);
        lcd->display();

        sleep_ms(3000);
        lcd->noBlink();


        lcd->clear();
        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor On   ",true);
        lcd->setCursor(1,7,true);
        lcd->cursor();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor Off   ",true);
        lcd->setCursor(1,7,true);
        lcd->noCursor();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor Blink ",true);
        lcd->setCursor(1,7,true);
        lcd->blink();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor Off2 ",true);
        lcd->setCursor(1,7,true);
        lcd->noBlink();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor Both ",true);
        lcd->setCursor(1,7,true);
        lcd->blink();
        lcd->cursor();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor noBlink",true);
        lcd->setCursor(1,7,true);
        //lcd->cursor();
        lcd->noBlink();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Cursor Blink2 ",true);
        lcd->setCursor(1,7,true);
        lcd->blink();
        lcd->noCursor();
        sleep_ms(3000);

        lcd->setCursor(1,0,true);
        lcd->writeString("Scroll Display Right",true);
        lcd->setCursor(1,7,true);
        lcd->blink();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->scrollDisplayRight();
        sleep_ms(1000);
        lcd->setCursor(1,0,true);
        lcd->writeString("Scroll Display Left-",true);
        lcd->setCursor(1,7,true);
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
        lcd->home();
        sleep_ms(3000);

        lcd->clear();
        lcd->blink();
        lcd->setCursor(0,12);
        lcd->autoscroll();
        sleep_ms(2000);
        lcd->writeString("Scroll Left");
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(1,4);
        lcd->noAutoscroll();
        sleep_ms(2000);
        lcd->writeString("Left to Right");
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(2,2);
        lcd->autoscroll();
        lcd->rightToLeft();
        sleep_ms(2000);
        lcd->writeString("Scroll Right");
        sleep_ms(2000);
        lcd->clear();
        lcd->setCursor(3,19);
        lcd->noAutoscroll();
        sleep_ms(2000);
        lcd->writeString("Right to Left");
        sleep_ms(2000);
        lcd->leftToRight();


    }


    //lcd_clear();
    while (1)
    {
        gpio_put(TRIGGER, 1);
        sleep_us(1);
        gpio_put(TRIGGER, 0);
        lcd->setCursor(3, 1);
    }

    return 0;

}