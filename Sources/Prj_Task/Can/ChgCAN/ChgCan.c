/* ChgCan.c */
#include "CanIf_Type.h"
#include "CanIf.h"
#include "ChgCan.h"
#include "SchIf.h"
#include "BmsSysSt.h"
#include "BatterySt.h"
#include "BatterySysSt.h"
#include "GbChgCan.h"

#include "ChargeSt.h"
#include "ChgM-B.h"

#ifdef CHG_CAN_EN

typedef struct
{
    uByte TaskPeriod;
    uByte Life;
}sChgCan_ComType;

sChgCan_ComType  ChgCan_Com;

sChgCan_MsgType ChgCan_Msg[MAX_CHG_CAN_SEND_BUF]; 
sChgCan_MsgType ChgCan_TestMsg;

uByte flag_test_chg_can = 0; 


uByte ChgCan_GetLife(void)
{
    return ChgCan_Com.Life;
}
void ChgCan_UpdateLife(uByte life)
{
    ChgCan_Com.Life = life;
}

void ChgCan_Init(void)
{
    uWord i;
    uByte *ptr;
    sCanIf_AcceptanceCodeType accept;
    sCanIf_AcceptanceMaskType mask;
        
   /* sChgCan_CtrlType *CtrPtr; */
 
    ptr = (uByte*)(ChgCan_Msg);
    for(i = 0; i < sizeof(ChgCan_Msg); i++)
    {
        *(ptr++) = 0;
    }
    
    ptr = (uByte *)(&ChgCan_Com);
    for(i = 0; i < sizeof(ChgCan_Com); i++)
    {
        *(ptr++) = 0;
    }
    
    CanIf_Init(CHG_CAN_CHANNEL, CANIF_BAUDRATE_250K);

    accept.AccCode1 = 0x00000000;
    mask.AccMask1 =   0xFFFFFFFF;

    accept.AccCode2 = 0x00000000;
    mask.AccMask2   = 0xFFFFFFFF;           
    CanIf_SetAcceptanceMaskCode(CHG_CAN_CHANNEL, &accept, &mask);
 
 
#ifdef ECU_M_ENABLE_GBCHGIF    
     GbChgCan_Init();
#endif     
    // project engineer must initial these msg 
    // if not use Msg set id 0      
    ChgCan_Msg[0].Msg.id = 0x18fd7524;//0x1806E5F4;
    ChgCan_Msg[0].Msg.length = 8;
    ChgCan_Msg[0].Period = TM_MS(500);
    ChgCan_Msg[0].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[1].Msg.id = 0x18e5;
    ChgCan_Msg[1].Msg.length = 8;
    ChgCan_Msg[1].Period = TM_MS(500);
    ChgCan_Msg[1].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[2].Msg.id = 0x1406e5f4;
    ChgCan_Msg[2].Msg.length = 8;
    ChgCan_Msg[2].Period = TM_MS(1000);
    ChgCan_Msg[2].SendCtrl.DelayTime= 0;

    ChgCan_Msg[3].Msg.id = 0;
    ChgCan_Msg[3].Msg.length = 8;
    ChgCan_Msg[3].Period = TM_MS(400);
    ChgCan_Msg[3].SendCtrl.DelayTime= 0; 
    
    ChgCan_Msg[4].Msg.id = 0;
    ChgCan_Msg[4].Msg.length = 8;
    ChgCan_Msg[4].Period = TM_500MS;
    ChgCan_Msg[4].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[5].Msg.id = 0;
    ChgCan_Msg[5].Msg.length = 8;
    ChgCan_Msg[5].Period = TM_500MS;
    ChgCan_Msg[5].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[6].Msg.id = 0;
    ChgCan_Msg[6].Msg.length = 8;
    ChgCan_Msg[6].Period = TM_500MS;
    ChgCan_Msg[6].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[7].Msg.id = 0;
    ChgCan_Msg[7].Msg.length = 8;
    ChgCan_Msg[7].Period = TM_500MS;
    ChgCan_Msg[7].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[8].Msg.id = 0;
    ChgCan_Msg[8].Msg.length = 8;
    ChgCan_Msg[8].Period = TM_500MS;
    ChgCan_Msg[8].SendCtrl.DelayTime= 0;
    
    ChgCan_Msg[9].Msg.id = 0;
    ChgCan_Msg[9].Msg.length = 8;
    ChgCan_Msg[9].Period = TM_500MS;
    ChgCan_Msg[9].SendCtrl.DelayTime= 0; 

  


    ChgCan_TestMsg.Msg.id = 0x04ff56f4;   //for test chg can id
    ChgCan_TestMsg.Msg.length = 8;
    for(i = 0; i < 8; i++)
    {
        ChgCan_TestMsg.Msg.data[i] = i+1;    
    }
    ChgCan_TestMsg.Period = TM_500MS;
    ChgCan_TestMsg.SendCtrl.DelayTime= 0;                                       
    /* initial other variable */  
          
}

