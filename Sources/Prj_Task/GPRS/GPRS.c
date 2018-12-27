#include "GPRS.h"
//#include "CPU_RS485.h"
//#include "CPU_RS485_DR.h"

#include "BmsSysSt.h"
#include "BatterySt.h"
#include "BatterySysSt.h"
#include "ErrorAlarm.h"
#include "ChgCan.h"
#include "ChgM-B.h"
#include "HvM-B.h"
#include <string.h>
#include "WdgIf.h"
#include "AM1_SD.h"
#include "GPRS.h"
#include "AM2_GPRS.h"
#include "ExternFlashIf.h"

#include "SysConf.h"
#include "CPUCSI.h"                  
#include "ExctCmd.h"
#include "gps1.h"
#include "IntCan.h"
#include "EE.h"

#include "ClockIf.h"

#define GET_HIGH_BYTE(val) ((uByte)(val >> 8))
#define GET_LOW_BYTE(val)   ((uByte)(val))

#define GET_HH_WORD(val)  ((uByte) (0x30|((val >> 12)&0x0f)))
#define GET_HL_WORD(val)  ((uByte) (0x30|((val >> 8)&0x0f)))
#define GET_LH_WORD(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_WORD(val)  ((uByte) (0x30|(val & 0x0f)))

#define GET_LH_BYTE(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_BYTE(val)  ((uByte) (0x30|(val & 0x0f)))

uWord AppGPRS_cnt,AppGPRS_cnt1,AppGPRS_cnt2,AppGPRS_cnt3,AppGPRS_cnt4,AppGPRS_cnt5;
uByte GPRS_rvnum;

uByte len[3];
uByte GPRS_fullrv_flag;
uByte GPRS_data_type;
uWord cnt_recv_bytes =0;
dword down_Line=1;
dword update_Line=0;


uByte GPRS_ttrbuff[700];	//485串口发送缓存
uByte GPRS_trnum;			  //485串口发送计数器         GPRS_trbuff
uByte GPRS_trlen;         
uByte GPRS_rvlen;
uByte GPRS_tx_busy;
Byte  GPRS_cmd[60];
uWord GPRSsendlen=0;
uWord GPRSSendIndex=0;
uWord S19Crc=0;

//Byte * GPRSCmd;
uWord DelayTimes=200;
byte flag=0;
byte enable_sendmsg=0 ;
byte sendMsgOk=0 ;
uWord sumRows=0;
uWord sumPacks=0;//one pack equals to :12 rows
uWord requesPackNo=1;

uByte VIN[13];
uByte IP[15];
uByte CARNUM[8];
uByte PORT[5];
uByte IMEI[15];
uByte PlatForm;    //1:BHN01 2:BHN02 3:BLN01 4:BHN02一体机
uByte YT_VNUM;
uByte YT_TNUM;

uWord YT_CellVolt[120];
uByte YT_CellTemp[60];

uByte InAh[4];
uByte OutAh[4];

uByte Flag_step=0;

uByte GprsLife=0 ;
bool flagSaving=FALSE;

sClockIf_TimeType time;
    
uByte *data1;
uByte pageWaitToSave=0;

uByte flagRecv[48]; //0：未接收完成 1：接收完成
uByte flagSave[8];
uByte startToSave=0;
uByte ConfigData[80];
uByte readCmd=0;


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

uByte GPRS_rvbuff[44];//88*3*2
uByte GPRS_rvbuff_backup[8][264];

uByte GPRS_buffRead[264];

#pragma DATA_SEG DEFAULT

GPRS_STEP GPRS_Step;
BCUCONFIG_STEP BcuConfig_Step;
BCUREAD_STEP   BcuReadStep   ;

void Clear_Rvbuff(void) 
{
   int i=0;
   
   for(i=0;i<44;i++) 
   {
     GPRS_rvbuff[i]=0;
   }
}

int SaveToTempArray(int Line)    //Line :1~N
{
  int i=0;
  int temp1=(Line-1)%48;
  int num1=temp1 /6;
  int num2= temp1%6;
  
  if(flagRecv[num1*6+num2]==0)
  {
    for( i=0;i <42;i++) 
    {
       GPRS_rvbuff_backup[num1][44*num2+i]= GPRS_rvbuff[2+i];
    }
    flagRecv[num1*6+num2]=1;
  }
  
      for(i=0;i<48;i++)
    {
      if(flagRecv[i]==0) 
      {
        return 0;
      }
    }
    
    return 1;
 
 /*
  temp1=Line%48;
  
  if(temp1==0) 
  {
    //pageWaitToSave=4;
    
    for(i=0;i<48;i++)
    {
      if(flagRecv[i]==0) 
      {
        return 0;
      }
    }
    
    return 1;
  } 
  else if(Line==sumRows)
  {
    for(i=0;i<temp1;i++)
    {
      if(flagRecv[i]==0) 
      {
        return 0;
      }
    }
    
    return 1;
  }
  
   return 0; */
}

 void GPRSdelay_times( Word time) 
{
    while(time--) 
    {
       asm nop;
       asm nop;
       
       if(time%500==0) 
       {
        
         //WdgIf_Trig();    
       }
    }
}



//***********************************************************************//
word CCRC_16_CalcModBUS(Byte cDataIn,word wCRCIn);
void Crc_16_CheckModbus(const Byte* pDataIn,int iLenIn,word* pCRCout)
{
    word wHi = 0;
    word wLo = 0;
    word wCRC,i;
    wCRC = 0xFFFF;
    //word i = 0;
    
    for(i = 0; i < iLenIn; i++)
    {
        wCRC = CCRC_16_CalcModBUS(*pDataIn,wCRC);
        pDataIn++;
        
        if( i%50==0)
         {
          
          // WdgIf_Trig();  
        }
    }
    wHi = wCRC/256;
    wLo = wCRC%256;
    wCRC = (wHi << 8) | wLo;
    *pCRCout = wCRC;
}
word CCRC_16_CalcModBUS(Byte cDataIn,word wCRCIn)
{
    word wCheck = 0,i;
    wCRCIn = wCRCIn ^ cDataIn;
    
    for(i = 0; i < 8; i++)
    {
       wCheck = wCRCIn & 1;
       wCRCIn = wCRCIn >> 1; 
       wCRCIn = wCRCIn & 0x7fff;
      // WdgIf_Trig();  
       if(wCheck == 1)
       {
          wCRCIn = wCRCIn ^ 0xa001;//0x8005;//
       }
       wCRCIn = wCRCIn & 0xffff;
    }
    return wCRCIn;
}
//****************************************************************//

