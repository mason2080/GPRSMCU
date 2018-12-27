#include "CH376.h"
#include "CPU_RS485.h"
#include "CPU_RS485_DR.h"

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
#include "IntCan.h"
#include "stdlib.h"


#define GET_HIGH_BYTE(val) ((uByte)(val >> 8))
#define GET_LOW_BYTE(val)   ((uByte)(val))

#define GET_HH_WORD(val)  ((uByte) (0x30|((val >> 12)&0x0f)))
#define GET_HL_WORD(val)  ((uByte) (0x30|((val >> 8)&0x0f)))
#define GET_LH_WORD(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_WORD(val)  ((uByte) (0x30|(val & 0x0f)))

#define GET_LH_BYTE(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_BYTE(val)  ((uByte) (0x30|(val & 0x0f)))

uWord AppCH376_cnt,AppCH376_cnt1,AppCH376_cnt2,AppCH376_cnt3;
uByte CH376_rvnum;
uByte CH376_rvbuff[20];
uByte CH376_fullrv_flag;
uByte CH376_data_type;


uByte CH376_ttrbuff[100];	//485串口接收缓存
uByte History[1400];	//485串口接收缓存
uByte CH376_trnum;			  //485串口发送计数器         CH376_trbuff
uByte CH376_trlen;
uByte CH376_rvlen;
uByte CH376_tx_busy;
Byte  CH376_cmd[30];
Byte sendlen=0;
Byte SendIndex=0;
Byte flagSendOk=0;
Byte flagDeleteOldFile=0;

Byte MaxDelay=200;

uWord SendStartIndex=0;
uWord SendRows=0;

int pp=0;

byte tempYear,tempMonth,tempDay,tempHour,tempMinute,tempSecond;
CH376_Step ch376_Step;

 void delay_times( Word time) 
{
    while(time--) 
    {
       asm nop;
       asm nop;
       WdgIf_Trig();         
    }
}



void CH376_TxApp(void)//rs485_send_dat(void)
{

    flagSendOk=0xaa; 
  
  /*
  if(SendIndex<=sendlen) 
    {		
      AM1_SD_SendChar(CH376_ttrbuff[SendIndex++]);
			               
    }
    else
    {
 
    }*/  

}

void CH376_MainCountCbk(void)
{

   (AppCH376_cnt < 65535) ? (AppCH376_cnt++) : (AppCH376_cnt = 65535);
   (AppCH376_cnt1 < 65535) ? (AppCH376_cnt1++) : (AppCH376_cnt1 = 65535);
   (AppCH376_cnt2 < 65535) ? (AppCH376_cnt2++) : (AppCH376_cnt2 = 65535);
   (AppCH376_cnt3 < 65535) ? (AppCH376_cnt3++) : (AppCH376_cnt3 = 65535);
}

void SendChars_SD(unsigned char *str) 
{
   int i=0;
   int j=0;
   dword timesout=0;
   while(*str!='\0') 
   {
      timesout=0; 
     flagSendOk=0;
     AM1_SD_SendChar(*str);
     str++;
     while(flagSendOk!=0xaa)
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
     
        i++;
        if(i%200==0)
        {
            WdgIf_Trig(); 
        }  
   }
}


