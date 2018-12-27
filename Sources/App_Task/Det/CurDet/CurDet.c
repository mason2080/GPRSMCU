// CurDet.c
#include "CurDet.h"
#include "CurrentIf.h"
#include "EE.h"
#include "ErrorAlarm.h"
#include "SchIf.h"
#include "BatterySt.h"
#include "ErrorAlarm.h"

#if(ECU_RAM_USE_MODE)
#ifdef ECU_RAM_USE_GPGAE
#pragma DATA_SEG __GPAGE_SEG PAGED_RAM
#endif
#ifdef ECU_RAM_USE_RPGAE
#pragma DATA_SEG __RPAGE_SEG PAGED_RAM
#endif
#else
#pragma DATA_SEG DEFAULT
#endif

typedef struct
{
    uLWord Chg;
    uLWord Dch;
} sCurrentDet_AhCntType;

typedef struct
{
    uWord Gain;
} sCurrentDet_GainOffType;

typedef struct
{
    sCurrent_ChipConfigType ChipConf;
    sCurrentDet_AhCntType   AhCntThrhd;
    sCurrentDet_GainOffType GainOff;
} sCurrentDet_Config;

sCurrentDet_Config CurDetConf;
#pragma DATA_SEG DEFAULT



typedef struct
{
    uByte Flg;
    uWord Energy;
    sCurrentDet_AhCntType AhCnt;
    sCurrentDet_AhCntType Ah;
} sCurrentDet_EnergyType;

sCurrentDet_EnergyType  CurDet_Eng;

#define CUR_AMPLIFY  100000

uByte  gby_curdeterrcnt;

#define CURDET_HDERR_CNT 20


typedef struct  
{
    uByte InsCurTmCnt;
    uByte EngTmCnt;
} sCurDet_TmCntType;

sCurDet_TmCntType CurDet_TmCnt;

uWord cur_ad;
uLWord cur_real_value;
uByte clb_flg = 0;
uByte gain_offsef[4];

/////////////temp define
#define CURDET_NUM 2


 void CurDet_InitCurDet(void);
//static void CurDet_ProcCurDet(void);
static void CurrentDet_CalcAh(void);
static void CurrentDet_CalcAh_By_SoftWare(uWord Current,uByte FlgCurDirection,uWord TimeMs);

void CurDet_Init(void)
{
    CurDet_TmCnt.InsCurTmCnt = 0;
    CurDet_TmCnt.EngTmCnt = 0;
    CurDet_InitCurDet();
}

void CurDet_Count(void)
{
    (CurDet_TmCnt.InsCurTmCnt < 255) ? (CurDet_TmCnt.InsCurTmCnt++) : (CurDet_TmCnt.InsCurTmCnt = 255);
    (CurDet_TmCnt.EngTmCnt < 255) ? (CurDet_TmCnt.EngTmCnt++) : (CurDet_TmCnt.EngTmCnt = 255);
}

#ifdef __DEBUG_CUR_CHECK_AD
uWord gwd_cur_ad = 0;
uWord gwd_cur = 0;
uWord gwd_cur_avg_ad = 0;
uWord gwd_cur_avg = 0;
#endif
#ifdef __DEBUG_CUR_CLBR
uWord gwd_CurDetRslt = 0;
uByte gby_FlagCLbrSlect = 0; /* 1 --- CurChnOffset
                              * 2 --- CurChnGain
                              * 3 --- VolChnOffset
                              * 4 --- VolChnGain
                              */
uLWord glwd_ClbrValue = 0;
#endif

