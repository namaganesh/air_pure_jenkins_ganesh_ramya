#ifndef _AIR_OK_OPS_H
#define _AIR_OK_OPS_H

#include <stdint.h>
#include <stdbool.h>
#import <Arduino.h>
#include"pitches.h"
#include "EEpromOps.h"
#include "esp_system.h"
#include "TouchSensor.h"
#include <EEPROM.h>
#include <AT42QT2120.h>
#include <Wire.h>
#include "IRSensor.h"


#include<AT42QT.h>
#define buzzerPin   16

//Power touch switch pin configuration
#define PWR_TOUCH    8

//Fan control touch
#define speed1key   6
#define speed2key   4
#define speed3key   5
#define speed4key   7

//Power touch switch modes
#define ON_STDBY            0
#define NORMAL              1
#define SLEEP               2
#define FILTER_EXPIRED      3
#define AUTO_PWROFF_ACTIVE  4

// Power Time out modes
#define TWO_HR    0
#define FOUR_HR   1
#define EIGHT_HR  2


//Fan speed control modes
#define FANOFF 0
#define SPEED1 1
#define SPEED2 2
#define SPEED3 9
#define SPEED4 4

//Fan speed control pins
#define MCU_F1   5
#define MCU_F2   18
#define MCU_F3   19
// Fan speed4 combines F2 and F3

//Fan speed LED pins
#define FAN_LED1  23
#define FAN_LED2  4
#define FAN_LED3  15
#define FAN_LED4  2

#define SLEEP_TIMEOUT_COUNT 10 //120 sec
#define FILTER_EXPIRE_TIMEOUT 500 // just to check for now
#define TIME_OUT_2HR 30
#define TIME_OUT_4HR 40
#define TIME_OUT_8HR 50

typedef struct {
  int stateMachine;
  int currentSpeed;
  bool pwrOffTimerEnabled;
  int pwrOffTimeoutCtr;
  int pwrOffTimeLimit;
  int sleepTimeoutCtr;
  int filterExpireTimeoutCtr;
  bool smInfoChanged;
} stateMachineInfo_t;

// @TODO: implement this once modularisation is tested
#if 0
typedef struct {
  uint32_t touchMask = 0;
  int touchTime = 0;
  int releaseTime = 0;
  bool touchStatus = false;
  uint32_t touchKey, IRKey;
} keyChangeInfo_t;
#endif

extern uint32_t touchMask;
extern int touchTime;
extern int releaseTime;
extern bool touchStatus;
extern uint32_t touchKey;
extern uint32_t IRKey;
extern stateMachineInfo_t stateMachineInfo;
extern bool reportStateChange;
//extern IRrecv irrecv;
extern bool change_line_activated;
//extern decode_results results;

extern void enforceModeONSTDBY();
extern void enforceModeNORMAL();
extern void enforceModeSLEEP();
extern void enforceFilterExpireMode();
extern void configurePins();
extern void operateFanControl(uint32_t keys);
extern void configureRGBLed();
extern void beep(int note, int duration);
extern void resetfilter();
extern void storeSMToEEPROM();
extern void resetfilter();
extern void reportState();
extern void beginSetup();
extern void FilterExpire();


#endif //End of _AIR_OK_OPS_H
