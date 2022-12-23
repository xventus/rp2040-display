//
// vim: ts=4 et
// Copyright (c) 2022 Vanek
//
/// @file   tm1637.h
/// @author Vanek

#include <stdlib.h>
#include <stdio.h>

#include "tm1637.h"

#define CLK_PIN 2
#define DIO_PIN 3

int main() {

  TM1637 tmdsp(pio0, DIO_PIN, CLK_PIN);

  tmdsp.init();

  tmdsp.backlit(7);
  tmdsp.rdy();

  sleep_ms(500);
  for(auto i = 0; i <10; i++ ) {
    tmdsp.clock(i, i+1, i%2);
    sleep_ms(500);
  }

  tmdsp.print(1234);
  sleep_ms(500);   
  tmdsp.print(-123);
  sleep_ms(500);   
  tmdsp.print(-1230);

  auto i = 0;
  while(true) {
    tmdsp.backlit(i).print(1234);
    i++;
    if (i > 7) i = 0;
    sleep_ms(500);
  } ;
  
}
