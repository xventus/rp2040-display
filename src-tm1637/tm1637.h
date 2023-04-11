//
// vim: ts=4 et
// Copyright (c) 2022 Vanek
//
/// @file   tm1637.h
/// @author Vanek

#pragma once

#include <memory.h>

#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"

class TM1637
{

    // TODO: HEX byte and Address

    /*
        Notes: https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/digi_clock/TM1637.pdf

        The first byte input from DIO at CLK failing edge acts as a command.
        7 6
        0 1 Data command setting                    [0x40]
        1 0 Display and control command setting     [0x80]
        1 1 Address command setting                 [0xC0]

        Data commnad setting - command is to set data write and data read. 01 and 11 are not permitted to set for 1 and 0 bits
        7 6 5 4 3 2 1 0
        0 1         0 0 -  Write data to display register
        0 1         1 0 -  Read key scan data
        0 1       0     - Automatic address adding
        0 1       1     - Fix address
        0 1     0       - Normal mode
        0 1     1       - Test mode

        Address command - bits
        7 6 5 4 3 2 1 0
        1 1     0 0 0 0 - C0H
        1 1     0 0 0 1 - C1H
        1 1     0 0 1 0 - C2H
        1 1     0 0 1 1 - C3H
        1 1     0 1 0 0 - C4H
        1 1     0 1 0 1 - C5H


        Display Control - bits
        7 6 5 4 3 2 1 0
        1 0       0 0 0 - 1/16
        1 0       0 0 1 - 2/16
        1 0       0 1 0 - 4/16
        1 0       0 1 1 - 10/16
        1 0       1 0 0 - 11/16
        1 0       1 0 1 - 12/16
        1 0       1 1 0 - 13/16
        1 0       1 1 1 - 14/16
        1 0     0		- display OFF [0x80]
        1 0     1       - display ON  [0x88]

    */

    static constexpr uint8_t _writeMode = 0x04;
    static constexpr uint8_t _writeAddr = 0xC0;
    static constexpr uint8_t _displOn = 0x88;
    static constexpr uint8_t _displOff = 0x80;
    static constexpr uint8_t _maxDigit = 0xC0;
    static constexpr uint8_t _numHex[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71}; ///< 0-9A-F
    static constexpr uint8_t _special[] = {0x40, 0x50, 0x6E, 0x80, 0x78, 0x5C, 0x73};                                                      ///< -ry.toP

public:
    enum class DisplayVariant
    {
        FourDigits,
        SixDigits
    };

    /**
     * @brief ctor TM1637 object
     *
     * @param pio -  PIO instance
     * @param dio - DIO pin number
     * @param clk - CLK pin number
     */
    explicit TM1637(PIO pio, uint8_t dio, uint8_t clk, DisplayVariant disp);

    /**
     * @brief  SM and object initialization
     *
     * @param offset - when one display is already initialized it is possible to use the offset program for another SM
     */
    void init(uint offset = 0);

    /**
     * @brief Clear screen
     *
     * @return TM1637&
     */
    TM1637 &cls();

    /**
     * @brief Displays hours and minutes on a 4-digit display with the option of displaying a tick as a seconds hand.
     *
     * @param hours - value of hours
     * @param minutes - value of minutes
     * @param dot - show dot
     */
    void clock(uint8_t hours, uint8_t minutes, bool dot);

    /**
     * @brief Complex print function for displaying integer values with a sign.
     *
     * @param value - Integer value that should be displayed on the screen. If the value exceeds the display capabilities, the Err message is displayed
     * @param fromPos - Display from position. Position 0 is the leftmost position.
     * @param maxLen - Number of digits used for display, maximum for tm1637 is 6
     * @param leading - Display of leading zero
     * @param dot - Displaying the tuple at the digit position. If we don't want to display, we specify out of range value
     * @return TM1637&
     */
    TM1637 &print(int32_t value, bool leading = true, uint8_t dot = 0);

    /**
     * @brief Adjust the brightness of the display.
     *
     * @param value Value 0 - turns off the display. Maximum value is 7. 0xFF - Forwards the last known brightness value to the display.
     * @return TM1637&
     */
    TM1637 &backlit(uint8_t value = 0xFF);

    /**
     * @brief Displays the Err message
     *
     */
    void error();

    /**
     * @brief Stop message
     * 
     */
    void stop();

    /**
     * @brief Displays the RDY message
     *
     */
    void rdy();

    /**
     * @brief Get the Offset object for next display init(offset)
     *
     * @return uint
     */
    uint getOffset() const { return _offset; }

    /**
     * @brief   prints date, year only on 6-digit display 
     * 
     * @param day day
     * @param month  month
     * @param year  (00-99) only short year
     */
    void date(uint8_t day, uint8_t month, uint8_t year);

    /**
     * @brief prints the contents of the buffer. Each position corresponds to one position. 0-3 or 0-5
     * 
     * @param segments 
     */
    void writeSegments(const uint8_t segments[]);

private:
    void writeData(uint32_t data);

private:
    uint8_t _clk{0};
    uint8_t _dio{0};
    pio_sm_config _smCfg;
    int _sm{0};
    PIO _pio{nullptr};
    uint8_t _bright{7};
    uint8_t _segments[6];
    uint _offset{0};
    DisplayVariant _disp;
};
