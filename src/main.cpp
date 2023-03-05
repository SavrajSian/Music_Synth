#include <Arduino.h>
#include <U8g2lib.h>
#include <HardwareTimer.h>
#include <STM32FreeRTOS.h>

// Calculate step sizes and frequencies during compilation
constexpr uint32_t samplingFreq = 22050;                  // Hz
constexpr double twelfthRootOfTwo = pow(2.0, 1.0 / 12.0); // 12th root of 2

// Returns frequency for given note
constexpr uint32_t calculateFreq(float semiTone)
{
  return static_cast<uint32_t>(440.00f * std::pow(2.00f, static_cast<float>(semiTone) / 12.0f));
}
// Returns step size from note
constexpr uint32_t calculateStepSize(float frequency)
{
  return static_cast<uint32_t>((pow(2, 32) * frequency) / samplingFreq);
}
// 2 - 8 Octaves of step sizes - super long
constexpr uint32_t stepSizes[] = {
  calculateStepSize(calculateFreq(-33)),// C2
  calculateStepSize(calculateFreq(-32)),// C#2
  calculateStepSize(calculateFreq(-31)),// D2
  calculateStepSize(calculateFreq(-30)),// D#2
  calculateStepSize(calculateFreq(-29)),// E2
  calculateStepSize(calculateFreq(-28)),// F2
  calculateStepSize(calculateFreq(-27)),// F#2
  calculateStepSize(calculateFreq(-26)),// G2
  calculateStepSize(calculateFreq(-25)),// G#2
  calculateStepSize(calculateFreq(-24)),// A2
  calculateStepSize(calculateFreq(-23)),// A#2
  calculateStepSize(calculateFreq(-22)),// B2
  calculateStepSize(calculateFreq(-21)),// C3
  calculateStepSize(calculateFreq(-20)),// C#3
  calculateStepSize(calculateFreq(-19)),// D3
  calculateStepSize(calculateFreq(-18)),// D#3
  calculateStepSize(calculateFreq(-17)),// E3
  calculateStepSize(calculateFreq(-16)),// F3
  calculateStepSize(calculateFreq(-15)),// F#3
  calculateStepSize(calculateFreq(-14)),// G3
  calculateStepSize(calculateFreq(-13)),// G#3
  calculateStepSize(calculateFreq(-12)),// A3
  calculateStepSize(calculateFreq(-11)),// A#3
  calculateStepSize(calculateFreq(-10)),// B3
  calculateStepSize(calculateFreq(-9)), // C4
  calculateStepSize(calculateFreq(-8)), // C#4
  calculateStepSize(calculateFreq(-7)), // D4
  calculateStepSize(calculateFreq(-6)), // D#4
  calculateStepSize(calculateFreq(-5)), // E4
  calculateStepSize(calculateFreq(-4)), // F4
  calculateStepSize(calculateFreq(-3)), // F#4
  calculateStepSize(calculateFreq(-2)), // G4
  calculateStepSize(calculateFreq(-1)), // G#4
  calculateStepSize(calculateFreq(0)), // A4
  calculateStepSize(calculateFreq(1)), // A#4
  calculateStepSize(calculateFreq(2)), // B4
  calculateStepSize(calculateFreq(3)), // C5
  calculateStepSize(calculateFreq(4)), // C#5
  calculateStepSize(calculateFreq(5)), // D5
  calculateStepSize(calculateFreq(6)), // D#5
  calculateStepSize(calculateFreq(7)), // E5
  calculateStepSize(calculateFreq(8)), // F5
  calculateStepSize(calculateFreq(9)), // F#5
  calculateStepSize(calculateFreq(10)), // G5
  calculateStepSize(calculateFreq(11)), // G#5
  calculateStepSize(calculateFreq(12)), // A5
  calculateStepSize(calculateFreq(13)), // A#5
  calculateStepSize(calculateFreq(14)), // B5
  calculateStepSize(calculateFreq(15)), // C6
  calculateStepSize(calculateFreq(16)), // C#6
  calculateStepSize(calculateFreq(17)), // D6
  calculateStepSize(calculateFreq(18)), // D#6
  calculateStepSize(calculateFreq(19)), // E6
  calculateStepSize(calculateFreq(20)), // F6
  calculateStepSize(calculateFreq(21)), // F#6
  calculateStepSize(calculateFreq(22)), // G6
  calculateStepSize(calculateFreq(23)), // G#6
  calculateStepSize(calculateFreq(24)), // A6
  calculateStepSize(calculateFreq(25)), // A#6
  calculateStepSize(calculateFreq(26)), // B6
  calculateStepSize(calculateFreq(27)), // C7
  calculateStepSize(calculateFreq(28)), // C#7
  calculateStepSize(calculateFreq(29)), // D7
  calculateStepSize(calculateFreq(30)), // D#7
  calculateStepSize(calculateFreq(31)), // E7
  calculateStepSize(calculateFreq(32)), // F7
  calculateStepSize(calculateFreq(33)), // F#7
  calculateStepSize(calculateFreq(34)), // G7
  calculateStepSize(calculateFreq(35)), // G#7
  calculateStepSize(calculateFreq(36)), // A7
  calculateStepSize(calculateFreq(37)), // A#7
  calculateStepSize(calculateFreq(38)), // B7
  calculateStepSize(calculateFreq(39)), // C8
  calculateStepSize(calculateFreq(40)), // C#8
  calculateStepSize(calculateFreq(41)), // D8
  calculateStepSize(calculateFreq(42)), // D#8
  calculateStepSize(calculateFreq(43)), // E8
  calculateStepSize(calculateFreq(44)), // F8
  calculateStepSize(calculateFreq(45)), // F#8
  calculateStepSize(calculateFreq(46)), // G8
  calculateStepSize(calculateFreq(47)), // G#8
  calculateStepSize(calculateFreq(48)), // A8
  calculateStepSize(calculateFreq(49)), // A#8
  calculateStepSize(calculateFreq(50))  // B8
};

