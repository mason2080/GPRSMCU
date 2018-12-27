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



typedef struct
{
    uByte StartPreChg;
    uByte HvPowerOnEnd;
    uByte FirstBoot;
    uByte ReadMinVolt;
    uByte ReadSumVolt;
    uByte ForbidPowerOn;
    uByte PreFirstEnd;
    uByte NegRelaySt;
    uByte PosRelaySt;
    uByte PreRelaySt;
    uByte ChgRelaySt;
    uByte CloseNegRly;
    uByte OpenNegRly;

} sHvM_FlagType;

typedef struct
{
    uWord   StartPre ;
    uWord   EndPre;
    uWord   ReadSumVolt;
    uWord   ReadMinVolt;
    uWord   OpenRelay;
    uWord   WaitVcuCmd;
    uWord   Step;
    uWord   fault;
    uWord   TriggerOff;
    uWord   NegRelayFilter;
    uWord   ChgRlyOffDelay;
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


#define CHECK_PRE

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

#define HVM_CNT_30MS  6
#define HVM_CNT_50MS  10
#define HVM_CNT_100MS 20
#define HVM_CNT_120MS 24
#define HVM_CNT_150MS 30
#define HVM_CNT_500MS 100
#define HVM_CNT_600MS 120
#define HVM_CNT_1S    200
#define HVM_CNT_2S    400
#define HVM_CNT_3S    600
#define HVM_CNT_5S    1000
#define HVM_CNT_10S   2000
#define HVM_CNT_20S   4000

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

uByte gby_chg_relay_off_flag;

uByte Bms_St=0;
uByte Bms_Mode=0;



static void HvM_ModuleInit(void);
static void HvM_VoltChk(sHvM_FlagType *p_flg);

static void HvM_PowOffChk(sHvM_CtrlHvType *p_ctrl, sHvM_FlagType *p_flg, sHvM_CntType *p_cnt)
{
    /*if(HVM_TRIGST_OFF != BmsSys_GetAwakeSt())
    {
        p_cnt->TriggerOff = 0;
    }*/
    
    if((1==Sensor_GetDiSt((uByte)CPU_DI1_ID)) && (1 == Chg_Mode))
    {
        p_cnt->TriggerOff = 0;
    }
    
    if( (Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= 2)
     || (Err_GetBatErrLevel(ERR_BAT_T_HIGH)>= 2)
     || (Err_GetHwErrLevel(ERR_HW_CAN4) >= 1)
     || (Err_GetBatErrLevel(ERR_BAT_SOC_LOW) >= 2)
     || (Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH) >= 2)
     || (Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= 2)
     || (Err_GetBatErrLevel(ERR_BAT_SUMV_LOW)>= 2))
    {
        //p_cnt->fault++;
    }
    else
    {
        p_cnt->fault = 0;
    }
    
    
    if( (p_cnt->TriggerOff >= HVM_CNT_50MS) || (p_cnt->fault >= HVM_CNT_1S))
    {        
        p_cnt->TriggerOff = 0;
        p_cnt->fault = 0;
        p_flg->ForbidPowerOn = HVM_FLAG_OK;
        
        p_ctrl->Step = PWR_OFF_SND;

        if(WAIT_PWR_ON == p_ctrl->Step)
        {
            p_ctrl->Step = PWR_OFF_SND;
        }
        
        if(PWR_ON == p_ctrl->Step)
        {
            p_ctrl->Step = WAIT_PWR_OFF;
            p_ctrl->BcuCmd = BCU_CTL_P_OFF;  // bcu request power off/
        }
        
        if(WAIT_PWR_OFF == p_ctrl->Step)
        {
            p_ctrl->Step = PWR_OFF_SND;
            p_ctrl->BcuCmd = BCU_CTL_P_OFF;  //bcu request power off 
        }        
    }
}

