#include "air_ok_ops.h"
#include<FastLED.h>

#define DATA_PIN 13
#define NUM_LEDS 1

#define RED   0
#define GREEN 1
#define BLUE  2
#define OFF   3


CRGB leds[NUM_LEDS];
const char *getSMString[] = {"ON_STANDBY", "NORMAL", "SLEEP", "FILTER_EXPIRED", "AUTO_PWROFF_ACTIVE"};

// variable for storing the touch pin value for fan touch switches
int speed1Touch, speed1TouchPrev;
int speed2Touch, speed2TouchPrev;
int speed3Touch, speed3TouchPrev;
int speed4Touch, speed4TouchPrev;
int previousSpeed = 0;

extern void restartAirTimer();

void setFanLED(uint8_t uiLevel)
{
  switch (uiLevel)
  {
    default:
    case SPEED1:
      // Set fan LED
      digitalWrite(FAN_LED1, HIGH);
      digitalWrite(FAN_LED2, LOW);
      digitalWrite(FAN_LED3, LOW);
      digitalWrite(FAN_LED4, LOW);
      break;
    case SPEED2:
      // Set fan LED
      digitalWrite(FAN_LED1, HIGH);
      digitalWrite(FAN_LED2, HIGH);
      digitalWrite(FAN_LED3, LOW);
      digitalWrite(FAN_LED4, LOW);
      break;
    case SPEED3:
      // Set fan LED
      digitalWrite(FAN_LED1, HIGH);
      digitalWrite(FAN_LED2, HIGH);
      digitalWrite(FAN_LED3, HIGH);
      digitalWrite(FAN_LED4, LOW);
      break;
    case SPEED4:
      // Set fan LED
      digitalWrite(FAN_LED1, HIGH);
      digitalWrite(FAN_LED2, HIGH);
      digitalWrite(FAN_LED3, HIGH);
      digitalWrite(FAN_LED4, HIGH);
      break;
    case FANOFF:
      digitalWrite(FAN_LED1, LOW);
      digitalWrite(FAN_LED2, LOW);
      digitalWrite(FAN_LED3, LOW);
      digitalWrite(FAN_LED4, LOW);
      break;
  }

}

void setFanSpeed(uint8_t uiLevel)
{
  switch (uiLevel)
  {
    default:
    case SPEED1:
      // Set fan control
      digitalWrite(MCU_F1, HIGH);
      digitalWrite(MCU_F2, LOW);
      digitalWrite(MCU_F3, LOW);


      Serial.println("AirPurifier: FanSpeed [ L1 ]");
      break;

    case SPEED2:
      // Set fan control
      digitalWrite(MCU_F1, LOW);
      digitalWrite(MCU_F2, HIGH);
      digitalWrite(MCU_F3, LOW);


      Serial.println("AirPurifier: FanSpeed [ L2 ]");
      break;

    case SPEED3:
      // Set fan control
      digitalWrite(MCU_F1, HIGH);
      //delay(250);
      digitalWrite(MCU_F2, HIGH);
      digitalWrite(MCU_F3, LOW);


      Serial.println("AirPurifier: FanSpeed [ L3 ]");
      break;

    case SPEED4:
      // Set fan control
      digitalWrite(MCU_F1, LOW);
      digitalWrite(MCU_F2, LOW);
      digitalWrite(MCU_F3, HIGH);


      Serial.println("AirPurifier: FanSpeed [ L4 ]");
      break;

    case FANOFF:
      // Set fan control
      digitalWrite(MCU_F1, LOW);
      digitalWrite(MCU_F2, LOW);
      digitalWrite(MCU_F3, LOW);


      break;
  }
}

static void setRGBColor(int color)
{
  switch (color)
  {
    case RED:
      {
        leds[0] = CRGB (255, 0, 0);
      }
      break;
    case GREEN:
      {
        leds[0] = CRGB (0, 255, 0);
      }
      break;
    case BLUE:
      {
        leds[0] = CRGB (0, 0, 255);
      }
      break;
    case OFF:
      {
        leds[0] = CRGB (0, 0, 0);
      }
      break;
  }

  FastLED.show();
}

static void setStateLEDS(uint8_t uiState)
{
  switch (uiState)
  {
    case ON_STDBY:
      setRGBColor(RED);
      break;

    case NORMAL:
      setRGBColor(BLUE);
      break;

    case SLEEP:
      setRGBColor(GREEN);

      break;

    default:
      Serial.println("Invalid state");
  }
}

static void resumeFan()
{
  setFanSpeed(stateMachineInfo.currentSpeed);
  setFanLED(stateMachineInfo.currentSpeed);


}

void configurePins()
{
  // Configure fan control pins as output
  pinMode (MCU_F1, OUTPUT);
  pinMode (MCU_F2, OUTPUT);
  pinMode (MCU_F3, OUTPUT);

  // Configure fan speed LED pins as output
  pinMode (FAN_LED1, OUTPUT);
  pinMode (FAN_LED2, OUTPUT);
  pinMode (FAN_LED3, OUTPUT);
  pinMode (FAN_LED4, OUTPUT);

  // Configure the buzzer pin
  pinMode(buzzerPin, OUTPUT);
}

void configureRGBLed()
{
  // init the LED object
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // set random seed
  randomSeed(analogRead(0));
}


void enforceModeNORMAL()
{

  setStateLEDS(NORMAL);

  resumeFan();
}

void enforceModeSLEEP()
{

  setStateLEDS(SLEEP);


  setFanLED(FANOFF);
}

void enforceModeONSTDBY()
{

  setStateLEDS(ON_STDBY);
  setFanSpeed(FANOFF);
  setFanLED(FANOFF);
}

