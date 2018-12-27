#ifndef APPGPS_H
#define APPGPS_H


#include "GeneralTypeDef.h"

extern uByte flag_Power_err;
//extern uByte flag_inverter_err;

extern void GPS_Init(void);
extern void GPS_MainCbk(void);
extern void GPS_RxApp(void);
extern void GPS_TxApp(void);
extern void GPS_CheckExist(void);
extern void GPS_MainCountCbk(void);


#endif