void SendBytes_SD(byte cmd[],uint length) 
{
    int i=0;
    
    int j=0;
    dword timesout=0;
    for(i=0;i< length;i++) 
    {
      timesout=0; 
     flagSendOk=0;
     AM1_SD_SendChar(cmd[i]);
    
     //AM1_SD_SendChar(CH376_ttrbuff[0]); 
     while(flagSendOk!=0xaa)
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
    

void SendBytes2_SD(byte cmd[],uint StartIndex,uint length) 
{
    int i=0;
    
    int j=0;
    
    dword timesout=0;
    
    byte endcmd[3];
    endcmd[0]= 0x57;
    endcmd[1]= 0xAB;
    endcmd[2]= 0x3d;
    
    for(i=0;i< length;i++) 
    {
     timesout=0; 
     flagSendOk=0;
     AM1_SD_SendChar(cmd[StartIndex+i]);
    
     //AM1_SD_SendChar(CH376_ttrbuff[0]); 
     while(flagSendOk!=0xaa)   //增加超时功能防止卡死
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
   
    for(i=0;i< 3;i++) 
    {
     timesout=0; 
     flagSendOk=0;
     AM1_SD_SendChar(endcmd[i]);
    
     //AM1_SD_SendChar(CH376_ttrbuff[0]); 
     while(flagSendOk!=0xaa)
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

void  CH376_CHECK_EXIST(void) 
{
    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x06;
    CH376_ttrbuff[p++]= 0x55;

    SendBytes_SD( CH376_ttrbuff,p);  
}

void CH376_SET_USB_MODE(void) 
{
    int  p=0;
    delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x15;
    CH376_ttrbuff[p++]= 0x03;
    SendBytes_SD( CH376_ttrbuff,p);
}

void CH376_DISK_CONNECT(void)
{
    int p=0;
    delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x30;
    //CH376_ttrbuff[p++]= 0x03;
    SendBytes_SD( CH376_ttrbuff,p); 
}

void CH376_DISK_MOUNT(void)
 {
    int p=0;
    delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x31;
    //CH376_ttrbuff[p++]= 0x03;
    SendBytes_SD( CH376_ttrbuff,p);
 }
 
 
void CH376_FILE_CREATE(void)
 {
    int p=0;
    
    delay_times(10000);
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
   // CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    CH376_ttrbuff[p++]= 0x30+time.hour/10;
    CH376_ttrbuff[p++]= 0x30+time.hour%10;
   // CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.minute/10;
    CH376_ttrbuff[p++]= 0x30+time.minute%10;
   // CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30;//+time.second/10;
    CH376_ttrbuff[p++]= 0x30+1;//+time.second%10;
    CH376_ttrbuff[p++]= '.';
    CH376_ttrbuff[p++]= 'C';
    CH376_ttrbuff[p++]= 'S';
    CH376_ttrbuff[p++]= 'V';  
    CH376_ttrbuff[p++]= '\0';
    
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x34; // FILE_CREATE
    SendBytes_SD( CH376_ttrbuff,p);

 }
 
void CH376_SET_DIR_NAME(void)
{
    int p=0;

    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    CH376_ttrbuff[p++]= 0x30+time.year/10;
    CH376_ttrbuff[p++]= 0x30+time.year%10;
    
    CH376_ttrbuff[p++]= 0x30+time.month/10;
    CH376_ttrbuff[p++]= 0x30+time.month%10;
    
    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    
    CH376_ttrbuff[p++]=  0;
    SendBytes_SD( CH376_ttrbuff,p); 
}

void CH376_DIR_OPEN(void)
{
    int p=0;
 
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    CH376_ttrbuff[p++]= 0x30+time.year/10;
    CH376_ttrbuff[p++]= 0x30+time.year%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.month/10;
    CH376_ttrbuff[p++]= 0x30+time.month%10;
   // CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    CH376_ttrbuff[p++]=  0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0d;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x64;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x32;
    SendBytes_SD( CH376_ttrbuff,p);
}


//文件被新建或者被重新打开时，当前文件指针都为 0，移动当前文件指针，通常用于从指定位置
//开始读写数据。例如，单片机希望跳过文件的前 158 字节再读写数据，那么可以用 CMD_BYTE_LOCATE
//命令带上参数 158 作为偏移字节数，命令执行成功后，紧接在后面的读写操作将从第 158 字节开始。
//对于写操作，如果单片机准备在原文件的尾部继续添加数据，而不希望影响前面的原有数据，那么可
//以指定很大的字节偏移，例如 0FFFFFFFFH，将文件指针移动原文件的末尾，以便追加数据。
void CH376_BYTE_LOCATE(void)
{

    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x39;
    
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x22;
    
    SendBytes_SD( CH376_ttrbuff,p);
}

  void CH376_SET_SIZE(void)
 {
    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0D;
    
    CH376_ttrbuff[p++]= 0x68;
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    CH376_ttrbuff[p++]= 0x0;
    
    SendBytes_SD( CH376_ttrbuff,p);
 }
 
   


  void CH376_FILE_DELETE1(void)
 {
    int p=0;
    
    byte i,j;
    
    for(i=0;i<1;i++) {
      for(j=0;j<60;j++) {
        
    p=0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    if(time.month>1) 
    {
      CH376_ttrbuff[p++]= 0x30+time.year/10;
      CH376_ttrbuff[p++]= 0x30+time.year%10;
    
      
      CH376_ttrbuff[p++]= 0x30+(time.month-1)/10;
      CH376_ttrbuff[p++]= 0x30+(time.month-1)%10;
    } 
    else
    {
      if(time.year>0) 
      {
        CH376_ttrbuff[p++]= 0x30+(time.year-1)/10;
        CH376_ttrbuff[p++]= 0x30+(time.year-1)%10;
      }
      CH376_ttrbuff[p++]= 0x30+1;
      CH376_ttrbuff[p++]= 0x30+2;
    }
    
    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    CH376_ttrbuff[p++]=  0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0d;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x64;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
   
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x32; //FILE_OPEN
    
 
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F; //设置将要操作的文件的文件名
    CH376_ttrbuff[p++]= 0x30+i/10;
    CH376_ttrbuff[p++]= 0x30+i%10;
    CH376_ttrbuff[p++]= 0x30+j/10;
    CH376_ttrbuff[p++]= 0x30+j%10;
    CH376_ttrbuff[p++]= 0x30+0;//+time.second/10;
    CH376_ttrbuff[p++]= 0x30+1;//+time.second%10;
    CH376_ttrbuff[p++]= '.';
    CH376_ttrbuff[p++]= 'C';
    CH376_ttrbuff[p++]= 'S';
    CH376_ttrbuff[p++]= 'V';  
    CH376_ttrbuff[p++]= 0;
   

    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x35;
    
    SendBytes_SD( CH376_ttrbuff,p);
      }
    }
 }
 
   void CH376_FILE_DELETE2(void)
 {
    int p=0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    
    /*
    if(time.month>1) 
    {
      CH376_ttrbuff[p++]= 0x30+time.year/10;
      CH376_ttrbuff[p++]= 0x30+time.year%10;

      CH376_ttrbuff[p++]= 0x30+(time.month-1)/10;
      CH376_ttrbuff[p++]= 0x30+(time.month-1)%10;
    } 
    else
    {
     // if(time.year>0) 
     // {
        CH376_ttrbuff[p++]= 0x30+(time.year-1)/10;
        CH376_ttrbuff[p++]= 0x30+(time.year-1)%10;
     // }

      CH376_ttrbuff[p++]= 0x30+1;
      CH376_ttrbuff[p++]= 0x30+2;
    }

    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    */
    
    CH376_ttrbuff[p++]= 0x30+1;
    CH376_ttrbuff[p++]= 0x30+7;
    CH376_ttrbuff[p++]= 0x30+0;
    CH376_ttrbuff[p++]= 0x30+1;
    CH376_ttrbuff[p++]= 0x30+1;
    CH376_ttrbuff[p++]= 0x30+4;
    CH376_ttrbuff[p++]=  '\0';
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0d;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x64;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
  
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x32; //FILE_OPEN   
    
 
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= 0x30+0;
    CH376_ttrbuff[p++]= 0x30+9;
    CH376_ttrbuff[p++]= 0x30+5;
    CH376_ttrbuff[p++]= 0x30+0;
    CH376_ttrbuff[p++]= 0x30;//+time.second/10;
    CH376_ttrbuff[p++]= 0x30+1;//+time.second%10;
    CH376_ttrbuff[p++]= '.';
    CH376_ttrbuff[p++]= 'C';
    CH376_ttrbuff[p++]= 'S';
    CH376_ttrbuff[p++]= 'V';  
    CH376_ttrbuff[p++]= '\0';

    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x35;
    
    SendBytes_SD( CH376_ttrbuff,p);  
    
   //  delay_times(10000);           
 }
      
 void CH376_FILE_OPEN(void)
 {
    int p=0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    CH376_ttrbuff[p++]= 0x30+time.year/10;
    CH376_ttrbuff[p++]= 0x30+time.year%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.month/10;
    CH376_ttrbuff[p++]= 0x30+time.month%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    CH376_ttrbuff[p++]=  0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0d;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x64;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x32; //FILE_OPEN
    
 
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F; //设置将要操作的文件的文件名
    CH376_ttrbuff[p++]= 0x30+time.hour/10;
    CH376_ttrbuff[p++]= 0x30+time.hour%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.minute/10;
    CH376_ttrbuff[p++]= 0x30+time.minute%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30;//+time.second/10;
    CH376_ttrbuff[p++]= 0x30+1;//+time.second%10;
    CH376_ttrbuff[p++]= '.';
    CH376_ttrbuff[p++]= 'C';
    CH376_ttrbuff[p++]= 'S';
    CH376_ttrbuff[p++]= 'V';  
    CH376_ttrbuff[p++]= 0;
   
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x32;
    
    ///
    SendBytes_SD( CH376_ttrbuff,p);             
 }
         
         

//该命令用于新建子目录（文件夹）并打开，如果子目录已经存在那么直接打开，只支持第一级子
//目录，参考 EXAM9 例子可以支持新建多级子目录。
//在新建子目录命令之前，应该先通过 CMD_SET_FILE_NAME 命令设置将要新建的子目录的目录名，
//格式与 CMD_FILE_CREATE 命令相同         
void CH376_CREATE_DIR(void)
{
    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2F;
    CH376_ttrbuff[p++]= '/';
    CH376_ttrbuff[p++]= 0x30+time.year/10;
    CH376_ttrbuff[p++]= 0x30+time.year%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.month/10;
    CH376_ttrbuff[p++]= 0x30+time.month%10;
    //CH376_ttrbuff[p++]= '-';
    CH376_ttrbuff[p++]= 0x30+time.day/10;
    CH376_ttrbuff[p++]= 0x30+time.day%10;
    CH376_ttrbuff[p++]= 0;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x0d;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x64;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    CH376_ttrbuff[p++]= 0x00;
    
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x40;
    
    SendBytes_SD( CH376_ttrbuff,p);
}
 
 
// CMD_BYTE_WRITE 命令用于以字节为单位向当前位置写入数据块，CMD_BYTE_WR_GO 命令用于继续
//字节写操作。写入成功后，CH376 自动同步移动文件指针，以便下次读写操作能够接着本次写入数据
//的结束位置开始。 该命令需要输入请求写入的字节数， 该请求字节数是以低字节在前 （Little-Endian）
//的 2 个字节表示的字数据（16 位）。当请求字节数为 0 时，仅用于刷新文件长度     
//一次完整的字节写操作通常由一个 CMD_BYTE_WRITE 命令启动操作，并由若干次中断通知和若干
//次数据块写入和若干个 CMD_BYTE_WR_GO 命令组成  
 void CH376_WRITE_BYTE(void)
 {
    int p=0;
     delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x3C;
    CH376_ttrbuff[p++]= 0x78;  // BYTE_WRITE 请求字节数(2)
    CH376_ttrbuff[p++]= 0x05;

    SendBytes_SD( CH376_ttrbuff,p);
 }
 
  void UpdateLen(void)
 {
    int p=0;
     delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x3C;
    CH376_ttrbuff[p++]= 0x0;  // BYTE_WRITE 请求字节数(2)
    CH376_ttrbuff[p++]= 0x0;

    SendBytes_SD( CH376_ttrbuff,p);
 }
         
  void CH376_WR_REQ_DATA(void)
 {
    int p=0;
    delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x2d;
    SendBytes_SD( CH376_ttrbuff,p);
 }
         
         
void CH376_BYTE_WR_GO(void)
{

    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x3d;
    SendBytes_SD( CH376_ttrbuff,p);
}
  
  
// 命令用于关闭当前已经打开的文件或者目录（文件夹）。该命令需要 1 个输入数据，表明是否
//允许更新文件长度，为 0 则禁止更新文件长度，为 1 则允许自动更新文件长度。
//打开文件或者目录（文件夹）读写完毕后，应该关闭文件       
void CH376_CLOSE_FILE(void)
{
    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x36;
    CH376_ttrbuff[p++]= 1;
    SendBytes_SD( CH376_ttrbuff,p);
}
         
void CH376_GET_STATUS(void)
{

    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x22;
    SendBytes_SD( CH376_ttrbuff,p);
}

void CH376_READ_FILE_INFO(void)
{

    int p=0;
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x27;
    SendBytes_SD( CH376_ttrbuff,p);
}
/////////////////////////////////////////////////////////////////////////
void CH376_Init(void)
{
  ch376_Step=CHECK_EXIST;
}

void NumToString(Word num,Byte pos) 
{
  uint v1,v2;
  Byte i;

  Byte tempString[10];

  
  v1=num/10000;
  
  tempString[0]=v1+0x30;
  
  v2=num%10000;
  
  tempString[1]=v2/1000+0x30; ///
 
  v2=v2%1000;

  tempString[2]=v2/100+0x30; ///
  
  v2=v2%100;
  
  tempString[3]=v2/10+0x30; ///
  
  v2=v2%10;
  
  tempString[4]=v2+0x30; //
  

    if(pos==1) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]='.';
     History[pp++]=tempString[4];
    } 
    else if(pos==2) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]='.';
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==3) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]='.';
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==4) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]='.';
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==0) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
   History[pp++]=',';
}

