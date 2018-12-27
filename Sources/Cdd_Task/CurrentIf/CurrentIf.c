
#include "CurrentIf.h"

#include "CPUCCS.h"
//#include "CPUCINT.h"
#include "CPUCSCK.h"
#include "CPUCSI.h"
#include "CPUCSO.h"
//#include "CURPOWER.h"
#include "WdgIf.h"
#include "AiIf.h"
#include "SensorAi.h"

#define CURRENTIF_FLAG_OK         1
#define CURRENTIF_FLAG_FAIL       0

uByte LemOffset;

const uByte cnst_cmd_cur[4] =
{
    RDCUROFF, RDCURGAIN, RDVOLOFF, RDVOLGAIN
};

/*
#define CURRENTIF_GET_SDO(pDevice) ((*(pDevice->SdoGetValPtr))())
#define CURRENTIF_GET_INT(pDevice) ((*(pDevice->IntGetValPtr))())
#define CURRENTIF_CS_SET(pDevice)  ((*(pDevice->CsPutValPtr))(TRUE))
#define CURRENTIF_CS_CLR(pDevice)  ((*(pDevice->CsPutValPtr))(FALSE))
#define CURRENTIF_SCK_SET(pDevice) ((*(pDevice->SckPutValPtr))(TRUE))
#define CURRENTIF_SCK_CLR(pDevice) ((*(pDevice->SckPutValPtr))(FALSE))
#define CURRENTIF_SDI_SET(pDevice) ((*(pDevice->SdiPutValPtr))(TRUE))
#define CURRENTIF_SDI_CLR(pDevice) ((*(pDevice->SdiPutValPtr))(FALSE))
#define CURRENTIF_POW_ON(pDevice)  ((*(pDevice->PowPutValPtr))(FALSE))
#define CURRENTIF_POW_OFF(pDevice) ((*(pDevice->PowPutValPtr))(TRUE))
*/
/* initiallize serial port of 5460     */
static void CurrentIf_InitSp(eCurrentIf_DeviceIdType DeviceId);
/* write a uByte to 5460                */
static void CurrentIf_WrByte(eCurrentIf_DeviceIdType DeviceId, uByte bak);
/* write an array to 5460  */
static void CurrentIf_WrArray(eCurrentIf_DeviceIdType DeviceId, uByte* p_array, uByte num);
/* read an array from 5460 */
static void CurrentIf_RdArray(eCurrentIf_DeviceIdType DeviceId, uByte* p_target, uByte cmd);
/* write statue register   */
static void CurrentIf_Wr_StatusReg(eCurrentIf_DeviceIdType DeviceId);

static GeneralReturnType CurrentIf_WrChipChnConf(eCurrentIf_DeviceIdType DeviceId,sCurrentIf_ConfigFormatType * RAMPTR ChnConfPtr)
{

    uByte lby_array[4], lby_array_bak[4];
    uByte j,k;
    uByte rd_cmd = 0;
    GeneralReturnType rslt;
   // const sCurrent_OperationType * pDevice;

  //  pDevice = GET_DEVICE_HANDLE(DeviceId);
    lby_array[0] = ChnConfPtr->Cmd;
    lby_array[1] = ChnConfPtr->Conf[0];
    lby_array[2] = ChnConfPtr->Conf[1];
    lby_array[3] = ChnConfPtr->Conf[2];

    if(ChnConfPtr->Cmd > 0x40)
    {
        rd_cmd = ChnConfPtr->Cmd - 0x40; /* 0x40 == 0x42 -0x02
                                        *  #define WRCUROFF		0x42
                                        *  #define RDCUROFF    0x02
                                        */
    }

    for (j = 0; j < 3; j++)
    {
        /* write register */
        CurrentIf_InitSp(DeviceId);
        CurrentIf_WrArray(DeviceId, lby_array, 4);

        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array_bak, rd_cmd);

        /* check if written registers are correct */
        for (k = 0; k < 3; k++)
        {
            if (lby_array[k + 1] != lby_array_bak[k + 1])
            {
                break;
            }
        }

        if (k == 3)
        {
            /* registers are correct */
            rslt = ERR_OK;
            break;
        }
        else
        {
            rslt = (GeneralReturnType)CURRENTIF_ERR_WRGAINOFFSET;
        }
    }

    /* when write three times, write next  */
    if (CURRENTIF_ERR_WRGAINOFFSET == rslt)
    {
        return (GeneralReturnType)CURRENTIF_ERR_WRGAINOFFSET;
    }
    
    return rslt; 
}