uWord test_flg1=0;
void CurDet_Main(void)
{
    uWord instant_cur_ad,instant_cur,cur_gain;
    uLWord instant_cur_tmp;
    uWord eng,eng_ad,avg_cur;
    uByte flag;
    uByte CntBak;
    GeneralReturnType rslt;

    Err_HwErrDetailType hderr_detail_orin;
    Err_HwErrDetailType hderr_detail;

    hderr_detail_orin = Err_GetHwErrDetail(ERR_HW_DETECT_CUR);
    hderr_detail = 0;

    flag = 0;
    rslt = 0;

#ifdef LEM_CUR
    if(CurDet_TmCnt.InsCurTmCnt >= TM_MS(10))
    {
        /* read instant current */ 
         CntBak = CurDet_TmCnt.InsCurTmCnt;
         CurDet_TmCnt.InsCurTmCnt = 0;     

         rslt = CurrentIf_RdCur(0,0,&instant_cur_ad, &flag);
         cur_gain = 1846;        

        if(instant_cur_ad > 13000) 
        {
            rslt = CurrentIf_RdCur(0,1,&instant_cur_ad, &flag);
            cur_gain = 22440;
            if(instant_cur_ad > 13000)
               instant_cur_ad = 0;                
        } 
        
        if(instant_cur_ad <= 40)
           instant_cur_ad = 0;

        if(!rslt)
        {             
            instant_cur_tmp = instant_cur_ad;
            instant_cur_tmp *= cur_gain;//22440;//CurDetConf.GainOff.Gain;
            //instant_cur_tmp *= 1846;//CurDetConf.GainOff.Gain;
            instant_cur_tmp /= CUR_AMPLIFY; //need modify
            
          //  instant_cur_tmp *= 976;      //need modify
          //  instant_cur_tmp /= 1000;     //need modify
            instant_cur = (uWord)instant_cur_tmp; 
            
            CurrentDet_CalcAh_By_SoftWare(instant_cur,flag,(CntBak * MIN_TIMER_MS));
           //// Bat_UpdateAccumulateDchgAh(CurDet_Eng.Ah.Dch);
           // Bat_UpdateAccumulateChgAh(CurDet_Eng.Ah.Chg);                    
            
#ifdef __DEBUG_CUR_CHECK_AD
            gwd_cur_ad = instant_cur_ad;
            gwd_cur = instant_cur;
#endif

            if(CURRENTIF_FLAG_DCH == flag)
            {
                instant_cur = instant_cur + OFFSET_CUR;
            }
            else if(CURRENTIF_FLAG_CHG == flag)
            {
                if(OFFSET_CUR > instant_cur)
                {
                    instant_cur = OFFSET_CUR - instant_cur;
                }
                else
                {
                    // set to error value "0xFFFF"
                    instant_cur = 0xFFFF;
                }
            }
            else
            {
                // set to error value "0xFFFF"
                instant_cur = 0xFFFF;
            }
            Bat_UpdateCurrent(INSCUR, instant_cur);
/*#ifndef __CUR_LEM__ 
            if(CurCnt >= 100) 
            {
               CurCnt = 0;
               CurSum = 0;
               instant_cur = (uWord)(CurSum / 100);
               Bat_UpdateCurrent(AVGCUR, instant_cur); 
            } 
            else 
            {
               CurSum += instant_cur;
               CurCnt++;
            }
            

#endif*/ /*__CUR_LEM__ */

        }
        else
        {
            hderr_detail = CURDET_HDERR_RD_INSCUR_FAILED;
        }
        hderr_detail_orin = Err_GetHwErrDetail(ERR_HW_DETECT_CUR);
        hderr_detail |= hderr_detail_orin;

        rslt = Err_UpdateHwErrDetail(ERR_HW_DETECT_CUR, hderr_detail);

    }

#else

    if(CurDet_TmCnt.InsCurTmCnt >= TM_MS(10))
    {
        /* read instant current */
        CurDet_TmCnt.InsCurTmCnt = 0;
        rslt = CurrentIf_RdCur(CURRENTIF_CS5460A_1, RDCURRINS,&instant_cur_ad, &flag);
        if(!rslt)
        {
            instant_cur_tmp = instant_cur_ad;
            instant_cur_tmp *= CurDetConf.GainOff.Gain;
            instant_cur_tmp /= CUR_AMPLIFY;
            
            instant_cur_tmp *= 976;
            instant_cur_tmp /= 1000;
            instant_cur = (uWord)instant_cur_tmp;
#ifdef __DEBUG_CUR_CHECK_AD
            gwd_cur_ad = instant_cur_ad;
            gwd_cur = instant_cur;
#endif

            if(instant_cur < 4)  //less than 0.4A then pass this value
            {
               instant_cur = 0;
            }
            if(CURRENTIF_FLAG_DCH == flag)
            {
                instant_cur = instant_cur + OFFSET_CUR;
            }
            else if(CURRENTIF_FLAG_CHG == flag)
            {
                if(OFFSET_CUR > instant_cur)
                {
                    instant_cur = OFFSET_CUR - instant_cur;
                }
                else
                {
                    // set to error value "0xFFFF"
                    instant_cur = 0xFFFF;
                }
            }
            else
            {
                // set to error value "0xFFFF"
                instant_cur = 0xFFFF;
            }
            Bat_UpdateCurrent(INSCUR, instant_cur);
        }
        else
        {
            hderr_detail = CURDET_HDERR_RD_INSCUR_FAILED;
        }
        hderr_detail_orin = Err_GetHwErrDetail(ERR_HW_DETECT_CUR);
        hderr_detail |= hderr_detail_orin;

        rslt = Err_UpdateHwErrDetail(ERR_HW_DETECT_CUR, hderr_detail);
    }

    if(CurDet_TmCnt.EngTmCnt >= TM_MS(1000))
    {
        /* read energy and average current */
        CurDet_TmCnt.EngTmCnt = 0;
        //test_flg1 =  CurrentIf_RdCur(CURRENTIF_CS5460A_1, RDENERGY, &eng_ad, &flag);
        rslt = CurrentIf_RdCur(CURRENTIF_CS5460A_1, RDENERGY, &eng_ad, &flag);
        if(!rslt)
        {
            CurDet_TmCnt.EngTmCnt = 0;
            eng = eng_ad;
            CurDet_Eng.Flg = flag;
            CurDet_Eng.Energy = eng;
            /* calculate average current */
            instant_cur_tmp = eng;
            instant_cur_tmp *= CurDetConf.GainOff.Gain;
            instant_cur_tmp /= CUR_AMPLIFY;
            avg_cur = (uWord)instant_cur_tmp;
#ifdef __DEBUG_CUR_CHECK_AD
            gwd_cur_avg_ad = eng_ad;
            gwd_cur_avg = avg_cur;
#endif
            if(avg_cur < 4)     //less than 0.4A then pass this cur value and do not cal AH 
            {
               avg_cur = 0;
               CurDet_Eng.Energy = 0;
            }
            if(CURRENTIF_FLAG_DCH == flag)
            {
                avg_cur = avg_cur + OFFSET_CUR;
            }
            else if(CURRENTIF_FLAG_CHG == flag)
            {
                if(OFFSET_CUR > avg_cur)
                {
                    avg_cur = OFFSET_CUR - avg_cur;
                }
                else
                {
                    // set to error value "0xFFFF"
                    avg_cur = 0xFFFF;
                }
            }
            else
            {
                // set to error value "0xFFFF"
                avg_cur = 0xFFFF;
            }
            Bat_UpdateCurrent(AVGCUR, avg_cur);

            /* calcalate Ah */
            CurrentDet_CalcAh();

        }
        else
        {
            //hderr_detail = CURDET_HDERR_RD_AVGCUR_FAILED;
        }
        hderr_detail_orin = Err_GetHwErrDetail(ERR_HW_DETECT_CUR);
        hderr_detail |= hderr_detail_orin;

        rslt = Err_UpdateHwErrDetail(ERR_HW_DETECT_CUR, hderr_detail);
    }
    
    if(hderr_detail) // have error
    {
        // hardware error
        gby_curdeterrcnt++;
        if(gby_curdeterrcnt >= CURDET_HDERR_CNT)
        {
            rslt = Err_UpdateHwErrLevel(ERR_HW_DETECT_CUR, ERR_LEVEL_TWO);
        }
    } 
    else
    {
        gby_curdeterrcnt = 0;
        rslt = Err_UpdateHwErrLevel(ERR_HW_DETECT_CUR, ERR_LEVEL_NORMAL);
    }

#endif

#ifdef __DEBUG_CUR_CLBR

    if(1 == gby_FlagCLbrSlect)
    {
        gwd_CurDetRslt = CurDet_ClbCurCurChnOffset(&glwd_ClbrValue);
    }
    asm NOP;
    if(2 == gby_FlagCLbrSlect)
    {
        gwd_CurDetRslt = CurDet_ClbCurCurChnGain(&glwd_ClbrValue);
    }
    asm NOP;
    asm NOP;
    if(3 == gby_FlagCLbrSlect)
    {
        gwd_CurDetRslt = CurDet_ClbCurVolChnOffset(&glwd_ClbrValue);
    }
    asm NOP;
    asm NOP;
    asm NOP;
    if(4 == gby_FlagCLbrSlect)
    {
        gwd_CurDetRslt = CurDet_ClbCurVolChnGain(&glwd_ClbrValue);
    }

    asm NOP;
    asm NOP;
    asm NOP;
    asm NOP;

    if(5 == gby_FlagCLbrSlect)
    {
        gwd_CurDetRslt = CurDet_UpdateAhCalcThrhd(0, 378);
        gwd_CurDetRslt = CurDet_UpdateAhCalcThrhd(1, 378);
    }
#endif
}