void Send_BmsInfo(void) 
{
   int i=0,j=0;
   dword timesout=0;
    
    for(i=0;i< GPRSsendlen;i++) 
    {
      
     flag=0;
     AM2_GPRS_SendChar(GPRS_ttrbuff[i]);
     timesout=0;
     
     while(flag!=0xaa)
     {
      if(timesout<0x10000000) 
       {
          timesout++; 
          asm nop;
          asm nop;
          j++;
          if(j%1000==0)
          {
              WdgIf_Trig(); 
          }  
       }
       else
      {
       break;
      } 
     }
     
     if(GPRSsendlen%200==0)
      {
          WdgIf_Trig(); 
      }
   }
   
   
}

void Send_Bytes(byte cmd[],byte length) 
{
    int i=0;
    
    int j=0;
    dword timesout=0;
    for(i=0;i< length;i++) 
    {
     timesout=0; 
     flag=0;
     AM2_GPRS_SendChar(cmd[i]);
     
     while(flag!=0xaa)
     {
       if(timesout<0x10000000) 
       {
         timesout++; 
       asm nop;
       asm nop;
        j++;
        if(j%1000==0)
        {
            WdgIf_Trig(); 
        }  
       } 
       else
       {
         break;
       }
     }
     
     if(i%200==0)
      {
          WdgIf_Trig(); 
      }
   }
}

void Send_Chars(unsigned char *str) 
{
   int i=0;
   int j=0;
  dword timesout=0;
  
   while(*str!='\0') 
   {
     timesout=0;
     flag=0;
     AM2_GPRS_SendChar(*str);
     str++;
     while(flag!=0xaa)
     {
       if(timesout<0x10000000) 
       {
         timesout++; 
         asm nop;
         asm nop;
          j++;
          if(j%1000==0)
          {
              WdgIf_Trig(); 
          }
       } 
       else 
       {
       
       }
     }
     
        i++;
        if(i%200==0)
        {
            WdgIf_Trig(); 
        }  
   }
}



void GPRS_Init(void)
{
  //CPUCSI_PutVal(TRUE);
  
  // CPUCSI_PutVal(FALSE);

  //GPRSCmd ="AT+CGCLASS=\"B\"\r";

  GPRSSendIndex = 0;
  GPRSsendlen=15;
  
  //GPRS_Step=GETIMEI;
  GPRS_Step=REPOWER; 
  
  RD_GPRSSysconfProductNo();  //Read IP Address
  RD_GPRSCfgVer();  //Port Number
  RD_GPRSCodeInfo();   //VIN 
  RD_GPRSVehiclePlate();  //Read PlatForm
  
   AppGPRS_cnt4=0;
  
  
}








void GPRS_CheckExist(void) 
{
   int p=0;
   
   GPRS_ttrbuff[p++]= 0x57;
   GPRS_ttrbuff[p++]= 0xAB;
   
   GPRS_ttrbuff[p++]= 0x06;
   
   GPRSsendlen=p;
}

void GPRS_TxApp(void)//rs485_send_dat(void)
{
   /* if(GPRSSendIndex<=GPRSsendlen) 
    {		
      AM2_GPRS_SendChar(*(GPRSCmd+GPRSSendIndex));
      GPRSSendIndex++  ;
			               
    }
    else
    {
         if(GPRS_Step ==SEND_MSG) 
         {
            AM2_GPRS_SendChar(0x1A);
         }
         
    }  */
    
    flag=0xAA; 
}


void GPRS_MainCountCbk(void)
{
   (AppGPRS_cnt < 65535) ? (AppGPRS_cnt++) : (AppGPRS_cnt = 65535);
   
   (AppGPRS_cnt1 < 65535) ? (AppGPRS_cnt1++) : (AppGPRS_cnt1 = 65535);
   
   (AppGPRS_cnt2 < 65535) ? (AppGPRS_cnt2++) : (AppGPRS_cnt2 = 65535);
   
   (AppGPRS_cnt3 < 65535) ? (AppGPRS_cnt3++) : (AppGPRS_cnt3 = 65535);
   
   (AppGPRS_cnt4 < 65535) ? (AppGPRS_cnt4++) : (AppGPRS_cnt4= 65535);
   
   (AppGPRS_cnt5 < 65535) ? (AppGPRS_cnt5++) : (AppGPRS_cnt5= 65535);
}

  
  
    

   
   
void LenToStr(int length)  //最大长度为3，仅处理3位
{

   uByte temp=0;
   
   GPRS_cmd[0]= 'A';
   GPRS_cmd[1]= 'T';
   GPRS_cmd[2]= '+';
   GPRS_cmd[3]= 'C';
   GPRS_cmd[4]= 'I';
   GPRS_cmd[5]= 'P';
   GPRS_cmd[6]= 'S';
   GPRS_cmd[7]= 'E';
   GPRS_cmd[8]= 'N';
   GPRS_cmd[9]= 'D';
   GPRS_cmd[10]= '=';
 
    temp=length/100;
   
   GPRS_cmd[11]=temp+0x30;
   
   temp=length/10;
   temp=temp%10;
   
   GPRS_cmd[12]=temp+0x30;
   
   temp=length%10;
   
   GPRS_cmd[13]=temp+0x30;
   
   GPRS_cmd[14]='\r';
}