//************  init current  *********************//
GeneralReturnType CurrentIf_Init(eCurrentIf_DeviceIdType DeviceId, sCurrent_ChipConfigType  *RAMPTR ChipConfPtr)
{
    uByte i;
    uByte lby_array[4], lby_array_bak[4];
    volatile GeneralReturnType rslt;
    //const sCurrent_OperationType * pDevice;

    if(DeviceId >= MAX_CURRENT_IC_MODULE_NUM)
    {
      return (GeneralReturnType) CURRENTIF_ID_INVALID;
    }
    //pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    
    //CURRENTIF_POW_ON(pDevice);
//    CURPOWER_PutVal(FALSE);
    
    
    /* init IO */
    //CURRENTIF_SCK_CLR(pDevice);
    CPUCSCK_PutVal(FALSE);
    //CURRENTIF_SDI_CLR(pDevice);
    //CPUCSI_PutVal(FALSE);
    //CURRENTIF_CS_CLR(pDevice);
    CPUCCS_PutVal(FALSE);

    WdgIf_Trig();
    /* power off -> power on */
    //CURRENTIF_POW_OFF(pDevice);
   // CURPOWER_PutVal(TRUE);
    (void)ECU_DELAYMS(1);   // need modify
    //CURRENTIF_POW_ON(pDevice);
   // CURPOWER_PutVal(FALSE);
    (void)ECU_DELAYMS(10);  // need modify
    WdgIf_Trig();

    /* write config register */
    lby_array[0] = WRCONFIG;
    lby_array[1] = 0x00;
    lby_array[2] = 0x00;
    lby_array[3] = 0x81;  /* start reset, set k=1 */

    for (i = 0 ; i < 3; i++)
    {
        CurrentIf_InitSp(DeviceId);
        CurrentIf_WrArray(DeviceId, lby_array, 4);

        /* check if register has been write correct */
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array_bak, RDCONFIG);

        if ( (lby_array[1] == lby_array_bak[1])
                && (lby_array[2] == lby_array_bak[2])
                && ((lby_array[3] & 0x7F) == lby_array_bak[3]) ) /* discard RS bit */
        {
            break;
        }
    }

    if(3 == i)
    {
        return (GeneralReturnType)CURRENTIF_ERR_WRCONFIG;
    }

    /* clear all bits in statue register */
    CurrentIf_InitSp(DeviceId);
    CurrentIf_Wr_StatusReg(DeviceId);

    /* write current/voltage offset/gain register */
    rslt = CurrentIf_WrChipChnConf(DeviceId,&(ChipConfPtr->CurChnOffsetConf));
    if(ERR_OK != rslt)
    {
      return rslt;
    }
    rslt = CurrentIf_WrChipChnConf(DeviceId,&(ChipConfPtr->CurChnGainConf));
    if(ERR_OK != rslt)
    {
      return rslt;
    }
    rslt = CurrentIf_WrChipChnConf(DeviceId,&(ChipConfPtr->VoltChnOffsetConf));
    if(ERR_OK != rslt)
    {
      return rslt;
    }
    rslt = CurrentIf_WrChipChnConf(DeviceId,&(ChipConfPtr->VoltChnGainConf));
    if(ERR_OK != rslt)
    {
      return rslt;
    }


    /* write mask */
    lby_array[0] = WRMASK;
    lby_array[1] = 0x80;  /* DYDT interrupt enable */
    lby_array[2] = 0;
    lby_array[3] = 0x00;

    for (i = 0 ; i < 3; i++)
    {           
        CurrentIf_InitSp(DeviceId);
        CurrentIf_WrArray(DeviceId, lby_array, 4);

        /* check if register has been write correct */
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array_bak, RDMASK);

        if ( (lby_array[1] == lby_array_bak[1])
                && (lby_array[2] == lby_array_bak[2])
                && (lby_array[3] == lby_array_bak[3]) )
        {
            break;
        }
        else
        {
            //return (GeneralReturnType)CURRENTIF_ERR_WRMASK;
        }
    }
    
    if(i > 3)
    {
       return (GeneralReturnType)CURRENTIF_ERR_WRMASK;
    }

    /* write cycle-count register */
    if ((ChipConfPtr->ChipCycle.Cmd ) == WRCC)
    {
        lby_array[0] = ChipConfPtr->ChipCycle.Cmd;
        lby_array[1] = ChipConfPtr->ChipCycle.Conf[0];
        lby_array[2] = ChipConfPtr->ChipCycle.Conf[1];
        lby_array[3] = ChipConfPtr->ChipCycle.Conf[2];
    }
    else
    {
        /* default value, 4000 cycle per computation, equals 1s */
        lby_array[0]=WRCC;
        lby_array[1]=0x00;
        lby_array[2]=0x0F;
        lby_array[3]=0xA0;
    }

    for (i = 0 ; i < 3; i++)
    {
        (void)i;
        CurrentIf_InitSp(DeviceId);
        CurrentIf_WrArray(DeviceId,lby_array, 4);

        /* check if register has been write correct */
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array_bak, RDCC);

        if ( (lby_array[1] == lby_array_bak[1])
                && (lby_array[2] == lby_array_bak[2])
                && (lby_array[3] == lby_array_bak[3]) )
        {
            break;
        }
        else
        {
            //return (GeneralReturnType)CURRENTIF_ERR_WRCC;
        }
    }
    
    if(i > 3)
    {
       return (GeneralReturnType)CURRENTIF_ERR_WRCC;
    }

    /* start continous conversion */
    lby_array[0] = STARTCONV;
    CurrentIf_WrArray(DeviceId, lby_array, 1);

    return (GeneralReturnType)CURRENTIF_ERR_OK;

} /* end of init current */

