//
// vim: ts=4 et
// Copyright (c) 2022 Vanek
//
/// @file   tm1637.cpp
/// @author Vanek

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/clocks.h>
#include "tm1637.h"
#include "tm1637.pio.h"

TM1637::TM1637(PIO pio, uint8_t dio, uint8_t clk, TM1637::DisplayVariant disp) : _pio(pio), _dio(dio), _clk(clk), _disp(disp)
{
}

void TM1637::init(uint offset)
{
  _sm = pio_claim_unused_sm(_pio, true);
  if (!offset)
  {
    _offset = pio_add_program(_pio, &tm1637_program);
  }
  else
  {
    _offset = offset;
  }
  _smCfg = tm1637_program_get_default_config(_offset);
  gpio_pull_up(_dio);
  gpio_pull_up(_clk);
  pio_gpio_init(_pio, _dio);
  pio_gpio_init(_pio, _clk);
  sm_config_set_sideset_pins(&_smCfg, _clk);
  uint32_t cntrlPin = (0x01u << _clk) | (0x01u << _dio);
  pio_sm_set_pins_with_mask(_pio, _sm, cntrlPin, cntrlPin);
  pio_sm_set_pindirs_with_mask(_pio, _sm, cntrlPin, cntrlPin);

  sm_config_set_out_pins(&_smCfg, _dio, 1);
  sm_config_set_set_pins(&_smCfg, _dio, 1);
  sm_config_set_out_shift(&_smCfg, true, false, 32);

  sm_config_set_clkdiv(&_smCfg, clock_get_hz(clk_sys) / 50000); // TODO
  pio_sm_init(_pio, _sm, _offset, &_smCfg);
  pio_sm_set_enabled(_pio, _sm, true);
  cls();
}

TM1637 &TM1637::cls()
{
  memset(_segments, 0, sizeof(_segments));
  writeSegments(_segments);
  return *this;
}

TM1637 &TM1637::backlit(uint8_t value)
{
  if (value == 0xff)
  {
    writeData(_bright ? (_displOn + _bright - 1) : _displOff);
  }
  else
  {
    _bright = value;
    if (_bright > 7)
      _bright = 7;
    writeData(_bright ? (_displOn + _bright - 1) : _displOff);
  }
  return *this;
}
/*
void TM1637::writeSegment(uint8_t from, const uint8_t segments[], uint8_t len)
{
  uint8_t pos{0};
  uint32_t sgment{0};
  uint8_t segmentAddr = _writeAddr + from;
  uint8_t cnt{0};
  do
  {
    if (len > 6 || len == 0)
      break;
    if (pos % 2)
    {
      // high byte
      sgment |= segments[pos++] << 8;
    }
    else
    {
      // low byte
      sgment |= segments[pos++];
    }

    if (pos == 2 || (pos == 1 && pos == len))
    {
      pio_sm_put_blocking(_pio, _sm, (sgment << 16) + (segmentAddr << 8) + _writeMode);
      printf("4>  0x%08x \n", (sgment << 16) + (segmentAddr << 8) + _writeMode);
      if (pos == len)
        break;
      sgment = 0;
    }

    if (pos == len)
    {
      if (pos <= 4) {
        pio_sm_put_blocking(_pio, _sm, (sgment << 16));
         printf("4>  0x%08x \n", (sgment << 16));
      }
      else {
        pio_sm_put_blocking(_pio, _sm, (sgment));
        printf("4>  0x%08x \n", (sgment));
      }
      break;
    }

    if (pos == 4)
    {
      sgment = sgment << 16;
    }

  } while (true);
}
*/

void TM1637::error()
{
  memset(_segments, 0, 6);
  auto point = 0;
  _segments[point++] = _numHex[0x0E];
  _segments[point++] = _special[0x01];
  _segments[point++] = _special[0x01];
  writeSegments(_segments);
}


void TM1637::stop()
{
  memset(_segments, 0, 6);
  auto point = 0;
  _segments[point++] = _numHex[0x05];
  _segments[point++] = _special[0x04];
  _segments[point++] = _special[0x05];
  _segments[point++] = _special[0x06];
  writeSegments(_segments);
}