void CurDet_InitCurDet(void)
{}

static void CurrentDet_CalcAh(void)
{
    if (CURRENTIF_FLAG_CHG == CurDet_Eng.Flg)
    {
        if(CurDetConf.AhCntThrhd.Chg == 0)
        {
            return;
        }

        CurDet_Eng.AhCnt.Chg += CurDet_Eng.Energy;

        if (CurDet_Eng.AhCnt.Chg > CurDetConf.AhCntThrhd.Chg)
        {
            CurDet_Eng.Ah.Chg += CurDet_Eng.AhCnt.Chg / CurDetConf.AhCntThrhd.Chg;
            CurDet_Eng.AhCnt.Chg = CurDet_Eng.AhCnt.Chg % CurDetConf.AhCntThrhd.Chg;
        }
        if (CurDet_Eng.Ah.Chg > 0xFFFF0000)
        {
            CurDet_Eng.Ah.Chg = 0xFFFF0000;
        }

        Bat_UpdateAccumulateChgAh(CurDet_Eng.Ah.Chg);
    }
    else
    {
        if (CurDetConf.AhCntThrhd.Dch == 0)
        {
            return;
        }

        CurDet_Eng.AhCnt.Dch += CurDet_Eng.Energy;

        if (CurDet_Eng.AhCnt.Dch > CurDetConf.AhCntThrhd.Dch)
        {
            CurDet_Eng.Ah.Dch += CurDet_Eng.AhCnt.Dch/CurDetConf.AhCntThrhd.Dch;
            CurDet_Eng.AhCnt.Dch = CurDet_Eng.AhCnt.Dch%CurDetConf.AhCntThrhd.Dch;
        }
        if (CurDet_Eng.Ah.Dch > 0xFFFF0000)
        {
            CurDet_Eng.Ah.Dch = 0xFFFF0000;
        }

        Bat_UpdateAccumulateDchgAh(CurDet_Eng.Ah.Dch);
    }

}


