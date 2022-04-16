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
#include <LCD_I2C-C.h>
#include <LCD_I2C.hpp>


//  hidden pointer to LCD_I2C instance

LCD_I2C* lcd = NULL;

#define Default_Addr  0x27
#define MAX_CHARS 20
#define MAX_LINES 4


/*
A helper to test if we have been initialized. If not, initialize anyway with default parameters.
*/

void nullCheck()   noexcept {
    if(lcd != NULL) return;
    lcd = new LCD_I2C(Default_Addr, MAX_CHARS, MAX_LINES, PICO_DEFAULT_I2C_INSTANCE);
}




// Pi Pico version in C. If already initialized, destroy the old one!

void lcd_init(byte address, byte columns, byte rows, i2c_inst * I2C)   noexcept {

    if(lcd != NULL) {
        lcd->show();    // be nice and dump the buffer
        sleep_us(2000); // wait for display to be idle for sure...
        delete lcd;
    }
    // create a new instance which will also init the display.
    lcd = new LCD_I2C(address, columns, rows, I2C); 
    
}



void lcd_clear(void)  noexcept
{
    nullCheck();
    lcd->clear();
}

void lcd_home(void)  noexcept
{
    nullCheck();
    lcd->home();
}

// go to location on LCD
void lcd_setCursor(byte line, byte position)  noexcept
{
    nullCheck();
    lcd->setCursor( line,  position);
}


void lcd_writeString(const char s[])  noexcept
{
    nullCheck();
    lcd->writeString(s);
}

void lcd_writeChar(byte c)  noexcept
{
    nullCheck();
    lcd->writeChar(c);
}

void lcd_writeBuffer(const uint8_t buffer[], size_t size)  noexcept
{
    nullCheck();
    lcd->write(buffer, size);
}


void lcd_backlight(void)  noexcept
{
    nullCheck();
    lcd->backlight();
}

void lcd_noBacklight()  noexcept
{
    nullCheck();
    lcd->noBacklight();
}

void lcd_cursor(void)  noexcept
{
    nullCheck();
    lcd->cursor();
}

void lcd_noCursor(void)  noexcept
{
    nullCheck();
    lcd->noCursor();
}

void lcd_blink(void)  noexcept
{
    nullCheck();
    lcd->blink();
}

void lcd_noBlink(void)  noexcept
{
    nullCheck();
    lcd->noBlink();
}

void lcd_display(void)  noexcept
{
    nullCheck();
    lcd->display();
}

void lcd_noDisplay(void)  noexcept
{
    nullCheck();
    lcd->noDisplay();
}

void lcd_scrollDisplayLeft(void)  noexcept
{
    nullCheck();
    lcd->scrollDisplayLeft();
}

void lcd_scrollDisplayRight(void)  noexcept
{
    nullCheck();
    lcd->scrollDisplayRight();
}

void lcd_autoscroll(void)  noexcept
{
    nullCheck();
    lcd->autoscroll();
}

void lcd_noAutoscroll(void)  noexcept
{
    nullCheck();
    lcd->noAutoscroll();
}

void lcd_rightToLeft(void)  noexcept
{
    nullCheck();
    lcd->rightToLeft();
}

void lcd_leftToRight(void)  noexcept
{
    nullCheck();
    lcd->leftToRight();
}

void lcd_createChar(byte charnum, const byte char_map[])  noexcept
{
    nullCheck();
    lcd->createChar(charnum, char_map);
}


