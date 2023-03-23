#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <math.h>

extern volatile int octaveRX[2] ;
extern  volatile uint32_t cur_message[2];
extern volatile bool playSong;

void songBank1()
{
  octaveRX[0] = 4;
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b100000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b100000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b100000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b000010000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b01000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b001000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  cur_message[0] = 0b000000010000;
  delay(200);
  octaveRX[0] = 3;
  cur_message[0] = 0b001000000000;
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000010000;
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  cur_message[0] = 0b000000010000;
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  delay(200);
  octaveRX[0] = 4;
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000100000;
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  cur_message[0] = 0b000000100000;
  delay(200);
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000100000;
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  cur_message[0] = 0b000000100000;
  delay(200);
  cur_message[0] = 0b000000000001;
  delay(200);
  cur_message[0] = 0b000000100000;
  delay(200);
  cur_message[0] = 0b001000000000;
  delay(200);
  cur_message[0] = 0b000000100000;
  delay(200);
  if (!playSong)
  {
    cur_message[0] = 0b000000000000;
    return;
  }
}