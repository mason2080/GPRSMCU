// CarCan.c
#include "CanIf_Type.h"
#include "CanIf.h"
#include "Ecu.h"
#include "CarCan.h"
#include "SchIf.h"
#include "BmsSysSt.h"
#include "BatterySt.h"
#include "BatterySysSt.h"
#include "ChargeSt.h"
#include "ErrorAlarm.h"
#include "HvM.h"

#include "ChgM-B.h"

#define VCU_CTL_PWR_ON     1
#define VCU_CTL_PWR_OFF    2

sCarCanMsgType sCarCan_Msg[MAX_CAR_CAN_SEND_BUF];
typedef struct
{
    uByte MsgLoadProCnt;
    uWord CanErrCnt;
}sCarCan_ProCntType;

#define CARCAN_TM_MS(cnt) (cnt / 5)

#define GET_HIGH_BYTE(val) ((uByte)(val >> 8))
#define GET_LOW_BYTE(val)   ((uByte)(val))
#define CarCan_FLAG_OK     1
#define CarCan_FLAG_FAIL   0
                                                
#define CARCAN_ERR_L1_MS TM_MS(2000)   // mS
#define CARCAN_ERR_L2_MS TM_MS(5000)   // mS    

const  uWord SUMV_MAX = 6500;
const  uWord SUMV_MIN = 4500;
const  uWord SUMV_MAX_MIN =  2000; 




const uWord ConstDisCurArray[CARCAN_DIS_CUR_MAX_T_POINT][CARCAN_CUR_MAX_SOC_POINT] =
{

 
  //   0     10%      20%    30%    40%     50%     60%   70%     80%     90%    100%
    {  0,    10,     10,     20,    20,     30,     30,   40,     40,     50,    50}, //-25
    {  0,    30,     30,     40,    40,     60,     60,   80,     80,     100,   100}, //0
    {  0,    40,     40,     80,    80,     150,    150,  200,    200,    300,   300}, //25
    {  0,    40,     40,     80,    80,     150,    150,  200,    200,    300,   300}, //55
    
    
};



const uWord ConstChgCurArray[CARCAN_CHG_CUR_MAX_T_POINT][CARCAN_CUR_MAX_SOC_POINT] =
{

     //   0     10%      20%    30%    40%     50%     60%   70%     80%     90%    100%
    {  40,      40,      30,    20,    20,     20,     10,   10,     0,       0,    0}, //0
    {  100,     100,     100,   80,    80,     60,     60,   50,     40,      40,   0}, //25
    {  80,      80,      80,    60,    60,     40,     40,   40,     30,      30,   0}, //55

    
    
};



uWord GET_ConstDisCur(uByte tem, uWord soc) 
{
      uByte t_point;
      uByte soc_point;
      uWord wtmp;
      soc_point = soc / 100; // 0 1 2 3  4 5 6 7 8 9 10
      
      
      if(tem < 15)    // -25
      {
          wtmp = 0;
         
      } 
      else if((tem >= 15) && (tem < 40))    // -25   0
      {
          t_point = 0;
          wtmp = ConstDisCurArray[t_point][soc_point];
         
      } 
      else if((tem >= 40) && (tem < 65))       // 0  25
      {
          t_point = 1;
          wtmp = ConstDisCurArray[t_point][soc_point];
         
      } 
      else if((tem >= 65) && (tem <= 95))     //25  55
      {
          t_point = 2;
          wtmp = ConstDisCurArray[t_point][soc_point];
         
      } 
      else if(tem == 95)      // 60
      {
          t_point = 3;
          wtmp = ConstDisCurArray[t_point][soc_point];
      }
      else if(tem > 95) 
      {
          wtmp = 0;
      }
      return wtmp;
      
}