void load_GPRS_data(void) 
{  
            uWord Crc=0;
            byte i=0;
            byte j=0;
            uWord bat_num=0;
            uWord temp_num=0;
            uWord wtemp=0;
            uWord wtemp1=0;
            byte btemp=0;
            byte gby_length;
            uWord p=0;
            uByte gby_data_bak2[30];
            uByte gby_data2[30];
           
            GPRS_ttrbuff[p++]= '$'; //START
            
            if(GPRS_Step==SEND_MSG) 
            { 
            GPRS_ttrbuff[p++]= 'G';
            GPRS_ttrbuff[p++]= 'P';
            GPRS_ttrbuff[p++]= 'R';
            GPRS_ttrbuff[p++]= 'S';
            
            GPRS_ttrbuff[p++]= 0; //dataLen HighByte
            GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
            GPRS_ttrbuff[p++]= 0x01;//Frame Number  one frame max length:500 Bytes
            GPRS_ttrbuff[p++]= PlatForm;    //1:BHN01 2:BHN02 3:BLN01 4:BHN02一体机

            GPRS_ttrbuff[p++]= GprsLife;//9 Life
       
            GPRS_ttrbuff[p++]= CARNUM[0];
            GPRS_ttrbuff[p++]= CARNUM[1];
            GPRS_ttrbuff[p++]= CARNUM[2];
            GPRS_ttrbuff[p++]= CARNUM[3];
            GPRS_ttrbuff[p++]= CARNUM[4];
            GPRS_ttrbuff[p++]= CARNUM[5];
            GPRS_ttrbuff[p++]= CARNUM[6];
            GPRS_ttrbuff[p++]= CARNUM[7];
  
            GPRS_ttrbuff[p++]= VIN[0]; //18
            GPRS_ttrbuff[p++]= VIN[1];
            GPRS_ttrbuff[p++]= VIN[2];
            GPRS_ttrbuff[p++]= VIN[3];
            GPRS_ttrbuff[p++]= VIN[4];
            GPRS_ttrbuff[p++]= VIN[5];
            GPRS_ttrbuff[p++]= VIN[6];
            GPRS_ttrbuff[p++]= VIN[7];
            GPRS_ttrbuff[p++]= VIN[8];
            GPRS_ttrbuff[p++]= VIN[9];
            GPRS_ttrbuff[p++]= VIN[10];
            GPRS_ttrbuff[p++]= VIN[11];
            GPRS_ttrbuff[p++]= VIN[12];
            //WdgIf_Trig();  
            
            GPRS_ttrbuff[p++]= IMEI[0];// 31 IMEI
            GPRS_ttrbuff[p++]= IMEI[1];//
            GPRS_ttrbuff[p++]= IMEI[2];//
            GPRS_ttrbuff[p++]= IMEI[3];//
            GPRS_ttrbuff[p++]= IMEI[4];//
            GPRS_ttrbuff[p++]= IMEI[5];//
            GPRS_ttrbuff[p++]= IMEI[6];//
            GPRS_ttrbuff[p++]= IMEI[7];//
            GPRS_ttrbuff[p++]= IMEI[8];//
            GPRS_ttrbuff[p++]= IMEI[9];//
            GPRS_ttrbuff[p++]= IMEI[10];//
            GPRS_ttrbuff[p++]= IMEI[11];//
            GPRS_ttrbuff[p++]= IMEI[12];//
            GPRS_ttrbuff[p++]= IMEI[13];//
            GPRS_ttrbuff[p++]= IMEI[14];//
            
            GPRS_ttrbuff[p++]= 0;//46 Reserve 6bytes
            GPRS_ttrbuff[p++]= 0x1;//
            GPRS_ttrbuff[p++]= InAh[0];//
            GPRS_ttrbuff[p++]= InAh[1];//
            GPRS_ttrbuff[p++]= InAh[2];//
            GPRS_ttrbuff[p++]= InAh[3];//
           
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[0];//BCU TIME
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[1];//
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[2];//
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[3];//
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[4];//
            GPRS_ttrbuff[p++]= sbms_Info.bms_time[5];//
            
           // WdgIf_Trig(); 
            GPRS_ttrbuff[p++]= 0;//58 reserve 6 btyes
            GPRS_ttrbuff[p++]= 0x1;//
            GPRS_ttrbuff[p++]= OutAh[0];//
            GPRS_ttrbuff[p++]= OutAh[1];//
            GPRS_ttrbuff[p++]= OutAh[2];//
            GPRS_ttrbuff[p++]= OutAh[3];//
            
            GPRS_ttrbuff[p++]= time.year;//GPRS Module TIME     64
            GPRS_ttrbuff[p++]= time.month;//
            GPRS_ttrbuff[p++]= time.day;//
            GPRS_ttrbuff[p++]= time.hour;//
            GPRS_ttrbuff[p++]= time.minute;//
            GPRS_ttrbuff[p++]= time.second;//
                       
            GPRS_ttrbuff[p++]= gpsx.utc.date1[4];//70GPS Date   UTC TIME
            GPRS_ttrbuff[p++]= gpsx.utc.date1[5];//
            GPRS_ttrbuff[p++]= gpsx.utc.date1[2];//
            GPRS_ttrbuff[p++]= gpsx.utc.date1[3];//
            GPRS_ttrbuff[p++]= gpsx.utc.date1[0];//
            GPRS_ttrbuff[p++]= gpsx.utc.date1[1];//
            
            GPRS_ttrbuff[p++]= gpsx.utc.time[0];//GPS TIME
            GPRS_ttrbuff[p++]= gpsx.utc.time[1];//
            GPRS_ttrbuff[p++]= gpsx.utc.time[2];//
            GPRS_ttrbuff[p++]= gpsx.utc.time[3];//
            GPRS_ttrbuff[p++]= gpsx.utc.time[4];//
            GPRS_ttrbuff[p++]= gpsx.utc.time[5];//
           
            //WdgIf_Trig(); 
            GPRS_ttrbuff[p++]= gpsx.latitude1[0];//82纬度
            GPRS_ttrbuff[p++]= gpsx.latitude1[1];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[2];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[3];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[4];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[5];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[6];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[7];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[8];//
            GPRS_ttrbuff[p++]= gpsx.latitude1[9];//

            
            GPRS_ttrbuff[p++]= gpsx.longitude1[0];//92//  经度
            GPRS_ttrbuff[p++]= gpsx.longitude1[1];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[2];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[3];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[4];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[5];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[6];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[7];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[8];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[9];//
            GPRS_ttrbuff[p++]= gpsx.longitude1[10];//
           // WdgIf_Trig(); 
            GPRS_ttrbuff[p++]= gpsx.nshemi;//93南北
            GPRS_ttrbuff[p++]= gpsx.ewhemi;//东西
            
            GPRS_ttrbuff[p++]= (byte)(sbms_Info.sumv>>8);//105 SUMV
            GPRS_ttrbuff[p++]= (byte)sbms_Info.sumv;//

            GPRS_ttrbuff[p++]= (byte)(sbms_Info.avgCurr>>8);//107Current
            GPRS_ttrbuff[p++]= (byte)sbms_Info.avgCurr;//  
            
            GPRS_ttrbuff[p++]= (byte)(sbms_Info.soc>>8);//109SOC
            GPRS_ttrbuff[p++]= (byte)sbms_Info.soc;//


            GPRS_ttrbuff[p++]= (byte)(sbms_Info.soh>>8);//SOH
            GPRS_ttrbuff[p++]= (byte)sbms_Info.soh;// 

            GPRS_ttrbuff[p++]= (byte)(sbms_Info.maxv>>8);//maxv
            GPRS_ttrbuff[p++]= (byte)sbms_Info.maxv;//

            GPRS_ttrbuff[p++]= (byte)(sbms_Info.maxv_no>>8);//maxvno
            GPRS_ttrbuff[p++]= (byte)sbms_Info.maxv_no;//

            GPRS_ttrbuff[p++]= (byte)(sbms_Info.minv>>8);//minv
            GPRS_ttrbuff[p++]= (byte)sbms_Info.minv;//

            GPRS_ttrbuff[p++]= (byte)(sbms_Info.minv_no>>8);//minvno
            GPRS_ttrbuff[p++]= (byte)sbms_Info.minv_no;//   
            //WdgIf_Trig();  
            GPRS_ttrbuff[p++]= sbms_Info.maxt;//maxt
            
            GPRS_ttrbuff[p++]= (byte)(sbms_Info.maxt_no>>8);// maxtno
            GPRS_ttrbuff[p++]= (byte)sbms_Info.maxt_no;
            
            GPRS_ttrbuff[p++]= sbms_Info.mint;//mint
            GPRS_ttrbuff[p++]= (byte)(sbms_Info.mint_no>>8);// mintno
            GPRS_ttrbuff[p++]= (byte)sbms_Info.mint_no;  
            
            GPRS_ttrbuff[p++]= sbms_Info.io_state[0];//117 IO Status
            GPRS_ttrbuff[p++]= sbms_Info.io_state[1];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[2];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[3];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[4];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[5];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[6];//
            GPRS_ttrbuff[p++]= sbms_Info.io_state[7];//  
            
            
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[0];//125 Bat_ErrCode0
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[1];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[2];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[3];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[4];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[5];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[6];//
            GPRS_ttrbuff[p++]= sbms_Info.bat_err[7];// 
            //WdgIf_Trig(); 
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[0];//133 BatSYS_ErrCode0
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[1];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[2];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[3];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[4];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[5];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[6];//
            GPRS_ttrbuff[p++]= sbms_Info.batSys_err[7];// 
            
            GPRS_ttrbuff[p++]= sbms_Info.other_err[0];//141 Other_ErrCode0
            GPRS_ttrbuff[p++]= sbms_Info.other_err[1];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[2];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[3];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[4];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[5];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[6];//
            GPRS_ttrbuff[p++]= sbms_Info.other_err[7];// 
            //WdgIf_Trig(); 
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[0];//149 Hd_ErrCode0
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[1];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[2];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[3];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[4];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[5];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[6];//
            GPRS_ttrbuff[p++]= sbms_Info.hard_err[7];// 

            if((PlatForm==0x01)||(PlatForm==0x02)) //1:BHN01 2:BHN02 3:BLN01 4:BHN02一体机
            {
              GPRS_ttrbuff[p++]= sbms_Info.bmu_num;// 167 BMUNumber
              wtemp=0;
              wtemp1=0;
              
              for(i=0;i<sbms_Info.bmu_num;i++) 
              {
                 wtemp+= sbms_Info.bmu_volt_num[i+1];
                 
                 wtemp1+= sbms_Info.bmu_temp_num[i+1];
              }

              if(wtemp<5) 
              {
                 wtemp=5;
              }
              
              if(wtemp>200) //MaxCellNumber=200
              {
                wtemp=200;
              }
              
              if(wtemp1>100) 
              {
                wtemp1=100;
              }
              
              if(wtemp1<2) 
              {
               wtemp1=2;
              }
              
              GPRS_ttrbuff[p++]= (byte)(wtemp>>8);// 168 VoltNumber  
              GPRS_ttrbuff[p++]= (byte)wtemp;
              
              GPRS_ttrbuff[p++]= (byte)(wtemp1>>8);
              GPRS_ttrbuff[p++]= (byte)wtemp1;//171 TempNumber  MAXTEMPNUM=100;
              
              if(wtemp==5) 
              {
                      wtemp= Bat_GetCellV(1,1);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(1,2);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                       wtemp= Bat_GetCellV(1,3);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(1,4);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(1,5);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
              }else 
              {
                for(i=0;i<sbms_Info.bmu_num;i++) 
                {
                   for(j=0;j<sbms_Info.bmu_volt_num[i+1];j++)
                   {
                    if(p<172+400) 
                    {
                      wtemp= Bat_GetCellV(i+1,j+1);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     // 172
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      
                      if(p%100==0) 
                      {
                         //WdgIf_Trig(); 
                      }
                    }
                   }                                                        
                }
              }
                  
             if(wtemp1==2) 
              {
                  btemp=  Bat_GetTemper(1,1);
                  GPRS_ttrbuff[p++]=btemp;
                  
                  btemp=  Bat_GetTemper(1,2);
                  GPRS_ttrbuff[p++]=btemp;
              } 
              else
              {
                for(i=0;i<sbms_Info.bmu_num;i++) 
                {
                   for(j=0;j<sbms_Info.bmu_temp_num[i+1];j++)
                   {
                   
                    btemp=  Bat_GetTemper(i+1,j+1);
                    GPRS_ttrbuff[p++]=  btemp;
                    
                      if(p%100==0) 
                      {
                        // WdgIf_Trig(); 
                      }
                   }
                }
              }
            }//End PlatForm 1 2
             else if((PlatForm==0x03)||(PlatForm==0x04)) //1:BHN01 2:BHN02 3:BLN01 4:BHN02一体机
            {
              GPRS_ttrbuff[p++]= 0;;// 167 BMUNumber

              wtemp=YT_VNUM;
              wtemp1=YT_TNUM;

              if(wtemp<5) 
              {
                 wtemp=5;
              }
              
              if(wtemp>200) //MaxCellNumber=200
              {
                wtemp=200;
              }
              
              if(wtemp1>100) 
              {
                wtemp1=100;
              }
              
              if(wtemp1<2) 
              {
               wtemp1=2;
              }
              
              GPRS_ttrbuff[p++]= (byte)(wtemp>>8);// 168 VoltNumber  
              GPRS_ttrbuff[p++]= (byte)wtemp;
              
              GPRS_ttrbuff[p++]= (byte)(wtemp1>>8);
              GPRS_ttrbuff[p++]= (byte)wtemp1;//171 TempNumber  MAXTEMPNUM=100;
              
              if(wtemp==5) 
              {
                      wtemp= Bat_GetCellV(0,1);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(0,2);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                       wtemp= Bat_GetCellV(0,3);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(0,4);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      wtemp= Bat_GetCellV(0,5);
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
              }else 
              {
                   for(j=0;j<YT_VNUM;j++)
                   {
                    if(p<172+400) 
                    {
                      wtemp= YT_CellVolt[j];
                      GPRS_ttrbuff[p++]=(byte)(wtemp>>8);     // 172
                      GPRS_ttrbuff[p++]=(byte)(wtemp);
                      
                      if(p%100==0) 
                      {
                         //WdgIf_Trig(); 
                      }
                    }
                   }                                                        
              }
                  
             if(wtemp1==2) 
              {
                  btemp=  Bat_GetTemper(0,1);
                  GPRS_ttrbuff[p++]=btemp;
                  
                  btemp=  Bat_GetTemper(0,2);
                  GPRS_ttrbuff[p++]=btemp;
              } 
              else
              {
                   for(j=0;j<YT_TNUM;j++)
                   {
                   
                    btemp= YT_CellTemp[j];
                    GPRS_ttrbuff[p++]=  btemp;
                    
                      if(p%100==0) 
                      {
                         //WdgIf_Trig(); 
                      }
                   }
              }
            }//End PlatForm 3 4
            
            } 
            else if(GPRS_Step==DOWNFILE) 
            {
            
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= '0';
              
              GPRS_ttrbuff[p++]= 0; //dataLen HighByte
              GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
            
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'N';
              GPRS_ttrbuff[p++]= 'T';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'R';
              GPRS_ttrbuff[p++]= 'D';
              GPRS_ttrbuff[p++]= 'O';
              GPRS_ttrbuff[p++]= 'W';           
              GPRS_ttrbuff[p++]= 'N';
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'O';
              GPRS_ttrbuff[p++]= 'D';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= (byte)(requesPackNo>>24);
              GPRS_ttrbuff[p++]= (byte)(requesPackNo>>16);
              GPRS_ttrbuff[p++]= (byte)(requesPackNo>>8);
              GPRS_ttrbuff[p++]= (byte)(requesPackNo);
              

            }    
            else if(GPRS_Step==UPDATE) 
            {
            
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= '1';
              
              GPRS_ttrbuff[p++]= 0; //dataLen HighByte
              GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
            
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'N';
              GPRS_ttrbuff[p++]= 'T';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'R';
              GPRS_ttrbuff[p++]= 'U';
              GPRS_ttrbuff[p++]= 'P';
              GPRS_ttrbuff[p++]= 'D';           
              GPRS_ttrbuff[p++]= 'A';
              GPRS_ttrbuff[p++]= 'T';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'D';
              GPRS_ttrbuff[p++]= (byte)(SendCanIndex>>8);
              GPRS_ttrbuff[p++]= (byte)(SendCanIndex);
              GPRS_ttrbuff[p++]= (byte)(sumRows>>8);
              GPRS_ttrbuff[p++]= (byte)(sumRows);
            }
            else if(GPRS_Step==CHECKFILE) 
            {
            

              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= '4';
              
              GPRS_ttrbuff[p++]= 0; //dataLen HighByte
              GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
            
              GPRS_ttrbuff[p++]= 'C';
              GPRS_ttrbuff[p++]= 'H';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'C';
              GPRS_ttrbuff[p++]= 'K';
              GPRS_ttrbuff[p++]= 'F';
              GPRS_ttrbuff[p++]= 'I';
              GPRS_ttrbuff[p++]= 'L';           
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'O';
              GPRS_ttrbuff[p++]= 'D';
              GPRS_ttrbuff[p++]= 'E';
              
              EE_LoadVar(S19_FILE_INFOR);
              SysConf_GetParaValueFromRam(S19_FILE_INFOR, gby_data2,&gby_length);
              if((gby_data2[0]==gby_data2[4])&&(gby_data2[1]==gby_data2[5])&&(gby_data2[2]==gby_data2[6])&&(gby_data2[3]==gby_data2[7])) 
             {
              GPRS_ttrbuff[p++]= gby_data2[0];
              GPRS_ttrbuff[p++]= gby_data2[1];
              GPRS_ttrbuff[p++]= gby_data2[2];
              GPRS_ttrbuff[p++]= gby_data2[3];
             } 
             else
             {
              GPRS_ttrbuff[p++]= 0;
              GPRS_ttrbuff[p++]= 0;
              GPRS_ttrbuff[p++]= 0;
              GPRS_ttrbuff[p++]= 0;          
             }

            }
            else if(GPRS_Step ==CONFIG) //ConfigResult
            {
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= '5';
              
              GPRS_ttrbuff[p++]= 0; //dataLen HighByte
              GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
            
              GPRS_ttrbuff[p++]= 'C';
              GPRS_ttrbuff[p++]= 'O';
              GPRS_ttrbuff[p++]= 'N';
              GPRS_ttrbuff[p++]= 'F';
              GPRS_ttrbuff[p++]= 'I';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= 'R';
              GPRS_ttrbuff[p++]= 'E';           
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'U';
              GPRS_ttrbuff[p++]= 'L';
              GPRS_ttrbuff[p++]= 'T';
              GPRS_ttrbuff[p++]= ConfigData[0];
              GPRS_ttrbuff[p++]= ConfigResult;
            }
            
             else if(GPRS_Step ==READ) //READ_Result
            {
              GPRS_ttrbuff[p++]= 'M';
              GPRS_ttrbuff[p++]= 'S';
              GPRS_ttrbuff[p++]= 'G';
              GPRS_ttrbuff[p++]= '6';
              
              GPRS_ttrbuff[p++]= 0; //dataLen HighByte
              GPRS_ttrbuff[p++]= 0; //dataLen LowByte
            
              GPRS_ttrbuff[p++]= 'R';
              GPRS_ttrbuff[p++]= 'E';
              GPRS_ttrbuff[p++]= 'A';
              GPRS_ttrbuff[p++]= 'D';
              GPRS_ttrbuff[p++]= readCmd;
              GPRS_ttrbuff[p++]= ConfigResult;
              
              for(j=0;j<63;j++) 
              {
                GPRS_ttrbuff[p++] =readData[j];
              }
            }
            
            
            wtemp=p;
            GPRS_ttrbuff[5]= (byte)(wtemp>>8);
            GPRS_ttrbuff[6]= (byte)wtemp;// len
            
            Crc_16_CheckModbus(GPRS_ttrbuff,p,&Crc);
            GPRS_ttrbuff[p++]= (byte)(Crc>>8);// CRC High
            GPRS_ttrbuff[p++]= (byte)Crc;// CRC Low
            
            GPRS_ttrbuff[p++]= '#';
            GPRS_ttrbuff[p++]= 'E';
            GPRS_ttrbuff[p++]= 'N';
            GPRS_ttrbuff[p++]= 'D';
            GPRSsendlen=p;
            
            LenToStr(p);

}