/*
 *  interface for Calibration
 */
static void CurDet_BakupConfigTo(sCurrentDet_Config * RAMPTR ConfigPtr)
{
    uByte * RAMPTR p_byte_bak;
    uByte * RAMPTR p_byte;
    uWord wcnt;
    p_byte_bak = (uByte * RAMPTR)ConfigPtr;
    p_byte = (uByte * RAMPTR)(&CurDetConf);

    for(wcnt = 0; wcnt < sizeof(sCurrentDet_Config); wcnt++)
    {
        *(p_byte_bak++) = *(p_byte++);
    }

}

static GeneralReturnType CurDet_CompareConfigWith(sCurrentDet_Config * RAMPTR ConfigPtr)
{
    uByte * RAMPTR p_byte_bak;
    uByte * RAMPTR p_byte;
    GeneralReturnType rslt = ERR_OK;
    uWord wcnt;
    p_byte_bak = (uByte * RAMPTR)ConfigPtr;
    p_byte = (uByte * RAMPTR)(&CurDetConf);

    for(wcnt = 0; wcnt < sizeof(sCurrentDet_Config); wcnt++)
    {
        if((*(p_byte_bak++)) != (*(p_byte++)))
        {
            rslt = ERR_CURDET_PARA_STORE_TO_EE;
            return rslt;
        }
    }

    return rslt;
}

static GeneralReturnType CurDet_SaveConfAndCompare(void)
{}


GeneralReturnType CurDet_UpdateGainAndOffset(uWord Gain, uLWord Offset)
{
    volatile GeneralReturnType rslt;

    CurDetConf.GainOff.Gain = Gain;
    (void)Offset;

    rslt = CurDet_SaveConfAndCompare();

    return rslt;
}

GeneralReturnType CurDet_ClbCurCurChnOffset(uLWord * OffsetPtr)
{
    volatile GeneralReturnType rslt;
#ifdef LEM_CUR
    rslt = CurrentIf_ClbCur(0,0,&(CurDetConf.ChipConf.CurChnOffsetConf));
#else
    rslt = CurrentIf_ClbCur(CURRENTIF_CS5460A_1,CLBRTCUROFF,&(CurDetConf.ChipConf.CurChnOffsetConf));
#endif
    if(rslt)
    {
        *OffsetPtr = 0x5A5A5A5A; // set to wrong value
        return rslt;
    }

    LemOffset = CurDetConf.ChipConf.CurChnOffsetConf.Cmd;
    *OffsetPtr = *((uLWord *RAMPTR)(&CurDetConf.ChipConf.CurChnOffsetConf.Cmd));

    rslt = CurDet_SaveConfAndCompare();

    return rslt;
}

