#include "air_ok_ops.h"
#include <EEPROM.h>

void setupEEProm()
{
  EEPROM.begin(512);
}

void getStateMachineInfo()
{
  EEPROM.get(0, stateMachineInfo);
}

void setStateMachineInfo()
{
  EEPROM.put(0, stateMachineInfo);
  EEPROM.commit();
  stateMachineInfo.smInfoChanged = false;
}