//************ CurrentIf_WrByte()  *********************//
static void CurrentIf_WrByte(eCurrentIf_DeviceIdType DeviceId, uByte data)
{
    uByte j;
    uByte btmp;
    //const sCurrent_OperationType * pDevice;

    //pDevice = GET_DEVICE_HANDLE(DeviceId);
   

    for (j = 0; j < 8; j++)
    {
        btmp = data;

        //CURRENTIF_SCK_CLR(pDevice);
         CPUCSCK_PutVal(FALSE);

        if (btmp & 0x80)
        {
            //CURRENTIF_SDI_SET(pDevice);
            //CPUCSI_PutVal(TRUE);
        }
        else
        {
            //CURRENTIF_SDI_CLR(pDevice);
            //CPUCSI_PutVal(FALSE);
        }

        data <<= 1;

        //CURRENTIF_SCK_SET(pDevice);
        CPUCSCK_PutVal(TRUE);
        //CURRENTIF_SCK_CLR(pDevice);
        CPUCSCK_PutVal(FALSE);
    }
}

//************ CurrentIf_WrArray()  *********************//
static void CurrentIf_WrArray(eCurrentIf_DeviceIdType DeviceId, uByte* p_array, uByte dlen)
{
    uByte i;
    //const sCurrent_OperationType * pDevice;

    //pDevice = GET_DEVICE_HANDLE(DeviceId);

    //CURRENTIF_CS_CLR(pDevice);
    CPUCCS_PutVal(FALSE);

    for (i = 0; i < dlen; i++)
    {
        CurrentIf_WrByte(DeviceId, *(p_array++));
    }

    //CURRENTIF_CS_SET(pDevice);
    CPUCCS_PutVal(TRUE);
}

