//
// vim: ts=4 et
// Copyright (c) 2021 Vanek
//
/// @file   i2c_display.h
/// @author Vanek

#include "i2c_display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

LCDI2C::LCDI2C(i2c_inst_t *i2c,
               uint8_t sda,
               uint8_t scl,
               uint8_t address,
               uint8_t rows,
               uint8_t col) : _i2c(i2c),
                               _sda(sda),
                               _scl(scl),
                               _address(address),
                               _rows(rows),
                               _col(col)
{
}

void LCDI2C::init()
{

  i2c_init(_i2c, 100 * 1000);
  gpio_set_function(_sda, GPIO_FUNC_I2C);
  gpio_set_function(_scl, GPIO_FUNC_I2C);
  gpio_pull_up(_sda);
  gpio_pull_up(_scl);

  sleep_ms(45);
  command(0x03);
  sleep_ms(5);
  command(0x03);
  sleep_us(150);
  command(0x03);
  sleep_us(150);
  command(0x02);

  command(_cmdEntryModeSet | _cmdEntryLeft);
  command(_cmdFunctionSet | (_rows > 1 ? _cmd2line : 0) | (_rows == 1 ? _cmd5x10 : 0));
  command(_displayControl | _cmdDispOn);
  cls();
  home();
}

LCDI2C& LCDI2C::cls()
{
  command(_clearDisplay);
  sleep_us(2000);
  return *this;
}

LCDI2C& LCDI2C::home()
{
  command(_cmdReturnHome);
  _c = 0;
  _r = 0;
  sleep_us(2000);
  return *this;
}


void LCDI2C::command(uint8_t value)
{
  send(value, 0);
}

void LCDI2C::send(uint8_t value, uint8_t mode)
{
  uint8_t highnib = value & (uint8_t)0xf0;
  uint8_t lownib = (value << 4) & (uint8_t)0xf0;
  write4bits(highnib | mode);
  write4bits(lownib | mode);
}

void LCDI2C::write4bits(uint8_t data)
{
  data |= _backlightval ? _backlight : 0;
  writeIO(data);
  sleep_us(600);
  writeIO(data | _enable);
  sleep_us(700);
  writeIO(data & ~_enable);
  sleep_us(600);
}

void LCDI2C::oneChar(uint8_t o)
{
  send(o, _rs);
}

LCDI2C &LCDI2C::backLight(bool bckl)
{
  _backlightval = bckl;
  auto data = _backlightval ? _backlight : 0;
  writeIO(data);
  return *this;
}
 
void LCDI2C::writeIO(uint8_t data)
{
  i2c_write_blocking(_i2c, _address, &data, 1, false);
}

LCDI2C& LCDI2C::at(uint8_t row, uint8_t col)
{
  uint8_t const offset[]= { 0x00, 0x40, 0x14, 0x54 };
  if (row >= _rows) row = _rows - 1;
  if (col >= _col) col = _col - 1;
  _c = col;
  _r = row;
  command(_cmdSetAddr | (col + offset[row]));
  return *this;
}

LCDI2C& LCDI2C::print(const char *str)
{
  while (*str)
  {
    if (_wrap) {
      if ((_c+1) > _col && (_r+1) < _rows) {
        _c = 0;
        _r++;
        at(_r,_c);
      } else if ((_r+1) >= _rows && (_c) >= _col) {
        // no more space for text
        break;
      }
    } else {
        if ((_c+1) > _col) break;
    }
   
    _c++;
    oneChar(*str++);
  }
  return *this;
}

LCDI2C& LCDI2C::print(int32_t in)
{
  char str[15];
  sprintf(str, "%ld", in);
  print(str);
  return *this;
}
