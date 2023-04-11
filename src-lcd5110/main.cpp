
//
// vim: ts=4 et
// Copyright (c) 2022 Petr Vanek, petr@fotoventus.cz
//
/// @file   main.cpp
/// @author Petr Vanek

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "lcd5110.h"
#include "generated/codeSquaredRegular.h"
#include "generated/topaz.h"
#include "generated/modeseven.h"

#define LCD_SPI   spi0
#define LCD_BIAS   3 
#define LCD_CONTRAST  70
#define LCD_RST_PIN 8
#define LCD_CE_PIN 12
#define LCD_DC_PIN 11
#define LCD_DIN_PIN 7
#define LCD_CLK_PIN 6
#define LCD_LIGHT_PIN 10

int main()
{
    stdio_init_all();


/*
spi_inst_t *spi,
            uint8_t rst = 8,
            uint8_t ce = 5,
            uint8_t dc = 4,
            uint8_t din = 7,
            uint8_t clk = 6,
            uint8_t light = 10)
*/

    // pin asigment
    //Lcd5110 lcd(LCD_SPI, LCD_RST_PIN, LCD_CE_PIN, LCD_DC_PIN, LCD_DIN_PIN, LCD_CLK_PIN, LCD_LIGHT_PIN);
    Lcd5110 lcd(spi0, 8, 5, 4, 7, 6, 10);

    // initialize interface, sets bias and contrast
    //lcd.init(4, 60); 
    lcd.init(3, 70);    // blue - usable for blue backlight
    // lcd.init(4,127); // white - setting for some types with white backlit 
    
    // backlit sets to ON
    lcd.backLight(true);

    // use font CodeSquaredRegular and print 0-9 into internal buffer
    lcd.withFont(&codeSquaredRegular::fnt[0][0], codeSquaredRegular::glypHeight, codeSquaredRegular::glypWidth, codeSquaredRegular::firstChar, codeSquaredRegular::glyps);
    lcd.at(0, 0).print("0123456789");

    // use font Topaz and print 0-9 into internal buffer
    lcd.withFont(&topaz::fnt[0][0], topaz::glypHeight, topaz::glypWidth, topaz::firstChar, topaz::glyps);
    lcd.at(0, 13).print("0123456789");

    // use font Modeseven and print 0-9 into internal buffer
    lcd.withFont(&modeseven::fnt[0][0], modeseven::glypHeight, modeseven::glypWidth, modeseven::firstChar, modeseven::glyps);
    lcd.at(0, 24).print("0123456789");

    // display 
    lcd.refresh();

    sleep_ms(2000);

    // clear interna buffer
    lcd.cls();
    
    // print at position with different size
    lcd.at(0, 0).print("1").withSize(2).print("2").at(20, 0).withSize(3).print("3").at(40, 0).withSize(4).print("4");
    
    //display
    lcd.refresh();

    sleep_ms(2000);

    // draw line and fill rectagle example
    lcd.cls();
    lcd.draw(0, 0, Lcd5110::maxX, Lcd5110::maxY);
    lcd.draw(0, Lcd5110::maxY, Lcd5110::maxX, 0);
    lcd.fillRect(0, 0, 10, 10);
    lcd.fillRect(Lcd5110::maxX - 10, Lcd5110::maxY - 10, 10, 10);
    lcd.fillRect(0, Lcd5110::maxY - 10, 10, 10);
    lcd.fillRect(Lcd5110::maxX - 10, 0, 10, 10);
    lcd.refresh();

    sleep_ms(500);

    // displays the changing number at the position
    for (auto x = 0; x < 10; x++)
    {
        lcd.at(Lcd5110::maxX / 2 - 6, Lcd5110::maxY / 2 - 6).withSize(2).print(x);
        sleep_ms(500);
        lcd.refresh();
    }

    sleep_ms(2000);
    
    // display long text with a crop on the first line (missign "end,")
    lcd.cls();
    lcd.at(0, 0).withSize(1).print("This is the end,\nmy friend.");
    lcd.refresh();
    while (true)
    {
    };
}