GeneralReturnType CurDet_ClbCurCurChnGain(uLWord * GainPtr)
{
    volatile  GeneralReturnType rslt;
    rslt = CurrentIf_ClbCur(CURRENTIF_CS5460A_1,CLBRTCURGAIN,&(CurDetConf.ChipConf.CurChnGainConf));

    if(rslt)
    {
        *GainPtr = 0x5A5A5A5A; // set to wrong value
        return rslt;
    }

    *GainPtr = *((uLWord *RAMPTR)(&CurDetConf.ChipConf.CurChnGainConf.Cmd));

    rslt = CurDet_SaveConfAndCompare();

    return rslt;
}

//  voltage channel interface
GeneralReturnType CurDet_ClbCurVolChnOffset(uLWord * OffsetPtr)
{
    volatile GeneralReturnType rslt;
    /*rslt = CurrentIf_ClbCur(CURRENTIF_CS5460A_1,CLBRTVOLTOFF,&(CurDetConf.ChipConf.VoltChnOffsetConf));

    if(rslt)
    {
        *OffsetPtr = 0x5A5A5A5A; // set to wrong value
        return rslt;
    } */

    /* use default for Voltage channel offset */
    CurDetConf.ChipConf.VoltChnOffsetConf.Cmd = WRVOLOFF;
    CurDetConf.ChipConf.VoltChnOffsetConf.Conf[0] = 0;
    CurDetConf.ChipConf.VoltChnOffsetConf.Conf[1] = 0;
    CurDetConf.ChipConf.VoltChnOffsetConf.Conf[2] = 0;

    *OffsetPtr = *((uLWord *RAMPTR)(&CurDetConf.ChipConf.VoltChnOffsetConf.Cmd));

    rslt = CurDet_SaveConfAndCompare();

    return rslt;
}

GeneralReturnType CurDet_ClbCurVolChnGain(uLWord * GainPtr)
{
    volatile GeneralReturnType rslt;
    /* rslt = CurrentIf_ClbCur(CURRENTIF_CS5460A_1,CLBRTVOLTGAIN,&(CurDetConf.ChipConf.VoltChnGainConf));

     if(rslt)
     {
         *GainPtr = 0x5A5A5A5A; // set to wrong value
         return rslt;
     } */

    /* use default for Voltage channel gain */
    CurDetConf.ChipConf.VoltChnGainConf.Cmd = WRVOLGAIN;
    CurDetConf.ChipConf.VoltChnGainConf.Conf[0] = 0xFF;
    CurDetConf.ChipConf.VoltChnGainConf.Conf[1] = 0xFF;
    CurDetConf.ChipConf.VoltChnGainConf.Conf[2] = 0xFF;

    *GainPtr = *((uLWord *RAMPTR)(&CurDetConf.ChipConf.VoltChnGainConf.Cmd));

    rslt = CurDet_SaveConfAndCompare();

    return rslt;
}



GeneralReturnType CurDet_UpdateAhCalcThrhd(uByte ChgOrDch, uLWord Thrhd)
{
    volatile GeneralReturnType rslt;
    if(CURRENTIF_FLAG_CHG == ChgOrDch)
    {
        CurDetConf.AhCntThrhd.Chg = Thrhd;
    }
    else if(CURRENTIF_FLAG_DCH == ChgOrDch)
    {
        CurDetConf.AhCntThrhd.Dch = Thrhd;
    }
    else
    {
        rslt = ERR_CURDET_CHGORDCH_FLAG_WRONG;
        return rslt;
    }

    rslt = CurDet_SaveConfAndCompare();

    return rslt;

}
static void CurrentDet_CalcAh_By_SoftWare(uWord Current,uByte FlgCurDirection,uWord TimeMs)
{
   uLWord tmp_ah;
   
   CurDet_Eng.Flg = FlgCurDirection;
   tmp_ah = Current;
   tmp_ah *= TimeMs; // 0.1mas
   CurDet_Eng.Energy = tmp_ah;
   CurDetConf.AhCntThrhd.Chg = 36000;
   CurDetConf.AhCntThrhd.Dch = 36000;
   CurrentDet_CalcAh();
}