//************ CurrentIf_RdArray()  *********************//
static void CurrentIf_RdArray(eCurrentIf_DeviceIdType DeviceId, uByte* p_target, uByte cmd)
{
    uByte i, j;
    uByte btmp;
    uByte lby_array[4];
    //const sCurrent_OperationType * pDevice;

    //pDevice = GET_DEVICE_HANDLE(DeviceId);

    //CURRENTIF_CS_CLR(pDevice);
    CPUCCS_PutVal(FALSE);

    lby_array[0] = cmd;
    lby_array[1] = SYNC0;
    lby_array[2] = SYNC0;
    lby_array[3] = SYNC0;

    for (i = 0; i < 4; i++)
    {
        btmp = 0;
        for (j = 0; j < 8; j++)
        {
            /* write cmd and sync0 */
            if (lby_array[i] & 0x80)
            {
                //CURRENTIF_SDI_SET(pDevice);
                //CPUCSI_PutVal(TRUE);
            }
            else
            {
                //CURRENTIF_SDI_CLR(pDevice);
                //CPUCSI_PutVal(FALSE);
            }

            //CURRENTIF_SCK_CLR(pDevice);
            CPUCSCK_PutVal(FALSE);

            lby_array[i] <<= 1;

            /* read data */
            btmp <<= 1;

            // CURRENTIF_SCK_SET(pDevice);
            CPUCSCK_PutVal(TRUE);

            //if (CURRENTIF_GET_SDO(pDevice))
            if(CPUCSO_GetVal())
            {
                btmp |= 0x01;
            }
        }

        *p_target++ = btmp;
    }

    //CURRENTIF_SDI_CLR(pDevice);
    //CPUCSI_PutVal(FALSE);
    //CURRENTIF_SCK_CLR(pDevice);
    CPUCSCK_PutVal(FALSE);
    //CURRENTIF_CS_SET(pDevice);
    CPUCCS_PutVal(TRUE);
}


/* write status register */
static void CurrentIf_Wr_StatusReg(eCurrentIf_DeviceIdType DeviceId)
{
    uByte lby_array[4];

    lby_array[0] = WRSTATUS;
    lby_array[1] = 0xff;
    lby_array[2] = 0xff;
    lby_array[3] = 0xff;

    CurrentIf_WrArray(DeviceId, lby_array, 4);
}

static void CurrentIf_InitSp(eCurrentIf_DeviceIdType DeviceId)
{
    uByte lby_array[4];

    lby_array[0] = SYNC1;
    lby_array[1] = SYNC1;
    lby_array[2] = SYNC1;
    lby_array[3] = SYNC0;

    CurrentIf_WrArray(DeviceId, lby_array, 4);
}


