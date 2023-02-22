#include "air_ok_ops.h"
#include <Wire.h>
#include <IRremote.h>

// Define sensor pin
const int RECV_PIN = 17;
IRrecv irrecv(RECV_PIN);
decode_results results;
//extern uint32_t IRKey;

void setupIRReceiver()
{
  irrecv.enableIRIn();
}

bool decodedResults()
{
  if (irrecv.decode(&results))
    return true;
  else
    return false;
}

void resumeIR()
{
  irrecv.resume();
}

void beepsound()
{
  for (int i = 0; i < 3; i++)
  {
    delay(500);

    beep(NOTE_E7, 250);
    delay(500);
  }
}
void getIRKeyStatus()
{
  switch (results.value)
  {
    case PWR_BTN_HEX:
      IRKey = 1 << PWR_TOUCH;
      touchStatus = true;
      break;
    case FAN_L1_HEX:
      IRKey = 1 << speed1key;
      touchStatus = true;
      break;
    case FAN_L2_HEX:
      IRKey = 1 << speed2key;
      touchStatus = true;
      break;
    case FAN_L3_HEX:
      IRKey = 1 << speed3key;
      touchStatus = true;
      break;
    case FAN_L4_HEX:
      IRKey = 1 << speed4key;
      touchStatus = true;
      break;
    case PWR_2HR_HEX:
      stateMachineInfo.pwrOffTimerEnabled = true;
      stateMachineInfo.pwrOffTimeLimit = TIME_OUT_2HR;
      if (stateMachineInfo.stateMachine == NORMAL || stateMachineInfo.stateMachine == SLEEP )
        Serial.println("AirPurifier: Auto power off timer set for 2 hours");
      if (stateMachineInfo.stateMachine != ON_STDBY )
        beepsound();

      stateMachineInfo.smInfoChanged = true;
      touchStatus = false;
      break;
    case PWR_4HR_HEX:
      stateMachineInfo.pwrOffTimerEnabled = true;
      stateMachineInfo.pwrOffTimeLimit = TIME_OUT_4HR;
      if (stateMachineInfo.stateMachine == NORMAL || stateMachineInfo.stateMachine == SLEEP )
        Serial.println("AirPurifier: Auto power off timer set for 4 hours");
      if (stateMachineInfo.stateMachine != ON_STDBY )
        beepsound();
      stateMachineInfo.smInfoChanged = true;
      touchStatus = false;
      break;
    case PWR_8HR_HEX:
      stateMachineInfo.pwrOffTimerEnabled = true;
      stateMachineInfo.pwrOffTimeLimit = TIME_OUT_8HR;
      if (stateMachineInfo.stateMachine == NORMAL || stateMachineInfo.stateMachine == SLEEP )
        Serial.println("AirPurifier: Auto power off timer set for 8 hours");
      if (stateMachineInfo.stateMachine != ON_STDBY )
        beepsound();
      stateMachineInfo.smInfoChanged = true;
      touchStatus = false;
      break;
    default:
      IRKey = 0;
      touchStatus = false;
  }
}