// Audio definitions
const uint32_t interval = 100; // Display update interval
volatile uint32_t currentStepSize1;
volatile uint32_t currentStepSize2;
const char *notes[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// Key Matrix
volatile int key1 = -1;
volatile int key2 = -1;
volatile uint8_t keyArray[7];
SemaphoreHandle_t keyArrayMutex;

// Knob Variables
volatile int volume = 4;
volatile int function = 0;
volatile int octaveSelect = 4;

// Pin definitions
// Row select and enable
const int RA0_PIN = D3;
const int RA1_PIN = D6;
const int RA2_PIN = D12;
const int REN_PIN = A5;

// Matrix input and output
const int C0_PIN = A2;
const int C1_PIN = D9;
const int C2_PIN = A6;
const int C3_PIN = D1;
const int OUT_PIN = D11;

// Audio analogue out
const int OUTL_PIN = A4;
const int OUTR_PIN = A3;

// Joystick analogue in
const int JOYY_PIN = A0;
const int JOYX_PIN = A1;

// Output multiplexer bits
const int DEN_BIT = 3;
const int DRST_BIT = 4;
const int HKOW_BIT = 5;
const int HKOE_BIT = 6;

// Display driver object
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, bitIdx & 0x01);
  digitalWrite(RA1_PIN, bitIdx & 0x02);
  digitalWrite(RA2_PIN, bitIdx & 0x04);
  digitalWrite(OUT_PIN, value);
  digitalWrite(REN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(REN_PIN, LOW);
}

uint8_t readCols()
{
  uint8_t colVals = 0;
  // Read column values
  colVals |= (digitalRead(C0_PIN) << 0);
  colVals |= (digitalRead(C1_PIN) << 1);
  colVals |= (digitalRead(C2_PIN) << 2);
  colVals |= (digitalRead(C3_PIN) << 3);

  return colVals;
}

void setRow(uint8_t rowIdx)
{
  // Disable row select enable
  digitalWrite(REN_PIN, LOW);
  // Set row select pins
  digitalWrite(RA0_PIN, rowIdx & 0x01);
  digitalWrite(RA1_PIN, (rowIdx >> 1) & 0x01);
  digitalWrite(RA2_PIN, (rowIdx >> 2) & 0x01);
  // Enable row select enable
  digitalWrite(REN_PIN, HIGH);
}