uWord GET_ConstChgCur(uByte tem, uWord soc) 
{
      uByte t_point;
      uByte soc_point;
      uWord wtmp;
      soc_point = soc / 100; // 0 1 2 3  4 5 6 7 8 9 10
      
      
      if(tem < 40)            // -25
      {
          wtmp = 0;
         ;
      } 
      else if((tem >= 40) && (tem < 65))   //0 25
      {
          t_point = 0;
          wtmp = ConstChgCurArray[t_point][soc_point];
         
      } 
      else if((tem >= 65) && (tem < 85))    //25  45
      {
          t_point = 1;
          wtmp = ConstChgCurArray[t_point][soc_point];
         
      } 
      else if(tem == 85)    //45   
      {
          
          t_point = 2;
          wtmp = ConstChgCurArray[t_point][soc_point];
          
      } 
      else if(tem > 85)
      {
           wtmp = 0;
      }
      return wtmp;
      
}



sCarCan_ProCntType CarCan_ProCnt;

sCarCanMsgType sCarCan_TestMsg;  //test can

uByte flag_test_car_can=0; 

uByte Msg0_Times = 0;
uByte Msg0_Cnt=0;

static void CarCan_CheckStatus(void);

//#ifdef CAR_CAN_EN
void CarCan_Init(void)
{
    uWord i;
    uByte *ptr;

    sCanIf_AcceptanceCodeType accept;
    sCanIf_AcceptanceMaskType mask;


    ptr = (uByte*)sCarCan_Msg;
    for(i = 0; i < sizeof(sCarCan_Msg); i++)
    {
        *(ptr++) = 0;
    }
    
    CanIf_Init(CAR_CAN_CHANNEL, CANIF_BAUDRATE_250K );
    accept.AccCode1 = 0x00000000;
    mask.AccMask1 =   0xffffffff;

    accept.AccCode2 = 0x00000000;
    mask.AccMask2   = 0xffffffff;           
    CanIf_SetAcceptanceMaskCode(CAR_CAN_CHANNEL, &accept, &mask);    

    // project engineer must initial these msg
    // if not use Msg set id 0
    
 
 
    sCarCan_Msg[0].Msg.id = 0;//0x1818d0f3;//0x18018d0f3; //
    sCarCan_Msg[0].Msg.length = 8;
    sCarCan_Msg[0].Period = TM_MS(100);
    sCarCan_Msg[0].SendCtrl.DelayTime= 0;

    sCarCan_Msg[1].Msg.id = 0;//0x1819d0f3;//0x1819d0f3; //
    sCarCan_Msg[1].Msg.length = 8;
    sCarCan_Msg[1].Period = TM_MS(100);
    sCarCan_Msg[1].SendCtrl.DelayTime= 0;

    sCarCan_Msg[2].Msg.id = 0;//0x181ad0f3;//0x181b80f4; //
    sCarCan_Msg[2].Msg.length = 8;
    sCarCan_Msg[2].Period = TM_MS(100);
    sCarCan_Msg[2].SendCtrl.DelayTime= 0;

    sCarCan_Msg[3].Msg.id = 0;//0x181bd0f3;//0x181bd0f3; //
    sCarCan_Msg[3].Msg.length = 8;
    sCarCan_Msg[3].Period = TM_MS(1000);
    sCarCan_Msg[3].SendCtrl.DelayTime= 0;

    sCarCan_Msg[4].Msg.id = 0;
    sCarCan_Msg[4].Msg.length = 8;
    sCarCan_Msg[4].Period = TM_MS(100);
    sCarCan_Msg[4].SendCtrl.DelayTime= 0;

    sCarCan_Msg[5].Msg.id = 0;
    sCarCan_Msg[5].Msg.length = 8;
    sCarCan_Msg[5].Period = TM_MS(100);
    sCarCan_Msg[5].SendCtrl.DelayTime= 0;

    sCarCan_Msg[6].Msg.id = 0;
    sCarCan_Msg[6].Msg.length = 8;
    sCarCan_Msg[6].Period = TM_MS(100);
    sCarCan_Msg[6].SendCtrl.DelayTime= 0;

    sCarCan_Msg[7].Msg.id = 0;
    sCarCan_Msg[7].Msg.length = 8;
    sCarCan_Msg[7].Period = TM_MS(100);
    sCarCan_Msg[7].SendCtrl.DelayTime= 0;

    sCarCan_Msg[8].Msg.id = 0;
    sCarCan_Msg[8].Msg.length = 8;
    sCarCan_Msg[8].Period = TM_500MS;
    sCarCan_Msg[8].SendCtrl.DelayTime= 0;

    sCarCan_Msg[9].Msg.id = 0;
    sCarCan_Msg[9].Msg.length = 8;
    sCarCan_Msg[9].Period = TM_500MS;
    sCarCan_Msg[9].SendCtrl.DelayTime= 0;
    
    // test car can don't move these code!!!!!!
    sCarCan_TestMsg.Msg.id = 0x002;
    sCarCan_TestMsg.Msg.length = 8;
    for(i = 0; i < 8; i++)
    {
        sCarCan_TestMsg.Msg.data[i] = i+1;
    }
    sCarCan_TestMsg.Period = TM_MS(500);
    sCarCan_TestMsg.SendCtrl.DelayTime= 0;
    // initial other variable
    
    (void)CanIf_SetCanMode(CAR_CAN_CHANNEL,LISTEN_MODE);
}