void DWordToString(dword num,Byte pos) 
{
  dword v1,v2;
  Byte i;

  Byte tempString[10];

  v1=num/1000000000; 
  tempString[0]=v1+0x30;
  v2=num%1000000000;
  tempString[1]=v2/100000000+0x30;
  v2=num%100000000;
  tempString[2]=v2/10000000+0x30;
  v2=num%10000000;
  tempString[3]=v2/1000000+0x30;
  v2=num%1000000;
  tempString[4]=v2/100000+0x30;
  v2=num%100000;
  tempString[5]=v2/10000+0x30;
  v2=num%10000;
  tempString[6]=v2/1000+0x30;  
  v2=v2%1000;
  tempString[7]=v2/100+0x30; ///
  v2=v2%100;
  tempString[8]=v2/10+0x30; ///
  v2=v2%10;
  tempString[9]=v2+0x30; //
  

    if(pos==1) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
     History[pp++]=tempString[5];
     History[pp++]=tempString[6];
     History[pp++]=tempString[7];
     History[pp++]=tempString[8];    
     History[pp++]='.';
     History[pp++]=tempString[9];
    } 
    else if(pos==2) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
     History[pp++]=tempString[5];
     History[pp++]=tempString[6];
     History[pp++]=tempString[7];
     History[pp++]='.';
     History[pp++]=tempString[8];    
     History[pp++]=tempString[9];
    }
    else if(pos==3) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
     History[pp++]=tempString[5];
     History[pp++]=tempString[6];
     History[pp++]='.';
     History[pp++]=tempString[7];
     History[pp++]=tempString[8];    
     History[pp++]=tempString[9];
    }
    else if(pos==0) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
     History[pp++]=tempString[5];
     History[pp++]=tempString[6];
     History[pp++]=tempString[7];
     History[pp++]=tempString[8];    
     History[pp++]=tempString[9];
    }
   History[pp++]=',';
}


