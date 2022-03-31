
#include <memory>
#include <cstdlib>
#include <array>

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "LCD_I2C.hpp"
#include "xLCD_I2C.hpp"



int main()
{
    stdio_init_all();

    sleep_ms(2000);


    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    int i2cspeed = i2c_init(i2c_default, 100 * 1000);
    //    i2c_init(i2c_default, 100 * 1000);

    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_1pin_with_name(SDA, "[SDA] LCD screen serial data pin"))
    bi_decl(bi_1pin_with_name(SCL, "[SCL] LCD screen serial clock pin"))


    constexpr auto LCD_ADDRESS = 0x27;
    constexpr auto LCD_COLUMNS = 20;
    constexpr auto LCD_ROWS = 4;


    auto xlcd = std::make_unique<xLCD_I2C>(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default, SDA, SCL);
    auto lcd = std::make_unique<LCD_I2C>(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS, i2c_default);


    xlcd->Clear();
    xlcd->SetCursor(0,0);
    xlcd->PrintString("Hello World!");
    xlcd->BacklightOn();
    sleep_ms(5000);


    char output[21];
    absolute_time_t start;
    double T1 = 0, T2 = 0;
    int i;

    sprintf(output, "I2C speed %-10ld", i2cspeed);
    lcd->setCursor(0, 0);
    lcd->writeString(output);
    sprintf(output, "4 fld new %6.2f", T1);
    lcd->setCursor(1, 0);
    lcd->writeString(output);
    
    int loops = 1000;

    while (1) {

        start = get_absolute_time();
        for (i = 0; i <= loops; i++)
        {
            lcd->setCursor(1,0, true);
            lcd->writeString("Ti    ", true);
            lcd->setCursor(1,0, true);
            lcd->writeString("  mi  ", true);
            lcd->setCursor(1,0, true);
            lcd->writeString("    ng", true);
            lcd->setCursor(1,0, true);
            lcd->writeString("      ", true);
        }
        T1 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
        sprintf(output, "4 fld new %6.2f", T1);
        lcd->setCursor(1, 0);
        lcd->writeString(output);

        start = get_absolute_time();
        for (i = 0; i <= loops; i++)
        {
            xlcd->SetCursor(0, 0);
            xlcd->PrintString("Timing");
            xlcd->SetCursor(0, 0);
            xlcd->PrintString("Timing");
            xlcd->SetCursor(0, 0);
            xlcd->PrintString("      ");
            xlcd->SetCursor(0, 0);
            xlcd->PrintString("      ");
        }
        T1 = absolute_time_diff_us(start, get_absolute_time()) / (1000.0 * loops);
        sprintf(output, "4 fld old %6.2f", T1);
        xlcd->SetCursor(0, 0);
        xlcd->PrintString(output);
    
    }



    return 0;

}