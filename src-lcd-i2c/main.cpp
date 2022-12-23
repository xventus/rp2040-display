
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
#include "hardware/i2c.h"
#include "i2c_display.h"


int main()
{
    stdio_init_all();

    /*
        I2C0 – SDA	GP0/GP4/GP8/GP12/GP16/GP20
        I2C0 – SCL	GP1/GP5/GP9/GP13/GP17/GP21
        I2C1 – SDA	GP2/GP6/GP10/GP14/GP18/GP26
        I2C1 – SCL	GP3/GP7/GP11/GP15/GP19/GP27
    */
 
    // 16x2 LCD display PIN 2 - SDA, PIN 3 - SCL on I2C1, address 0x27
    LCDI2C lcd(i2c1, 2, 3, 0x27,2,16);
    lcd.init();
    lcd.at(0,0).print("--- LCD 1602 ---");
    lcd.at(1,1).print("i2c interface");
    while(true) {};

    // 20x4 LCD display PIN 2 - SDA, PIN 3 - SCL on I2C1, address 0x27
    /*
    LCDI2C lcd(i2c1, 2, 3, 0x27,4,20);
    lcd.init();
    lcd.withWrap(true).print("A long text that demonstrates the flow of text to the next line.");
    sleep_ms(3000);
    lcd.cls();
	lcd.at(0,1).print("Print on position");
    lcd.at(1,5).print("[5]");
    lcd.at(2,10).print("[10]");
    lcd.at(3,16).print("[16] here");
    sleep_ms(3000);
    lcd.cls();
	lcd.at(0,1).print("--- LCD 2004A ---");
    lcd.at(1,3).print(" i2c interface ");
    lcd.at(3,16).print(">end");
    while(true) {};
    */
}