// call by 10 ms task
void CarCan_MainLoadData(void)
{
   
    uWord wtmp,wtmp1,k;
    uByte btmp,btmp1,btmp2;
    //uLWord lwd_p;
    uLWord lwtmp,lwtmp1;

    sErr_ByteNType bat_byn;
    sErr_ByteNType hd_byn;
    sErr_ByteNType batsys_byn;
    
        //0x1818d0f3
    wtmp = Bat_GetSumV(OUT_SUMV);
    sCarCan_Msg[0].Msg.data[0] = GET_LOW_BYTE(wtmp);  
    sCarCan_Msg[0].Msg.data[1] = GET_HIGH_BYTE(wtmp);
    wtmp = Bat_GetCurrent(AVGCUR) + 2000;
    sCarCan_Msg[0].Msg.data[2] = GET_LOW_BYTE(wtmp);
    sCarCan_Msg[0].Msg.data[3] = GET_HIGH_BYTE(wtmp);
    wtmp = Bat_GetSOC();
    wtmp >>=2;
    sCarCan_Msg[0].Msg.data[4] = GET_LOW_BYTE(wtmp);
    wtmp = Bat_GetMinT();
    sCarCan_Msg[0].Msg.data[5] = GET_LOW_BYTE(wtmp);
    btmp = BatSys_GetPosRlySt() & 0x01;
    btmp1 = BatSys_GetNegRlySt() & 0x01;
    btmp1 <<= 1;
    btmp = btmp + btmp1;
    btmp1 = BatSys_GetPreRlySt() & 0x01;
    btmp1 <<= 2;
    btmp = btmp + btmp1;
    sCarCan_Msg[0].Msg.data[6] = btmp;
    btmp = BatSys_GetChgRlySt() & 0x01;
    sCarCan_Msg[0].Msg.data[7]= btmp;
    
    //0x1819d0f3
    wtmp = Bat_GetMinV()/5;   // 0.005V/bit
    sCarCan_Msg[1].Msg.data[0] = GET_LOW_BYTE(wtmp);
    sCarCan_Msg[1].Msg.data[1] = GET_HIGH_BYTE(wtmp);
    
    wtmp = Bat_GetMaxV()/5;  //0.005V/bit
    sCarCan_Msg[1].Msg.data[2] = GET_LOW_BYTE(wtmp);
    sCarCan_Msg[1].Msg.data[3] = GET_HIGH_BYTE(wtmp);
    wtmp = Bat_GetMaxT();
    sCarCan_Msg[1].Msg.data[4] = (uByte)wtmp;
    wtmp = Bat_GetMinT();
    sCarCan_Msg[1].Msg.data[5] = (uByte)wtmp;
    sCarCan_Msg[1].Msg.data[6] = (uByte)(Bat_GetDeltaT()) + 40;
    sCarCan_Msg[1].Msg.data[7] = 0xff;
    
    //0x181ad0f3
    btmp = Bat_GetMaxT();
    wtmp = Bat_GetSOC();
    wtmp1 = GET_ConstDisCur(btmp, wtmp)*10 + 32000;
    if(wtmp1 >= 65535) 
    {
        wtmp = 65535;
    }
    sCarCan_Msg[2].Msg.data[0] = GET_LOW_BYTE(wtmp1);
    sCarCan_Msg[2].Msg.data[1] = GET_HIGH_BYTE(wtmp1);
    btmp = Bat_GetMaxT();
    wtmp = Bat_GetSOC();
    wtmp1 = GET_ConstChgCur(btmp, wtmp)*10 + 32000;
    if(wtmp1 >= 65535) 
    {
        wtmp = 65535;
    }
    sCarCan_Msg[2].Msg.data[2] = GET_LOW_BYTE(wtmp1);
    sCarCan_Msg[2].Msg.data[3] = GET_HIGH_BYTE(wtmp1);
    if(Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH) >= ERR_LEVEL_TWO) 
    {
        btmp = 0x02;   //0000 0010
    } 
    else 
    {
        btmp = Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH);
    }
    if(Err_GetBatErrLevel(ERR_BAT_SUMV_HIGH) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x08;  //0000 10XX
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_SUMV_HIGH);
        btmp1 <<=2;
        btmp = btmp + btmp1;
    }
    if(Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x20;     //0010 XXXX
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_CELLV_LOW);
        btmp1 <<= 4;            //00xx 0000
        btmp = btmp +btmp1; 
    }
    if(Err_GetBatErrLevel(ERR_BAT_SUMV_LOW) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x80;    //1000 0000
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_SUMV_LOW);
        btmp1 <<= 6;
        btmp = btmp + btmp1;
    }
    sCarCan_Msg[2].Msg.data[4] = btmp;
    if(Err_GetBatErrLevel(ERR_BAT_T_LOW) >= ERR_LEVEL_TWO) 
    {
        btmp = 0x02;
    } 
    else 
    {
        btmp = Err_GetBatErrLevel(ERR_BAT_T_LOW);
    }
    if(Err_GetBatErrLevel(ERR_BAT_T_HIGH) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x08;
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_T_HIGH);
        btmp1 <<=2;
        btmp = btmp + btmp1;
    }
    if(Err_GetBatErrLevel(ERR_BAT_DELTV_ALLPACK) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x20;
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_DELTV_ALLPACK);
        btmp1 <<= 4;
        btmp = btmp +btmp1; 
    }
    if(Err_GetBatErrLevel(ERR_BAT_DELTT_ALLPACK) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x80;
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_DELTT_ALLPACK);
        btmp1 <<= 6;
        btmp = btmp + btmp1;
    }
    sCarCan_Msg[2].Msg.data[5] = btmp;
    
    if(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH) >= ERR_LEVEL_TWO) 
    {
        btmp = 0x02;
    } 
    else 
    {
        btmp = Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH);
    }
    if(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x08;
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG);
        btmp1 <<=2;
        btmp = btmp + btmp1;
    }
    if(Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= ERR_LEVEL_TWO) 
    {
        btmp |=   0x20;    //0010 0000 
    } 
    else 
    {
        btmp1 = Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR);
        btmp1 <<= 4;
        btmp = btmp +btmp1; 
    }
    if(Err_GetBatErrLevel(ERR_BAT_SOC_LOW) >= ERR_LEVEL_TWO) 
    {
        btmp = btmp | 0x80;
    } 
    else 
    {
        btmp1 = Err_GetBatErrLevel(ERR_BAT_SOC_LOW);
        btmp1 <<= 6;
        btmp = btmp + btmp1;
    }
    sCarCan_Msg[2].Msg.data[6] = btmp;
    sCarCan_Msg[2].Msg.data[7] = 0xff;
    
    
    k  = (Bat_GetSumV(INT_SUMV) - SUMV_MIN) ;  // 0~2000     0~100
    k /= 20;
    wtmp =  k * Bat_GetRatedEnergy();
    //wtmp *= Bat_GetSOC()/100;
    sCarCan_Msg[3].Msg.data[0] = GET_LOW_BYTE(wtmp);
    sCarCan_Msg[3].Msg.data[1] = GET_HIGH_BYTE(wtmp);

    wtmp =  Bat_GetRatedEnergy();
    sCarCan_Msg[3].Msg.data[2] = GET_LOW_BYTE(wtmp);
    sCarCan_Msg[3].Msg.data[3] = GET_HIGH_BYTE(wtmp);
   
    sCarCan_Msg[3].Msg.data[4] = k;
    sCarCan_Msg[3].Msg.data[5] = 0xff;
    sCarCan_Msg[3].Msg.data[6] = 0xff;//flag_carchg_run;
    sCarCan_Msg[3].Msg.data[7] = 0xff;//flag_carchg_fault;
    
    //*extern  uWord Bat_GetRemainEnergy(void);           

 }


