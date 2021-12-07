/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
//#include "hardware/i2c.h"
#include "i2c.h"
#include "pico/binary_info.h"

/* Example code to drive a 16x2 LCD panel via a I2C bridge chip (e.g. PCF8574)

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 4 (pin 6)-> SDA on LCD bridge board
   GPIO 5 (pin 7)-> SCL on LCD bridge board
   3.3v (pin 36) -> VCC on LCD bridge board
   GND (pin 38)  -> GND on LCD bridge board
*/
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display and cursor control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display and cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flag for backlight control
const int LCD_BACKLIGHT = 0x08;
//const int LCD_BACKLIGHT = 0;

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x4 // Enable bit
#define Rw 0x2 // Read/Write bit
#define Rs 0x1 // Register select bit

#define LCD_ENABLE_BIT En

// By default these LCD display drivers are on bus address 0x27

// Modes for lcd_send_byte
#define LCD_CHARACTER 1
#define LCD_COMMAND 0

#define MAX_LINES 4
#define MAX_CHARS 20

static uint8_t _Addr = 0x27;
static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
static uint8_t _numlines;
static uint8_t _cols;
static uint8_t _rows;
static uint8_t _backlightval = LCD_BACKLIGHT;

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val)
{
#ifdef i2c_default
    i2c_write_blocking(i2c_default, _Addr, &val, 1, false);
#endif
}

void lcd_toggle_enable(uint8_t val)
{
    // Toggle enable pin (clock) on LCD display
    // We cannot do this too quickly or things don't work
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(1); // enable for >450 nm
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(50); // most commands take 37 usec
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode)
{
    uint8_t high = mode | (val & 0xF0) | _backlightval;
    uint8_t low = mode | ((val << 4) & 0xF0) | _backlightval;

    // build sequence of writes for display controller

    char bytes[10];
    uint8_t i = 0;
    bytes[i++] = high;                  // set R/W and command before enable
    bytes[i++] = high | LCD_ENABLE_BIT; // set enable and data, choose operation
    bytes[i++] = high;                  // enable low clocks in data
    bytes[i++] = low | LCD_ENABLE_BIT;  // change data lines, enable high
    bytes[i++] = low;                   // drop enable, clock data

    i2c_write_blocking(i2c_default, _Addr, bytes, i, false);
    /*
    i2c_write_byte(high);
    lcd_toggle_enable(high);
    //i2c_write_byte(low);
    lcd_toggle_enable(low);
*/
}

void lcd_clear(void)
{
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
    sleep_us(2000); // command takes a long time
}

// go to location on LCD
const static uint8_t row_address_offset[4] = {0x80, 0xC0, 0x80 + 20, 0xC0 + 20};

