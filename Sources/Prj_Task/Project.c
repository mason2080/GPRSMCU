#include "CanIf_Type.h"
#include "CanIf.h"
#include "Ecu.h"
#include "BatterySt.h"
#include "BatterySysSt.h"
#include "ChargeSt.h"
#include "BmsSysSt.h"
#include "IntCan.h"
#include "SysConf.h"
#include "ErrorAlarm.h"
#include "SchIf.h"
#include "EcuDebug.h"
#include "AppDo.h"
#include "DoIf.h"
#include "SensorDi.h"
#include "BalM.h"
#include "SensorAi.h"
#include "DataStore.h"
#include "LvM.h"
#include "Project.h"

typedef struct
{
    uByte ProCnt;
} sPrj_ProType;

sPrj_ProType Prj_Pro;

uByte gby_ReadyFlag; //
uWord gwd_RecvBmuMsgFlag;
uWord gwd_RecvBmuMsgFlagMask;

uByte flag_all_err_send = 0;
uWord Bat_Err_Detail=0x0000;
uByte Fault_Sts = 0;
uByte Fault_Lock_Sts = 0x00; 

void Prj_CarCanErr(void);

// intitial
void Prj_PreMainInit(void)
{
    gby_ReadyFlag = DEFAULT_READY;
}

// check have hardware error
void Prj_PosMainInit(void)
{
    sErr_ByteNType err_byte;
    uByte i;
    uByte btmp;
    uWord wtmp;

    err_byte = Err_GetHwAllErrLevel();
    for(i = 0; i < 8; i++)
    {
        if(err_byte.By[i])
        {
            break;
        }
    }

    // no hardware error
    if(8 == i)
    {
        gby_ReadyFlag |= HARDWARE_READY;
    }


    btmp = Bat_GetBmuNum();
    for(i = 0; i < btmp; i++)
    {
        wtmp = 1;
        wtmp <<= (btmp - 1);
        gwd_RecvBmuMsgFlagMask |= wtmp;
    }
}

// check had receive all bmu first frame
void Prj_UpdateRecvFirFrame(uByte bmu_no)
{
    uWord wtmp = 1;

    wtmp <<= (bmu_no - 1);
    gwd_RecvBmuMsgFlag |= wtmp;

}

// check all bmu frame
void Prj_CheckAllBmuFrame(void)
{
    if((gwd_RecvBmuMsgFlag & gwd_RecvBmuMsgFlagMask) == gwd_RecvBmuMsgFlagMask)
    {
        gby_ReadyFlag |= ALL_BMU_READY;
    }
}

// check Current and sumv init
void Prj_CheckCur(void)
{
    if(Bat_GetCurrent(INSCUR) != 0)
    {
        gby_ReadyFlag |= CUR_READY;
        gby_ReadyFlag |= SUMV_READY;
    }
}

// check all max and min temper had receive
void Prj_CheckMaxMinT(void)
{
    if((Bat_GetMaxT() > 0) && (Bat_GetMinT() > 0))
    {
        gby_ReadyFlag |= MAXMIN_TEMPER_READY;
    }
}

// check all max and min cellv had receive
void Prj_CheckMaxMinV(void)
{
    if((Bat_GetMaxV() > 0) && (Bat_GetMinV() > 0))
    {
        gby_ReadyFlag |= MAXMIN_CELLV_READY;
    }
}

// check all cellv had receive
void Prj_CheckAllCellV(void)
{
    uByte btmp;
    uByte i;
    uByte j;
    uByte bmu_no;

    bmu_no = Bat_GetBmuNum();

    if(bmu_no > 1)
    {
        // check all bmu fist cellv and last cellv

        for(i = 0; i < bmu_no; i++)
        {
            btmp = (uByte)Bat_GetPackSerialNum(i+1);
            if(btmp > 0)
            {
                if((Bat_GetCellV(i+1,1) == 0) && (Bat_GetCellV(i+1, btmp -1) == 0))
                {
                    break;
                }
            }
        }


        if(i == bmu_no)
        {
            gby_ReadyFlag |= ALL_CELLV_READY;
        }
    }
    else
    {
        btmp = (uByte)Bat_GetPackSerialNum(1);
        {
            // first cellv and last cellv have receive
            if((Bat_GetCellV(1,1) > 0) && (Bat_GetCellV(1, btmp - 1) > 0))
            {
                gby_ReadyFlag |= ALL_CELLV_READY;
            }
        }
    }

}