static void CarCan_ProcMsg(sCanIf_MsgType msg)
{
    sHvM_VcuInfoType VcuInfo;
    uByte life,btmp;
    uWord wtmp, wtmp1,wtmp2; 
    uWord car_charger_suv_out;
    uWord car_charger_cur_out;
    uByte car_charger_t;
    uByte car_charger_fault;
    
    if(0x0c06f3d0 == msg.id)  //VCU
    {
        btmp = msg.data[0];
        life = msg.data[7];
        if(btmp == 0x11) 
        {
           VcuInfo.Cmd = 0x11;//VCU_CTL_PWR_ON; 
        }
        if(btmp == 0x10) 
        {
           VcuInfo.Cmd = 0x00;//VCU_CTL_PWR_OFF;  
        }
        //VcuInfo.VcuSumV = msg.data[1] + (msg.data[2] * 256);
       // HvM_UpdateVcuInfo(&VcuInfo);
        CarCan_ProCnt.CanErrCnt = 0;    
    }
    
    if(0x0c05f3d0 == msg.id) 
    {
      flag_onchg_connect =1;
      wtmp  = msg.data[0];
      wtmp1 = msg.data[1];
      wtmp1 <<= 8;
      car_charger_suv_out = wtmp1 + wtmp;
      car_charger_suv_out -= 10000;
      Chg_UpdateOutPutVoltage(car_charger_suv_out);

      
      wtmp  = msg.data[2];
      wtmp1 = msg.data[3];
      wtmp1 <<= 8;
      car_charger_cur_out = wtmp1 + wtmp;
      car_charger_cur_out -= 10000;
      Chg_UpdateOutPutCurrent(car_charger_cur_out);
      Chg_UpdateOutPutMaxCurrent(car_charger_cur_out);
      
      car_charger_t = msg.data[4];
      car_charger_fault = msg.data[5];
     /* if(((car_charger_t & 0x01) == 1) && ((car_charger_fault & 0x01) == 1))         // running  and starting
      {
          flag_carchg_run = 1;
      } 
      else 
      {
           flag_carchg_run = 0;
      }
      if(car_charger_fault > 0x01) 
      {
           flag_carchg_fault = 0;    
      } 
      else 
      {
           flag_carchg_fault = 1;     // NO FAULT
      }     */
      //Chg_UpdateErrorCode(car_charger_fault);
    }
     
    ///*****//for test car can   don't move these code !!!!!!!!!
    if(0x001 == msg.id)   
    {
        flag_test_car_can = 1;
    }
    else
    {
        flag_test_car_can = 0;
    }
    
    /////////////////////////////////////////////////////////////  

}