void sampleISR()
{
  switch(function) {
    case 0:
      // SAW
      {
        uint32_t stepSize1 = currentStepSize1;
        uint32_t stepSize2 = currentStepSize2;
        static uint32_t phaseAcc1 = 0;
        static uint32_t phaseAcc2 = 0;

        phaseAcc1 += stepSize1;
        phaseAcc2 += stepSize2;
        int32_t Vout1 = (phaseAcc1 >> 24) - 128;
        Vout1 = Vout1 >> (8 - volume);
        int32_t Vout2 = (phaseAcc2 >> 24) - 128;
        Vout2 = Vout2 >> (8 - volume);
        analogWrite(OUTR_PIN, Vout1 + Vout2 + 128);

      }
      break;
    case 1:
      // SQUARE
      {
        uint32_t stepSize = currentStepSize1;
        static uint32_t phaseAcc = 0;
        phaseAcc += stepSize;
        int32_t Vout;
        if (phaseAcc <= UINT32_MAX / 2)
        {
          Vout = 128;
        }
        else
        {
          Vout = 0;
        }
        Vout = Vout >> (8 - volume);
        analogWrite(OUTR_PIN, Vout);
      }
      break;
    case 2:
      // TRIANGLE
      {
        uint32_t stepSize1 = currentStepSize1;
        uint32_t stepSize2 = currentStepSize2;
        static uint32_t phaseAcc1 = 0;
        static uint32_t phaseAcc2 = 0;
    
        phaseAcc1 += stepSize1;
        phaseAcc2 += stepSize2;

        int32_t Vout1 = 0;
        if (phaseAcc1 < UINT32_MAX / 2)
        {
          Vout1 = (phaseAcc1 >> 23) - 128;
        }
        else
        {
          Vout1 = (UINT32_MAX - phaseAcc1) >> 23;
          Vout1 = -128 + (Vout1 & 0xFF);
        }
        
        int32_t Vout2 = 0;
        if (phaseAcc2 < UINT32_MAX / 2)
        {
          Vout2 = (phaseAcc2 >> 23) - 128;
        }
        else
        {
          Vout2 = (UINT32_MAX - phaseAcc2) >> 23;
          Vout2 = -128 + (Vout2 & 0xFF);
        }

        Vout1 = (Vout1 * volume) / 8;
        Vout2 = (Vout2 * volume) / 8;
        analogWrite(OUTR_PIN, Vout1 + Vout2 + 128);
      }
      break;
    case 3:
      // SINE
      {
        uint32_t stepSize = currentStepSize1;
        static uint32_t phaseAcc = 0;
        phaseAcc += stepSize;
        int32_t Vout;
        if (phaseAcc < UINT32_MAX / 2)
        {
          float sinValue = sin(phaseAcc * 2.0 * PI / UINT32_MAX);
          Vout = 128 + 1 * sinValue;
          Vout = Vout >> (8 - volume);
        }
        else {
          Vout = 0;
        }
        analogWrite(OUTR_PIN, Vout);
      }
      break;
    default:
      // invalid value of "function"
      break;
  }
}

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
    else if (keyVal == 0b00 && m_prevAB == 0b11)
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

void scanKeysTask(void *pvParameters)
{
  const TickType_t xFrequency = 20 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint32_t stepSize1;
  uint32_t stepSize2;
  uint8_t prevAB = 0;
  uint8_t currVol = 0;
  int prevTransition = 0;

  // Knob Constructors
  Knob volumeKnob(0, 8, &volume);
  Knob functionKnob(0, 3, &function);
  Knob octaveKnob(2, 8, &octaveSelect);

  bool key1Pressed = false;

  while (1)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    key1 = -1;
    key2 = -1;
    key1Pressed = false;

    // Loop over rows 0 to 2 to detect key presses
    for (uint8_t row = 0; row < 5; row++)
    {
      setRow(row);
      delayMicroseconds(3);

      xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
      keyArray[row] = readCols();
      // Translate binary key values to key position (1-12)
      if (row < 3)
      { 
        if ((keyArray[row] & 0b0001) == 0b0000)
        {
          if (key1Pressed == false){
            key1 = (4 * row);
            key1Pressed = true;
          }
          else{
            key2 = (4 * row);
          }
        }
        if ((keyArray[row] & 0b0010) == 0b0000)
        {
          if (key1Pressed == false){
            key1 = (4 * row) + 1;
            key1Pressed = true;
          }
          else{
            key2 = (4 * row) + 1;
          }
        }
        if ((keyArray[row] & 0b0100) == 0b0000)
        {
          if (key1Pressed == false){
            key1 = (4 * row) + 2;
            key1Pressed = true;
          }
          else{
            key2 = (4 * row) + 2;
          }
        }
        if ((keyArray[row] & 0b1000) == 0b0000)
        {
          if (key1Pressed == false){
            key1 = (4 * row) + 3;
            key1Pressed = true;
          }
          else{
            key2 = (4 * row) + 3;
          }
        }
      }

      // Serial.print(key1);
      // Serial.print(":");
      // Serial.println(key2);

      // Update Knobs
      volumeKnob.update(keyArray[4] >> 2);    // KNOB 0       ( 0 )    ( 1 )    ( 2 )    ( 3 )
      functionKnob.update(keyArray[4] & 0x03);// KNOB 1      [4]>>2  [4]&0x03  [3]>>2  [3]&0x03
      octaveKnob.update(keyArray[3] & 0x03);  // KNOB 3       Row4     Row4     Row3     Row3

      xSemaphoreGive(keyArrayMutex);
    }
    // If no key pressed disable sound
    stepSize1, stepSize2 = 0;
    if (key1 != -1){
      stepSize1 = stepSizes[key1+12*(octaveSelect-2)];
      if (key2 != -1){
        stepSize2 = stepSizes[key2+12*(octaveSelect-2)];
      }
      else {
        stepSize2 = 0;
      }
    }
    else {
      stepSize1 = 0;
    }
    __atomic_store_n(&currentStepSize1, stepSize1, __ATOMIC_RELAXED);
    __atomic_store_n(&currentStepSize2, stepSize2, __ATOMIC_RELAXED);
  }
}

