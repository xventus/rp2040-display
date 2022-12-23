//
// vim: ts=4 et
// Copyright (c) 2021, 2022 Vanek
//
/// @file   i2c_display.h
/// @author Vanek

#pragma once

#include "hardware/i2c.h"
#include "pico/stdlib.h"

/*
LCD TC2004A : https://cdn-shop.adafruit.com/datasheets/TC2004A-01.pdf
Expander:
https://www.ti.com/lit/ds/symlink/pcf8574.pdf?ts=1636353239996&ref_url=https%253A%252F%252Fwww.google.com%252F

*/

/**
 * @brief wrapped I2C base LCD 4x20 display and connected via PCF8574
 * DATA must be wired at D4-D7
 *
 *  CPU PINS:  PC5 - SCL
 *             PC4 - SDA
 *
 */
class LCDI2C
{

  // fixed data bits
  // static constexpr uint8_t  _d4 = 4;
  // static constexpr uint8_t  _d5 = 5;
  // static constexpr uint8_t  _d6 = 6;
  // static constexpr uint8_t  _d7 = 7;

  // pins
  // RW 0x02 not used
  static constexpr uint8_t _enable = 0x04;
  static constexpr uint8_t _rs = 0x01;
  static constexpr uint8_t _backlight = 0x08;

  static constexpr uint8_t _clearDisplay = 0x01;
  static constexpr uint8_t _cmdReturnHome = 0x02;
  static constexpr uint8_t _cmdEntryModeSet = 0x04;
  static constexpr uint8_t _displayControl = 0x08;

  static constexpr uint8_t _cmdEntryLeft = 0x02;
  static constexpr uint8_t _cmdShiftIncrement = 0x01;
  static constexpr uint8_t _cmdFunctionSet = 0x20;

  static constexpr uint8_t _cmd2line = 0x08;
  static constexpr uint8_t _cmd5x10 = 0x04;

  static constexpr uint8_t _cmdDispOn = 0x04;
  static constexpr uint8_t _cmdDispOff = 0x00;
  // static constexpr uint8_t _cmdCurOn = 0x02;
  // static constexpr uint8_t _cmdCurOff = 0x00;
  // static constexpr uint8_t _cmdBlinOn = 0x01;
  // static constexpr uint8_t _cmdBlinkOff = 0x00;
  static constexpr uint8_t _cmdSetAddr = 0x80;

public:
  /**
   * @brief Construct a new LCDI2C object
   *
   * @param sda
   * @param scl
   * @param address - display address
   */
  explicit LCDI2C(i2c_inst_t *i2c,
                  uint8_t sda,
                  uint8_t scl,
                  uint8_t address = 0x27,
                  uint8_t rows = 2,
                  uint8_t col = 16);

  void init();

  LCDI2C &cls();

  LCDI2C &home();

  LCDI2C &backLight(bool bckl = true);

  LCDI2C &print(const char *str);

  LCDI2C &print(int32_t in);

  LCDI2C &at(uint8_t row, uint8_t col);

  LCDI2C &withWrap(bool wrap)
  {
    _wrap = wrap;
    return *this;
  }

private:
  void oneChar(uint8_t o);

  /**
   * @brief write nibble
   *
   * @param data
   */
  void write4bits(uint8_t data);

  /**
   * @brief write commnad to the display
   *
   * @param value
   */
  void command(uint8_t value);

  /**
   * @brief send send data splitted to the two nibbles
   *
   * @param value
   * @param mode
   */
  void send(uint8_t value, uint8_t mode);

  /**
   * @brief write data to PCF8574
   *
   * @param data
   */
  void writeIO(uint8_t data);

private:
  i2c_inst_t *_i2c{nullptr};
  uint8_t _sda;
  uint8_t _scl;
  uint8_t _address;
  uint8_t _displaycontrol{0};
  bool _backlightval{true};
  uint8_t _rows{2};
  uint8_t _col{16};
  uint8_t _c{0};
  uint8_t _r{0};
  bool _wrap{false};
  bool _cursor{false};
};