GeneralReturnType CurrentIf_ClbCur(eCurrentIf_DeviceIdType DeviceId, uByte Type, sCurrentIf_ConfigFormatType * RAMPTR ChnConfPtr)
{
#ifdef LEM_CUR
    uByte  i;
    uWord ad;
    uWord ad_bak[CUR_AD_SAMPLE];
    uWord ad_max;
    uWord ad_min;
    uLWord ad_sum;
    GeneralReturnType rslt = ERR_OK ;
    (void)DeviceId;
    (void)Type;
    
    ad_max = 0;
    ad_min = 0xffff;
    ad_sum = 0;

    for(i = 0; i < CUR_AD_SAMPLE; i++)
    { 
        (void)AiIf_MeasureChannel(AIIF_CPUAI_AC_P3_NO, &ad_bak[i]);
        if(ad_bak[i] > ad_max)
        {
            ad_max = ad_bak[i];
        }
        if(ad_bak[i] < ad_min)
        {
            ad_min = ad_bak[i];
        }
        ad_sum += ad_bak[i];
    }
    ad_sum -= ad_max;
    ad_sum -= ad_min;
    
    ad = (uWord)(ad_sum / (CUR_AD_SAMPLE - 2)); 
    
    if(ad > (CPU_ADC_ACCURACY / 2)) 
    {
       ad = ad - (CPU_ADC_ACCURACY / 2);
       ad |= 0x80;
    } 
    else 
    {
       ad = (CPU_ADC_ACCURACY / 2) - ad;
    }
    ChnConfPtr->Cmd = ad;
   
    rslt = ERR_OK;   
    return rslt;
#else
    uByte i,j;
    uByte lby_array[4];
    uByte lby_rslt_clbrt;  /*flag indicate if celibrate is complete*/
    //const sCurrent_OperationType * pDevice;

    if(DeviceId >= MAX_CURRENT_IC_MODULE_NUM)
    {
      return (GeneralReturnType) CURRENTIF_ID_INVALID;
    }
    //pDevice = GET_DEVICE_HANDLE(DeviceId);

    //CURRENTIF_POW_OFF(pDevice);
    //CURPOWER_PutVal(TRUE);
    WdgIf_Trig();
    (void)ECU_DELAYMS(100);
    //CURRENTIF_POW_ON(pDevice);
    //CURPOWER_PutVal(FALSE);
    (void)ECU_DELAYMS(20);
    WdgIf_Trig();
    CurrentIf_InitSp(DeviceId);

    /* set parameter of 5460 */
    lby_array[0] = WRCONFIG;  /*write config register*/
    lby_array[1] = 0x00;
    lby_array[2] = 0x00;
    lby_array[3] = 0x81;      /*start reset,set k=1*/

    CurrentIf_WrArray(DeviceId, lby_array, 4);
    //delay_long();
    (void)ECU_DELAYMS(1);

    CurrentIf_InitSp(DeviceId);
    CurrentIf_Wr_StatusReg(DeviceId);
    //delay_short();
    (void)ECU_DELAYMS(1);

    CurrentIf_InitSp(DeviceId);
    lby_array[0] = Type;
    CurrentIf_WrArray(DeviceId, lby_array, 1);

    for (i = 0; i < 2; i++)
    {
        /* wait for celibration complete */
        for(j = 0; j < 12 ; j++)
        {
             WdgIf_Trig();
             (void)ECU_DELAYMS(100); 
        }
        WdgIf_Trig();
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array, RDSTATUS);

        /* check if complete */
        if ((lby_array[1] & 0x80) == 0x80) /* if DRDY is set */
        {
            lby_rslt_clbrt = CURRENTIF_FLAG_OK;  /*celibration successes*/

            break;
        }
        else
        {
            lby_rslt_clbrt = CURRENTIF_FLAG_FAIL;

            /* re-start celibration */
            CurrentIf_InitSp(DeviceId);
            lby_array[0] = Type;
            CurrentIf_WrArray(DeviceId, lby_array, 1);
        }
    }

    if (lby_rslt_clbrt == CURRENTIF_FLAG_OK)
    {
        /* change celibrate command into read accordance register */
        i = ((Type & 0x18)>>1) - ((Type & 0x01) <<1);

        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array, i);

        /*store parameters as well as set register command */
        lby_array[0] = i | 0x40;

       /* for (i = 0; i < 4; i++)
        {
            *(p_off_and_gain++) = lby_array[i];
        } */
       ChnConfPtr->Cmd = lby_array[0];
       ChnConfPtr->Conf[0] = lby_array[1];
       ChnConfPtr->Conf[1] = lby_array[2];
       ChnConfPtr->Conf[2] = lby_array[3];

        /* return the result of celibration */
        return (GeneralReturnType)CURRENTIF_ERR_OK;
    }
    else
    {
        /* return the result of celibration */
        return (GeneralReturnType)CURRENTIF_ERR_CLBCUR;
    }
