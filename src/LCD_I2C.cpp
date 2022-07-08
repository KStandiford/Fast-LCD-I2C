/**
 * @file LCD_I2C.cpp
 * @author Keith Standiford
 * @brief C++ Implementation for the Fast LCD I2C driver
 * @version 1.01
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * 
 */

// Includes for Arduino or Pi Pico
//#define ARDUINO 
#ifdef ARDUINO
//#include <inttypes.h>
//#include <Print.h>
//#include <Wire.h>
#include "LCD_I2C.h"
#else
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <string.h>
#include <cstdint>
#include <LCD_I2C.hpp>
#endif






#ifdef ARDUINO
LCD_I2C::LCD_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize) :
	_Addr(lcd_addr),_cols(lcd_cols),_rows(lcd_rows),_charsize(charsize),_backlight(LCD_BACKLIGHT)
{
    if(lcd_rows > MAX_LINES) _rows = MAX_LINES; // check against limits
    if(lcd_cols > MAX_CHARS) _cols = MAX_CHARS;
}
#else

// Pi Pico version

LCD_I2C::LCD_I2C(byte address, byte columns, byte rows, i2c_inst * I2C) :
    _Addr(address),_rows(rows),_cols(columns), _backlight(LCD_NOBACKLIGHT), I2C_instance(I2C)
{        
    if(rows > MAX_LINES) _rows = MAX_LINES; // check against limits
    if(columns > MAX_CHARS) _cols = MAX_CHARS;
    init();
}
#endif

// commands

/* Quick helper function for single byte transfers */
void LCD_I2C::write_byte(byte val,bool Enable_Buffering )   
{
    static byte data;

    // We always use the buffer. So make sure it isn't full
    if(_bufferIn>=BUFFER_LENGTH) show();   // OOPS! It is full, empty it NOW
    data = val | _backlight;

/*   // For Arduino
    #ifdef ARDUINO
    if(_bufferIn == 0)    // there is nothing in the buffer, so start
        Wire.beginTransmission(_Addr);
    Wire.write((int) (data));   // put data in buffer for transmission
    #else
*/
    //  For Pi Pico
    _buffer[_bufferIn] = data;  // put the data in our own buffer
//    #endif

    _bufferIn++;    // Increment buffer position (or counter)
    if(!Enable_Buffering) show(); // If we aren't buffering, then empty the buffer now.
}


void LCD_I2C::send_byte(byte val, int  mode,bool Enable_Buffering )  
{

    static byte high;
    static byte low;
    // We always use the buffer. So make sure it isn't full
    // We may insert 4-5 bytes since we must use nibbles
    // Don't send mode unless it changed
    if(_bufferIn>=BUFFER_LENGTH-4-(_last_mode==mode?0:1)) show();   // OOPS! It is too full, empty it NOW

    high = val &0xF0u | mode;           // get high 4 bit nibble and mode bits
    low = ((val & 0xfu) << 4 | mode);   // get low 4 bit nibble and mode bits
    if(_last_mode != mode){             // if mode changed, we must output it first
        _last_mode = mode;
        write_byte(mode, true);   // Must set cmmd and R/W bits before enable goes high
    }
    write_byte(high | ENABLE, true);    // Raise enable to latch modes, set up data bits
    write_byte(high & ~ENABLE, true);   // Turn enable off to latch data bits of high nibble
    write_byte(low | ENABLE, true);     // Raise enable, set up data bit for low nibble
    write_byte(low & ~ENABLE, true);    // Drop enable to latch low nibble
                                        // This initiates data write or command execution    
    if(!Enable_Buffering) show();       // if we are not supposed to be buffering, do it now!
}


void LCD_I2C::clear(void)
{
    send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
    sleep_us(2000); // command takes a long time
}

void LCD_I2C::home(void)
{
    send_byte(LCD_RETURNHOME, LCD_COMMAND);
    sleep_us(2000); // command takes a long time
}

// go to location on LCD
// Can't believe parameters are swapped between these
#ifdef ARDUINO
void LCD_I2C::setCursor(byte position, byte line, bool Enable_Buffering)
#else
void LCD_I2C::setCursor(byte line, byte position, bool Enable_Buffering)
#endif
{
    if(line >= _rows) line = _rows-1;   // Check against limits for display
    if(position >= _cols) position = _cols-1;
    byte val = row_address_offset[line] + position;
    send_byte(val, LCD_COMMAND, Enable_Buffering);
}


void LCD_I2C::writeString(const char s[], bool Enable_Buffering)
{
    byte c, high, low;

    if(s!=NULL) {           // Trust but verify!
        while(c=*s++) {                     // for all the characters...
            send_byte(c,LCD_CHARACTER,true);    // let him do the work
        }
    }
    if(!Enable_Buffering) show();  // display the result if not told to wait
}

void LCD_I2C::writeChar(byte c, bool Enable_Buffering)
{
    send_byte(c, LCD_CHARACTER, Enable_Buffering);
}

size_t LCD_I2C::write(const uint8_t *buffer, size_t size, bool Enable_Buffering)
{
    if(buffer == NULL)  size=0;     // Don't do anyting if there is no buffer
    size_t i = size;
    while(i--) {
        send_byte(*buffer++,LCD_CHARACTER,true);    // let him do the work
    }
    if (!Enable_Buffering) show();
    return size;
}

