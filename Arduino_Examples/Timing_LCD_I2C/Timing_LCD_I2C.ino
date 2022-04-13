/**
 * @file Timing_LCD_I2C.ino
 * @author Keith Standiford
 * @brief Compare Timing of Fast LCD I2C against Liquicrystal I2C
 * @version 1.0
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved.
 * 
 * This sketch compares the timing of the FAST LCD I2C driver against
 * another library named LiquidCrystal_I2C, which is NOT supplied here.
 * There is a standard Arduino library which can be used, or you may
 * download one from an online source like github. BOTH libraries will 
 * be talking to the same display! This does usually work, but your
 * mileage may vary. (The stock Arduino library works.)
 * 
 */

#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>
#include <LCD_I2C.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27
#define LDC_ADDR 0x27
// Set up for a 16 chars and 2 line display
LCD_I2C lcd(LDC_ADDR, 16, 2);
LiquidCrystal_I2C lq(LDC_ADDR, 16, 2); // The stock version


void setup()
{
	// initialize the LCD
 int WireClock = 100000;
 Wire.setClock(WireClock);
 lcd.begin();
 lq.begin();
 Serial.begin(9600);
 

	// Turn on the blacklight and print a message.
	lcd.backlight();
  lq.backlight();
	lq.print("Hello, world!");
  delay(5000);
  lcd.setCursor(0,0);
  lcd.print("I2C speed ");
  lcd.print(WireClock);
  delay(5000);

}

void loop()
{
	unsigned long int start_time,end_time, elapsed_time;
  float  T1, T2;
  int loops = 1000;

  while (1) {
    start_time = millis();
    for(int i =0;i<loops;i++) {
    lcd.setCursor(0,1,true);
    lcd.write("Ti    ",6,true);
    lcd.setCursor(0,1,true);
    lcd.write("  mi  ",6,true);
    lcd.setCursor(0,1,true);
    lcd.write("    ng",6,true);
    lcd.setCursor(0,1,true);
    lcd.write("      ",6,false);
    }
    end_time = millis();
    elapsed_time = end_time-start_time;
    T1 = (float) elapsed_time / float (loops);
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("4 Flds new ");
    lcd.print(T1,2);
  
    start_time = millis();
    for(int i =0;i<loops;i++) {
    lq.setCursor(0,0);
    lq.print("Ti    ");
    lq.setCursor(0,0);
    lq.print("  mi  ");
    lq.setCursor(0,0);
    lq.print("    ng");
    lq.setCursor(0,0);
    lq.print("      ");
    }
    end_time = millis();
    elapsed_time = end_time-start_time;
    T2 = ((float) elapsed_time / float (loops));
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("4 Flds old ");
    lcd.print(T2,2);
  }
  
  
}
