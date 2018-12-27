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


uByte CH376_ttrbuff[100];	//485���ڽ��ջ���
uByte History[1400];	//485���ڽ��ջ���
uByte CH376_trnum;			  //485���ڷ��ͼ�����         CH376_trbuff
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
     while(flagSendOk!=0xaa)   //���ӳ�ʱ���ܷ�ֹ����
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


//�ļ����½����߱����´�ʱ����ǰ�ļ�ָ�붼Ϊ 0���ƶ���ǰ�ļ�ָ�룬ͨ�����ڴ�ָ��λ��
//��ʼ��д���ݡ����磬��Ƭ��ϣ�������ļ���ǰ 158 �ֽ��ٶ�д���ݣ���ô������ CMD_BYTE_LOCATE
//������ϲ��� 158 ��Ϊƫ���ֽ���������ִ�гɹ��󣬽����ں���Ķ�д�������ӵ� 158 �ֽڿ�ʼ��
//����д�����������Ƭ��׼����ԭ�ļ���β������������ݣ�����ϣ��Ӱ��ǰ���ԭ�����ݣ���ô��
//��ָ���ܴ���ֽ�ƫ�ƣ����� 0FFFFFFFFH�����ļ�ָ���ƶ�ԭ�ļ���ĩβ���Ա�׷�����ݡ�
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
    CH376_ttrbuff[p++]= 0x2F; //���ý�Ҫ�������ļ����ļ���
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
    CH376_ttrbuff[p++]= 0x2F; //���ý�Ҫ�������ļ����ļ���
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
         
         

//�����������½���Ŀ¼���ļ��У����򿪣������Ŀ¼�Ѿ�������ôֱ�Ӵ򿪣�ֻ֧�ֵ�һ����
//Ŀ¼���ο� EXAM9 ���ӿ���֧���½��༶��Ŀ¼��
//���½���Ŀ¼����֮ǰ��Ӧ����ͨ�� CMD_SET_FILE_NAME �������ý�Ҫ�½�����Ŀ¼��Ŀ¼����
//��ʽ�� CMD_FILE_CREATE ������ͬ         
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
 
 
// CMD_BYTE_WRITE �����������ֽ�Ϊ��λ��ǰλ��д�����ݿ飬CMD_BYTE_WR_GO �������ڼ���
//�ֽ�д������д��ɹ���CH376 �Զ�ͬ���ƶ��ļ�ָ�룬�Ա��´ζ�д�����ܹ����ű���д������
//�Ľ���λ�ÿ�ʼ�� ��������Ҫ��������д����ֽ����� �������ֽ������Ե��ֽ���ǰ ��Little-Endian��
//�� 2 ���ֽڱ�ʾ�������ݣ�16 λ�����������ֽ���Ϊ 0 ʱ��������ˢ���ļ�����     
//һ���������ֽ�д����ͨ����һ�� CMD_BYTE_WRITE ���������������������ɴ��ж�֪ͨ������
//�����ݿ�д������ɸ� CMD_BYTE_WR_GO �������  
 void CH376_WRITE_BYTE(void)
 {
    int p=0;
     delay_times(10000);
    CH376_ttrbuff[p++]= 0x57;
    CH376_ttrbuff[p++]= 0xAB;
    CH376_ttrbuff[p++]= 0x3C;
    CH376_ttrbuff[p++]= 0x78;  // BYTE_WRITE �����ֽ���(2)
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
    CH376_ttrbuff[p++]= 0x0;  // BYTE_WRITE �����ֽ���(2)
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
  
  
// �������ڹرյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼���ļ��У�����������Ҫ 1 ���������ݣ������Ƿ�
//��������ļ����ȣ�Ϊ 0 ���ֹ�����ļ����ȣ�Ϊ 1 �������Զ������ļ����ȡ�
//���ļ�����Ŀ¼���ļ��У���д��Ϻ�Ӧ�ùر��ļ�       
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

   if((GPRS_Step !=DOWNFILE)&&(GPRS_Step !=UPDATE))//�������ļ��͸��³���ʱ��ͣ���ݴ洢���� 
  {
    
   if(AppCH376_cnt3>20000)//��ʱ���¿�ʼ 
   {
      AppCH376_cnt3=0;
      ch376_Step= CHECK_EXIST;
      
      CH376_CLOSE_FILE();
   }
   
   if((AppCH376_cnt>MaxDelay)&&(AppCH376_cnt2>600))   //��ʱ3�뿪ʼ����
   {
       
       AppCH376_cnt=0;

      //�� CH376 �����ڴ���ͨѶ��ʽʱ����λ��Ĭ�ϵ�ͨѶ
      //�������� BZ/D4��SCK/D5��SDI/D6 �������ŵĵ�ƽ����趨���ο����ֲ�� 6.4 �ڣ�����Щ���Ŷ���
      //��ʱĬ���� 9600bps    
      if(ch376_Step== CHECK_EXIST) 
      {
         SendStartIndex=0;
         CH376_CHECK_EXIST(); //���������ڲ���ͨѶ�ӿں͹���״̬���Լ�� CH376 �Ƿ�������������������Ҫ���� 1 �����ݣ�
                              //�������������ݣ���� CH376 ������������ô CH376 ������������������ݵİ�λȡ��
      }
         
      if((CH376_rvbuff[19] ==0xAA)&&(ch376_Step ==CHECK_EXIST)) 
      {
          //CH376_rvbuff[1]=0;
          ch376_Step= SET_USB_MODE;
          CH376_SET_USB_MODE();// 03H ʱ�л��� SD ������ģʽ�����ڹ���ʹ�ȡ SD ���е��ļ�
          return;
      }
      
      if((CH376_rvbuff[19] ==0x51)&&(ch376_Step ==SET_USB_MODE)) 
      {
          //CH376_rvbuff[1]=0;
          ch376_Step= DISK_MOUNT;
          CH376_DISK_MOUNT();   //���������ڳ�ʼ�����̲����Դ����Ƿ�����������ӵ� USB �洢�豸���� SD ������ͨ��������
                                //���г�ʼ����Ȼ����ܽ����ļ���������Щ USB �洢�豸������Ҫ��γ�ʼ�����ܷ��ز����ɹ�״
                                //̬ USB_INT_SUCCESS�����⣬���ļ������Ĺ����У�������Ҳ������ʱ���ڲ��Դ����Ƿ������
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
          CH376_WR_REQ_DATA(); //�������������ڲ�ָ��������д�� CH376 ��������ݿ�
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
         //��ʼд��һ��
      }
      
      if(((CH376_rvbuff[19])!=0)&&(ch376_Step ==WR_REQ_DATA)) //����
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




 