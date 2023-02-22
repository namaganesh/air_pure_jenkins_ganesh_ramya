#include "air_ok_ops.h"

#define KEY_THRESHOLD 3

const int CHANGE_PIN = 12;
AT42QT2120 touch_sensor(Wire, CHANGE_PIN);
const int RESET_DELAY = 2000;
const int CALIBRATION_LOOP_DELAY = 50;
//extern uint32_t touchKey;

bool change_line_activated;

static void changeLineActivated()
{
  change_line_activated = true;
}

void setKeyThresholds()
{
  uint8_t i = 0;
  for (i = 0; i < 12; i++)
  {
    touch_sensor.setKeyDetectThreshold(i, KEY_THRESHOLD);
  }
}

void configureTouchSensor()
{
  // Begine touch sensor setup
  touch_sensor.begin();
  touch_sensor.reset();
  delay(RESET_DELAY);
  
  Serial.println("AirPurifier: Trigger calibration");
  touch_sensor.triggerCalibration();
  delay(CALIBRATION_LOOP_DELAY);
  Serial.println("AirPurifier: Calibrating...");
  while (touch_sensor.calibrating())
  {
    delay(CALIBRATION_LOOP_DELAY);
  }
  Serial.println("AirPurifier: Finished calibration");
  touch_sensor.attachChangeCallback(changeLineActivated);

  Serial.println("AirPurifier: Confugring new threshold values");
  setKeyThresholds();
  Serial.println("AirPurifier: Done");
}

void getValidKey()
{
  if (change_line_activated)
  {
    change_line_activated = false;
    AT42QT2120::Status status = touch_sensor.getStatus();
    Serial.print("Touch key status: ");
    Serial.println(status.keys);
    
    if (status.keys)
    {
      touchMask |= status.keys;
      touchTime = millis();
      touchStatus = false;
      Serial.print("Touch / Release Mask: ");
      Serial.println(touchMask);
    }
    else
    {
      if ( ((touchMask & (touchMask - 1)) == 0) && (touchMask != 0) )
      {
        releaseTime = millis();
        Serial.print("Time difference: ");
        Serial.println(releaseTime - touchTime);
        if ((releaseTime - touchTime) > 250)
        {
          touchStatus = true;
          touchKey = touchMask;
        }

        /// Resetting filter check for 5 seconds? yes sir ok / while sending to them change 5000 t0 20000, ok
        if (((releaseTime - touchTime) > 5000) && (touchMask == 256) && (stateMachineInfo.stateMachine == FILTER_EXPIRED))
        {
          resetfilter();
          FilterExpire();
          touchStatus = true;
          touchKey = touchMask;
        }

      }
      else
      {
        Serial.println("Multi touch error");
        touchStatus = false;
        touchKey = 0;
      }
      touchMask = 0;
      touchTime = 0;
      releaseTime = 0;
    }
  }
}