void IntNumToString(Word num,Byte pos,Word Offset) 
{
  uint v1,v2;
  Byte i;

  Byte tempString[10];
  
  if(num<Offset)
  {
     History[pp++]='-';
     num=Offset-num;
  }
  else
  {
     num=num-Offset;
  }

  
  v1=num/10000;
  
  tempString[0]=v1+0x30;
  
  v2=num%10000;
  
  tempString[1]=v2/1000+0x30; ///
 
  v2=v2%1000;

  tempString[2]=v2/100+0x30; ///
  
  v2=v2%100;
  
  tempString[3]=v2/10+0x30; ///
  
  v2=v2%10;
  
  tempString[4]=v2+0x30; //
  

    if(pos==1) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]='.';
     History[pp++]=tempString[4];
    } 
    else if(pos==2) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]='.';
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==3) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]='.';
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==4) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]='.';
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }
    else if(pos==0) 
    {
     History[pp++]=tempString[0]; 
     History[pp++]=tempString[1];
     History[pp++]=tempString[2];
     History[pp++]=tempString[3];
     History[pp++]=tempString[4];
    }

  
  
  
  History[pp++]=',';


}




void load_CH376_data(void) 
{  
    int p=0;
    int i=0;
    int j=0;
    
    byte temp=0;
    
    pp=0;
    memset(History,0,sizeof( History));
    
   
    if(SendRows==0)//Create Title 
    {
      byte i,j;
      unsigned char *title="datetime,bmstime,Life,sumv,extsumv,curr,avgcurr,soc,soh,maxv,maxvno,minv,minvno,maxt,maxtno,mint,mintno,totalin,totalin,POS,NEG,PRE,CHG,FAN,HEAT,baterr,batsyserr,hderr,othererr,";
      
      
      
      for( i=0;i<strlen(title);i++) 
      {
        History[pp++]=*(title+i);
      }
      
      for(i=0;i<sbms_Info.bmu_num;i++) 
      {
      
         if(pp>1180) 
         {
           break;
         }
           
         for(j=0;j<sbms_Info.bmu_volt_num[i+1];j++) 
         {
           History[pp++]='V';
           History[pp++]=(i+1)/10+0x30;
           History[pp++]=(i+1)%10+0x30;
           History[pp++]='-';
           History[pp++]=(j+1)/10+0x30;
           History[pp++]=(j+1)%10+0x30;
           History[pp++]=',';
           if(pp>1380) 
           {
             break;
           }
         }
         
         for(j=0;j<sbms_Info.bmu_temp_num[i+1];j++) 
         {
           History[pp++]='T';
           History[pp++]=(i+1)/10+0x30;
           History[pp++]=(i+1)%10+0x30;
           History[pp++]='-';
           History[pp++]=(j+1)/10+0x30;
           History[pp++]=(j+1)%10+0x30;
           History[pp++]=',';
           
            if(pp>1380) 
           {
             break;
           }
         }
         
      }
            
      History[pp++]='\n';
    } 
    else//Load real time data
    {
      History[pp++]= 0x30+time.year/10;
      History[pp++]= 0x30+time.year%10;
      History[pp++]= '-';
      History[pp++]= 0x30+time.month/10;
      History[pp++]= 0x30+time.month%10;
      History[pp++]= '-';
      History[pp++]= 0x30+time.day/10;
      History[pp++]= 0x30+time.day%10;
      History[pp++]= ' ';
      History[pp++]= 0x30+time.hour/10;
      History[pp++]= 0x30+time.hour%10;
      History[pp++]= '-';
      History[pp++]= 0x30+time.minute/10;
      History[pp++]= 0x30+time.minute%10;
      History[pp++]= '-';
      History[pp++]= 0x30+time.second/10;
      History[pp++]= 0x30+time.second%10;
      
      History[pp++]= ','; ///////////////
      
      History[pp++]= 0x30+sbms_Info.bms_time[0]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[0]%10;
      History[pp++]= '-';
      History[pp++]= 0x30+sbms_Info.bms_time[1]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[1]%10;
      History[pp++]= '-';
      History[pp++]= 0x30+sbms_Info.bms_time[2]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[2]%10;
      History[pp++]= ' ';
      History[pp++]= 0x30+sbms_Info.bms_time[3]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[3]%10;
      History[pp++]= '-';
      History[pp++]= 0x30+sbms_Info.bms_time[4]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[4]%10;
      History[pp++]= '-';
      History[pp++]= 0x30+sbms_Info.bms_time[5]/10;
      History[pp++]= 0x30+sbms_Info.bms_time[5]%10;
      
      History[pp++]= ','; ///////////////
      
      NumToString(sbms_Info.bms_life,0);
      NumToString(sbms_Info.sumv,1);
      NumToString(sbms_Info.sumvOut1,1);
      IntNumToString(sbms_Info.curr,1,30000);
      IntNumToString(sbms_Info.avgCurr,1,30000);
      NumToString(sbms_Info.soc,1);
      NumToString(sbms_Info.soh,1);
      NumToString(sbms_Info.maxv,3);
      NumToString(sbms_Info.maxv_no,0);
      NumToString(sbms_Info.minv,3);
      NumToString(sbms_Info.minv_no,0);
      IntNumToString(sbms_Info.maxt,0,40);
      NumToString(sbms_Info.maxt_no,0);
      IntNumToString(sbms_Info.mint,0,40);
      NumToString(sbms_Info.mint_no,0);
      
      DWordToString(sbms_Info.inah,3);
      DWordToString(sbms_Info.outah,3);
      
      
      if(((sbms_Info.io_state[0] & 0xc0) >> 6) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }
      
      if(((sbms_Info.io_state[0] & 0x30) >> 4) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }
      
      if(((sbms_Info.io_state[0] & 0x0c) >> 2) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }      

      if((sbms_Info.io_state[0] & 0x03) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }

      if(((sbms_Info.io_state[1] & 0xc0) >> 6) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }
      
      if(((sbms_Info.io_state[1] & 0x30) >> 4) == 1) 
      {
        History[pp++]='O';
        History[pp++]='N';
        History[pp++]= ',';
      }
      else
      {
        History[pp++]='O';
        History[pp++]='F';
        History[pp++]='F';
        History[pp++]= ',';
      }            
 
      for(i=0;i<8;i++) 
      {
        temp=sbms_Info.bat_err[i]/16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.bat_err[i]/16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.bat_err[i]/16-10;
        }
        
        
        temp=sbms_Info.bat_err[i]%16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.bat_err[i]%16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.bat_err[i]%16-10;
        }
        
      }  
      
      History[pp++]= ','; ///////////////
      
       for(i=0;i<8;i++) 
      {
        
        temp=sbms_Info.batSys_err[i]/16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.batSys_err[i]/16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.batSys_err[i]/16-10;
        }
        
        
        temp=sbms_Info.bat_err[i]%16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.batSys_err[i]%16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.batSys_err[i]%16-10;
        }
      }  
      
      History[pp++]= ','; ///////////////
      
       for(i=0;i<8;i++) 
      {
        
        temp=sbms_Info.hard_err[i]/16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.hard_err[i]/16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.hard_err[i]/16-10;
        }
        
        
        temp=sbms_Info.bat_err[i]%16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.hard_err[i]%16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.hard_err[i]%16-10;
        }
      }  
      
        History[pp++]= ','; ///////////////
      
           for(i=0;i<8;i++) 
      {
        
        temp=sbms_Info.other_err[i]/16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.other_err[i]/16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.other_err[i]/16;
        }
        
        
        temp=sbms_Info.bat_err[i]%16;
        
        if(temp<10) 
        {
          History[pp++]= '0'+sbms_Info.other_err[i]%16;
        } 
        else
        {
          History[pp++]= 'A'+sbms_Info.other_err[i]%16;
        }
      }  
      
      History[pp++]= ','; ///////////////
      

      
      for(i=0;i<sbms_Info.bmu_num;i++) 
      {
      
              if(pp>1380) 
           {
             break;
           }
           
         for(j=0;j<sbms_Info.bmu_volt_num[i+1];j++) 
         {
         
           NumToString(Bat_GetCellV(i+1,j+1),3);
           
           if(pp>1380) 
           {
             break;
           }
         }
         
         for(j=0;j<sbms_Info.bmu_temp_num[i+1];j++) 
         {
           IntNumToString(Bat_GetTemper(i+1,j+1),0,40);
           
           if(pp>1380) 
           {
             break;
           }
         }
      }
       History[pp++]='\r';
    }
}



