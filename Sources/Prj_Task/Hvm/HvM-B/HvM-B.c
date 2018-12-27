// HvM.c

#include "HvM-B.h"
#include "ErrorAlarm.h"
#include "AppRelay.h"
#include "BatterySt.h"
#include "ChargeSt.h"
#include "BmsSysSt.h"
#include "BatterySysSt.h"
#include "SensorDi.h"
#include "SchIf.h"
#include "LvM.h"
#include "DataStore.h"
#include "ChgM-B.h"
#include "AppDo.h" 
#include "APP485.h"

#ifdef USE_HVM_B

typedef struct
{
    uByte StartPreChg;
    uByte HvPowerOnEnd;
    uByte FirstBoot;
    uByte ReadMinVolt;
    uByte ReadSumVolt;
    uByte ForbidPowerOn;
    uByte PreFirstEnd;
    uByte ChgRelaySt;
    uByte NegRelaySt;
    uByte PosRelaySt;
    uByte PreRelaySt;
    uByte OpenNegRly;
    uByte CloseNegRly;

} sHvM_FlagType;

typedef struct
{
    uWord   StartPre ;
    uWord   EndPre;
    uWord   ReadSumVolt;
    uWord   ReadMinVolt;
    uWord   OpenRelay;
    uWord   reOpenRel;
    uWord   WaitVcuCmd;
    uWord   Step;
    uWord   fault;
    uWord   TriggerOff;
    uWord   NegRelayFilter;
    uWord   ChgRlyOffDelay;
    uWord   ChgRlyOnDelay;
    uWord   ChgRlyClose;
} sHvM_CntType;

typedef struct
{
    uByte   Step;
    uByte   BcuCmd;
    uByte   NegNeedChk;
    uByte   PosNeedChk;
    uWord   BcuSumvOutBak;

} sHvM_CtrlHvType;

typedef struct
{
    uByte   IsoErr;
    uWord   SumvOut;
    uWord   SumvIn;
    uWord   Current;
} sHvM_BatInf;

typedef struct
{
    byte cnt;
} sHvM_TmCntType;


//#define CHECK_PRE

#define PWR_ON_INIT       0
#define N_CHK    	        1
#define N_ON							2
#define P_CHK			        3
#define WAIT_PWR_ON				4
#define PWR_ON						5
#define PWR_ON_P_CHK  		6
#define WAIT_PWR_OFF			7
#define PWR_OFF_FST_VCU		8
#define PWR_OFF_FST_BCU		9
#define PWR_OFF_SND				10
#define PWR_OFF_END				11


#define MAX_PRE_CUR   20
#define SUMV_20V      200
#define SUMV_50V      500
#define SUMV_100V     1000
#define SUMV_150V     1500
#define SUMV_250V     2500
#define SUMV_10V      100
#define CUR_1A        10
#define CUR_10A       100

#define VCU_CTL_PWR_ON     1
#define VCU_CTL_PWR_OFF    2
#define BCU_CTL_P_OFF     1
#define BCU_CTL_N_OFF     2
#define HVM_FLAG_OK   1
#define HVM_FLAG_FAIL 0
#define HVM_FLAG_ENABLE   1
#define HVM_FLAG_DISABLE  0
#define HVM_HIGH	1
#define HVM_LOW  0
#define HVM_TRIGST_OFF 0
#define HVM_TRIGST_ON  1

#define HVM_CNT_30MS  3//6
#define HVM_CNT_50MS  5//10
#define HVM_CNT_100MS 10//20
#define HVM_CNT_120MS 12//24
#define HVM_CNT_150MS 15//30
#define HVM_CNT_500MS 50//100
#define HVM_CNT_600MS 60//120
#define HVM_CNT_1S    100//200
#define HVM_CNT_2S    200//400
#define HVM_CNT_3S    600//
#define HVM_CNT_5S    1000//

#define HVM_CNT_20S   2000//4000

#define RLY_STA_OPEN     0x00
#define RLY_STA_CLOSE	   0x01
#define RLY_STA_PASTE	   0x02
#define RLY_STA_INVALID	 0x04

#define PRE_CHG_OVERTIME  0x01
#define PRE_CHG_LOAD_OPEN  0x02
#define PRE_CHG_LOAD_SHORT 0x04
#define PRE_CHG_CIRCURT_OPEN 0x08


