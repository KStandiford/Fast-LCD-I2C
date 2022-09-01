/**
 * @file LCD_I2C.hpp
 * @author Keith Standiford
 * @brief C++ Implementation for the Fast LCD I2C driver
 * @version 1.01
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved. 
 * @remark Based loosely on the Pico SDK example and the Arduino LiquidCrystal API
 * @remark Compiles for Arduino or for Pi Pico
 * 
 */
#pragma once
//#define ARDUINO 

//  If we are compiling for Arduino, we need some extra files

#ifdef ARDUINO
#include <Arduino.h>
#include <inttypes.h>
#include <Print.h>
#include <Wire.h>
//  and some function alias'
inline void sleep_ms(uint32_t time) {delay(time);}
inline void sleep_us(uint64_t time) {delayMicroseconds(time);}
#else
#include <stdint.h>
#endif

//  For Arduino, we are part of the print class
//  For Pi Pico, we are stand alone
/**
 * @brief A class for *efficiently* driving an LDC display connected to the I2C bus on Arduino or Pi Pico
 * 
 * For the Arduino, the class is derived from the Print class so that all the Arduino Print or
 * write methods can be used.
 * 
 * For the Pi Pico, this is a stand-alone class.
 * \nosubgrouping
 * 
 */
#ifdef ARDUINO
class LCD_I2C : public Print {
#else
class LCD_I2C {
#endif
 private:

    using byte = uint8_t;

    // commands
    static constexpr byte  LCD_CLEARDISPLAY = 0x01;
    static constexpr byte  LCD_RETURNHOME = 0x02;
    static constexpr byte  LCD_ENTRYMODESET = 0x04;
    static constexpr byte  LCD_DISPLAYCONTROL = 0x08;
    static constexpr byte  LCD_DISPLAYSHIFT = 0x10;
    static constexpr byte  LCD_FUNCTIONSET = 0x20;
    static constexpr byte  LCD_SETCGRAMADDR = 0x40;
    static constexpr byte  LCD_SETDDRAMADDR = 0x80;

    // flags for display entry mode
    // use with LCD_ENTRYMODESET
    static constexpr byte  LCD_ENTRYRIGHT = 0x00;
    static constexpr byte  LCD_ENTRYLEFT = 0x02;
    static constexpr byte  LCD_DISPLAYENTRYSHIFT = 0x01;
    static constexpr byte  LCD_NODISPLAYENTRYSHIFT = 0x00;

    // flags for display and cursor control
    // use with LCD_DISPLAYCONTROL
    static constexpr byte  LCD_DISPLAYON = 0x04;
    static constexpr byte  LCD_DISPLAYOFF = 0x00;
    static constexpr byte  LCD_CURSORON = 0x02;
    static constexpr byte  LCD_CURSOROFF = 0x00;
    static constexpr byte  LCD_BLINKON = 0x01;
    static constexpr byte  LCD_BLINKOFF = 0x00;

    // flags for display and cursor shift
    // use with LCD_DISPLAYSHIFT
    static constexpr byte  LCD_DISPLAYMOVE = 0x08;
    static constexpr byte  LCD_CURSORMOVE = 0x00;
    static constexpr byte  LCD_MOVERIGHT = 0x04;
    static constexpr byte  LCD_MOVELEFT = 0x00;

    // flags for function set
    static constexpr byte  LCD_8BITMODE = 0x10;
    static constexpr byte  LCD_4BITMODE = 0x00;
    static constexpr byte  LCD_2LINE = 0x08;
    static constexpr byte  LCD_1LINE = 0x00;

    // Control bits in the I2C interface data words
    static constexpr byte  LCD_BACKLIGHT = 0x08;
    static constexpr byte  LCD_NOBACKLIGHT = 0x00;

    static constexpr byte  En = 0x4; // Enable bit
    static constexpr byte  Rw = 0x2; // Read/Write bit
    static constexpr byte  Rs = 0x1; // Register select bit

    static constexpr byte  ENABLE = En;

    // By default these LCD display drivers are on bus address 0x27

    // Modes for lcd_send_byte
    static constexpr byte  LCD_CHARACTER = 1;
    static constexpr byte  LCD_COMMAND = 0;

    static constexpr byte  MAX_LINES = 4;
    static constexpr byte  MAX_CHARS = 20;

