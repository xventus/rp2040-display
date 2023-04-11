//
// vim: ts=4 et
// Copyright (c) 2022 Vanek
//
/// @file   tm1637.h
/// @author Vanek

#include <stdlib.h>
#include <stdio.h>

#include "tm1637.h"

#define CLK_PIN 21
#define DIO_PIN 20

#define CLK_PIN2 18
#define DIO_PIN2 19

int main()
{
  stdio_init_all();

  TM1637 tmdsp(pio0, DIO_PIN, CLK_PIN, TM1637::DisplayVariant::SixDigits);     // 6 digit
  TM1637 tmdsp2(pio0, DIO_PIN2, CLK_PIN2, TM1637::DisplayVariant::FourDigits); // 4 digit

  tmdsp.init();
  tmdsp2.init(tmdsp.getOffset());

  tmdsp.backlit(7);
  tmdsp2.backlit(7);

  tmdsp.stop();
  tmdsp2.stop();

  sleep_ms(1000);

  tmdsp.rdy();
  tmdsp2.rdy();

  sleep_ms(1000);

  for (auto i = -10; i < 11; i++)
  {
    tmdsp.print(i);
    tmdsp2.print(i, false);
    sleep_ms(200);
  }

  for (auto i = 0; i < 20; i++)
  {
    tmdsp.clock(i, i + 2, i % 2);
    tmdsp2.clock(i, i + 1, i % 2);
    sleep_ms(200);
  }

  tmdsp.date(29, 1, 2023 - 2000);
  tmdsp2.date(29, 1, 2023 - 2000);
  sleep_ms(1000);

  auto l = 0;
  while (true)
  {
    tmdsp.backlit(l);
    tmdsp2.backlit(l);
    
    tmdsp.error();
    tmdsp2.error();

    l++;
    if (l > 7)
      l = 0;

    sleep_ms(100);
  }
}