/* main call back */
void CH376_MainCbk(void)
{

   if((GPRS_Step !=DOWNFILE)&&(GPRS_Step !=UPDATE))//在下载文件和更新程序时暂停数据存储功能 
  {
    
   if(AppCH376_cnt3>20000)//超时重新开始 
   {
      AppCH376_cnt3=0;
      ch376_Step= CHECK_EXIST;
      
      CH376_CLOSE_FILE();
   }
   
   if((AppCH376_cnt>MaxDelay)&&(AppCH376_cnt2>600))   //延时3秒开始保存
   {
       
       AppCH376_cnt=0;

      //在 CH376 工作于串口通讯方式时，复位后默认的通讯
      //波特率由 BZ/D4，SCK/D5，SDI/D6 三个引脚的电平组合设定（参考本手册第 6.4 节），这些引脚都悬
      //空时默认是 9600bps    
      if(ch376_Step== CHECK_EXIST) 
      {
         SendStartIndex=0;
         CH376_CHECK_EXIST(); //该命令用于测试通讯接口和工作状态，以检查 CH376 是否正常工作。该命令需要输入 1 个数据，
                              //可以是任意数据，如果 CH376 正常工作，那么 CH376 的输出数据是输入数据的按位取反
      }
         
      if((CH376_rvbuff[19] ==0xAA)&&(ch376_Step ==CHECK_EXIST)) 
      {
          //CH376_rvbuff[1]=0;
          ch376_Step= SET_USB_MODE;
          CH376_SET_USB_MODE();// 03H 时切换到 SD 卡主机模式，用于管理和存取 SD 卡中的文件
          return;
      }
      
      if((CH376_rvbuff[19] ==0x51)&&(ch376_Step ==SET_USB_MODE)) 
      {
          //CH376_rvbuff[1]=0;
          ch376_Step= DISK_MOUNT;
          CH376_DISK_MOUNT();   //该命令用于初始化磁盘并测试磁盘是否就绪。新连接的 USB 存储设备或者 SD 卡必须通过该命令
                                //进行初始化后，然后才能进行文件操作，有些 USB 存储设备可能需要多次初始化才能返回操作成功状
                                //态 USB_INT_SUCCESS。另外，在文件操作的过程中，该命令也可以随时用于测试磁盘是否就绪。
          return;
      }
         
         
      if(((CH376_rvbuff[19] ==0x14))&&(ch376_Step ==DISK_MOUNT)) 
      {
         CH376_FILE_DELETE1();
         CH376_CLOSE_FILE();
         // CH376_rvbuff[1]=0;
           SendRows=0;
         ch376_Step= CREATE_DIR;
         CH376_CREATE_DIR();
         return;
          
      } else if((CH376_rvbuff[19] !=0x14)&&(ch376_Step ==DISK_MOUNT)) 
      {
      
          CH376_DISK_MOUNT();   
          return;
      }
      
  
      if((CH376_rvbuff[19] ==0x14)&&(ch376_Step ==CREATE_DIR)) 
      {
      
          //CH376_rvbuff[1]=0;

          ch376_Step= DIR_OPEN;
          CH376_DIR_OPEN();
          return;
      }    
     
      if((CH376_rvbuff[19] ==0x41)&&(ch376_Step ==DIR_OPEN)) 
      {
      
          //CH376_rvbuff[1]=0;
          ch376_Step= FILE_CREATE;
          CH376_FILE_CREATE();
          return;
      }  
      ////////////////////////////////////////////////////////////////////////////////        
      if((CH376_rvbuff[19] ==0x14)&&(ch376_Step ==FILE_CREATE))//BYTE_LOCATE)) 
      {
          if((AppCH376_cnt1>100)&&(time.second!=tempSecond)) 
          {
            load_CH376_data();
           // CH376_rvbuff[1]=0;
            AppCH376_cnt1=0;
            ch376_Step= WRITE_BYTE;
            
            SendRows++;
            
            if((SendRows%10==0)&&(SendRows!=0) )
            {
               UpdateLen();
            }
            
            CH376_WRITE_BYTE();
            

          }
          
          tempSecond=time.second;
          return;
      }
      
      if((CH376_rvbuff[19] ==0x1E)&&(ch376_Step ==WRITE_BYTE)) 
      {
          //CH376_rvbuff[1]=0;
          
          MaxDelay=5;
          ch376_Step= WR_REQ_DATA;
          CH376_WR_REQ_DATA(); //该命令用于向内部指定缓冲区写入 CH376 请求的数据块
          return;
      } 
      else if((CH376_rvbuff[19] ==0x14)&&(ch376_Step ==WRITE_BYTE)) 
      {
         //CH376_rvbuff[1]=0;
         //ch376_Step= SaveData;
         SendStartIndex=0;
         ch376_Step =WRITE_BYTE;
         //CH376_WRITE_BYTE();
         
          if((AppCH376_cnt1>100)&&(time.second!=tempSecond)) 
          {
            load_CH376_data();
            AppCH376_cnt1=0;
            
           SendRows++;
            
            if((SendRows%10==0)&&(SendRows!=0) )
            {
               UpdateLen();
            }
            
            
            CH376_WRITE_BYTE();
            

         
            if(SendRows==3600)
            {
             ch376_Step= SaveData;
            }
          }
          
          tempSecond=time.second;
         //开始写下一行
      }
      
      if(((CH376_rvbuff[19])!=0)&&(ch376_Step ==WR_REQ_DATA)) //发送
      {
          SendStartIndex+=CH376_rvbuff[19];
          
          ch376_Step= WRITE_BYTE;
          SendBytes2_SD(History,SendStartIndex-CH376_rvbuff[19],CH376_rvbuff[19]);
          
          AppCH376_cnt3=0;
          //CH376_rvbuff[1]=0;
          return;
      } 
      ///////////////////////////////////////////////////////////////////////////////////
      if(ch376_Step ==SaveData) 
      {
          //CH376_rvbuff[1]=0;
          ch376_Step= CLOSE_FILE;
          
          CH376_CLOSE_FILE();
          
           ch376_Step= DISK_MOUNT;
           
          CH376_rvbuff[19] =0x14;
          return;
      } 
      
      if((CH376_rvbuff[19] ==0x14)&&(ch376_Step ==CLOSE_FILE)) 
      {
         // CH376_rvbuff[1]=0;
          ch376_Step= Idle;
          
  
          return;
      }
     //  }
   }
   }
}

