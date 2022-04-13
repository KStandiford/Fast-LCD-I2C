/**
 * @file Demo1602.ino
 * @author Keith Standiford
 * @brief Demo all display modes on 1602 display
 * @version 1.0
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022 Keith Standiford. All rights reserved.
 * 
 * This sketch demonstrates all operating modes of the display on a 
 * 2 line, 16 character per line display, model 1602.
 */


#include <Wire.h>
#include <LCD_I2C.h>


// we have a 16 char, 2 line display at address 027 hex
#define MAX_LINES 2
#define MAX_CHARS 16
#define LCD_ADDRESS 0x27
#define WIRESPEED 100000

// pointer to LCD object

LCD_I2C* lcd = NULL;

/**
 * @brief Helper Function to write characters slowly so screen action can be seen
 * 
 * @param LCD pointer to the LCD opbjec
 * @param str string to write
 * @param msDelay mSec to wait between characters
 */
void slowWrite(LCD_I2C *LCD, const char str[], uint32_t msDelay ) {
    if (str != NULL) while(char c = *str++) {
        LCD->print(c);
        if(msDelay != 0) delay(msDelay);
    }
}

//  Set up the display
void setup()
{
  lcd = new LCD_I2C(LCD_ADDRESS, MAX_CHARS, MAX_LINES);

  // initialize the LCD
  delay(2000);  // give the user a chance to focus ...
  Wire.setClock(WIRESPEED);
  lcd->begin();
  Serial.begin(9600);

  lcd->setCursor(0,0);
  slowWrite(lcd,"Hello ",500);
  if(MAX_CHARS<20) 
    lcd->setCursor(0,1);  // new line on 1602's!
  lcd->print("Arduino World!");     
  delay(3000);
  lcd->clear();

}