// check all temper had receive
void Prj_CheckAllTemper(void)
{
    uByte btmp;
    uByte i;
    uByte j;
    uByte bmu_no;

    bmu_no = Bat_GetBmuNum();

    if(bmu_no > 1)
    {
        // check all bmu fist Temper and last Temper

        for(i = 0; i < bmu_no; i++)
        {
            btmp = Bat_GetPackTemperNum(i+1);
            if(btmp > 0)
            {

                if((Bat_GetTemper(i+1,1) == 0) && (Bat_GetTemper(i+1, btmp -1) == 0))
                {
                    break;
                }
            }
        }


        if(i == bmu_no)
        {
            gby_ReadyFlag |= ALL_TEMPER_READY;
        }
    }
    else
    {
        btmp = Bat_GetPackTemperNum(1);
        {
            // first temper and last temper have receive
            if((Bat_GetTemper(1,1) > 0) && (Bat_GetTemper(1, btmp - 1) > 0))
            {
                gby_ReadyFlag |= ALL_TEMPER_READY;
            }
        }
    }
}

// engener add code here
void Prj_MainPro(void)
{

}


void Prj_MainCounterCbk(void)
{
    (Prj_Pro.ProCnt < 255) ? (Prj_Pro.ProCnt++) : (Prj_Pro.ProCnt = 255);
}

void Prj_MainProCbk(void)
{
   
    Prj_CarCanErr();
   
    /*
    // for engener use for project
    if(Prj_Pro.ProCnt >= TM_MS(100))
    {
        Prj_MainPro();
        Prj_Pro.ProCnt  = 0;
    }

    // these for init flag
    if((gby_ReadyFlag & CUR_READY) != CUR_READY)
    {
        Prj_CheckCur();
    }

    // All Bmu
    if((gby_ReadyFlag & ALL_BMU_READY) != ALL_BMU_READY)
    {
        Prj_CheckAllBmuFrame();
    }

    // MaxMinT
    if((gby_ReadyFlag & MAXMIN_TEMPER_READY) != MAXMIN_TEMPER_READY)
    {
        Prj_CheckMaxMinT();
    }

    // MaxMinV
    if((gby_ReadyFlag & MAXMIN_CELLV_READY) != MAXMIN_CELLV_READY)
    {
        Prj_CheckMaxMinV();
    }

    // ALL CellV
    if((gby_ReadyFlag & ALL_CELLV_READY) != ALL_CELLV_READY)
    {
        Prj_CheckAllCellV();
    }

    //ALL Temper
    if((gby_ReadyFlag & ALL_TEMPER_READY) != ALL_TEMPER_READY)
    {
        Prj_CheckAllTemper();
    }
    */
}

uByte Prj_GetReadyFlag(void)
{
    return gby_ReadyFlag;
}

void Prj_CarCanErr(void)
{
    uByte btmp;
    if(flag_all_err_send == 0)    //
    {
        flag_all_err_send = 1;
        
        // ISO ERR
        if(3 == Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR))   // < 100¦¸/v
        {
            Bat_Err_Detail |= 0x0001;
        } 
        else if(2 == Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR)) //100¦¸/v < && < 500¦¸/v
        {
            Bat_Err_Detail |= 0x0002;
        }
        
        // T  HIHG
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_T_HIGH)) 
        {
            Bat_Err_Detail |= 0x0004;
        } 
        else if(1 == Err_GetBatSysErrLevel(ERR_BAT_T_HIGH)) 
        {
            Bat_Err_Detail |= 0x0008;
        }
        
        // DETA T
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_DELTT_ALLPACK)) 
        {
            Bat_Err_Detail |= 0x0010;
        } 
        
        // CELLV  HIHG
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_CELLV_HIGH)) 
        {
            Bat_Err_Detail |= 0x0020;
        } 
        else if(1 == Err_GetBatSysErrLevel(ERR_BAT_CELLV_HIGH)) 
        {
            Bat_Err_Detail |= 0x0040;
        }
        
         // CELLV  LOW
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_CELLV_HIGH)) 
        {
            Bat_Err_Detail |= 0x0080;
        } 
        else if(1 == Err_GetBatSysErrLevel(ERR_BAT_CELLV_HIGH)) 
        {
            Bat_Err_Detail |= 0x0100;
        }
        
          // DISCHG  CUR
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_CUR_HIGH_DCH)) 
        {
            Bat_Err_Detail |= 0x0200;
        } 
        
          // CHG CUR  
        if(2 == Err_GetBatSysErrLevel(ERR_BAT_CUR_HIGH_CHG)) 
        {
            Bat_Err_Detail |= 0x0400;
        } 
    }
    
    
     if(Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= 3)
     {
        Fault_Sts = 0x02;
        Fault_Lock_Sts = 0x40;
     }
     else if( (Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_T_HIGH)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_T_LOW)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_SOC_HIGH)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_SOC_LOW)>= 1)
            ||(Err_GetHwErrLevel(ERR_HW_CAN4) >= 1)
            ||(Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= 2)
            ||(Err_GetBatErrLevel(ERR_BAT_SUMV_LOW)>= 1)
            ||(Err_GetBatErrLevel(ERR_BAT_SUMV_HIGH)>= 1) )
     {
         Fault_Sts = 0x01;
         Fault_Lock_Sts = 0x00; 
     }

}