    byte  _Addr;
    byte _displayfunction;
    byte _displaycontrol;
    byte _displaymode;
    byte _cols;
    byte _rows;
    byte _charsize = 0;     // used as boolean flag for 10 pixel high characters
    byte _backlight;
    byte _last_mode;

    uint8_t row_address_offset[MAX_LINES] = {0x80, 0xC0, 0x80 + 20, 0xC0 + 20};
    
    /*
     * For Arduino, the I2C interface (Wire) has an internal buffer
     * The buffer length is defined as BUFFER_LENGTH.
     * Care MUST be taken, since ONLY BUFFER_LENGTH characters can be sent 
     * in a singe transmission, and excess characters will be discarded.
     * Note that BUFFER_LENGTH varies and can be quite short. UNO is about 30
     * characters, while the Pi Pico implementation is over 120! 
     * 
     * In the Pi Pico SDK, the I2C interface does not buffer, so we can set the
     * size to suit ourselves.
     * 
     * Note that we will ALWAYS buffer internally and transmit in a block, since
     * testing with Arduino showed that it was faster than sending single bytes
     * to the Arduino Wire routines.
     * */
    #ifdef ARDUINO
        // The default Arduino length definition is BUFFER_LENGTH
        // The Pi Pico Arduino version of wire.h defines a DIFFERENT variable name
        // than some of the other Arduino environments. So we will check!
        // If we can't find a length we will assume 30 bytes...
        #ifndef BUFFER_LENGTH
            #ifdef WIRE_BUFFER_SIZE
                static constexpr size_t  BUFFER_LENGTH = WIRE_BUFFER_SIZE;
            #else
                static constexpr size_t  BUFFER_LENGTH = 30;
            #endif
        #endif
    #else
    static constexpr size_t  BUFFER_LENGTH = 128;
    #endif

    byte _buffer[BUFFER_LENGTH];


    /*
     * This is the buffer pointer (and character counter).
     * 
     */
    size_t _bufferIn = 0;  

    /*
     * The Pico system needs to know which I2C hardware to use, so we need
     * to save it! Note that on Arduino systems with more than one I2C bus,
     * only the first one pointed to by wire can be used!
    */
    #ifndef ARDUINO
    i2c_inst *I2C_instance {nullptr};
    #endif

    /**
     * Output a byte to the interface chip.
     *
     * @param val Value to be written
     * @param Enable_Buffering If true, data is added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display. 
     */
    void write_byte(byte val, bool Enable_Buffering = false)  noexcept;

    /**
     * Output a byte to the display as two 4 bit nibbles.
     *
     * @param val Value to be written
     * @param mode Specifies command or data mode bits 'or'ed with data
     * @param Enable_Buffering If true, data is added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display. 
     */
    void send_byte(byte val, int mode, bool Enable_Buffering = false)  noexcept;

    /**
     * Helper function to put the display in a known state.
     *
     * In 4 bit mode, cleared, with the cursor at 0,0
     * Display enabled, backlight on
     */
    void init()  noexcept;


 public:

 #ifdef ARDUINO
 // DO NOT Document for Arduino
 ///@cond
 #endif

    /**
     * @brief A reminder of the size of custom character arrays
     * 
     */
    static constexpr uint8_t CUSTOM_SYMBOL_SIZE = 8;

    /** @brief needed for Arduino Constructor */
    static constexpr byte  LCD_5x10DOTS = 0x04; 
    /** @brief needed for Arduino Constructor */
    static constexpr byte  LCD_5x8DOTS = 0x00; 
#ifdef ARDUINO
// end conditional documentation and begin Arduino_diff group
///@endcond
/**
 * @defgroup Arduino_diff  Differences in the C++ interfaces for Arduino
 * 
 * In the Arduino environment, the %LCD_I2C class constructor is different, as well as the setCursor
 * function parameters. Here are the differences.
 */
#endif



    #ifdef ARDUINO
    /** @name Arduino Constructor and Start-up Functions
     * 
     * The Arduino object constructor, and the start-up begin function 
     */
    ///@{

    /**
	 * @brief The ARDUINO Constructor
     * 
     * Remember that constructor usage for ARDUINO and Pi Pico is quite different.
     * 
     * For ARDUINO:
     * Create the object with the constructor and then call the %begin()
     * method. If you wish to change the bus speed, do it *before*
     * calling %begin().
     *  
	 *
	 * @param lcd_addr	I2C slave address of the LCD display. Most likely printed on the
	 *					LCD circuit board, or look in the supplied LCD documentation.
	 * @param lcd_cols	Number of columns your LCD display has.
	 * @param lcd_rows	Number of rows (lines) your LCD display has.
	 * @param charsize	The size in dots that the display has, use LCD_5x10DOTS or LCD_5x8DOTS. (5x8 is
     * the default if charsize is omitted.)
     * 
     * @ingroup Arduino_diff
	 */
	LCD_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize = LCD_5x8DOTS) noexcept;