void GPRS_Connect(void) 
{
     uByte p=0;
     
      //Send_Chars("AT+CIPCLOSE\r"); ///////////
      //Send_Chars("AT+CPOWD=1\r"); ///////////
      //Send_Chars("AT+CIPMODE=1\r");
      Send_Chars("AT+CGCLASS=\"B\"\r");
      Send_Chars("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");
      Send_Chars("AT+CGATT=1\r");
      //Send_Chars("AT+CLPORT=\"TCP\",\"9900\"\r");

     GPRS_cmd[p++]= 'A';
     GPRS_cmd[p++]= 'T';
     GPRS_cmd[p++]= '+';
     GPRS_cmd[p++]= 'C';
     GPRS_cmd[p++]= 'I';
     GPRS_cmd[p++]= 'P';
     GPRS_cmd[p++]= 'S';
     GPRS_cmd[p++]= 'T';
     GPRS_cmd[p++]= 'A';
     GPRS_cmd[p++]= 'R';
     GPRS_cmd[p++]= 'T';
     GPRS_cmd[p++]= '=';
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= 'T';
     GPRS_cmd[p++]= 'C';
     GPRS_cmd[p++]= 'P';
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= ',';
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= IP[0];
     GPRS_cmd[p++]= IP[1];
     GPRS_cmd[p++]= IP[2];
     GPRS_cmd[p++]= IP[3];
     GPRS_cmd[p++]= IP[4];
     GPRS_cmd[p++]= IP[5];
     GPRS_cmd[p++]= IP[6];
     GPRS_cmd[p++]= IP[7];
     GPRS_cmd[p++]= IP[8];
     GPRS_cmd[p++]= IP[9];
     GPRS_cmd[p++]= IP[10];
     GPRS_cmd[p++]= IP[11];
     GPRS_cmd[p++]= IP[12];
     GPRS_cmd[p++]= IP[13];
     GPRS_cmd[p++]= IP[14];
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= ',';
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= PORT[0];
     GPRS_cmd[p++]= PORT[1];
     GPRS_cmd[p++]= PORT[2];
     GPRS_cmd[p++]= PORT[3];
     GPRS_cmd[p++]= PORT[4];
     GPRS_cmd[p++]= '\"';
     GPRS_cmd[p++]= '\r';

     Send_Bytes(GPRS_cmd,p); 
     //Send_Chars("AT+CIPSTART=\"TCP\",\"122.228.019.057\",\"10894\"\r");
}