sHvM_FlagType    HvM_Flag;
sHvM_CntType     HvM_Cnt;
sHvM_CtrlHvType  HvM_CtrlHv;
sHvM_ErrType     HvM_Err;
sHvM_BatInf      HvM_BatInf;
sActor_CtrlRelayCmdType HvM_CtrlRelayCmd;
sHvM_TmCntType HvM_TmCnt;
sHvM_VcuInfoType HvM_VcuInfo;
sHvM_VcuInfoType VcuInfo ;
sHvM_CntType  HvM_Judge;

uWord Hvm_Relay_cnt1;
uWord Hvm_Relay_cnt2;
uWord Err_Alarm_cnt;

uByte Hvm_ErrBat_bak[8];
uByte Hvm_ErrBatSys1_bak[8];
uByte Hvm_ErrBatSys2_bak[8];
uByte Hvm_ErrHw_bak[8];

uByte Bms_St=0;
uByte Bms_Mode=0;

uByte flag_PosPaste=0;
//uByte flag_Paste_err=0;

static void HvM_ModuleInit(void);
static void HvM_VoltChk(sHvM_FlagType *p_flg);

uByte Err_CheckErrChanged_HvM(void);

static void HvM_PowOffChk(sHvM_CtrlHvType *p_ctrl, sHvM_FlagType *p_flg, sHvM_CntType *p_cnt)
{
    uWord wtmp,wtmp2;
    GeneralReturnType return_rslt = ERR_OK;
    HvM_CtrlRelayCmd.CmdType = IMMED_RELAY_CMD;

   
    
}

static void HvM_NFDBKHvCtrl(sHvM_CtrlHvType *p_ctrl, sHvM_FlagType *p_flg, sHvM_CntType *p_cnt, sHvM_ErrType  *p_err)
{
    uWord wtmp,wtmp1,wtmp2;
    uLWord llwtmp;
    uWord lwd_sumv_int;
    uWord lwd_sumv_out;
    uByte lby_iso_err;
    uWord lwd_cur;
    uWord gwd_maxv;
    uByte gby_maxt;
    uWord gby_mint;
    uByte bat_type;
    GeneralReturnType return_rslt = ERR_OK;
    HvM_CtrlRelayCmd.CmdType = IMMED_RELAY_CMD;
    
    gwd_maxv = Bat_GetMaxV();
    gby_maxt = Bat_GetMaxT();
    bat_type = Bat_GetMaterialType();

}

static void HvM_ModuleInit(void)
{
 uByte *p_byte;
    uWord i;

    p_byte = (uByte *)(&HvM_Flag);
    for(i = 0; i < sizeof(HvM_Flag); i++)
    {
        *(p_byte++) = 0;
    }
    p_byte = (uByte *)(&HvM_Cnt);
    for(i = 0; i < sizeof(HvM_Cnt); i++)
    {
        *(p_byte++) = 0;
    }
    p_byte = (uByte *)(&HvM_CtrlHv);
    for(i = 0; i < sizeof(HvM_CtrlHv); i++)
    {
        *(p_byte++) = 0;
    }
    p_byte = (uByte *)(&HvM_Err);
    for(i = 0; i < sizeof(HvM_Err); i++)
    {
        *(p_byte++) = 0;
    }
    p_byte = (uByte *)(&HvM_CtrlRelayCmd);
    for(i = 0; i < sizeof(HvM_CtrlRelayCmd); i++)
    {
        *(p_byte++) = 0;
    }
    
    HvM_CtrlHv.PosNeedChk = HVM_FLAG_OK;
    HvM_CtrlHv.NegNeedChk = HVM_FLAG_OK;
    HvM_CtrlHv.Step = PWR_ON_INIT;

    HvM_TmCnt.cnt = 0;
#ifdef __DEBUG_TEST_HVM

   // HvM_Flag.ReadMinVolt = 1;
   // HvM_Flag.ReadSumVolt = 1;
    HvM_VcuInfo.Cmd = 1;
    Bat_UpdateSumV(INT_SUMV, 4000);
    Bat_UpdateMaxV(3000);
    Bat_UpdateMinV(3000);
    BmsSys_UpdateAwakeSt(0);

#endif


}

void HvM_UpdateVcuInfo(sHvM_VcuInfoType *p_vcu)
{
    HvM_VcuInfo.Cmd = p_vcu->Cmd;
    HvM_VcuInfo.Life = p_vcu->Life;
    HvM_VcuInfo.VcuSumV = p_vcu->VcuSumV;
}