	/**
	 * @brief For Arduino, initialize the I2C bus and the display
     * 
     * The I2C bus is initialized by calling Wire.begin(). Then the LCD display 
     * is placed in the correct initial state. 
     * %begin() must be called before anything else is done.
     * 
     * @ingroup Arduino_diff
	 */
	inline void begin() noexcept {
        Wire.begin();
        init();
    } ;
    ///@}

    ///@cond

    #else

    /** @name Pi Pico Constructor
     */
    ///@{

    /**
     * @brief The Pi Pico Constructor
     * 
     * 
     * @param address The I2C address
     * @param columns The LCD's number of columns
     * @param rows The LCD's number of rows (lines)
     * @param I2C The I2C instance
     * @note The Pi Pico pins and the I2C bus are **not** initialized! (See LCD_I2C_Setup())
     * 
     * The Pico constructor initializes the object using the
     * provided I2C instance and calls the display's Init() function.
     * 
     * Remember that constructor usage for ARDUINO and Pi Pico is quite different.
     * 
     * For ARDUINO:
     * Create the object with the constructor and then call the begin()
     * method. If you wish to change the bus speed, do it *before*
     * calling begin().
     * 
     * For PI PICO:
     * Initialize the I2C instance and pins (See LCD_I2C_Setup()) 
     * and then create the object by calling the constructor.
     * The bus speed is set during the I2C bus initialization.
     * 
     */
    LCD_I2C(byte address, byte columns, byte rows, i2c_inst *I2C = PICO_DEFAULT_I2C_INSTANCE) noexcept;
    ///@}

    #endif


    /** @name Writing to the screen on the Pi Pico
     * These are the simplest output routines for writing characters to the LCD screen. They
     * are in the spirit of the example in the Pi Pico SDK. Of course the Arduino routines below can
     * also be used on the Pico, and these routines can be used on Arduino. 
     * 
     * Also see the [Theory of Operation](@ref theory) for hints on maximizing display speed.
     */
    ///@{
    /**
     * @brief Output a character to the screen.
     *
     * @param val is the character to display
     * @param Enable_Buffering If true, data is simply added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display. 
     */
    void  writeChar(byte val, bool Enable_Buffering = false) noexcept;

    /**
     * @brief Output a string to the screen. 
     * Naming is more in the style of the Pico SDK, and the
     * Enable_Buffering parameter can be utilized.
     *
     * @param str is pointer to the string to display
     * @param Enable_Buffering If true, data is simply added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display. 
     */
    void writeString(const char str[], bool Enable_Buffering = false) noexcept;

    /**
     * @brief An alias for writeString (for compatibility with earlier programs)
     * 
     * @param str is a pointer to the string to display
     */
	inline void printstr(const char str[]) noexcept 
    {writeString(str ); } ;
    ///@}

    /** @name Writing to the screen on Arduino
     * On the Arduino, the LCD I2C class is derived from the Arduino Print class, so all
     * the print() methods need to work. These routines override various forms of the write() methods 
     * in the print class to efficiently direct the data from the print() methods to the LCD display. 
     * Of course these methods can be called directly on the Arduino or the Pi Pico as well. 
     * 
     * Also see the [Theory of Operation](@ref theory) for hints on maximizing display speed.
     *      
     */
    ///@{

    /**
     * @brief Override the write byte method of the Arduino print class
     * 
     * @param c the byte to output
     * @return (size_t) The number of bytes written
     */
    inline size_t write(uint8_t c) noexcept 
    { writeChar(c); return 1; } ;


    /**
     * @brief Override the write array of bytes method of the Arduino print class
     * 
     * This is the method used by the print strings and some of the print
     * number forms within the Print class as well.
     * 
     * @param buffer    The array of bytes to output
     * @param size      The number of bytes to write
     * @param Enable_Buffering  If true, the data is simply added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display.
     * @return (size_t)   The number of bytes written
     */
    size_t write(const uint8_t *buffer, size_t size, bool Enable_Buffering = false) noexcept;

