// IntCan.h
#ifndef INTCAN_H
#define INTCAN_H

#include "CanIf_Type.h"
#include "Ecu.h" 
#include "EcuConf.h"
#include "GeneralTypeDef.h"
#include "CanTp_Type.h"
#include "Version_Type.h"


// For IncCAN 0x4200~0x421F
#define ERR_INTCAN_ERROR_CMD  ERR_PROTOCOL_INTCAN_BASE  // 0x4200

//define for slave relay ctrl
typedef enum
{
    RELAY_OFF,
    RELAY_ON,
}eIntCan_SlaveRelayCtrlType;

typedef enum
{
    SLAVE_RELAY1 ,
    SLAVE_RELAY2 ,
    SLAVE_RELAY3 ,
    SLAVE_RELAY4 ,
    SLAVE_RELAY5 ,
    SLAVE_RELAY6 ,
}eIntCan_SlaveRelayNoType;

// define for control slave relay st
typedef union
{
    uWord wd;
    struct
    {
        uByte rly4 : 2;
        uByte rly3 : 2;
        uByte rly2 : 2;
        uByte rly1 : 2;

        uByte rly8 : 2;
        uByte rly7 : 2;
        uByte rly6 : 2;
        uByte rly5 : 2;
    } bit;
} uIntCan_SlaveRlyStType;

// define for BCU control BMU relay
typedef struct
{
    uIntCan_SlaveRlyStType RlySt;
    uByte BcuCtrlEn;
} sIntCan_BcuCtrlBmuRlyType;

 
// define for balance
typedef enum
{
    CAL_BAL = 1,
    START_BAL,
    STOP_BAL,
    FORCE_BAL,
}eIntCan_BalCmdType;


typedef struct 
{
  uWord sumv;
  uWord sumvOut1;
  uWord sumvOut2;
  uWord avgCurr;
  uWord curr;
  uWord soc;
  uWord soh;
  uWord maxv;
  uWord maxv_no;
  uWord minv;
  uWord minv_no;
  uByte maxt;
  uWord maxt_no;
  uByte mint;
  uWord mint_no;
  uByte io_state[8];
  uByte bat_err[8];
  uByte batSys_err[8];
  uByte other_err[8];
  uByte hard_err[8];
  uByte bmu_num;
  uByte bms_life;
  uByte supply_voltage;
  uByte bmu_volt_num[15];
  uByte bmu_temp_num[15];
  uByte bms_time[6];
  dword inah;
  dword outah;
}sBms_Info;

extern sBms_Info sbms_Info;

extern sCanTp_MsgType CanTpMsgTx;
extern sCanTp_MsgType CanTpMsgRx;




extern void IntCan_Init(void);
extern void IntCan_MainReceiveMsg(void);
extern void IntCan_MainSendMsg(void);
extern void IntCan_MainProcess(void);
void IntCan_CountCbk(void);


// for balance
extern void IntCan_SendBalCmd(eIntCan_BalCmdType cmd, uByte* ptr, uByte len);

// for control slave relay
void IntCan_CtrlSlaveRelay(uByte bmu_no, eIntCan_SlaveRelayNoType rly_num, eIntCan_SlaveRelayCtrlType cmd);

void IntCan_WrTestFrameToBuff(uByte index,uByte* ptr, uByte en);

uByte IntCan_GetTestModeSt(void);

uWord IntCan_GetBmuMaxOutLineCnt(void);
uWord IntCan_GetBmuOutLineCnt(uByte bmu);
sCanIf_MsgType IntCan_GetSelfConfRecvMsg0(void);
sCanIf_MsgType IntCan_GetSelfConfRecvMsg1(void);
sCanIf_MsgType IntCan_GetSelfConfRecvMsg2(void);
extern Bool FlagUpdate;
extern Bool FlagConfig;
extern Bool FlagRead;
extern uByte readData[68];

extern uByte ConfigResult;  //0:Unknow 1:PASS  2:FAIL  3:TIME OUT

#endif /* INTCAN_H */