void enforceFilterExpireMode()
{
  setStateLEDS(ON_STDBY);
  setFanSpeed(FANOFF);
  setFanLED(FANOFF);
}

void operateFanControl(uint32_t keys)
{
  const int CHANGE_PIN = 12;

  if (keys & (1 << speed1key ))
  {
    /* Save the current speed */
    stateMachineInfo.currentSpeed = SPEED1;
    stateMachineInfo.smInfoChanged = true;
    if (stateMachineInfo.currentSpeed != previousSpeed)
    {
      beep(NOTE_E7, 250);
      beep(NOTE_E7, 250);
      previousSpeed = stateMachineInfo.currentSpeed;
    }

    /* Set fan speed to level1 */
    setFanSpeed(stateMachineInfo.currentSpeed);
    setFanLED(stateMachineInfo.currentSpeed);

    /* Reset the sleep timeout counter */
    stateMachineInfo.sleepTimeoutCtr = 0;
  }

  if (keys & (1 << speed2key ))
  {
    /* Save the current speed */
    stateMachineInfo.currentSpeed = SPEED2;
    stateMachineInfo.smInfoChanged = true;

    if (stateMachineInfo.currentSpeed != previousSpeed)
    {
      beep(NOTE_E7, 250);
      beep(NOTE_E7, 250);
      previousSpeed = stateMachineInfo.currentSpeed;
    }


    /* Set fan speed to level2 */
    setFanSpeed(stateMachineInfo.currentSpeed);
    setFanLED(stateMachineInfo.currentSpeed);



    /* Reset the sleep timeout counter */
    stateMachineInfo.sleepTimeoutCtr = 0;
  }



  if (keys & (1 << speed3key ))
  {
    /* Save the current speed */
    stateMachineInfo.currentSpeed = SPEED3;
    stateMachineInfo.smInfoChanged = true;
    if (stateMachineInfo.currentSpeed != previousSpeed)
    {
      beep(NOTE_E7, 250);
      beep(NOTE_E7, 250);
      previousSpeed = stateMachineInfo.currentSpeed;
    }


    /* Set fan speed to level3 */
    setFanSpeed(stateMachineInfo.currentSpeed);
    setFanLED(stateMachineInfo.currentSpeed);



    /* Reset the sleep timeout counter */
    stateMachineInfo.sleepTimeoutCtr = 0;
  }

  if (keys & (1 << speed4key ))
  {

    /* Save the current speed */
    stateMachineInfo.currentSpeed = SPEED4;
    stateMachineInfo.smInfoChanged = true;
    if (stateMachineInfo.currentSpeed != previousSpeed)
    {
      beep(NOTE_E7, 250);
      beep(NOTE_E7, 250);
      previousSpeed = stateMachineInfo.currentSpeed;
    }


    /* Set fan speed to level4 */
    setFanSpeed(stateMachineInfo.currentSpeed);
    setFanLED(stateMachineInfo.currentSpeed);


    /* Reset the sleep timeout counter */
    stateMachineInfo.sleepTimeoutCtr = 0;
  }

}


void beep(int note, int duration)
{
  tone(buzzerPin, note, duration);
}

void reportState()
{
  switch (stateMachineInfo.stateMachine) //2
  {
    case SLEEP:
      beep(NOTE_E7, 250);
      Serial.println("AirPurifier: state - SLEEP");
      break;

    case ON_STDBY:
      Serial.println("AirPurifier: state - ON | STANDBY");
      break;

    case NORMAL:
      Serial.println("AirPurifier: state - NORMAL");
      break;

    case FILTER_EXPIRED:

      FilterExpire();
      Serial.println("AirPurifier: state - FILTER HAS EXPIRED ");



  }
  reportStateChange = false;
}

void FilterExpire()
{
  for (int i = 0; i < 5; i++)
  {
    beep(NOTE_E7, 250);
    setRGBColor(RED);
    delay(500);
    setRGBColor(OFF);

    delay(500);
  }

}

static void restoreStateMachine()
{
  if (stateMachineInfo.filterExpireTimeoutCtr >= FILTER_EXPIRE_TIMEOUT)
  {
    stateMachineInfo.stateMachine = FILTER_EXPIRED;
    reportStateChange = true;
    enforceFilterExpireMode();
  }
  else
  {
    Serial.println("\n\n");
    Serial.print("AirPurifier: State ");
    Serial.println(getSMString[stateMachineInfo.stateMachine]);
    if (stateMachineInfo.stateMachine == NORMAL)
    {
      setFanSpeed(stateMachineInfo.currentSpeed);
      setFanLED(stateMachineInfo.currentSpeed);

      restartAirTimer();
    }
    else if (stateMachineInfo.stateMachine == SLEEP)
    {
      setFanSpeed(stateMachineInfo.currentSpeed);
      setFanLED(FANOFF);
      restartAirTimer();
    }
    Serial.println ("AirPurifier: Waiting for touch...");
    Serial.print("Current auto power off status: ");
    Serial.println(stateMachineInfo.pwrOffTimerEnabled);

  }
}

void beginSetup()
{
  // Setup the eeprom
  setupEEProm();

  //  setStateMachineInfo();

  // Get the initial state of device from eeprom
  getStateMachineInfo();

  // ConfigsetFanSpeedure the IOs
  configurePins();

  // Setup RGB LEDs
  configureRGBLed();

  // Configure the touch sensor;
  configureTouchSensor();

  // Setup IR receiver
  setupIRReceiver();

  // Restore the last saved state machine
  restoreStateMachine();
}

void resetfilter()
{
  Serial.println("....................Filter resetted............ ");
  stateMachineInfo.stateMachine =  NORMAL;
  stateMachineInfo.filterExpireTimeoutCtr = 0;
}