void TM1637::rdy()
{
  memset(_segments, 0, 6);
  auto point = 0;
  _segments[point++] = _special[0x01];
  _segments[point++] = _numHex[0x0D];
  _segments[point++] = _special[0x02];
  writeSegments(_segments);
}

void TM1637::date(uint8_t day, uint8_t month, uint8_t year)
{
  if (_disp == DisplayVariant::FourDigits)
  {
    auto hh = day % 10;
    auto h = (day - hh) / 10;
    _segments[0] = _numHex[h];
    _segments[1] = _numHex[hh] | _special[0x03];
    hh = month % 10;
    h = (month - hh) / 10;
    _segments[2] = _numHex[h];
    _segments[3] = _numHex[hh];
  }
  else
  {
    auto hh = day % 10;
    auto h = (day - hh) / 10;
    _segments[0] = _numHex[h];
    _segments[1] = _numHex[hh] | _special[0x03];
    hh = month % 10;
    h = (month - hh) / 10;
    _segments[2] = _numHex[h];
    _segments[3] = _numHex[hh] | _special[0x03];
    hh = year % 10;
    h = (year - hh) / 10;
    _segments[4] = _numHex[h];
    _segments[5] = _numHex[hh];
  }

  writeSegments(_segments);
}

void TM1637::clock(uint8_t hours, uint8_t minutes, bool dot)
{
  memset(_segments, 0, sizeof(_segments));

  if (_disp == DisplayVariant::FourDigits)
  {
    auto hh = hours % 10;
    auto h = (hours - hh) / 10;
    _segments[0] = _numHex[h];
    _segments[1] = _numHex[hh] | (dot ? _special[0x03] : 0);
    hh = minutes % 10;
    h = (minutes - hh) / 10;
    _segments[3] = _numHex[hh];
    _segments[2] = _numHex[h];
  }
  else
  {
    auto hh = hours % 10;
    auto h = (hours - hh) / 10;
    _segments[1] = _numHex[h];
    _segments[2] = _numHex[hh] | (dot ? _special[0x03] : 0);
    hh = minutes % 10;
    h = (minutes - hh) / 10;
    _segments[3] = _numHex[h];
    _segments[4] = _numHex[hh];
  }

  writeSegments(_segments);
}

TM1637 &TM1637::print(int32_t value, bool leading, uint8_t dot)
{

  auto negative = (value < 0);
  auto tmp = value;
  uint8_t positions{0};
  uint8_t point{0};

  while (tmp)
  {
    positions++;
    tmp /= 10;
  }

  memset(_segments, leading ? _numHex[0] : 0, 6);

  if (negative)
  {
    _segments[point++] = _special[0];
    tmp = -1 * value;
    positions++;
  }
  else
  {
    tmp = value;
  }

  auto maxlen = (_disp == DisplayVariant::SixDigits) ? 6 : 4;
  if (tmp == 0)
  {
    _segments[maxlen] = _numHex[0];
  }

  while (tmp)
  {
    if (point == maxlen)
      break;

    auto p = maxlen - (negative ? 0 : 1) - point++;
    _segments[p] = _numHex[tmp % 10];
    if (p == dot)
      _segments[p] |= _special[0x03];
    tmp /= 10;
  }

  writeSegments(_segments);

  return *this;
}

void TM1637::writeSegments(const uint8_t segments[])
{
  uint32_t data;

  if (_disp == DisplayVariant::FourDigits)
  {
    data = ((segments[1] + (segments[2] << 8)) << 16) + ((_writeAddr + 1) << 8) + _writeMode;
    writeData(data);
    data = segments[3] << 16;
    writeData(data);
    data = segments[0] << 24;
    writeData(data);
  }
  else
  {
    data = ((segments[1] + (segments[0] << 8)) << 16) + ((_writeAddr + 1) << 8) + _writeMode;
    writeData(data);
    data = ((segments[5] + (segments[4] << 8)) << 16);
    writeData(data);
    data = ((segments[3] + (segments[2] << 8)) << 16);
    writeData(data);
  }
}

void TM1637::writeData(uint32_t data)
{
   pio_sm_put_blocking(_pio, _sm, data);
}
