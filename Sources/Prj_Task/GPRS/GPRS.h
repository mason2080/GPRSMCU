#ifndef GPRS_H
#define GPRS_H


#include "GeneralTypeDef.h"
#include "ClockIf.h"

extern uByte flag_Power_err;
//extern uByte flag_inverter_err;

extern void GPRS_Init(void);
extern void GPRS_MainCbk(void);
extern void GPRS_RxApp(void);
extern void GPRS_TxApp(void);
extern void GPRS_CheckExist(void);
extern void GPRS_MainCountCbk(void);
extern uByte VIN[13];
extern uByte IP[15];
extern uByte CARNUM[8];
extern uByte PORT[5];
extern uByte PlatForm;
extern uByte YT_VNUM;
extern uByte YT_TNUM;

extern uWord YT_CellVolt[120];
extern uByte YT_CellTemp[60];
extern uByte IMEI[15];

extern uByte InAh[4];
extern uByte OutAh[4];
extern int  SendCanIndex;

extern uByte ConfigData[80];


typedef enum {Idle1,
              ATCMD1,
              GETIMEI,
              ATCMD3,
              ATCMD4,
              ATCMD5,
              ATCMD6,
              SEND1,
              SEND2,
              SEND_START,
              SEND_MSG,
              CHECK_EXIST1,
              INIT,
              REPOWER,
              DOWNFILE,
              UPDATE,
              CHECKFILE,
              CONFIG,
              READ
              }GPRS_STEP;
              
typedef enum 
        {
            reserve,//0 :reserve
            SetSoc,
            SetSoh,
            SetClock,
            SetCarNumber,
            SetProductNo,     
            SetConfigVersion,
            
            SetAh,
            SetVoltNumber,
            SetTempNumber,
            
            SetTemControl,
            SetChgInfo ,
            
            SetLevel1,
            SetLevel2,
            SetLevel3,        
            SetBatInfo,
            SetChanVolNum,
            SetFuseChan,
            Set2dCode,
            SetChgAh,
            SetCellNum,
            SetChgTimes,
            SetBmuNo
        }BCUCONFIG_STEP;
        
 typedef enum 
        {
            reserve1,//0 :reserve
            ReadSoc,
            ReadSoh,
            ReadClock,
            ReadCarNumber,
            ReadProductNo,
            ReadConfigVersion,
            ReadAh,
            ReadVoltNumber,
            ReadTempNumber,
            ReadTemControl,
            ReadChgInfo,
            ReadLevel1,
            ReadLevel2,
            ReadLevel3,
            ReadBatInfo,
            Read2dCode,
            ReadSoftVer,
            
            ReadBmuNo,
            ReadChanVolNum,    
            ReadFuseChan,
            
            ReadChgAh,  
            ReadCellNum,
            ReadChgTimes,
        }BCUREAD_STEP;     
              
extern GPRS_STEP GPRS_Step;
extern BCUCONFIG_STEP BcuConfig_Step;
extern BCUREAD_STEP   BcuReadStep   ;
extern sClockIf_TimeType time;
extern uByte readCmd;


#endif