#endif    
}

#pragma MESSAGE DISABLE C5919

/***************************************************************/
/* Function: read current
   paramater transmit��gwd_energy,gwd_cur
*/
/***************************************************************/
uByte test_int = 0;

GeneralReturnType CurrentIf_RdCur(eCurrentIf_DeviceIdType DeviceId, uByte Type, uWord* CurAdPtr, uByte * FlgChgDch)
{
#ifdef LEM_CUR
    uByte  i;
    uWord ad, CurThrhd = 0;
    uWord ad_bak[CUR_AD_SAMPLE];
    uWord ad_max;
    uWord ad_min;
    uLWord ad_sum;
    GeneralReturnType rslt = ERR_OK ;
    (void)DeviceId;
    (void)Type;
    
    ad_max = 0;
    ad_min = 0xffff;
    ad_sum = 0;
  
    for(i = 0;i < CUR_AD_SAMPLE; i++)
    {        
        if(0==Type) {
          (void)AiIf_MeasureChannel(AIIF_CPUAI1_NO, &ad_bak[i]);
          LemOffset = 4;
        } 
        else{
          (void)AiIf_MeasureChannel(AIIF_CPUAI2_NO, &ad_bak[i]);
          LemOffset = 10;        
        }
        //ad_bak[i] = Sensor_GetAiSt(1);
        if(ad_bak[i] > ad_max)
        {
            ad_max = ad_bak[i];
        }
        if(ad_bak[i] < ad_min)
        {
            ad_min = ad_bak[i];
        }
        ad_sum += ad_bak[i];
    }
    ad_sum -= ad_max;
    ad_sum -= ad_min;
    
    ad = (uWord)(ad_sum / (CUR_AD_SAMPLE - 2));
   //LemOffset = 4;
   //LemOffset = 10; 
   if(0x80 == (LemOffset & 0x80)) 
   {
      CurThrhd = (LemOffset & 0x7F) + (CPU_ADC_ACCURACY / 2);
   } 
   else 
   {
      CurThrhd =  (CPU_ADC_ACCURACY / 2) - LemOffset;
   }
   
    if(ad > CurThrhd) 
    {
       *FlgChgDch = CURRENTIF_FLAG_DCH;
       *CurAdPtr = ad - CurThrhd;
       *CurAdPtr *= 10;
    } 
    else 
    {
       *FlgChgDch = CURRENTIF_FLAG_CHG; 
       *CurAdPtr = CurThrhd - ad;
       *CurAdPtr *= 10;
    } 
    rslt = ERR_OK;

#else 

    uByte i;
    uByte lby_array[4], lby_arraybak[4];
    uByte flag_chg_dch;
    uLWord lwd_result;
    GeneralReturnType rslt;
    //const sCurrent_OperationType * pDevice;
    rslt = CURRENTIF_ERR_OK;
    if(DeviceId >= MAX_CURRENT_IC_MODULE_NUM)
    {
      return (GeneralReturnType) CURRENTIF_ID_INVALID;
    }
    //pDevice = GET_DEVICE_HANDLE(DeviceId);

    /* read instantaneous current */
    if (Type == RDCURRINS)
    {
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array, RDCURRINS);

        if ((lby_array[1] & 0x80) == 0x80)
        {
            /* charging, direction of current is minus */
            lwd_result = lby_array[1] & 0x7f;
            lwd_result <<= 8;
            lwd_result += lby_array[2];
            lwd_result--;
            lwd_result ^= 0xFFFF;
            lwd_result &= 0x7fff;

            flag_chg_dch = CURRENTIF_FLAG_CHG;
        }
        else
        {
            /* discharging, direction of current is positive */
            lwd_result = lby_array[1];
            lwd_result <<= 8;
            lwd_result += lby_array[2];

            flag_chg_dch = CURRENTIF_FLAG_DCH;
        }

        /* prevent floating zero */
        if (lwd_result <= 5)
        {
            flag_chg_dch = CURRENTIF_FLAG_DCH;
            lwd_result = 0;
        }

        *CurAdPtr =  (uWord)lwd_result;
        *FlgChgDch = flag_chg_dch;
        return (GeneralReturnType)CURRENTIF_ERR_OK;

    } /* end of intanstaneous current */

    //if (!CURRENTIF_GET_INT(pDevice))  //CUR_INT == 0
    


