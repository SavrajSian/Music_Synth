#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <math.h>

extern float initialY ;
extern volatile float pitchBend;
extern float calZero;
extern volatile int effect ;
extern volatile int arp1Effect ;
extern volatile int arp2Effect ;
extern volatile int vibratoEffect;
extern volatile int pressedKeys;
extern  volatile uint32_t cur_message[2];
extern volatile float vibrato ;
extern volatile bool vibToggle ;
extern volatile float arpegio ;
extern volatile bool arpToggle ;
extern volatile float vibratoMulti[3] ;
extern volatile float arpeggio1Multi[3][3] ;
extern volatile float arpeggio2Multi[3][4] ;

void pitchControl()
{
  // Read joystick (stepsize)
  float joyY = analogRead(A0);
  float joyYscale = (joyY / 1023);
  pitchBend = 1.00f;

  if (joyYscale < calZero - 0.05)
  {
    pitchBend = 1 + (calZero - joyYscale) * 0.5;
  }
  else if (joyYscale > calZero + 0.05)
  {
    pitchBend = 1 - (joyYscale - calZero) * 0.5;
  }

  // Vibrato
  if (effect == 1 && (pressedKeys != 0 || cur_message[0] != 0 || cur_message[1] != 0))
  {
    if (vibrato < vibratoMulti[vibratoEffect] && vibToggle == false)
    {
      vibrato += 0.01;
      pitchBend = 1 + vibrato;
      if (vibrato >= vibratoMulti[vibratoEffect])
      {
        vibToggle = true;
      }
    }
    if (vibToggle == true)
    {
      vibrato -= 0.01;
      pitchBend = 1 + vibrato;
      if (vibrato <= 0)
      {
        vibToggle = false;
      }
    }
  }

  // Arpegiator 1
  if (effect == 3)
  {
    if (arpegio <= arpeggio1Multi[arp1Effect][2])
    {
      arpegio += 0.05;
      if (arpegio > arpeggio1Multi[arp1Effect][1])
      {
        pitchBend = 1.5;
      }
      else if (arpegio > arpeggio1Multi[arp1Effect][0])
      {
        pitchBend = 1.25;
      }
      else
      {
        pitchBend = 1;
      }
      if (arpegio >= arpeggio1Multi[arp1Effect][2])
      {
        arpegio = 0;
      }
    }
  }

  // Arpegiator 2
  else if (effect == 4)
  {
    if (arpegio <= arpeggio2Multi[arp2Effect][3])
    {
      arpegio += 0.05;
      if (arpegio > arpeggio2Multi[arp2Effect][2])
      {
        pitchBend = 1.5;
      }
      else if (arpegio > arpeggio2Multi[arp2Effect][1])
      {
        pitchBend = 1.25;
      }
      else if (arpegio > arpeggio2Multi[arp2Effect][0])
      {
        pitchBend = 1.5;
      }
      else
      {
        pitchBend = 1;
      }

      if (arpegio >= arpeggio2Multi[arp2Effect][3])
      {
        arpegio = 0;
      }
    }
  }

  if (pressedKeys == 0 && cur_message[0] == 0 && cur_message[1] == 0)
  {
    arpegio = 0;
  }
}