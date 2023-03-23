#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <math.h>
  
extern float initialY = analogRead(A1);
extern volatile bool OctToggle ;
extern volatile int octaveSelect;
extern const int MAX_OCT ;
extern const int MIN_OCT ;

void octaveControl()
{
  // Read joystick (octaves)
  float joyX = analogRead(A1);
  float joyXscale = (joyX / 1023) * 100;

  if (joyXscale > 80 && OctToggle == false)
  {
    __atomic_store_n(&octaveSelect, max(__atomic_load_n(&octaveSelect, __ATOMIC_RELAXED) - 1, MIN_OCT), __ATOMIC_RELAXED);
    OctToggle = true;
  }
  else if (joyXscale < 15 && OctToggle == false)
  {
    __atomic_store_n(&octaveSelect, min(__atomic_load_n(&octaveSelect, __ATOMIC_RELAXED) + 1, MAX_OCT), __ATOMIC_RELAXED);
    OctToggle = true;
  }
  else if (joyXscale >= 15 && joyXscale <= 80)
  {
    OctToggle = false;
  }
}