#endif

    return rslt;
}


GeneralReturnType CurrentIf_CheckStatusReg(eCurrentIf_DeviceIdType DeviceId)
{
    uByte i;
    uByte lby_array[4];
    //const sCurrent_OperationType * pDevice;

    if(DeviceId >= MAX_CURRENT_IC_MODULE_NUM)
    {
      return (GeneralReturnType) CURRENTIF_ID_INVALID;
    }
    //pDevice = GET_DEVICE_HANDLE(DeviceId);

    CurrentIf_InitSp(DeviceId);
    CurrentIf_RdArray(DeviceId, lby_array, RDSTATUS);

    if ((((lby_array[1] & 0x80) == 0x80) || (lby_array[3] & 0x20) == 0x20)
            && (lby_array[1] != 0xFF)) /* floating pin */
    {
        /* clear statue register */
        CurrentIf_InitSp(DeviceId);
        CurrentIf_Wr_StatusReg(DeviceId);
    }

    for (i = 0; i < 3; i++)
    {}

    /* if statue has not been cleared for 3 times, return failed */
    if (i == 3)
    {
        return (GeneralReturnType)CURRENTIF_ERR_CHECKREG;
    }
}

#pragma MESSAGE WARNING C5919
GeneralReturnType CurrentIf_Check_ConfigReg(eCurrentIf_DeviceIdType DeviceId, uByte* p_config)
{
    uByte i,k;
    uByte lby_array[4];
    
    if(DeviceId >= MAX_CURRENT_IC_MODULE_NUM)
    {
      return (GeneralReturnType) CURRENTIF_ID_INVALID;
    }

    CurrentIf_InitSp(DeviceId);
    CurrentIf_RdArray(DeviceId, lby_array, RDCONFIG);

    if ( (lby_array[1] != 0)
            || (lby_array[2] != 0)
            || ( (lby_array[3] & 0x7F) != 0x01) ) /* discard RS bit */
    {
        return (GeneralReturnType)CURRENTIF_ERR_CHECKREG;
    }

    for (i = 0; i < 4; i++)
    {
        CurrentIf_InitSp(DeviceId);
        CurrentIf_RdArray(DeviceId, lby_array, cnst_cmd_cur[i]);

        /* discard commmand uByte */
        p_config++;

        /* check if written registers are correct */
        for (k = 0; k < 3; k++)
        {
            if ( *(p_config++) != lby_array[k + 1])
            {
                return (GeneralReturnType)CURRENTIF_ERR_CHECKREG;
            }
        }
    }

    CurrentIf_InitSp(DeviceId);
    CurrentIf_RdArray(DeviceId, lby_array, RDMASK);

    if ( (lby_array[1] != 0x80)
            || (lby_array[2] != 0)
            || (lby_array[3] != 0) )
    {
        return (GeneralReturnType)CURRENTIF_ERR_CHECKREG;
    }

    CurrentIf_InitSp(DeviceId);
    CurrentIf_RdArray(DeviceId, lby_array, RDCC);

    if ( (lby_array[1] != 0)
            || (lby_array[2] != 0x0F)
            || ( lby_array[3] != 0xA0) )
    {
        return (GeneralReturnType)CURRENTIF_ERR_CHECKREG;
    }

    return CURRENTIF_ERR_OK;
}