void loop()
{
    unsigned long int start_time;
    char output[21];
    double T1, T2;
    uint16_t i,m;
    
    sprintf(output, "I2C clk %-3ld kbps", WIRESPEED/1000);
    lcd->setCursor(0, 0);
    lcd->print(output);

    start_time = millis();
    int loops = 1000;
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(0, 0);
        lcd->print(output);
    }
    T1 = (millis()-start_time) / (1 * (double) loops);
    sprintf(output, "noBfr%7.3f ms", T1);
    lcd->setCursor(0,1);
    lcd->writeString(output);
    start_time = millis();
    for (i = 0; i <= loops; i++)
    {
        lcd->setCursor(0,1,true); // buffer the cursor command
        lcd->writeString(output,true); // flush after string
        lcd->show();
    }
    T2 = (millis()-start_time) / (1* (double) loops);
    sprintf(output, "Buffr%7.3f ms", T2);
    lcd->setCursor(0, 0);
    lcd->writeString(output);

    delay(3000);
    lcd->clear();

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
    lcd->load_custom_character(7,d7); // test the alias for the function


    while(1) {

        //  Display the messages
        static const char *message[] =
            {
                "FAST Driver for", "LCDs on I2C bus",
                "Copyright (C)", "Keith Standiford",
                "Works on Pi Pico", "and Arduino",
                "This program", "demonstrates ALL",
                "LCD modes of", "operation!",
                "", "Enjoy!"};


        int num_messages = sizeof(message) / sizeof(message[0]);
        for (i = 1; i--;)
        {
            for (m = 0; m < num_messages; m += MAX_LINES)
            {
                for (int line = 0; line < MAX_LINES && m + line < num_messages; line++)
                {
                    lcd->setCursor((MAX_CHARS / 2) - strlen(message[m + line]) / 2,line);
                    lcd->writeString(message[m + line]);
                }
                delay(5000);
                lcd->clear();
            }
        }

        // test each of the function calls

        lcd->clear();
        lcd->noCursor();
        lcd->noBlink();

        // test special characters
        lcd->setCursor(0,0);
        lcd->writeString("Custom 01234567");
        lcd->setCursor(0,1);
        lcd->writeString("Chars=>");
        for(int i=0;i<8;i++) lcd->writeChar((char) i);
        delay(4000);

        // test buffer overflow
        lcd->clear();
        char bigoutput[50];
        for (int i=0;i<6;i++) {
            lcd->setCursor(0,0,true);
            sprintf(output, "Buffer Test   %2d", i);
            lcd->writeString(output, true); // eventually the buffer overflows 
            for (int j=0;j<20;j++) lcd->setCursor(0,1,true);
            sprintf(output, "#'s != is good%2d", i);
            lcd->writeString(output, true); // eventually the buffer overflows 
            // but the rest of the data isn't dumped yet ...
            delay(1000);
        }

        // Test the backlight
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Backlight OFF");
        delay(2000);
        lcd->setBacklight(0);
        lcd->setCursor(0,1,true);
        lcd->printstr("Backlight ON");
        delay(2000);
        lcd->setBacklight(1);
        delay(2000);

        // Test blanking the display
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("  Blank display");
        delay(2000);
        lcd->noDisplay();
        lcd->setCursor(0,1,true);
        lcd->writeString("unBlank display");
        delay(2000);
        lcd->display();
        delay(3000);


        // turn cursor on and off
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor On  *",true);
        lcd->setCursor(11,0,true);
        lcd->cursor();
        delay(3000);

        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor Off *",true);
        lcd->setCursor(11,0,true);
        lcd->noCursor();
        delay(3000);

        // Show blinking cursor
        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor   Blink *",true);
        lcd->setCursor(15,0,true);
        lcd->blink();
        delay(3000);

        lcd->setCursor(0,1,true);
        lcd->writeString("Cursor noBlink *",true);
        lcd->setCursor(15,1,true);
        lcd->noBlink();
        delay(3000);

        lcd->clear();
        lcd->setCursor(0,0,true);
        lcd->writeString("Cursor Both *",true);
        lcd->setCursor(12,0,true);
        lcd->blink();
        lcd->cursor();
        delay(3000);

        lcd->noBlink();
        lcd->noCursor();
        lcd->clear();


        // Test scroll display right and left
        lcd->setCursor(0,0,true);
        lcd->writeString("Scroll to Right",true);
        lcd->setCursor(10,0,true);
        lcd->blink();
        delay(1000);
        lcd->scrollDisplayRight();
        delay(1000);
        lcd->scrollDisplayRight();
        delay(1000);
        lcd->scrollDisplayRight();
        delay(1000);
        lcd->setCursor(0,0,true);
        lcd->writeString("Scroll to Left ",true);
        lcd->setCursor(10,0,true);
        lcd->blink();
        delay(1000);
        lcd->scrollDisplayLeft();
        delay(1000);
        lcd->scrollDisplayLeft();
        delay(1000);
        lcd->scrollDisplayLeft();
        delay(1000);
        lcd->scrollDisplayLeft();
        delay(1000);
        lcd->clear();
    
        // Autoscroll  and write left to right  and right to left
        // Note that autoscroll right to left is a bit WIERD!

        lcd->clear();
        lcd->blink();
        lcd->setCursor(14,0);
        lcd->autoscroll();
        slowWrite(lcd,"autoScroll L=>R",500);
        delay(2000);
        lcd->clear();
        lcd->setCursor(0,1);
        lcd->noAutoscroll();
        slowWrite(lcd,"Write L=>R",500);
        delay(2000);
        lcd->clear();
        lcd->setCursor(0,1);
        lcd->autoscroll();
        lcd->rightToLeft();
        slowWrite(lcd," L>=R llorcSotua",500);
        delay(2000);
        lcd->clear();
        lcd->setCursor(15,1);
        lcd->noAutoscroll();
        slowWrite(lcd,"L>=R etirW",500);
        delay(2000);
        lcd->leftToRight();
    }
}
