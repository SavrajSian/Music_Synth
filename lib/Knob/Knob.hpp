#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <math.h>




class Knob
{
public:
  Knob(int minValue, int maxValue, volatile int *target)
      : m_minValue(minValue), m_maxValue(maxValue), m_target(target) {}

  void increment()
  {
    __atomic_store_n(m_target, min(__atomic_load_n(m_target, __ATOMIC_RELAXED) + 1, m_maxValue), __ATOMIC_RELAXED);
  }

  void decrement()
  {
    __atomic_store_n(m_target, max(__atomic_load_n(m_target, __ATOMIC_RELAXED) - 1, m_minValue), __ATOMIC_RELAXED);
  }

  void update(int keyVal)
  {
    // Increment Value State
    if ((keyVal == 0b10 && m_prevAB == 0b11) ||
        (keyVal == 0b01 && m_prevAB == 0b00))
    {
      increment();
      m_prevTransition = 1;
    }
    // Decrement Value State
    else if ((keyVal == 0b11 && m_prevAB == 0b10) ||
             (keyVal == 0b00 && m_prevAB == 0b01))
    {
      decrement();
      m_prevTransition = -1;
    }
    // Best-Guess States
    else if ((keyVal == 0b00 && m_prevAB == 0b11) ||
             (keyVal == 0b11 && m_prevAB == 0b00) ||
             (keyVal == 0b10 && m_prevAB == 0b01) ||
             (keyVal == 0b01 && m_prevAB == 0b10))
    {
      if (m_prevTransition == 1)
      {
        increment();
        m_prevTransition = 1;
      }
      else if (m_prevTransition == -1)
      {
        decrement();
        m_prevTransition = -1;
      }
    }
    m_prevAB = keyVal;
  }

private:
  int m_minValue;
  int m_maxValue;
  volatile int *m_target;
  int m_prevAB = 0;
  int m_prevTransition = 0;
};