    /**
     * @brief Override write string method of the Arduino Print class 
     * 
     * @param str   A pointer to the string to output
     * @return (size_t)   The number of bytes written
     */
    inline size_t write(const char *str) noexcept 
    {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    } ;

    /**
     * @brief Override the write buffer of type char method from Arduino print
     *  
     * @param buffer    The array of type char to output
     * @param size      The number of characters to write
     * @param Enable_Buffering  If true, the data is simply added to the output buffer.
     * If false or missing, data is added to the output buffer and the buffer
     * is immediately written to the display.
     * @return (size_t)   The number of characters written
     */

    inline size_t write(const char *buffer, size_t size, bool Enable_Buffering = false) noexcept 
    {
      return write((const uint8_t *)buffer, size, Enable_Buffering);
    } ;
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
    #ifndef ARDUINO
    /**
     * @param line Specifies the row (or line) on the display
     * @param position Specifies the position on the row (or column) on the display
     * @param Enable_Buffering If true, the command is simply added to the output buffer.
     * If false or missing, the command is added to the output buffer and the buffer
     * is immediately written to the display. 
     * @warning Line and position parameters **swap positions** between Arduino and Pi Pico SDK!
     * @warning (So Sad!) This is the Pi Pico version!
     */

    void setCursor(byte line, byte position, bool Enable_Buffering= false) noexcept;
    #else
    ///@endcond
    /** @name Arduino Cursor Positioning
     *      
     */
    ///@{
    /**
     * @brief For Arduino, move the input cursor to a location on the screen.
     * 
     * All output to the screen is placed at the current cursor position. 
     * In the default mode, the cursor then advances one position to the right.
     * The cursor *does not* scroll to the next line.
     *
     */

    /**
     * @param position Specifies the position on the row (or column) on the display
     * @param line Specifies the row (or line) on the display
     * @param Enable_Buffering If true, the command is simply added to the output buffer.
     * If false or missing, the command is added to the output buffer and the buffer
     * is immediately written to the display. 
     * @warning Line and position parameters **swap positions** between Arduino and Pi Pico SDK!
     * @warning (So Sad!) This is the ARDUINO version!
     * 
     * @ingroup Arduino_diff
     */

    void setCursor(byte position, byte line, bool Enable_Buffering= false) noexcept;
    ///@cond
    #endif    

    /**
     * @brief Clear the display.
     *
     * All data on the display is erased and the cursor returns to the top left (0,0) location.
     * This command causes the buffer to be output to the display before execution. It also 
     * takes a long time. 
     */
    void clear(void) noexcept;

    /**
     * @brief "Home" the display. 
     * 
     * Moves cursor to 0,0 and removes any shifts.
     * This command causes the buffer to be output to the display before execution. It also 
     * takes a long time.
     */
    void home(void) noexcept;


    /**
     * @brief Turn on the display backlight
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void backlight(void) noexcept;
    
    /**
     * @brief Turn off the display backlight  
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void noBacklight(void) noexcept;

    /**
     * @brief Set the backlight to a given value 
     *
     * This command causes the buffer to be output to the display before execution. 
     *
     * @param newVal is backlight value, != 0 is ON
     */
    inline void setBacklight(uint8_t newVal) noexcept 
    {
        newVal?backlight():noBacklight();
    };

    /**
     * @brief Turns the display on without modifying data on it.
     *
     * The display is unblanked, and all data sent to the display is displayed.
     * This command causes the buffer to be output to the display before execution. 
     */
    void display(void) noexcept;

    /**
     * @brief Turns the display off without modifying data on it.
     *
     * The display is blanked as though it had been cleared, but the data and cursor
     * position are not changed. 
     * This command causes the buffer to be output to the display before execution. 

     */
    void noDisplay(void) noexcept;

    ///@}

    /** @name Cursor Appearance Controls
     * The cursor can be displayed on the screen as an underscore at the current location,
     * or as a blinking inversion of the character at the cursor location. Both of these modes 
     * can be enabled at once, or they can both be turned off, making the cursor invisible!
     */
    ///@{
    /**
     * @brief Display the underline cursor at the current cursor location.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void cursor(void) noexcept;
    /**
     * @brief Alias for cursor() to turn the underline cursor on.
     * 
     */
	inline void cursor_on() noexcept 
    { cursor(); }