void displayKeysTask(void *pvParameters)
{
  const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont10_tf);

    u8g2.setCursor(90, 10);
    u8g2.print("Vol:");
    u8g2.print(volume);

    u8g2.setCursor(90, 30);
    u8g2.print("Oct:");
    u8g2.print(octaveSelect);

    u8g2.setCursor(2, 30);
    u8g2.print("WAVE:");
    switch (function)
    {
      case 0:
        u8g2.print("Saw");
        break;
      case 1:
        u8g2.print("Square");
        break;
      case 2:
        u8g2.print("Triangle");
        break;
      case 3:
        u8g2.print("Sine");
        break;
      default:
        u8g2.print("Unknown sound");
        break;
    }
    u8g2.setCursor(2, 10);
    // Do not display key
    if (key1 == -1)
    {
      u8g2.print("KEY: --");
      u8g2.setCursor(2, 20);
      u8g2.sendBuffer();
    }
    // Display key
    if (key1 != -1 && key2 == -1)
    {
      u8g2.print("KEY: ");
      u8g2.print(notes[key1]);
      u8g2.sendBuffer();
    }
    if (key1 != -1 && key2 != -1)
    {
      u8g2.print("KEY: ");
      u8g2.print(notes[key1]);
      u8g2.print(" ");
      u8g2.print(notes[key2]);
      u8g2.sendBuffer();
    }
    digitalToggle(LED_BUILTIN);
  }
}

void setup()
{
  // Set pin directions
  pinMode(RA0_PIN, OUTPUT);
  pinMode(RA1_PIN, OUTPUT);
  pinMode(RA2_PIN, OUTPUT);
  pinMode(REN_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(OUTL_PIN, OUTPUT);
  pinMode(OUTR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(C0_PIN, INPUT);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  pinMode(C3_PIN, INPUT);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);

  // Initialise display
  setOutMuxBit(DRST_BIT, LOW); // Assert display logic reset
  delayMicroseconds(2);
  setOutMuxBit(DRST_BIT, HIGH); // Release display logic reset
  u8g2.begin();
  setOutMuxBit(DEN_BIT, HIGH); // Enable display power supply

  // Initialise UART
  Serial.begin(9600);

  // Create the mutex and assign its handle
  keyArrayMutex = xSemaphoreCreateMutex();

  // Create timer for audio
  TIM_TypeDef *Instance = TIM1;
  HardwareTimer *sampleTimer = new HardwareTimer(Instance);
  sampleTimer->setOverflow(22050, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();

  // Runs the key detection and display on separate tasks
  TaskHandle_t scanKeysHandle = NULL;
  xTaskCreate(scanKeysTask, "scanKeys", 64, NULL, 2, &scanKeysHandle);
  TaskHandle_t displayKeysHandle = NULL;
  xTaskCreate(displayKeysTask, "displayKeys", 256, NULL, 1, &displayKeysHandle);
  vTaskStartScheduler();
}

void loop()
{
  delay(100);
}