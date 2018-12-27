#ifndef APP485_H
#define APP485_H


#include "GeneralTypeDef.h"

extern uByte flag_Power_err;
//extern uByte flag_inverter_err;

extern void APP485_Init(void);
extern void APP485_MainCountCbk(void);
extern void APP485_MainCbk(void);
extern void CPU485_RxApp(void);
extern void CPU485_TxApp(void);
#endif