static void HvM_NFDBKHvCtrl(sHvM_CtrlHvType *p_ctrl, sHvM_FlagType *p_flg, sHvM_CntType *p_cnt, sHvM_ErrType  *p_err)
{
    uWord wtmp;
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
    
   // HvM_VcuInfo.Cmd = 0x01; //for test

    if(HVM_FLAG_FAIL == p_flg->FirstBoot)   /* initialization using for the first time */
    {
        HvM_ModuleInit();
        p_flg->FirstBoot = HVM_FLAG_OK;
    }

    HvM_PowOffChk(&HvM_CtrlHv, &HvM_Flag, &HvM_Cnt);

    switch(p_ctrl->Step)
    {
        case PWR_ON_INIT:
        {
            HvM_VoltChk(&HvM_Flag);
/*            
            if(HVM_FLAG_FAIL == p_flg->CloseNegRly)
            {
                p_flg->CloseNegRly = HVM_FLAG_OK;
                HvM_CtrlRelayCmd.Bcmd = RELAY_CLOSE_CMD;
                return_rslt = Actor_CtrlRelay(CPU_FAN_ID, HvM_CtrlRelayCmd); //闭合电池箱内的继电器 用FAN来控制
                BatSys_UpdateFanSt(1);
                p_ctrl->Step = PWR_ON;
            }*/
            p_ctrl->Step = PWR_ON;
            break;
        }

        case PWR_ON:
        {
            HvM_CtrlRelayCmd.Bcmd = RELAY_CLOSE_CMD;
            return_rslt = Actor_CtrlRelay(POS_RELAY_ID, HvM_CtrlRelayCmd);
            p_ctrl->Step = WAIT_PWR_OFF;
/*
            HvM_CtrlRelayCmd.Bcmd = RELAY_OPEN_CMD;     //关断充电RL
            return_rslt = Actor_CtrlRelay(CHG_RELAY_ID, HvM_CtrlRelayCmd);
            BatSys_UpdateChgRlySt(0);

            if((HvM_VcuInfo.Cmd & 0x03) == 0x01)   //VCU CMD
            {
                if(HVM_FLAG_FAIL == p_flg->StartPreChg)
                {
                    p_flg->StartPreChg = HVM_FLAG_OK;
                    p_cnt->StartPre = 0;
                    p_cnt->EndPre = 0;
                    
                    HvM_CtrlRelayCmd.Bcmd = RELAY_CLOSE_CMD;     //
                    return_rslt = Actor_CtrlRelay(PRE_RELAY_ID,HvM_CtrlRelayCmd); //闭合预充   
                    BatSys_UpdatePreRlySt(1);
                    Bms_St = 2;  //预充电过程
                }
                // precharge voltage is above 90% sumv//
                lwd_sumv_int = Bat_GetSumV(INT_SUMV);
                lwd_sumv_out = Bat_GetSumV(OUT_SUMV);
                llwtmp = lwd_sumv_int;
                llwtmp *= 90;
                llwtmp /= 100;
                wtmp = (uWord)(llwtmp);

                if((lwd_sumv_out >= wtmp) && (lwd_sumv_int >= 1000) && (p_cnt->StartPre >= HVM_CNT_3S)) // the sumv is detectde by vcu //
                {
                    if(HVM_FLAG_FAIL == p_flg->PreFirstEnd)  //闭合总正
                    {
                        p_flg->PreFirstEnd = HVM_FLAG_OK;	 // forbid close pos again//
                        HvM_CtrlRelayCmd.Bcmd = RELAY_CLOSE_CMD;
                        return_rslt = Actor_CtrlRelay(POS_RELAY_ID, HvM_CtrlRelayCmd);
                        return_rslt = Actor_CtrlRelay(NEG_RELAY_ID, HvM_CtrlRelayCmd);   //控制DC/DC
                        p_cnt->EndPre = 0;
                        Bms_St = 3;  //高压闭合
                        Bms_Mode = 0x30;
                    }
                    if(p_cnt->EndPre >= HVM_CNT_50MS)  //50ms后断开预充
                    {
                        BatSys_UpdatePosRlySt(1);
                        p_cnt->EndPre = 0;
                        p_ctrl->Step = WAIT_PWR_OFF;
                        HvM_CtrlRelayCmd.Bcmd = RELAY_OPEN_CMD;
                        return_rslt = Actor_CtrlRelay(PRE_RELAY_ID, HvM_CtrlRelayCmd);
                        p_cnt->StartPre = 0;

                    }

                }
                else
                {
                    // power on failed //
                    if(p_cnt->StartPre > HVM_CNT_10S )  //预充失败
                    {
                        p_cnt->StartPre = 0 ;

                        // maybe need add other reason when power fail
                        (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_PRE_CHG, ERR_LEVEL_THREE);
                        (void)Err_UpdateHwErrDetail(ERR_HW_PRE_CHG, PRE_CHG_OVERTIME);

                        // precharge error //
                        p_ctrl->Step = PWR_OFF_SND;	  //open neg relay//
                        p_flg->ForbidPowerOn = HVM_FLAG_OK; // forbid power on again//
                    }
                }
            }
            else if((HvM_VcuInfo.Cmd & 0x03) == 0x00)
            {
                p_ctrl->Step = PWR_OFF_SND;
            }
*/
            break;
        }

        case WAIT_PWR_OFF:
        {
            if(0==Sensor_GetDiSt((uByte)CPU_DI1_ID) 
            {
                p_ctrl->Step = PWR_OFF_FST_VCU;
            }
/*            
            BatSys_UpdatePreRlySt(0);
            
            if((HvM_VcuInfo.Cmd & 0x03) == 0x00)
            {
                p_ctrl->Step = PWR_OFF_FST_VCU;//PWR_OFF_SND;
                p_cnt->OpenRelay = 0;
            }
*/            
            break;
        }

        case PWR_OFF_FST_VCU:
        {
            /* current is safety */
            lwd_cur = Bat_GetCurrent(INSCUR);
            if( (lwd_cur < (OFFSET_CUR + CUR_10A)) && (lwd_cur > (OFFSET_CUR - CUR_10A)))
            {
                p_ctrl->Step = PWR_OFF_SND;
            }
            else
            {
                if(p_cnt->OpenRelay > HVM_CNT_1S)
                {
                    p_cnt->OpenRelay = 0;
                    p_ctrl->Step = PWR_OFF_SND;
                    DataStore_EnableEventDataStore(RLY_ERR_OPEN);
                    (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_POS_OPEN_CUR, ERR_LEVEL_ONE);
                }
            }
            /* update relay life time */
            (void)BatSys_RlyLifeCalc(POS_RELAY_ID, Bat_GetCurrent(INSCUR));

            break;
        }
        case PWR_OFF_SND:
        {

            HvM_CtrlRelayCmd.Bcmd = RELAY_OPEN_CMD;
            
            return_rslt = Actor_CtrlRelay(PRE_RELAY_ID, HvM_CtrlRelayCmd);
            return_rslt = Actor_CtrlRelay(CPU_FAN_ID, HvM_CtrlRelayCmd);  //
            return_rslt = Actor_CtrlRelay(POS_RELAY_ID, HvM_CtrlRelayCmd);    // 断开主正
            return_rslt = Actor_CtrlRelay(NEG_RELAY_ID, HvM_CtrlRelayCmd);   //断开主负

            p_flg->OpenNegRly = HVM_FLAG_OK;
            p_ctrl->Step = PWR_OFF_END;
            
            Bms_St = 1;  // 高压断开
            Bms_Mode = 0x20;
            break;
        }
        case PWR_OFF_END:
        {
            if((HVM_FLAG_OK == p_flg->OpenNegRly))    //&& (p_cnt->RlyDone > HVM_CNT_50MS))
            {
                p_flg->OpenNegRly = HVM_FLAG_FAIL;
                BatSys_UpdatePosRlySt(0);
                BatSys_UpdateNegRlySt(0);
                BatSys_UpdatePreRlySt(0);
                BatSys_UpdateChgRlySt(0);
            }
            if(HVM_FLAG_FAIL == p_flg->ForbidPowerOn)
            {
                if((HvM_VcuInfo.Cmd & 0x03) == 0x01)   //VCU CMD
                {
                    p_flg->CloseNegRly = HVM_FLAG_FAIL;
                    p_flg->StartPreChg = HVM_FLAG_FAIL;     //HvM_Flag
                    p_flg->PreFirstEnd = HVM_FLAG_FAIL;
                    p_ctrl->Step = PWR_ON_INIT;
                }
            }
            break;
        }
        default : break;        
    }
    
    (void)return_rslt;
}

static void HvM_VoltChk(sHvM_FlagType *p_flg)
{
    uWord lwd_sumv_int;
    uWord lwd_sumv_acc;
    uWord lwd_maxv;
    uWord lwd_minv;

    /* is the sumv valid*/

    if(HVM_FLAG_FAIL == p_flg->ReadSumVolt)
    {
        lwd_sumv_acc = Bat_GetSumV(ACC_SUMV);
        lwd_sumv_int = Bat_GetSumV(INT_SUMV);
        if((lwd_sumv_acc > SUMV_100V) || (lwd_sumv_int > SUMV_100V))
        {
            p_flg->ReadSumVolt = HVM_FLAG_OK;
        }
        else
        {
            p_flg->ReadSumVolt = HVM_FLAG_FAIL;

        }
    }

    /* is the minv valid*/
    if(HVM_FLAG_FAIL == p_flg->ReadMinVolt)
    {
        lwd_maxv = Bat_GetMaxV();
        lwd_minv = Bat_GetMinV();

        if((lwd_maxv >0) && (lwd_minv > 0) && (lwd_minv != 0xFFFF))
        {
            p_flg->ReadMinVolt = HVM_FLAG_OK;
        }
        else
        {
            p_flg->ReadMinVolt = HVM_FLAG_FAIL;

        }
    }
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
    
    (HvM_Cnt.ChgRlyOffDelay < 65535) ? (HvM_Cnt.ChgRlyOffDelay++) : (HvM_Cnt.ChgRlyOffDelay = 65535);
    
}

void HvM_ModuleCbk(void)
{
#ifdef __DEBUG_TEST_HVM
    HvM_TmCnt.cnt  = 3;
#endif

    if(HvM_TmCnt.cnt >= TM_MS(10))
    {
        HvM_TmCnt.cnt = 0;
        HvM_NFDBKHvCtrl(&HvM_CtrlHv, &HvM_Flag, &HvM_Cnt, &HvM_Err);
    }
}