    /**
     * @brief Hides the underline cursor at the current cursor location.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void noCursor(void) noexcept;
    /**
     * @brief An alias for noCursor to turn off the underline cursor.
     * 
     */
	inline void cursor_off() noexcept 
    { noCursor(); }

    /**
     * @brief Display the blinking inverted cursor at the current cursor location
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void blink(void) noexcept;

    /**
     * @brief Alias for blink() to display the blinking inverted cursor
     * 
     */
	inline void blink_on() noexcept 
    { blink(); }

    /**
     * @brief Disable the blinking inverted cursor at the current cursor location
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void noBlink(void) noexcept;

    /**
     * @brief Alias for noBlink() to turn off the blinking inverted cursor
     * 
     */
	inline void blink_off() noexcept 
    { noBlink(); }
    ///@}
    /**
     * @name Advanced Display Control
     */

    ///@{
    
    /**
     * @brief Scrolls the display and cursor one position to the left.
     * home() will restore it to its original position
     *
     * This command is never buffered and causes the buffer to be flushed
     * before execution.
     */
    void scrollDisplayLeft(void) noexcept;

    /**
     * @brief Scrolls the display and cursor one position to the right.
     * home() will restore it to its original position
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void scrollDisplayRight(void) noexcept;

    /**
     * @brief Turns on automatic scrolling of the display.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void autoscroll(void) noexcept;

    /**
     * @brief Turns off automatic scrolling of the display.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void noAutoscroll(void) noexcept;

    /**
     * @brief Sets the text direction to left to right.
     * Each character output advances the cursor one position to the right. 
     * This is the default condition.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void leftToRight(void) noexcept;

    /**
     * @brief Sets the text direction to right to left.
     * Each character output advances the cursor one position to the left.
     *
     * This command causes the buffer to be output to the display before execution. 
     */
    void rightToLeft(void) noexcept;
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
     * This command causes the buffer to be output to the display before execution. 
     *
     * @param charnum The memory address (character code) 0-7
     * @param char_map The byte array
     */
    void createChar(byte charnum, const byte char_map[])  noexcept;
    /**
     * @brief An alias for createChar() for loading custom character data
     * 
     * @param char_num is the character number to create (0-7)
     * @param rows is the dot matrix data for the character
     */
	inline void load_custom_character(uint8_t char_num, uint8_t *rows) noexcept
    {	// alias for createChar()
        createChar(char_num, rows);
    };
    ///@}

    /**
     * @name Advanced Buffering Control
     * 
     */
    ///@{

    /**
     * @brief Empties the buffer to "show" any data not yet output to the screen.
     * 
     * Calling show when the buffer is empty is explicitly allowed! See the [Theory of Operation](@ref theory) for
     * a discussion about usage. This routine is *not* needed unless the
     * advanced buffering capabilities have been used by calling output and setCursor routines
     * with the enableBuffering parameter set to TRUE.
     * 
     * @return (int)  The number of bytes transmitted. Remember that each character or command sent to the display
     * may generate 4 or 5 bytes of output to be transmitted, so this will *not* match the number of characters 
     * or comands written.
     * 
     */
    int show(void) noexcept;
    ///@}
    #ifdef ARDUINO
    ///@endcond 
    #endif
};

#ifndef ARDUINO
#ifndef I2C_Setup_defined
    ///@cond    do not document in DOXYGEN
    #define I2C_Setup_defined
    ///@endcond

    /** @defgroup I2C_helper I2C Setup helper for Pi Pico
     * 
     * In the Pi Pico environment, there is a helper function to set up the 
     * I2C bus and the pins required.
     */

    /**
     * @brief Helper function to set up the I2C bus on Pi Pico
     * 
     * Sets up the I2C bus given instance on the given pins with a specified clock rate. Defines the pins for the
     * pico tools. It can be called from C *or* C++.
     * 
     * @param I2C   is the I2C bus instance to use. Either i2c0 or i2c1.
     * @param SDA_Pin   is the Pi Pico Pin for the I2C SDA (data) line. 
     * @param SCL_Pin   is the Pi Pico Pin for the I2C SCL (clock) line.
     * @param I2C_Clock is the I2C bus speed. Typically 100000 or 400000.
     * 
     * @return (int)  The actual (theoretical) I2C speed from i2c_init
     * 
     * @ingroup I2C_helper
     */

    extern "C" int LCD_I2C_Setup(i2c_inst_t* I2C, uint SDA_Pin, uint SCL_Pin, uint I2C_Clock)   noexcept;
;
#endif
#endif


