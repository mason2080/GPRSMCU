// CarCan.h
#ifndef CARCAN_H
#define CARCAN_H

#include "CanIf_Type.h"
#include "CanIf.h"
#include "Ecu.h"

#include "Project.h"


#define MAX_CAR_CAN_SEND_BUF 1   //the oringin is 10

#define CARCAN_CHG_CUR_MAX_T_POINT      3
#define CARCAN_CUR_MAX_SOC_POINT        11

#define CARCAN_DIS_CUR_MAX_T_POINT      4
#define CARCAN_CUR_MAX_SOC_POINT        11

typedef struct
{
    uWord TimeCnt;
    uByte FirstSendFlag;
    uByte DelayTime; 
}sCarCanCtrlType;

typedef struct
{
    sCanIf_MsgType Msg;
    uWord Period;
    sCarCanCtrlType SendCtrl; 
}sCarCanMsgType;


#ifdef CAR_CAN_EN
extern void CarCan_Init(void);
extern void CarCan_MainReceiveMsg(void);
extern void CarCan_MainSendMsg(void);
extern void CarCan_CountCbk(void);
extern void CarCan_ProMain(void);

extern uWord GET_ConstDisCur(uByte tem, uByte soc);
extern uWord GET_ConstChgCur(uByte tem, uByte soc); 
#endif


#endif /* CARCAN_H */