void SaveToFlash()  //Save To At45db
{
    int i=0;
    byte k=0;
    byte result=0;
    byte temp=0;
    uByte gby_length=0;
    
    uByte gby_data_bak1[30];
    uByte gby_data1[30];
   
   
    
   for(k=0;k<8;k++) 
   {
      if((flagSave[k]==0) &&(flagRecv[6*k+0]==1)&&(flagRecv[6*k+1]==1)&&(flagRecv[6*k+2]==1)&&(flagRecv[6*k+3]==1)&&(flagRecv[6*k+4]==1)&&(flagRecv[6*k+5]==1)) 
      {
        if(flagSaving==FALSE) 
        {
          AppGPRS_cnt5=0;
          flagSaving=TRUE;
          ExternFlashIf_WritePage((requesPackNo-1)*8+k, &GPRS_rvbuff_backup[k][0],264); 
        }
        if(AppGPRS_cnt5>6) 
        {
             ExternFlashIf_ReadPage((requesPackNo-1)*8+k, &GPRS_buffRead,264); 
             flagSaving=FALSE;
        
            for(i=0;i<264;i++)//check save result 
            {
                if( GPRS_rvbuff_backup[k][i]!= GPRS_buffRead[i]) 
                {
                    temp=i;
                    return;    
                }
            }
           flagSave[k]=1;
        }
      }
      result+=flagSave[k];
   }
   
   if(result==8) 
   {
     startToSave=0;
     
     for(i=0;i<48;i++) 
     {
       flagRecv[i]=0;
     }
     
     for(i=0;i<8;i++) 
     {
       flagSave[i]=0;
     }
     
     requesPackNo++;
     if(requesPackNo==2)//清除
     {
         
         
         gby_data_bak1[0]=0;
         gby_data_bak1[1]=0;
         
         gby_data_bak1[2]=0;
         gby_data_bak1[3]=0;
         
         gby_data_bak1[4]=0;
         gby_data_bak1[5]=0;
         
         gby_data_bak1[6]=0;
         gby_data_bak1[7]=0;
         
         for(k=0;k<5;k++) 
         {
            SysConf_UpdataParaToRam(S19_FILE_INFOR, gby_data_bak1);
            EE_SaveVarForCalib(BOTHEE, S19_FILE_INFOR);
         
            EE_LoadVar(S19_FILE_INFOR);
            SysConf_GetParaValueFromRam(S19_FILE_INFOR, gby_data1,&gby_length);
            
            for(i=0;i<8;i++)//check save result 
            {
                if( gby_data_bak1[i]!= gby_data1[i]) 
                {
                 break;
                }
                
                if(i==7) 
                {
                 return; 
                }
            }
         }  
     }
     
     if(requesPackNo>sumPacks) 
     {
         
         
         gby_data_bak1[0]=(byte)(S19Crc>>8);
         gby_data_bak1[1]=(byte)S19Crc;
         
         gby_data_bak1[2]=(byte)(sumRows>>8);
         gby_data_bak1[3]=(byte)sumRows;
         
         gby_data_bak1[4]=(byte)(S19Crc>>8);
         gby_data_bak1[5]=(byte)S19Crc;
         
         gby_data_bak1[6]=(byte)(sumRows>>8);
         gby_data_bak1[7]=(byte)sumRows;
         
         for(k=0;k<5;k++) 
         {
            SysConf_UpdataParaToRam(S19_FILE_INFOR, gby_data_bak1);
            EE_SaveVarForCalib(BOTHEE, S19_FILE_INFOR);
         
            EE_LoadVar(S19_FILE_INFOR);
            SysConf_GetParaValueFromRam(S19_FILE_INFOR, gby_data1,&gby_length);
            
            for(i=0;i<8;i++)//check save result 
            {
                if( gby_data_bak1[i]!= gby_data1[i]) 
                {
                 requesPackNo=0xFFF0; //Download file error
                 break;
                }
                
                if(i==7) 
                {
                 requesPackNo=0xFFFF; //Download file finish
                 return; 
                }
            }
         }  
     }
   }
}
/* main call back */
void GPRS_MainCbk(void)
{
    byte i=0;
    int num;
      
   SaveToFlash();  //保存到45db
   
   if(AppGPRS_cnt1>=10000) //超时重新开始
   {
     //CPUCSI_PutVal(FALSE);
     GPRSdelay_times(100);
    // GPRS_Step=GETIMEI;
     AppGPRS_cnt4=0;
      GPRS_Step=REPOWER;
     AppGPRS_cnt1=0;
     //CPUCSI_PutVal(TRUE);

   }

    if(AppGPRS_cnt2>=80)
   {  
      
       ClockIf_Init();
       ClockIf_ReadTime(&time);
       AppGPRS_cnt2=0;

   }
   
    if(GPRS_Step==REPOWER) 
   {
      if((AppGPRS_cnt4>=0) &&  (AppGPRS_cnt4<=100))
       {
        AppGPRS_cnt4=100;
        CPUCSI_PutVal(FALSE);    //POWERDOWN
      }
      
      if((AppGPRS_cnt4>=200) &&  (AppGPRS_cnt4<=300))
      {
         AppGPRS_cnt4=300;
        CPUCSI_PutVal(TRUE);
      } 
      
      if((AppGPRS_cnt4>=600) &&  (AppGPRS_cnt4<=700))
      {
        AppGPRS_cnt4=700;
        CPUCSI_PutVal(FALSE);  
      }
      
      if(AppGPRS_cnt4>=860)
      {
        CPUCSI_PutVal(TRUE); 
        
        GPRS_Step=GETIMEI ;
      }
   }
     
   if(AppGPRS_cnt>DelayTimes) 
   {

     if(GPRS_Step==GETIMEI) 
     {
     
       Send_Chars("ATE1\r");
       // Send_Chars("AT\r");
       //Send_Chars("ATE1\r");  
       // Send_Chars("AT&D1\r");
       Send_Chars("AT+CGSN\r");
       AppGPRS_cnt=0;
     } 
     else if(GPRS_Step==INIT) 
     {
       GPRS_Connect();              //1 GPRS Module Init
       AppGPRS_cnt=0;
     }
     else if((GPRS_Step ==SEND_MSG)||(GPRS_Step ==DOWNFILE)||(GPRS_Step ==UPDATE)||(GPRS_Step ==CHECKFILE)||(GPRS_Step ==CONFIG)||(GPRS_Step ==READ)   )
     { 
       if(((sendMsgOk==0xAA) ||  (AppGPRS_cnt>1200)) &&(enable_sendmsg!=0xAA))
       {  
          load_GPRS_data();            //2 Load BMS msg to buffer
          AppGPRS_cnt=0;
          sendMsgOk=0;
          enable_sendmsg=0;
          //WdgIf_Trig();
          Clear_Rvbuff(); 
          //memset(GPRS_rvbuff,0,sizeof( GPRS_rvbuff));
          Send_Bytes(GPRS_cmd,15);  //3 Tell GPRS Module start to send Msg
          
          if(GPRS_Step ==CHECKFILE) 
          {
            GPRS_Step=SEND_MSG;
          }
          
       }
       if((enable_sendmsg==0xAA) ||  (AppGPRS_cnt3>1500))  //加延时防止没有收到命令
       {
          GprsLife++;
          AppGPRS_cnt3=0;
          //WdgIf_Trig(); 
          Clear_Rvbuff();
          //memset(GPRS_rvbuff,0,sizeof( GPRS_rvbuff));
          Send_BmsInfo();           //4 Tell GPRS Send Msg to Server
          enable_sendmsg=0;
       }  
     }
   /*  else if(GPRS_Step==DOWNFILE)  
     { 
     
     }
     else if(GPRS_Step==UPDATE)  
     { 
     
     }
   */  
   }
}

