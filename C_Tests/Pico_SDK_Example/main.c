/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "LCD_I2C.hpp"

#define TRIGGER 14

int main()
{
    stdio_init_all();

    sleep_ms(10000);

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

    lcd_init();

    static char *message[] =
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
                lcd_set_cursor(line, (MAX_CHARS / 2) - strlen(message[m + line]) / 2);
                lcd_string(message[m + line]);
            }
            sleep_ms(2000);
            lcd_clear();
        }
    }
    //while(1);
    char output[21];
    absolute_time_t start;
    double T1, T2;
    sprintf(output, "I2C speed %-10ld", i2cspeed);
    lcd_set_cursor(0, 0);
    lcd_string(output);

    start = get_absolute_time();
    int loops = 10000;
    for (i = 0; i <= loops; i++)
    {
        lcd_set_cursor(1, 0);
        lcd_string(output);
    }
    T1 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "Set Cur,Wr%7.3f ms", T1);
    lcd_set_cursor(1, 0);
    lcd_string(output);
    //output[10] = 0;
    start = get_absolute_time();
    for (i = 0; i <= loops; i++)
    {
        //lcd_set_cursor(2,0);
        //lcd_string_pos(output, 2, 0);
    }
    T2 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
    sprintf(output, "Write wPos%7.3f ms", T2);
    lcd_set_cursor(2, 0);
    lcd_string(output);

    //lcd_clear();
    while (1)
    {
        gpio_put(TRIGGER, 1);
        sleep_us(1);
        gpio_put(TRIGGER, 0);
        lcd_set_cursor(3, 1);
    }

    return 0;

}