void LCD_I2C::init()
{
    _backlight = LCD_BACKLIGHT; // initialize a few variables
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYMODESET; // Roman languages
    _displayfunction = LCD_FUNCTIONSET | LCD_4BITMODE;
    if(_rows > 1)
        _displayfunction |= LCD_2LINE;
    else
        _displayfunction |= LCD_1LINE;
    
    // some 1 line displays allow 10 pixel high characters
    if(_rows==1 && _charsize!=0)
        _displayfunction |= LCD_5x10DOTS;
    else
        _displayfunction |= LCD_5x8DOTS;
    
    _displaycontrol = LCD_DISPLAYCONTROL | LCD_DISPLAYON;



    // Need to wait 40 ms for display to stabilize    

    sleep_ms(50);                   // Need 40 msec after display power up
    write_byte(_backlight);         // Set expander interface outputs low with backlight
    send_byte(0x03, LCD_COMMAND);   // set 4 bit mode three times ...
    sleep_us(4500);                 // Hitachi HD44780 datasheet pg 46
    send_byte(0x03, LCD_COMMAND); // two ...
    sleep_us(4500);
    send_byte(0x03, LCD_COMMAND); // three
    sleep_us(150);
    send_byte(0x02, LCD_COMMAND);

    send_byte(_displaymode, LCD_COMMAND);
    send_byte(_displayfunction, LCD_COMMAND);
    send_byte(_displaycontrol, LCD_COMMAND);
    clear();
}

int LCD_I2C::show()  
{
    int i = _bufferIn;

    if(_bufferIn >0) {  //If there is data in the buffer,...

        // For Arduino, we need to end transmission to send it.
        #ifdef ARDUINO
        Wire.beginTransmission(_Addr);
        Wire.write(_buffer,_bufferIn);
        Wire.endTransmission();
        #else
        // For Pi Pico, we do an I2C write pointing at our own buffer
        i2c_write_blocking(I2C_instance, _Addr, _buffer, _bufferIn, false);
        #endif
    }
    _bufferIn = 0;  // and set the buffer to empty
    return i;

}

void LCD_I2C::backlight(void)
{
    _backlight = LCD_BACKLIGHT;
    write_byte(_backlight); // Send interface command
}

void LCD_I2C::noBacklight()
{
    _backlight = LCD_NOBACKLIGHT;
    write_byte(_backlight); //Send interface command
}

void LCD_I2C::cursor(void)
{
    _displaycontrol |= LCD_CURSORON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::noCursor(void)
{
    _displaycontrol &= ~LCD_CURSORON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::blink(void)
{
    _displaycontrol |= LCD_BLINKON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::noBlink(void)
{
    _displaycontrol &= ~LCD_BLINKON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::display(void)
{
    _displaycontrol |= LCD_DISPLAYON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::noDisplay(void)
{
    _displaycontrol &= ~LCD_DISPLAYON;
    send_byte(_displaycontrol,LCD_COMMAND);
}

void LCD_I2C::scrollDisplayLeft(void)
{
    send_byte(LCD_DISPLAYSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, LCD_COMMAND);
}

void LCD_I2C::scrollDisplayRight(void)
{
    send_byte(LCD_DISPLAYSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, LCD_COMMAND);
}

void LCD_I2C::autoscroll(void)
{
    _displaymode |= LCD_DISPLAYENTRYSHIFT;
    send_byte(_displaymode, LCD_COMMAND);
}

void LCD_I2C::noAutoscroll(void)
{
    _displaymode &= ~LCD_DISPLAYENTRYSHIFT;
    send_byte(_displaymode, LCD_COMMAND);
}

void LCD_I2C::rightToLeft(void)
{
    _displaymode &= ~LCD_ENTRYLEFT;
    send_byte(_displaymode, LCD_COMMAND);
}

void LCD_I2C::leftToRight(void)
{
    _displaymode |= LCD_ENTRYLEFT;
    send_byte(_displaymode, LCD_COMMAND);
}

void LCD_I2C::createChar(byte charnum, const byte char_map[])
{
    #define MAXCHARNUM 7
    #define CUSTOMCHARSIZE 8
    if(charnum > MAXCHARNUM) charnum = MAXCHARNUM;
    send_byte(LCD_SETCGRAMADDR | charnum << 3, LCD_COMMAND,true);   // set ram address
    for(int i=0;i<CUSTOMCHARSIZE;i++) {
        send_byte(char_map[i], LCD_CHARACTER, true);
    }
    setCursor(0,0); // go back to data ram addressing (and flush buffer)
}

#ifndef ARDUINO
extern "C" int LCD_I2C_Setup(i2c_inst_t* I2C, uint SDA_Pin, uint SCL_Pin, uint I2C_Clock) {


    gpio_set_function(SDA_Pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_Pin, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_Pin);
    gpio_pull_up(SCL_Pin);
    // Make the I2C pins available to picotool
    bi_decl(bi_1pin_with_name(SDA_Pin, "[SDA] LCD screen serial data pin"))
    bi_decl(bi_1pin_with_name(SCL_Pin, "[SCL] LCD screen serial clock pin"))
    bi_decl(bi_2pins_with_func(SDA_Pin, SCL_Pin, GPIO_FUNC_I2C));

    int i2cspeed = i2c_init(I2C, 100 * 1000);

    return i2cspeed;

}
#endif