void lcd_set_cursor(uint8_t line, uint8_t position)
{
    uint8_t val = row_address_offset[line] + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static void inline lcd_char(char val)
{
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s)
{
    //    while (*s) lcd_char(*s++);
    char bytes[4 * 20 + 4], c;
    uint8_t i = 0, low, high;

    bytes[i++] = LCD_CHARACTER | _backlightval; // select mode with enable low
    while ((c = *s++) && i < sizeof(bytes) - 4)
    {
        high = LCD_CHARACTER | (c & 0xF0) | _backlightval;
        low = LCD_CHARACTER | ((c << 4) & 0xF0) | _backlightval;

        // build sequence of writes for display controller

        bytes[i++] = high | LCD_ENABLE_BIT; // set enable and data, choose operation
        bytes[i++] = high;                  // enable low clocks in data
        bytes[i++] = low | LCD_ENABLE_BIT;  // change data lines, enable high
        bytes[i++] = low;                   // drop enable, clock data
    }
    i2c_write_blocking(i2c_default, _Addr, bytes, i, false);
}
void lcd_string_pos(const char *s, uint8_t line, uint8_t position)
{
    //    while (*s) lcd_char(*s++);
    char bytes[4 * (20 + 4)], c;
    uint8_t i = 0, low, high;

    // first output command for cursor
    uint8_t val = row_address_offset[line] + position;
    bytes[i++] = LCD_COMMAND + _backlightval; // select command mode
    high = LCD_COMMAND | (val & 0xF0) | _backlightval;
    low = LCD_COMMAND | ((val << 4) & 0xF0) | _backlightval;

    // build sequence of writes for display controller

    bytes[i++] = high | LCD_ENABLE_BIT; // set enable and data, choose operation
    bytes[i++] = high;                  // enable low clocks in data
    bytes[i++] = low | LCD_ENABLE_BIT;  // change data lines, enable high
    bytes[i++] = low;                   // drop enable, clock data

    bytes[i++] = LCD_CHARACTER | _backlightval; // select mode with enable low
    while ((c = *s++) && i < sizeof(bytes) - 4)
    {
        high = LCD_CHARACTER | (c & 0xF0) | _backlightval;
        low = LCD_CHARACTER | ((c << 4) & 0xF0) | _backlightval;

        // build sequence of writes for display controller

        bytes[i++] = high | LCD_ENABLE_BIT; // set enable and data, choose operation
        bytes[i++] = high;                  // enable low clocks in data
        bytes[i++] = low | LCD_ENABLE_BIT;  // change data lines, enable high
        bytes[i++] = low;                   // drop enable, clock data
    }
    i2c_write_blocking(i2c_default, _Addr, bytes, i, false);
}

uint baudtest(uint freq_in, uint baudrate)
{

    // TODO there are some subtleties to I2C timing which we are completely ignoring here
    uint period = (freq_in + baudrate / 2) / baudrate;
    uint lcnt = period * 3 / 5; // oof this one hurts
    uint hcnt = period - lcnt;
    uint lcnt_old = lcnt;
    uint hcnt_old = hcnt;

    printf("\nclock freq %f MHx (%f nS), baud %d\nperiod %d, old lcnt %d, old hcnt = %d\r\n", (float)freq_in / 1e6, 1e9 / (float)freq_in,
           baudrate, period, lcnt, hcnt);

    // Now to tackle a more sophisticated bus timing model.
    /*
    First, we need to note that the RP2040 hardware does not start the high or low counters
    until the signal has completed the previous transition. This means the hardware includes the signal rise
    and fall times automatically. To really achieve the desired baud rate, we need to shorten the high and 
    low times. Indeed, the bus specification gives minimum high and low times and maximum settling times. 
    The sum of these is the proper period for the baud rate. In practice, the Pico easily beats the maximum
    rise and fall times for 100k and 400k specifications. If assume the maximum rise and fall times but
    the hardware works better, the bus will run faster than we expect. We probably want to avoid most of this effect,
    especially at the slowest speed. Here are the specs and our assumed values.

                     Baud Rate
    value            100k      400k     1000k
    T_low min         4.7       1.3      0.5     usec
    T_high min        4.0       0.6      0.26    usec
    T_r+T_f rise+fall 1300      600      240     nsec max

    We assume
    T_r+T_f            300      300      240     nsec typical

    And Target
    T_low              4.9      1.5       0.5    usec
    T_high             4.8      0.7       0.26   usec

    For arbitrary baud rates above 100k, we will linearly interpolate both high and low times. Note
    that this has the effect of also interpolating the expected rise and fall times. Below 100k baud we 
    scale the scale the low time and compute the high time.
    
    */
    static const int T_low_100k = 4900; // in nsec
    static const int T_high_100k = 4800;
    static const int T_low_400k = 1500;
    static const int T_high_400k = 700;
    static const int T_low_1M = 500;
    static const int T_high_1M = 260;
    static const int T_risefall_1M = 240;
    static const int T_risefall_400k = 300;
    static const int T_risefall_100k = 300;
    int T_baud = 1000000000 / baudrate; // in nsec
    int T_low, T_high, freq;

    if (baudrate <= 100000)
    {
        T_low = T_low_100k * 100000 / baudrate; // Scale T_low_100k
        T_high = T_baud - T_risefall_100k - T_low;
    }
    else if (baudrate <= 400000)
    {
        T_low = T_low_400k + ((400000 - baudrate) * (T_low_100k - T_low_400k)) / (400000 - 100000); // Interpolate T_low_400k
        T_high = T_high_400k + ((400000 - baudrate) * (T_high_100k - T_high_400k)) / (400000 - 100000);
    }
    else
    {
        T_low = T_low_1M + ((1000000 - baudrate) * (T_low_400k - T_low_1M)) / (1000000 - 400000); // Interpolate T_low_1M
        T_high = T_high_1M + ((1000000 - baudrate) * (T_high_400k - T_high_1M)) / (1000000 - 400000);
    }
    freq = 1000000000 / T_high;
    hcnt = (freq_in + freq - 1) / freq; // ROUND UP
    freq = 1000000000 / T_low;
    lcnt = (freq_in + freq - 1) / freq;
    // Per I2C-bus specification a device in standard or fast mode must
    // internally provide a hold time of at least 300ns for the SDA signal to
    // bridge the undefined region of the falling edge of SCL. A smaller hold
    // time of 120ns is used for fast mode plus.
    uint sda_tx_hold_count, sda_tx_hold_count_old;
    if (baudrate < 1000000)
    {
        // sda_tx_hold_count = freq_in [cycles/s] * 300ns * (1s / 1e9ns)
        // The latter terms reduce to the 1/frequency with 300 ns period.
        // So we will divide by the corresponding frequency to avoid overflow.
        static const uint FREQ300NS = 1000000000 / 300;

        sda_tx_hold_count_old = ((freq_in * 3) / 10000000) + 1;
        sda_tx_hold_count = (freq_in + FREQ300NS - 1) / FREQ300NS; // ROUND UP
    }
    else
    {
        // sda_tx_hold_count = freq_in [cycles/s] * 120ns * (1s / 1e9ns)
        // The latter terms reduce to the 1/frequency with 120 ns period.
        // So we will divide by the corresponding frequency to avoid overflow.
        static const uint FREQ120NS = 1000000000 / 120;
        sda_tx_hold_count_old = ((freq_in * 3) / 25000000) + 1;
        sda_tx_hold_count = (freq_in + FREQ120NS - 1) / FREQ120NS; // ROUND UP
    }

    // Per I2C-bus specification a device in fast or fast plus mode must
    // filter 50 ns spikes on the SCL and SCA lines.  The I2C hardware spklen
    // register sets the filter width in sys clock intervals with a minimum
    // value of 1. Filtering Low speed can't really hurt.
    uint spklen;
    // sda_tx_hold_count = freq_in [cycles/s] * 50ns * (1s / 1e9ns)
    // The latter terms reduce to the 1/frequency with 50 ns period, 20 MHz.
    // So we will divide by the corresponding frequency to avoid overflow.
    static const uint FREQ50NS = 1000000000 / 50;
    spklen = (freq_in + FREQ50NS - 1) / FREQ50NS; //ROUND UP
    if (spklen == 0)
        spklen = 1;
    printf("hold %d/%d, spklen %d, lcnt %d, hcnt %d\n", sda_tx_hold_count, sda_tx_hold_count_old,
           spklen, lcnt, hcnt);
    printf("low time %f nS, high time %f nS, cycletime %f nS\n",
           (float)(lcnt)*1e9 / (float)freq_in, (float)(hcnt)*1e9 / (float)freq_in,
           (float)(lcnt + hcnt) * 1e9 / (float)freq_in);
    return freq_in / period;
}

void lcd_init()
{
    sleep_ms(50);                     // Need 40 msec after display power up
    i2c_write_byte(0);                // Set expander interface outputs low with backlight
    lcd_send_byte(0x03, LCD_COMMAND); // set 4 bit mode three times ...
    sleep_us(4500);
    lcd_send_byte(0x03, LCD_COMMAND); // two ...
    sleep_us(4500);
    lcd_send_byte(0x03, LCD_COMMAND); // three
    sleep_us(150);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}
#define TRIGGER 14

int main()
{
    stdio_init_all();

    sleep_ms(10000);

    gpio_init(TRIGGER);
    gpio_set_dir(TRIGGER, GPIO_OUT);
    gpio_pull_up(TRIGGER);
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/lcd_1602_i2c example requires a board with I2C pins
#else
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

    //  Test baud settings

    int tests[][2] = {
        {125000000, 400000},
        {125000000, 400001},
        {125000000, 1000000},
        {125000000, 100000},
        {125000000, 100001},
        {125000000, 50000},
        {2700000, 100000},
        {12000000, 400000},
        {32000000, 1000000},

    };
    int cases = sizeof(tests) / __SIZEOF_INT__ / 2;
    for (int i = 0; i < cases; i++)
        baudtest(tests[i][0], tests[i][1]);
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
        lcd_string_pos(output, 2, 0);
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
#endif
}