#include "air_ok_ops.h"

// Version info
#define MAJOR   4
#define PATCH   0
#define MINOR   2
#define TWEAK   2

#define THRESHOLD_TIME 250 // min touch time 250 ms
extern void  setFanSpeed(uint8_t uiLevel);
extern void  setFanLED(uint8_t uiLevel);
//keyChangeInfo_t keyChangeInfo;
hw_timer_t *timer = NULL;
stateMachineInfo_t stateMachineInfo = {ON_STDBY, SPEED1, false, 0, 0, 0, 0, false};

uint32_t touchMask = 0;
int touchTime = 0;
int releaseTime = 0;
bool touchStatus = false;
uint32_t touchKey;
uint32_t IRKey;
bool reportStateChange = false;

static void onTimeOut()
{
  if (stateMachineInfo.filterExpireTimeoutCtr >= FILTER_EXPIRE_TIMEOUT)
  {
    stateMachineInfo.stateMachine = FILTER_EXPIRED;
    stateMachineInfo.pwrOffTimeoutCtr = 0;
    stateMachineInfo.sleepTimeoutCtr = 0;
    stateMachineInfo.pwrOffTimerEnabled = false;
    stateMachineInfo.smInfoChanged = true;
    reportStateChange = true;

    timerStop(timer);
  }
  switch (stateMachineInfo.stateMachine)
  {
    case ON_STDBY:
      {
        stateMachineInfo.pwrOffTimeoutCtr = 0;
        stateMachineInfo.sleepTimeoutCtr = 0;
        stateMachineInfo.pwrOffTimerEnabled = false;
        stateMachineInfo.smInfoChanged = true;

        timerStop(timer);
      }
      break;

    case NORMAL:
      {
        ++stateMachineInfo.filterExpireTimeoutCtr;
        ++stateMachineInfo.sleepTimeoutCtr;
        if (stateMachineInfo.pwrOffTimerEnabled)
          ++stateMachineInfo.pwrOffTimeoutCtr;
        if (stateMachineInfo.sleepTimeoutCtr > SLEEP_TIMEOUT_COUNT)
        {
          stateMachineInfo.stateMachine = SLEEP;
          reportStateChange = true;
          stateMachineInfo.sleepTimeoutCtr = 0;
        }
        stateMachineInfo.smInfoChanged = true;
      }
      break;
    case SLEEP:
      {
        ++stateMachineInfo.filterExpireTimeoutCtr;
        if (stateMachineInfo.pwrOffTimerEnabled)
        {
          ++stateMachineInfo.pwrOffTimeoutCtr;
          
          if (stateMachineInfo.pwrOffTimeoutCtr > stateMachineInfo.pwrOffTimeLimit)
          {
            stateMachineInfo.stateMachine = ON_STDBY;
            reportStateChange = true;
            stateMachineInfo.pwrOffTimeoutCtr = 0;
            timerStop(timer); // @VERIFY: timer must be stopped here as state machine went to ON_STDBY
          }
          
        }
        
        stateMachineInfo.smInfoChanged = true;
       
      }
      break;
  }
}

void restartAirTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimeOut, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
  timerRestart(timer);
}

void setup()
{
  // Begin serial debug port
  Serial.begin(115200);
  delay(500); // do not place any code between these prints
  Serial.println("");
  Serial.println("+--------------------------+");
  Serial.println("|   AIR OK DEBUG CONSOLE   |");
  Serial.println("+--------------------------+");
  Serial.print("Version: ");
  Serial.print(MAJOR);
  Serial.print(".");
  Serial.print(MINOR);
  Serial.print(".");
  Serial.print(PATCH);
  Serial.print(".");
  Serial.println(TWEAK);

  // your code goes from here before this no printing no other operation
  delay(500);
  beginSetup();
}

void loop()
{
  uint32_t key = 0;
  if (decodedResults())
  {
    getIRKeyStatus();
    change_line_activated = true;
    resumeIR();
  }
  else
  {
    getValidKey();
  }
  //  getValidKey();

  if (touchStatus)
  {
    key = touchKey | IRKey;
    Serial.print("Key touch detected: ");
    Serial.println(key);
    IRKey = 0;
    touchKey = 0;
    if (key & (1 << PWR_TOUCH ))
    {
      switch (stateMachineInfo.stateMachine)
      {
        case SLEEP:
          stateMachineInfo.stateMachine = NORMAL;
          enforceModeNORMAL();
          stateMachineInfo.smInfoChanged = true;
          reportStateChange = true;
          beep(NOTE_E7, 250);
          break;

        case ON_STDBY:
          stateMachineInfo.stateMachine = NORMAL;
          enforceModeNORMAL();
          stateMachineInfo.smInfoChanged = true;
          reportStateChange = true;
          restartAirTimer();
          beep(NOTE_E7, 500);
          break;

        case NORMAL:
          stateMachineInfo.stateMachine = ON_STDBY;
          enforceModeONSTDBY();
          stateMachineInfo.smInfoChanged = true;
          reportStateChange = true;
          beep(NOTE_E7, 500);
          break;

//                  case FILTER_EXPIRED:
//                   reportStateChange = true;
//                    break;
      }
    }
  }

  // Enforce mode as per current state machine
  switch (stateMachineInfo.stateMachine)
  {
    case ON_STDBY:
      enforceModeONSTDBY();
      break;

    case NORMAL:
      if (touchStatus)
      {
        operateFanControl(key);
        stateMachineInfo.pwrOffTimeoutCtr = 0;
        stateMachineInfo.sleepTimeoutCtr = 0;
        stateMachineInfo.pwrOffTimerEnabled = false;
        stateMachineInfo.smInfoChanged = true;
      }
      break;

    case SLEEP:
      enforceModeSLEEP();
      break;

    case FILTER_EXPIRED:
      if (touchStatus)
        FilterExpire();
      enforceFilterExpireMode();
     
      break;
  }



  if (reportStateChange)
  {
    reportState();
  }
  touchStatus = false;

  if (stateMachineInfo.smInfoChanged)
  {
    setStateMachineInfo();
  }
  delay(200);
}
