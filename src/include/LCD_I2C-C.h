/***************************************************************************
 * @file LCD_I2C-C.h
 * @author Keith Standiford
 * @date April 10, 2022
 * @brief Header file for C wrapper of the LCD_I2C class in Fast LCD for I2C.
 *
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * @remark Based loosely on the Pico SDK example and the Arduino LiquidCrystal API
 *
 * These are the wrappers to allow the Fast LCD I2C driver (which is written in C++)
 * to be used from a C program. The interface is simplified, and the buffering commands are
 * not available since C does not allow optional arguments. None the less, this allows 
 * users who have not made the transition to C++ to still enjoy substantial speed improvements 
 * over most other drivers.
 * 
 * Use of the C++ version is encouraged.
 **************************************************************************/
#pragma once
#include <stdint.h>
#include <hardware/i2c.h>

///@cond    Do not document in DOXYGEN
    typedef uint8_t byte;
///@endcond

#ifdef __cplusplus
extern "C" {
#endif

    /** @defgroup Cinter The C language interface
     * 
     * On the Pi Pico, if you prefer to work in C a reasonably complete 
     * subset of the program interface is available here with almost 
     * all of the functionality of the C++ version. The advanced buffering commands 
     * are *not* available. Even without them, substantial speed
     * improvements are still achieved. (But C++ is still recommended.)
     * @{
     */

    /** @name Initialization
     * Set up the lcd display. The I2C pins are *not* initialized. 
     * See LCD_I2C_Setup().
     * 
     * 
     * */
    ///@{
    /**
     * @brief Initialize the lcd display.
     * 
     * @param address The I2C address
     * @param columns The LCD's number of columns
     * @param rows The LCD's number of rows (lines)
     * @param I2C The I2C instance
     * 
     * @note If called more than once, the existing LCD class object will be 
     * @note discarded (and destroyed), and a new one created and initialized with the given
     * @note parameters. Use with care!
     * 
     * @warning The Pi Pico pins and the I2C bus are **not** initialized! (But see See LCD_I2C_Setup().)
     */

    void lcd_init(byte address, byte columns, byte rows, i2c_inst_t *I2C) ;
    ///@}


    /** @name Writing to the screen on the Pi Pico in C
     * These are the simplist output routines for writing characters to the LCD screen. They
     * are in the spirit of the example in the Pi Pico SDK.  
     * 
     * Also see the [Theory of Operation](@ref theory) for hints on maximizing display speed.
     */
    ///@{
    /**
     * @brief Output a character to the screen.
     *
     * @param val is the character to display
     */
    void  lcd_writeChar(byte val) ;

    /**
     * @brief Output a string to the screen. 
     * Naming is more in the style of the Pico SDK, and the
     * Enable_Buffering parameter can be utilized.
     *
     * @param str is pointer to the string to display
     */
    void lcd_writeString(const char str[]) ;


    /**
     * @brief Write array of bytes
     * 
     * @param buffer    The array of bytes to output
     * @param size      The number of bytes to write
     */
    void lcd_writeBuffer(const uint8_t *buffer, size_t size) ;

    ///@}

    /** @name Simple display control
     *      
     */
    ///@{
    /**
     * @brief Move the input cursor to a location on the screen.
     * 
     * All output to the screen is placed at the current cursor position. 
     * In the default mode, the cursor then advances one position to the right.
     * The cursor *does not* scroll to the next line.
     *
     */
    /**
     * @param line Specifies the row (or line) on the display
     * @param position Specifies the position on the row (or column) on the display
     */

    void lcd_setCursor(byte line, byte position) ;

    /**
     * @brief Clear the display.
     *
     */
    void lcd_clear(void) ;

    /**
     * @brief "Home" the display. 
     * 
     * Moves cursor to 0,0 and removes any shifts.
     */
    void lcd_home(void) ;


    /**
     * @brief Turn on the display backlight
     *
     */
    void lcd_backlight(void) ;
    
    /**
     * @brief Turn off the display backlight  
     *
     */
    void lcd_noBacklight(void) ;

    /**
     * @brief Turns the display on without modifying data on it.
     *
     * The display is unblanked, and all data sent to the display is displayed.
     */
    void lcd_display(void) ;

    /**
     * @brief Turns the display off without modifying data on it.
     *
     * The display is blanked as though it had been cleared, but the data and cursor
     * position are not changed. 

     */
    void lcd_noDisplay(void) ;

    ///@}

    /** @name Cursor Appearance Controls
     * The cursor can be displayed on the screen as an underscore at the current location,
     * or as a blinking inversion of the character at the cursor location. Both of these modes 
     * can be enabled at once, or they can both be turned off, making the cursor invisible!
     */
    ///@{
    /**
     * @brief Display the underline cursor at the current cursor location.
     */
    void lcd_cursor(void) ;

    /**
     * @brief Hides the underline cursor at the current cursor location.
     */
    void lcd_noCursor(void) ;

    /**
     * @brief Display the blinking inverted cursor at the current cursor location
     */
    void lcd_blink(void) ;

    /**
     * @brief Disable the blinking inverted cursor at the current cursor location
     */
    void lcd_noBlink(void) ;

    ///@}

    /**
     * @name Advanced Display Control
     */

    ///@{
    
    /**
     * @brief Scrolls the display and cursor one position to the left.
     * home() will restore it to its original position
     *
     */
    void lcd_scrollDisplayLeft(void) ;

    /**
     * @brief Scrolls the display and cursor one position to the right.
     * home() will restore it to its original position
     */
    void lcd_scrollDisplayRight(void) ;

    /**
     * @brief Turns on automatic scrolling of the display.
     */
    void lcd_autoscroll(void) ;

    /**
     * @brief Turns off automatic scrolling of the display.
     */
    void lcd_noAutoscroll(void) ;

    /**
     * @brief Sets the text direction to left to right.
     * Each character output advances the cursor one position to the right. 
     * This is the default condition.
     */
    void lcd_leftToRight(void) ;

    /**
     * @brief Sets the text direction to right to left.
     * Each character output advances the cursor one position to the left.
     */
    void lcd_rightToLeft(void) ;
    ///@}

    /**
     * @name Custom Characters
     * The display can store 8 user defined custom characters. They are stored 
     * at "memory addresses" 0-7. They are displayed when character codes 0-7 are 
     * written to the display. Of course, character value "0" cannot be included in a string
     * because it would be the string terminator. A 0 *can* be written using the
     * write single character routines, or the write() method with an array of characters 
     * and a character count. As an alternative, the display controller also *duplicates*
     * the special characters as character codes 8-15 (0x8-0xf), which can be encoded 
     * and written as strings.
     * 
     */

    ///@{

    /**
     * @brief Create a custom character.
     * 
     * Load a custom character into the display by specifying the location in memory to be
     * stored (8 locations maximum, starting from 0) and an array of 8 bytes. The bytes 
     * contain a bit map of the 5x7 dot matrix to display. Each byte specifies one row of
     * character dots starting with the top-most row. The lsb of each byte is the right-most
     * dot on its corresponding line of the character.
     *
     * @param charnum The memory address (character code) 0-7
     * @param char_map The byte array
     */
    void lcd_createChar(byte charnum, const byte char_map[])  ;

/**\} */
/**\} */



///@cond 

#ifndef I2C_Setup_defined
    #define I2C_Setup_defined

    /*  
     * @brief Helper function to set up the I2C bus on Pi Pico
     * 
     * Sets up the I2C bus given instance on the given pins with a specified 
     * clock rate. Defines the pins for the pico tools.
     * 
     * @param I2C   is the I2C bus instance to use. Either i2c0 or i2c1.
     * @param SDA   is the Pi Pico Pin for the I2C SDA (data) line. 
     * @param SCL   is the Pi Pico Pin for the I2C SCL (clock) line.
     * @param I2C_Clock is the I2C bus speed. Typically 100000 or 400000.
     * 
     * @return (int)  The actual *(theoretical)* I2C speed from i2c_init
     * 
     */

    int LCD_I2C_Setup(i2c_inst_t* I2C, uint SDA_Pin, uint SCL_Pin, uint I2C_Clock) ;
;
#endif

///@endcond

#ifdef __cplusplus
}
#endif