// call cycle 5mS by task
void CarCan_MainSendMsg(void)
{
    uByte i;
    uWord rslt = 0;
    
    
    if(1 == flag_test_car_can)
    {
        sCarCan_TestMsg.SendCtrl.TimeCnt++;
        if(sCarCan_TestMsg.SendCtrl.TimeCnt >= sCarCan_TestMsg.Period)    //send test msg
        {
            sCarCan_TestMsg.SendCtrl.TimeCnt = 0;
            (void)CanIf_RequestSend(CAR_CAN_CHANNEL, &sCarCan_TestMsg.Msg, rslt);
        }
    }
    else
    {
        sCarCan_TestMsg.SendCtrl.TimeCnt = 0;    
    }

    for(i = 0; i < MAX_CAR_CAN_SEND_BUF; i++)
    {
        if(0 != sCarCan_Msg[i].Msg.id)
        {
            sCarCan_Msg[i].SendCtrl.TimeCnt++;
            
            if(1 == sCarCan_Msg[i].SendCtrl.FirstSendFlag)
            {
                
                if(sCarCan_Msg[i].SendCtrl.TimeCnt >= sCarCan_Msg[i].Period)
                {
                    sCarCan_Msg[i].SendCtrl.TimeCnt = 0;
                    (void)CanIf_RequestSend(CAR_CAN_CHANNEL, &sCarCan_Msg[i].Msg, rslt);
                }


            }
            else
            {
                if(sCarCan_Msg[i].SendCtrl.TimeCnt >= sCarCan_Msg[i].SendCtrl.DelayTime)
                {
                    sCarCan_Msg[i].SendCtrl.FirstSendFlag = 1;
                    sCarCan_Msg[i].SendCtrl.TimeCnt = 0;
                   (void)CanIf_RequestSend(CAR_CAN_CHANNEL, &sCarCan_Msg[i].Msg, rslt);
                   
                }
                
            }
  
        }
    }

}