/* call by 10 ms task */
void ChgCan_MainLoadData(void)
{
    uWord wtmp;
    
    if(ChgCan_Com.TaskPeriod >= TM_MS(10))
    {
        ChgCan_Com.TaskPeriod = 0;
        SaveStatusReg();
        
        /* call BatterySt interface reach the battery data  */
       
#ifdef ECU_M_ENABLE_GBCHGIF      
        GbChgCan_LoadData();
#endif              
        wtmp = Chg_GetPermitMaxSumV(); 
    
        ChgCan_Msg[0].Msg.data[0] = (uByte)(wtmp >> 8);
        ChgCan_Msg[0].Msg.data[1] = (uByte)wtmp;
        
        wtmp = Chg_GetPermitChgCurExt();//Chg_GetPermitChgCurOn();
        ChgCan_Msg[0].Msg.data[2] = (uByte)(wtmp >> 8);
        ChgCan_Msg[0].Msg.data[3] = (uByte)wtmp;
        
        ChgCan_Msg[0].Msg.data[4] = Chg_Mode;
        
        ChgCan_Msg[0].Msg.data[5] = Heat_Mode;

        RestoreStatusReg(); 
        
        ChgCan_MainSendMsg();
               
#ifdef ECU_M_ENABLE_GBCHGIF      
        GbChgCan_TpcmProc();
#endif               
    }
}

 
void ChgCan_ProcMsg(sCanIf_MsgType msg)
{
   uByte life; 
      
   life = ChgCan_GetLife(); 
   life++;
   
   
   if(0x18ff50e5 == msg.id)  //铁城充电协议
   {
      flag_onchg_connect =1;
      if(0 == msg.data[4])   //充电无故障
      {
          flag_onchg_err = 1;
      }
      else
      {
          flag_onchg_err = 0;
      }
   }

   
   if(0x04fff456 == msg.id)  // for test chg can msg  don't move these code !!!!!!!!!!!!
   {
      flag_test_chg_can = 1;
   }
   else
   {
      flag_test_chg_can = 0;
   }
   
    
#ifdef ECU_M_ENABLE_GBCHGIF    
     GbChg_RxDataFromCharger(msg);    
#endif 
     ChgCan_UpdateLife(life);
     
}

/* call by 5MS main task  */
void ChgCan_MainSendMsg(void) 
{
 
    uWord rslt = 0;
    uByte i;


#ifdef ECU_M_ENABLE_GBCHGIF   
    GbChgCan_TxData();
    //GbChgCan_TpcmProc();
#endif 
    
    if(1 == flag_onchg_connect)
    {
        for(i = 0; i < MAX_CHG_CAN_SEND_BUF; i++)                                
        {
            if(0 != ChgCan_Msg[i].Msg.id)
            {
                ChgCan_Msg[i].SendCtrl.TimeCnt++;                
                
                if(1 == ChgCan_Msg[i].SendCtrl.FirstSendFlag)
                {    
                    if(ChgCan_Msg[i].SendCtrl.TimeCnt >= ChgCan_Msg[i].Period) 
                    {
                        ChgCan_Msg[i].SendCtrl.TimeCnt = 0;
                        (void)CanIf_RequestSend(CHG_CAN_CHANNEL, &ChgCan_Msg[i].Msg, rslt);        
                    }
                    
                }
                else
                {
                    if(ChgCan_Msg[i].SendCtrl.TimeCnt >= ChgCan_Msg[i].SendCtrl.DelayTime) 
                    {
                        ChgCan_Msg[i].SendCtrl.FirstSendFlag = 1;
                        ChgCan_Msg[i].SendCtrl.TimeCnt = 0;
                        (void)CanIf_RequestSend(CHG_CAN_CHANNEL, &ChgCan_Msg[i].Msg, rslt);                        
                    }
                }
            }
        }
    }



    if(1 == flag_test_chg_can)
    {
        ChgCan_TestMsg.SendCtrl.TimeCnt++;
        if(ChgCan_TestMsg.SendCtrl.TimeCnt >= ChgCan_TestMsg.Period) 
        {
            ChgCan_TestMsg.SendCtrl.TimeCnt = 0;
            (void)CanIf_RequestSend(CHG_CAN_CHANNEL, &ChgCan_TestMsg.Msg, rslt);        
        }
    }
    else
    {
        ChgCan_TestMsg.SendCtrl.TimeCnt = 0;
    }
 
}


/* call by main task */
void ChgCan_MainReceiveMsg(void)
{   
    GeneralReturnType  rslt;      
    uByte buf_length;
    sCanIf_MsgType msg;
    
    /* have data ? */
    buf_length = CanIf_GetRxBufferLength(CHG_CAN_CHANNEL);
    if(buf_length)
    {
        rslt = CanIf_RxIndication(CHG_CAN_CHANNEL,&msg);
        if(!rslt)
        {
            ChgCan_ProcMsg(msg);
        }
    }
}

void ChgCan_CountCbk(void)
{
    GbChg_CountCbk();
    (ChgCan_Com.TaskPeriod < 255) ? (ChgCan_Com.TaskPeriod++) : (ChgCan_Com.TaskPeriod = 255);
}


#endif


