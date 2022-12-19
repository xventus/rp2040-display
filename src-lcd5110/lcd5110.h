//
// vim: ts=4 et
// Copyright (c) 2022 Petr Vanek, petr@fotoventus.cz
//
/// @file   lcd5110.h
/// @author Petr Vanek

#pragma once

#include <inttypes.h>


class Lcd5110
{

    public:

        // LCD5110 screen size
        static const uint8_t maxX {84};
        static const uint8_t maxY {48};

        Lcd5110(spi_inst_t *spi, uint8_t rst = 8, uint8_t ce = 5, uint8_t dc = 4, uint8_t din = 7, uint8_t clk = 6, uint8_t light = 10) : 
            _spi{spi},
            _rst(rst),
            _ce(ce),
            _dc(dc),
            _din(din),
            _clk(clk),
            _light(light)
        {}

        void init(uint8_t bias = 0x05, uint8_t contrast= 0x7f);
        
        void withFont(const unsigned char *lcdFont, uint8_t hg, uint8_t wg, unsigned char firstChar, uint8_t sz) {
            _lcdFont = lcdFont;
            _hg = hg;
            _wg = wg;
            _firstChar = firstChar;
            _fntsz = sz;
        }

        
        void backLight(bool on);
        void cls();
        void plot(int16_t x, int16_t y, bool bg=false);
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, bool bg = false);
        void draw(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool bg = false);
        void charAt(int16_t x, int16_t y, unsigned char c, bool bg = false, uint8_t size = 1);
        void putc(uint8_t c);
        
        Lcd5110& withSize(uint8_t sz) {
            _printSize = sz;
            return *this;
        }

        Lcd5110& print(const char *strz);
        Lcd5110& at(int16_t x, int16_t y);
        Lcd5110& print(int32_t number);

        /// @brief display prepared buffeer to the screen
        void refresh();

    private:

        void cmd(uint8_t cm);      
    private:

    // extend mode
    #define PCD8544_EXTEND_INSTRUCTION 0x21
    #define PCD8544_BIAS 0x10
    #define PCD8544_VOP 0x80

    // normal mode
    #define PCD8544_INSTRUCTION 0x20
    #define PCD8544_NORMAL 0x0C
    #define PCD8544_YADDR 0x40
    #define PCD8544_XADDR 0x80

    const unsigned char  *_lcdFont {nullptr};
    spi_inst_t *_spi {nullptr}; 
    uint8_t _rst;
    uint8_t _ce;
    uint8_t _dc;
    uint8_t _din;
    uint8_t _clk;
    uint8_t _light;  
    uint8_t _buffer[maxX * maxY / 8];
    uint8_t _x{0};
    uint8_t _y{0}; 
    uint8_t _hg{0};
    uint8_t _wg{0};
    unsigned char _firstChar{' '};
    uint8_t _fntsz{0}; 
    uint8_t _printSize{1}; 

};
