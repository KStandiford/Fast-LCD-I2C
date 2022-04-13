/**
 * @file LCD_I2C-C.cpp
 * @author Keith Standiford
 * @brief C wrappers for the Fast LCD I2C driver
 * @version 1.0
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * 
 * These are the wrappers to allow the Fast LCD I2C driver (which is written in C++)
 * to be used from a C program. The interface is simplified, and the buffering commands are
 * not available since C does not allow optional arguments. None the less, this allows 
 * users who have not made the transition to C++ to still enjoy substantial speed improvements 
 * over most other drivers.
 * 
 * Use of the C++ version is encouraged.
 */

// Includes for Pi Pico
#include <string.h>
#include "LCD_I2C-C.h"
#include "LCD_I2C.hpp"


//  hidden pointer to LCD_I2C instance

LCD_I2C* lcd = NULL;

#define Default_Addr  0x27
#define MAX_CHARS 20
#define MAX_LINES 4


/*
A helper to test if we have been initialized. If not, initialize anyway with default parameters.
*/

void nullCheck() {
    if(lcd != NULL) return;
    lcd = new LCD_I2C(Default_Addr, MAX_CHARS, MAX_LINES, PICO_DEFAULT_I2C_INSTANCE);
}




// Pi Pico version in C. If already initialized, destroy the old one!

void lcd_init(byte address, byte columns, byte rows, i2c_inst * I2C) {

    if(lcd != NULL) {
        lcd->show();    // be nice and dump the buffer
        sleep_us(2000); // wait for display to be idle for sure...
        delete lcd;
    }
    // create a new instance which will also init the display.
    lcd = new LCD_I2C(address, columns, rows, I2C); 
    
}



void lcd_clear(void)
{
    nullCheck();
    lcd->clear();
}

void lcd_home(void)
{
    nullCheck();
    lcd->home();
}

// go to location on LCD
void lcd_setCursor(byte line, byte position)
{
    nullCheck();
    lcd->setCursor( line,  position);
}


void lcd_writeString(const char s[])
{
    nullCheck();
    lcd->writeString(s);
}

void lcd_writeChar(byte c)
{
    nullCheck();
    lcd->writeChar(c);
}

void lcd_writeBuffer(const uint8_t buffer[], size_t size)
{
    nullCheck();
    lcd->write(buffer, size);
}


void lcd_backlight(void)
{
    nullCheck();
    lcd->backlight();
}

void lcd_noBacklight()
{
    nullCheck();
    lcd->noBacklight();
}

void lcd_cursor(void)
{
    nullCheck();
    lcd->cursor();
}

void lcd_noCursor(void)
{
    nullCheck();
    lcd->noCursor();
}

void lcd_blink(void)
{
    nullCheck();
    lcd->blink();
}

void lcd_noBlink(void)
{
    nullCheck();
    lcd->noBlink();
}

void lcd_display(void)
{
    nullCheck();
    lcd->display();
}

void lcd_noDisplay(void)
{
    nullCheck();
    lcd->noDisplay();
}

void lcd_scrollDisplayLeft(void)
{
    nullCheck();
    lcd->scrollDisplayLeft();
}

void lcd_scrollDisplayRight(void)
{
    nullCheck();
    lcd->scrollDisplayRight();
}

void lcd_autoscroll(void)
{
    nullCheck();
    lcd->autoscroll();
}

void lcd_noAutoscroll(void)
{
    nullCheck();
    lcd->noAutoscroll();
}

void lcd_rightToLeft(void)
{
    nullCheck();
    lcd->rightToLeft();
}

void lcd_leftToRight(void)
{
    nullCheck();
    lcd->leftToRight();
}

void lcd_createChar(byte charnum, const byte char_map[])
{
    nullCheck();
    lcd->createChar(charnum, char_map);
}


