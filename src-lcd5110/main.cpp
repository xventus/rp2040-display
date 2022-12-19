
//
// vim: ts=4 et
// Copyright (c) 2022 Petr Vanek, petr@fotoventus.cz
//
/// @file   lcd5110.cpp
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


int main()
{
    stdio_init_all();

    Lcd5110 lcd(spi0, 8, 5, 4, 7, 6, 10);
    lcd.init(3,70); // blue
    //lcd.init(4,127); // white 
    lcd.backLight(true);
    
    lcd.withFont(&codeSquaredRegular::fnt[0][0],codeSquaredRegular::glypHeight,codeSquaredRegular::glypWidth,codeSquaredRegular::firstChar,codeSquaredRegular::glyps);
    lcd.at(0,0).print("0123456789");
    
    lcd.withFont(&topaz::fnt[0][0],topaz::glypHeight,topaz::glypWidth,topaz::firstChar,topaz::glyps);
    lcd.at(0,13).print("0123456789");
    
    lcd.withFont(&modeseven::fnt[0][0],modeseven::glypHeight,modeseven::glypWidth,modeseven::firstChar,modeseven::glyps);
    lcd.at(0,24).print("0123456789");
    
    lcd.refresh();

    sleep_ms(2000);

    lcd.cls();
    lcd.at(0,0).print("1").withSize(2).print("2").at(20,0).withSize(3).print("3").at(40,0).withSize(4).print("4");
    lcd.refresh();

    sleep_ms(2000);
    
    lcd.cls();
    lcd.draw(0, 0, Lcd5110::maxX, Lcd5110::maxY);
    lcd.draw(0, Lcd5110::maxY, Lcd5110::maxX, 0);
    lcd.fillRect(0,0,10,10);
    lcd.fillRect(Lcd5110::maxX-10,Lcd5110::maxY-10,10,10);
    lcd.fillRect(0,Lcd5110::maxY-10,10,10);
    lcd.fillRect(Lcd5110::maxX-10,0,10,10);
    lcd.refresh();

    sleep_ms(500);

    for (auto x = 0; x < 10; x++) {
        lcd.at(Lcd5110::maxX/2 - 6,Lcd5110::maxY/2 - 6).withSize(2).print(x);
        sleep_ms(500);
        lcd.refresh();
    }
    
    sleep_ms(2000);
    lcd.cls();
    lcd.at(0,0).withSize(1).print("This is the end,\n my friend.");
    lcd.refresh();
    while(true) {};

}