void CH376_RxApp(void)
{
    Byte dat_read,p,btmp;
    Word wtmp; 

    AM1_SD_RecvChar(&dat_read);

  	//CH376_rvbuff[0] = dat_read;
    CH376_rvbuff[0] = CH376_rvbuff[1];
    CH376_rvbuff[1] = CH376_rvbuff[2];
    CH376_rvbuff[2] = CH376_rvbuff[3];
    CH376_rvbuff[3] = CH376_rvbuff[4];
    CH376_rvbuff[4] = CH376_rvbuff[5];
    CH376_rvbuff[5] = CH376_rvbuff[6];
    CH376_rvbuff[6] = CH376_rvbuff[7];
    CH376_rvbuff[7] = CH376_rvbuff[8];
    CH376_rvbuff[8] = CH376_rvbuff[9];
    CH376_rvbuff[9] = CH376_rvbuff[10];
    CH376_rvbuff[10] = CH376_rvbuff[11];
    CH376_rvbuff[11] = CH376_rvbuff[12];
    CH376_rvbuff[12] = CH376_rvbuff[13];
    CH376_rvbuff[13] = CH376_rvbuff[14];
    CH376_rvbuff[14] = CH376_rvbuff[15];
    CH376_rvbuff[15] = CH376_rvbuff[16];
    CH376_rvbuff[16] = CH376_rvbuff[17];
    CH376_rvbuff[17] = CH376_rvbuff[18];
    CH376_rvbuff[18] = CH376_rvbuff[19];
    CH376_rvbuff[19] = dat_read;    
}




 