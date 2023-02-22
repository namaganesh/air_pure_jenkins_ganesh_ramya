#ifndef _IR_SENSOR
#define _IR_SENSOR

#define PWR_BTN_HEX 0x1B7E10EF

#define FAN_L1_HEX  0x1B7E807F
#define FAN_L2_HEX  0x1B7E40BF
#define FAN_L3_HEX  0x1B7EC03F
#define FAN_L4_HEX  0x1B7E20DF
#define PWR_2HR_HEX  0x1B7E906F
#define PWR_4HR_HEX  0x1B7E08F7
#define PWR_8HR_HEX  0x1B7E48B7

void getIRKeyStatus();
void setupIRReceiver();
bool decodedResults();
void resumeIR();

#endif