void HvM_ModuleCountCbk(void)
{
    (HvM_TmCnt.cnt < 255) ? (HvM_TmCnt.cnt++) : (HvM_TmCnt.cnt = 255);
    //(HvM_Judge.OpenRelay  < 65535) ? (HvM_Judge.OpenRelay ++) : (HvM_Judge.OpenRelay  = 65535);     
    (HvM_Judge.ReadSumVolt  < 65535) ? (HvM_Judge.ReadSumVolt ++) : (HvM_Judge.ReadSumVolt  = 65535);                                                                
    (HvM_Cnt.StartPre < 65535) ? (HvM_Cnt.StartPre++) : (HvM_Cnt.StartPre = 65535);
    (HvM_Cnt.EndPre < 65535) ? (HvM_Cnt.EndPre++) : (HvM_Cnt.EndPre = 65535);
    (HvM_Cnt.ReadSumVolt < 65535) ? (HvM_Cnt.ReadSumVolt++) : (HvM_Cnt.ReadSumVolt = 65535);
    (HvM_Cnt.ReadMinVolt < 65535) ? (HvM_Cnt.ReadMinVolt++) : (HvM_Cnt.ReadMinVolt = 65535);
    (HvM_Cnt.OpenRelay < 65535) ? (HvM_Cnt.OpenRelay++) : (HvM_Cnt.OpenRelay = 65535);
    (HvM_Cnt.WaitVcuCmd < 65535) ? (HvM_Cnt.WaitVcuCmd++) : (HvM_Cnt.WaitVcuCmd = 65535);
    (HvM_Cnt.Step < 65535) ? (HvM_Cnt.Step++) : (HvM_Cnt.Step = 65535);
    (HvM_Cnt.fault < 65535) ? (HvM_Cnt.fault++) : (HvM_Cnt.fault = 65535);
    (HvM_Cnt.TriggerOff < 65535) ? (HvM_Cnt.TriggerOff++) : (HvM_Cnt.TriggerOff = 65535);
    (HvM_Cnt.NegRelayFilter < 65535) ? (HvM_Cnt.NegRelayFilter++) : (HvM_Cnt.NegRelayFilter = 65535);
    (HvM_Cnt.ChgRlyOnDelay < 65535) ? (HvM_Cnt.ChgRlyOnDelay++) : (HvM_Cnt.ChgRlyOnDelay = 65535);
    (HvM_Cnt.ChgRlyOffDelay < 65535) ? (HvM_Cnt.ChgRlyOffDelay++) : (HvM_Cnt.ChgRlyOffDelay = 65535);
}

void HvM_ModuleCbk(void)
{
#ifdef __DEBUG_TEST_HVM
    HvM_TmCnt.cnt  = 3;
#endif
    if(HvM_TmCnt.cnt >= TM_MS(20))
    {
        HvM_TmCnt.cnt = 0;
        HvM_NFDBKHvCtrl(&HvM_CtrlHv, &HvM_Flag, &HvM_Cnt, &HvM_Err);        
    }
}

uByte Get_HvM_Ctrl_Step(void) 
{
    return HvM_CtrlHv.Step;
}
uByte Err_CheckErrChanged_HvM(void)
{
    uByte i, flag = 0;
    sErr_ByteNType err_byte;
    
    err_byte = Err_GetBatAllErrLevel();  // bat error
    for(i = 0; i < 8; i++)
    {
        if(Hvm_ErrBat_bak[i] != err_byte.By[i])
        {          
            Hvm_ErrBat_bak[i] = err_byte.By[i];
            if(0 !=err_byte.By[i])
               flag = 1;
        }
    }
    err_byte = Err_GetBatSysAllErrLevel(0);  //  BatSys error
    for(i = 0; i < 8; i++)
    {
        if(Hvm_ErrBatSys1_bak[i] != err_byte.By[i])
        {          
            Hvm_ErrBatSys1_bak[i] = err_byte.By[i];
            if(0 !=err_byte.By[i])
               flag = 1;
        }
    }
    err_byte = Err_GetBatSysAllErrLevel(1);  // other err
    for(i = 0; i < 8; i++)
    {
        if(Hvm_ErrBatSys2_bak[i] != err_byte.By[i])
        {          
            Hvm_ErrBatSys2_bak[i] = err_byte.By[i];
            if(0 !=err_byte.By[i])
               flag = 1;
        }
    }
    err_byte = Err_GetHwAllErrLevel();  // hardware err
    for(i = 0; i < 8; i++)
    {
        if(Hvm_ErrHw_bak[i] != err_byte.By[i])
        {          
            Hvm_ErrHw_bak[i] = err_byte.By[i];
            if(0 !=err_byte.By[i])
               flag = 1;
        }
    }
    return flag;
}

#endif // #ifdef USE_HVM_B

