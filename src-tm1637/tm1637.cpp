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

TM1637::TM1637(PIO pio, uint8_t dio, uint8_t clk) : _pio(pio), _dio(dio), _clk(clk)
{
}

void TM1637::init()
{
  _sm = pio_claim_unused_sm(_pio, true);
  auto offset = pio_add_program(_pio, &tm1637_program);
  _smCfg = tm1637_program_get_default_config(offset);
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
  pio_sm_init(_pio, _sm, offset, &_smCfg);
  pio_sm_set_enabled(_pio, _sm, true);
  memset(_segments, 0, sizeof(_segments));
  writeSegment(0, _segments, 6);
}

TM1637 &TM1637::cls()
{
  memset(_segments, 0, sizeof(_segments));
  writeSegment(0, _segments, 6);
  return *this;
}

TM1637 &TM1637::backlit(uint8_t value)
{
  if (value == 0xff)
  {
    // last known value
    pio_sm_put_blocking(_pio, _sm, _bright ? (_displOn + _bright - 1) : _displOff);
  }
  else
  {
    _bright = value;
    if (_bright > 7)
      _bright = 7;
    pio_sm_put_blocking(_pio, _sm, _bright ? (_displOn + _bright - 1) : _displOff);
  }
  return *this;
}

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
      if (pos == len)
        break;
      sgment = 0;
    }

    if (pos == len)
    {
      if (pos <= 4)
        pio_sm_put_blocking(_pio, _sm, (sgment << 16));
      else
        pio_sm_put_blocking(_pio, _sm, (sgment));
      break;
    }

    if (pos == 4)
    {
      sgment = sgment << 16;
    }

  } while (true);
}

void TM1637::error(uint8_t fromPos, uint8_t maxLen)
{
  memset(_segments, 0, 6);
  auto point = 0;
  _segments[point++] = _numHex[0x0E];
  _segments[point++] = _special[0x01];
  _segments[point++] = _special[0x01];
  writeSegment(fromPos, _segments, maxLen);
}

void TM1637::rdy(uint8_t fromPos, uint8_t maxLen)
{
  memset(_segments, 0, 6);
  auto point = 0;
  _segments[point++] = _special[0x01];
  _segments[point++] = _numHex[0x0D];
  _segments[point++] = _special[0x02];
  writeSegment(fromPos, _segments, maxLen);
}

void TM1637::clock(uint8_t hours, uint8_t minutes, bool dot) {
    print(hours, 0, 2, true, dot?1:3);
    print(minutes,2,2, true, 5).backlit(7);
}

TM1637 &TM1637::print(int32_t value, uint8_t fromPos, uint8_t maxLen, bool leading, uint8_t dot)
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

  if (positions <= maxLen)
  {
    if (tmp == 0)
    {
      _segments[fromPos + maxLen - 1] = _numHex[0];
    }

    while (tmp)
    {
      if (point == maxLen)
        break;
      auto p = maxLen - (negative ? 0 : 1) - point++;
      _segments[p] = _numHex[tmp % 10];
      if (p == dot)
        _segments[p] |= _special[0x03];
      tmp /= 10;
    }
    writeSegment(fromPos, _segments, maxLen);
  }
  else
  {
    error(fromPos, maxLen);
  }

  return *this;
}