//call by main task
void CarCan_MainReceiveMsg(void)
{   
    GeneralReturnType  rslt;      
    uByte buf_length;
    sCanIf_MsgType msg;
    
    /* have data ? */
    buf_length = CanIf_GetRxBufferLength(CAR_CAN_CHANNEL);
    while(buf_length)
    {
         rslt = CanIf_RxIndication(CAR_CAN_CHANNEL,&msg);
         if(!rslt)
         {
             CarCan_ProcMsg(msg);
         } 
         buf_length--;       
    }

}
static void CarCan_CheckStatus(void)
{
    CarCan_ProCnt.CanErrCnt++;
    if(CarCan_ProCnt.CanErrCnt >= CARCAN_ERR_L2_MS)  // 500* 10ms
    {
        (void)Err_UpdateHwErrLevel(ERR_HW_CAN0, ERR_LEVEL_TWO);
        CarCan_ProCnt.CanErrCnt = 500;
    }
    else if(CarCan_ProCnt.CanErrCnt >= CARCAN_ERR_L1_MS)
    {
        (void)Err_UpdateHwErrLevel(ERR_HW_CAN0, ERR_LEVEL_ONE);
    }
}

void CarCan_CountCbk(void)
{
    (CarCan_ProCnt.MsgLoadProCnt < 255) ? (CarCan_ProCnt.MsgLoadProCnt++) : (CarCan_ProCnt.MsgLoadProCnt = 255);
 
}

void CarCan_ProMain(void)
{
    if(CarCan_ProCnt.MsgLoadProCnt >= TM_MS(5))
    {

        SaveStatusReg();
        CarCan_MainLoadData();   
 
        RestoreStatusReg();
        CarCan_ProCnt.MsgLoadProCnt = 0;
        
//        CarCan_CheckStatus();
    }

}

//#endif