void GPRS_RxApp(void)
{   char *p;
    Byte dat_read,btmp;
    int i;
    Word wtmp; 
     dword rows=0;
    AM2_GPRS_RecvChar(&dat_read);
   
    
    cnt_recv_bytes++;
    
    if(cnt_recv_bytes%400==0) 
    {
       WdgIf_Trig(); 
    }

    for(i=1;i<44;i++) 
    {
       GPRS_rvbuff[i-1]=GPRS_rvbuff[i];
    }
    
    GPRS_rvbuff[43] = dat_read;

     if( GPRS_Step==GETIMEI) 
     {
          btmp=0;
          for(i=0;i<44;i++) 
          {
            if((GPRS_rvbuff[i]>=0x30) && (GPRS_rvbuff[i]<=0x39)) 
            {
               IMEI[btmp++]=GPRS_rvbuff[i];
               if(btmp>=15) 
               {
                GPRS_Step= INIT;
               }
            } 
            else 
            {
              btmp=0;
            }
          }
          return;

     }
      
     else if(GPRS_Step ==INIT) 
     {
      
      if((GPRS_rvbuff[34]=='C') &&  (GPRS_rvbuff[35]=='O') &&  (GPRS_rvbuff[36]=='N')&&  (GPRS_rvbuff[37]=='N')
      &&  (GPRS_rvbuff[38]=='E')&&  (GPRS_rvbuff[39]=='C')&&  (GPRS_rvbuff[40]=='T')&&  (GPRS_rvbuff[42]=='O')&&  (GPRS_rvbuff[43]=='K'))
       {
          GPRS_Step= SEND_MSG;
      }
      
      /*
        p=strstr((const char *)GPRS_rvbuff,"CONNECT OK");//strstr(GPRS_rvbuff,"AT+CGCLASS");
        if(p!=NULL) 
        {
          GPRS_Step= SEND_MSG;
          return;
        }
     */
     
     if((GPRS_rvbuff[29]=='A') &&  (GPRS_rvbuff[30]=='L') &&  (GPRS_rvbuff[31]=='R')&&  (GPRS_rvbuff[32]=='E')
      &&  (GPRS_rvbuff[33]=='A')&&  (GPRS_rvbuff[34]=='D')&&  (GPRS_rvbuff[35]=='Y')
      &&  (GPRS_rvbuff[37]=='C')&&  (GPRS_rvbuff[38]=='O')&&  (GPRS_rvbuff[39]=='N')
      &&  (GPRS_rvbuff[40]=='N')&&  (GPRS_rvbuff[41]=='E')&&  (GPRS_rvbuff[42]=='C')&&  (GPRS_rvbuff[43]=='T'))
       {
          GPRS_Step= SEND_MSG;
      }
      
        /*
        p=strstr((const char *)GPRS_rvbuff,"ALREADY CONNECT");//strstr(GPRS_rvbuff,"AT+CGCLASS");
        if(p!=NULL) 
        {
          GPRS_Step= SEND_MSG;
          
          return;
        }  */
     }    
     else if((GPRS_Step ==SEND_MSG)||(GPRS_Step ==DOWNFILE)||(GPRS_Step ==UPDATE)||(GPRS_Step ==CONFIG)||(GPRS_Step ==READ)  )
    {
        if((GPRS_rvbuff[0]=='#') && (GPRS_rvbuff[1]=='&')&& (GPRS_rvbuff[42]=='@')&& (GPRS_rvbuff[43]=='$')) 
        {
          rows=(GPRS_rvbuff[2]<<8)+GPRS_rvbuff[3];
        
          if((rows<requesPackNo*48+1)&&(rows>(requesPackNo-1)*48)) 
          {
             AppGPRS_cnt=0; 
             AppGPRS_cnt3=0;
             
             if(startToSave!=1) 
             {
               //临时存储的位置
               startToSave=SaveToTempArray(rows);
             }
          }
          return;
        }
        
        if((GPRS_rvbuff[0]=='&') && (GPRS_rvbuff[1]=='&')&& (GPRS_rvbuff[42]=='@')&& (GPRS_rvbuff[43]=='@')) 
        { //参数设置命令
        
          if(FlagConfig!=TRUE) 
          {
            for(i=0;i<40;i++) 
            {
             ConfigData[i]=GPRS_rvbuff[2+i];
            }
            ConfigResult=0;
            GPRS_Step = CONFIG; 
            FlagConfig = TRUE;
          }
          return;
        }
        
        
        
        if((GPRS_rvbuff[35]=='$') && (GPRS_rvbuff[36]=='$')&& (GPRS_rvbuff[37]=='R')&& (GPRS_rvbuff[38]=='E')
        &&(GPRS_rvbuff[39]=='A') && (GPRS_rvbuff[40]=='D')&& (GPRS_rvbuff[42]=='#')&& (GPRS_rvbuff[43]=='#')) 
        { //参数读取命令
        
          if(FlagRead!=TRUE) 
          {
            readCmd=GPRS_rvbuff[41];
            ConfigResult=0;
            GPRS_Step = READ; 
            FlagRead = TRUE;
          }
          return;
        }
                
        
        if((GPRS_rvbuff[37]=='S') && (GPRS_rvbuff[38]=='E')&& (GPRS_rvbuff[39]=='N')&& (GPRS_rvbuff[40]=='D')&& (GPRS_rvbuff[41]==32)&& (GPRS_rvbuff[42]=='O')&& (GPRS_rvbuff[43]=='K')) 
        {
          sendMsgOk=0xAA;
          //WdgIf_Trig();
          Clear_Rvbuff();
          //memset(GPRS_rvbuff,0,sizeof( GPRS_rvbuff));
          AppGPRS_cnt1=0;
          return;
        }
        
        if((GPRS_rvbuff[32]=='S') && (GPRS_rvbuff[33]=='E')&& (GPRS_rvbuff[34]=='N')&& (GPRS_rvbuff[35]=='D')&& (GPRS_rvbuff[36]=='=')&& (GPRS_rvbuff[43]=='>'))  
        {  
         //WdgIf_Trig();
         Clear_Rvbuff();
          enable_sendmsg=0xAA;
          //AppGPRS_cnt1=0;
          return;
        }
       
       //Enter DownLoad File Mode 
      if((GPRS_rvbuff[29]=='d') && (GPRS_rvbuff[30]=='o')&& (GPRS_rvbuff[31]=='w')&& (GPRS_rvbuff[32]=='n')&& (GPRS_rvbuff[33]=='m')&& (GPRS_rvbuff[34]=='o')&& (GPRS_rvbuff[35]=='d')&& (GPRS_rvbuff[36]=='e')&& (GPRS_rvbuff[41]=='e')&& (GPRS_rvbuff[42]=='n')&& (GPRS_rvbuff[43]=='d'))  
        {
          GPRS_Step= DOWNFILE;
          requesPackNo=1;
          
          S19Crc=(GPRS_rvbuff[37] <<8)+GPRS_rvbuff[38];
          sumRows=(GPRS_rvbuff[39] <<8)+GPRS_rvbuff[40];
          sumPacks=sumRows/48;
          
          if(sumRows%48!=0) 
          {
            sumPacks+=1;
          }
          
          DelayTimes=50;
          return;
        }
        
       /* p=strstr((const char *)GPRS_rvbuff,"downmode");
        if(p!=NULL) 
        {
          GPRS_Step= DOWNFILE;
          DelayTimes=50;
          return;
        } */
        
       //Enter Update Mode 
         if((GPRS_rvbuff[34]=='u') && (GPRS_rvbuff[35]=='p')&& (GPRS_rvbuff[36]=='d')&& (GPRS_rvbuff[37]=='a')&& (GPRS_rvbuff[38]=='t')&& (GPRS_rvbuff[39]=='e')&& (GPRS_rvbuff[40]=='m')&& (GPRS_rvbuff[41]=='o')&& (GPRS_rvbuff[42]=='d')&& (GPRS_rvbuff[43]=='e'))  
        {
          GPRS_Step= UPDATE;
          FlagUpdate=TRUE;
          return;
        }
        
       //Check File Mode
         if((GPRS_rvbuff[34]=='c') && (GPRS_rvbuff[35]=='h')&& (GPRS_rvbuff[36]=='e')&& (GPRS_rvbuff[37]=='c')&& (GPRS_rvbuff[38]=='k')&& (GPRS_rvbuff[39]=='f')&& (GPRS_rvbuff[40]=='m')&& (GPRS_rvbuff[41]=='o')&& (GPRS_rvbuff[42]=='d')&& (GPRS_rvbuff[43]=='e'))  
        {
          GPRS_Step= CHECKFILE;
          //FlagUpdate=TRUE;
          return;
        }
       /*
       
       
        p=strstr((const char *)GPRS_rvbuff,"updatemode");
        if(p!=NULL) 
        {
          GPRS_Step= UPDATE;
          
          return;
        }*/
        
        if((GPRS_rvbuff[33]=='m') && (GPRS_rvbuff[34]=='o')&& (GPRS_rvbuff[35]=='n')&& (GPRS_rvbuff[36]=='i')&& (GPRS_rvbuff[37]=='t')&& (GPRS_rvbuff[38]=='o')&& (GPRS_rvbuff[39]=='r')&& (GPRS_rvbuff[40]=='m')&& (GPRS_rvbuff[41]=='o')&& (GPRS_rvbuff[42]=='d')&& (GPRS_rvbuff[43]=='e'))  
        {
          GPRS_Step= SEND_MSG;
          
          return;
        }
        /*
        p=strstr((const char *)GPRS_rvbuff,"monitormode");
        if(p!=NULL) 
        {
          GPRS_Step= SEND_MSG;
          
          return;
        }  */
        

    }

}


 