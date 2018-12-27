#include "GPS.h"
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
#include "AM3_GPS.h"
#include "gps1.h"


#define GET_HIGH_BYTE(val) ((uByte)(val >> 8))
#define GET_LOW_BYTE(val)   ((uByte)(val))

#define GET_HH_WORD(val)  ((uByte) (0x30|((val >> 12)&0x0f)))
#define GET_HL_WORD(val)  ((uByte) (0x30|((val >> 8)&0x0f)))
#define GET_LH_WORD(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_WORD(val)  ((uByte) (0x30|(val & 0x0f)))

#define GET_LH_BYTE(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_BYTE(val)  ((uByte) (0x30|(val & 0x0f)))

uWord AppGPS_cnt;
uByte GPS_rvnum;
uByte GPS_rvbuff[100];
uByte GPS_fullrv_flag;
uByte GPS_data_type;


uByte GPS_ttrbuff[200];	//485串口接收缓存
uByte GPS_trnum;			  //485串口发送计数器         GPS_trbuff
uByte GPS_trlen;
uByte GPS_rvlen;
uByte GPS_tx_busy;
Byte  GPS_cmd[30];
Byte GPSsendlen=0;
Byte GPSSendIndex=0;

nmea_msg gpsx;


//GPS_Step GPS_step;

 void GPSdelay_times( Word time) 
{
    while(time--) 
    {
       asm nop;
       asm nop;
       WdgIf_Trig();         
       
    }
}

void GPS_Init(void)
{
  //GPS_step=CHECK_EXIST;
}



void GPS_TxApp(void)//rs485_send_dat(void)
{

}

void GPS_MainCountCbk(void)
{

   (AppGPS_cnt < 65535) ? (AppGPS_cnt++) : (AppGPS_cnt = 65535);
   
}

void load_GPS_data(void) 
{  
      int p=0;
      
     // char *Title;
     // Title="124132132";
    
    /*  
      switch(GPS_step)
      {
        case Idle:
         {
            GPSSendIndex = 0;
            GPSsendlen=0;
            
            break;
         }
        case CHECK_EXIST:
         {
            p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x06;
            GPS_ttrbuff[p++]= 0x55;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         case SET_USB_MODE:
         {
            p=0;
            GPSdelay_times(10000);
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x15;
            GPS_ttrbuff[p++]= 0x03;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
          case DISK_CONNECT:
         {
            p=0;
            GPSdelay_times(10000);
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x30;
            //GPS_ttrbuff[p++]= 0x03;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
         case DISK_MOUNT:
         {
            p=0;
            GPSdelay_times(10000);
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x31;
            //GPS_ttrbuff[p++]= 0x03;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
         case FILE_CREATE:
         {
            p=0;
            
            GPSdelay_times(10000);
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
           // GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '/';
            GPS_ttrbuff[p++]= '1';
            GPS_ttrbuff[p++]= '6';
            GPS_ttrbuff[p++]= '_';
            GPS_ttrbuff[p++]= '1';
            GPS_ttrbuff[p++]= '0';
            GPS_ttrbuff[p++]= '_';
            GPS_ttrbuff[p++]= '0';
            GPS_ttrbuff[p++]= '8';
            GPS_ttrbuff[p++]= '.';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]= 'S';
            GPS_ttrbuff[p++]= 'V';  
            GPS_ttrbuff[p++]= '\0';
            
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x34;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
             
         case SET_DIR_NAME:
         {
         
            p=0;
         
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '/';
            GPS_ttrbuff[p++]= 'A';
            GPS_ttrbuff[p++]= 'B';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]=  0;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
            
         }
        case DIR_OPEN: 
        {
            p=0;
         
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '/';
            GPS_ttrbuff[p++]= 'A';
            GPS_ttrbuff[p++]= 'B';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]=  0;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x0d;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x64;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x32;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
        }
         
         case BYTE_LOCATE:
         {
         
             p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x39;
            
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x22;
            
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
          case SET_SIZE:
         {
         
             p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x0D;
            
            GPS_ttrbuff[p++]= 0x68;
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            GPS_ttrbuff[p++]= 0x0;
            
            
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }

         
         case FILE_OPEN:
         {
            p=0;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '/';
            GPS_ttrbuff[p++]= 'A';
            GPS_ttrbuff[p++]= 'B';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]=  0;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x0d;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x64;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x32;
            
         
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '1';
            GPS_ttrbuff[p++]= '6';
            GPS_ttrbuff[p++]= '_';
            GPS_ttrbuff[p++]= '1';
            GPS_ttrbuff[p++]= '0';
            GPS_ttrbuff[p++]= '_';
            GPS_ttrbuff[p++]= '0';
            GPS_ttrbuff[p++]= '8';
            GPS_ttrbuff[p++]= '.';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]= 'S';
            GPS_ttrbuff[p++]= 'V';  
            GPS_ttrbuff[p++]= 0;
           
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x32;
            
            ///
                        
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
         
         
          case CREATE_DIR:
         {
            p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2F;
            GPS_ttrbuff[p++]= '/';
            GPS_ttrbuff[p++]= 'A';
            GPS_ttrbuff[p++]= 'B';
            GPS_ttrbuff[p++]= 'C';
            GPS_ttrbuff[p++]= 0;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x0d;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x64;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            GPS_ttrbuff[p++]= 0x00;
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x40;
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
          case WRITE_BYTE:
         {
            p=0;

            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x3C;
            GPS_ttrbuff[p++]= 11;
            GPS_ttrbuff[p++]= 0;
            


            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
          case WR_REQ_DATA:
         {
            p=0;
            GPSdelay_times(10000);
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x2d;
            
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
         case SaveData:
         {
            p=0;
            GPSdelay_times(10000);
            GPS_ttrbuff[p++]= '1';
            GPS_ttrbuff[p++]= ',';
            GPS_ttrbuff[p++]= '2';
            GPS_ttrbuff[p++]= ',';
            GPS_ttrbuff[p++]= '3';
            GPS_ttrbuff[p++]= ',';
            GPS_ttrbuff[p++]= '4';
            GPS_ttrbuff[p++]= ',';
            GPS_ttrbuff[p++]= '5';
            GPS_ttrbuff[p++]= ',';
  
            
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x3d;  
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
         }
         
        case  BYTE_WR_GO: 
        {
        
            p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x3d;
 
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
        
        }
         
        case  CLOSE_FILE: 
        {
        
            p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x36;
            GPS_ttrbuff[p++]= 1;
 
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
        
        }
         
        case  GET_STATUS: 
        {
        
            p=0;
            GPS_ttrbuff[p++]= 0x57;
            GPS_ttrbuff[p++]= 0xAB;
            GPS_ttrbuff[p++]= 0x22;
 
            GPSsendlen=p;
            GPSSendIndex = 0;
            break;
        
        }
         

         
      } */
}



/* main call back */
void GPS_MainCbk(void)
{
   if(AppGPS_cnt>200) 
   {
   
       AppGPS_cnt=0;
       
       //load_GPS_data();
       
       //if(GPS_step!= Idle) 
       //{
         // AM3_GPS_SendChar(GPS_ttrbuff[GPSSendIndex++]);
      // }
   }
}

void GPS_RxApp(void)
{
    Byte dat_read,p,btmp;
    Word wtmp; 

    AM3_GPS_RecvChar(&dat_read);

    GPS_rvbuff[0] = GPS_rvbuff[1];
    GPS_rvbuff[1] = GPS_rvbuff[2];
    GPS_rvbuff[2] = GPS_rvbuff[3];
    GPS_rvbuff[3] = GPS_rvbuff[4];
    GPS_rvbuff[4] = GPS_rvbuff[5];
    GPS_rvbuff[5] = GPS_rvbuff[6];
    GPS_rvbuff[6] = GPS_rvbuff[7];
    GPS_rvbuff[7] = GPS_rvbuff[8];
    GPS_rvbuff[8] = GPS_rvbuff[9];
    GPS_rvbuff[9] = GPS_rvbuff[10];
    GPS_rvbuff[10] = GPS_rvbuff[11];
    GPS_rvbuff[11] = GPS_rvbuff[12];
    GPS_rvbuff[12] = GPS_rvbuff[13];
    GPS_rvbuff[13] = GPS_rvbuff[14];
    GPS_rvbuff[14] = GPS_rvbuff[15];
    GPS_rvbuff[15] = GPS_rvbuff[16];
    GPS_rvbuff[16] = GPS_rvbuff[17];
    GPS_rvbuff[17] = GPS_rvbuff[18];
    GPS_rvbuff[18] = GPS_rvbuff[19];
    GPS_rvbuff[19] = GPS_rvbuff[20];
    GPS_rvbuff[20] = GPS_rvbuff[21];
    GPS_rvbuff[21] = GPS_rvbuff[22];
    GPS_rvbuff[22] = GPS_rvbuff[23];
    GPS_rvbuff[23] = GPS_rvbuff[24];
    GPS_rvbuff[24] = GPS_rvbuff[25];
    GPS_rvbuff[25] = GPS_rvbuff[26];
    GPS_rvbuff[26] = GPS_rvbuff[27];
    GPS_rvbuff[27] = GPS_rvbuff[28];
    GPS_rvbuff[28] = GPS_rvbuff[29];
    GPS_rvbuff[29] = GPS_rvbuff[30];
    GPS_rvbuff[30] = GPS_rvbuff[31];
    GPS_rvbuff[31] = GPS_rvbuff[32];
    GPS_rvbuff[32] = GPS_rvbuff[33];
    GPS_rvbuff[33] = GPS_rvbuff[34];
    GPS_rvbuff[34] = GPS_rvbuff[35];
    GPS_rvbuff[35] = GPS_rvbuff[36];
    GPS_rvbuff[36] = GPS_rvbuff[37];
    GPS_rvbuff[37] = GPS_rvbuff[38];
    GPS_rvbuff[38] = GPS_rvbuff[39];
    GPS_rvbuff[39] = GPS_rvbuff[40];
    GPS_rvbuff[40] = GPS_rvbuff[41];
    GPS_rvbuff[41] = GPS_rvbuff[42];
    GPS_rvbuff[42] = GPS_rvbuff[43];
    GPS_rvbuff[43] = GPS_rvbuff[44];
    GPS_rvbuff[44] = GPS_rvbuff[45];
    GPS_rvbuff[45] = GPS_rvbuff[46];
    GPS_rvbuff[46] = GPS_rvbuff[47];
    GPS_rvbuff[47] = GPS_rvbuff[48];
    GPS_rvbuff[48] = GPS_rvbuff[49];
    GPS_rvbuff[49] = GPS_rvbuff[50];
    
    GPS_rvbuff[50] = GPS_rvbuff[51];
    GPS_rvbuff[51] = GPS_rvbuff[52];
    GPS_rvbuff[52] = GPS_rvbuff[53];
    GPS_rvbuff[53] = GPS_rvbuff[54];
    GPS_rvbuff[54] = GPS_rvbuff[55];
    GPS_rvbuff[55] = GPS_rvbuff[56];
    GPS_rvbuff[56] = GPS_rvbuff[57];
    GPS_rvbuff[57] = GPS_rvbuff[58];
    GPS_rvbuff[58] = GPS_rvbuff[59];
    GPS_rvbuff[59] = GPS_rvbuff[60];
    
    
    GPS_rvbuff[60] = GPS_rvbuff[61];
    GPS_rvbuff[61] = GPS_rvbuff[62];
    GPS_rvbuff[62] = GPS_rvbuff[63];
    GPS_rvbuff[63] = GPS_rvbuff[64];
    GPS_rvbuff[64] = GPS_rvbuff[65];
    GPS_rvbuff[65] = GPS_rvbuff[66];
    GPS_rvbuff[66] = GPS_rvbuff[67];
    GPS_rvbuff[67] = GPS_rvbuff[68];
    GPS_rvbuff[68] = GPS_rvbuff[69];
    GPS_rvbuff[69] = GPS_rvbuff[70];
    
    
    GPS_rvbuff[70] = GPS_rvbuff[71];
    GPS_rvbuff[71] = GPS_rvbuff[72];
    GPS_rvbuff[72] = GPS_rvbuff[73];
    GPS_rvbuff[73] = GPS_rvbuff[74];
    GPS_rvbuff[74] = GPS_rvbuff[75];
    GPS_rvbuff[75] = GPS_rvbuff[76];
    GPS_rvbuff[76] = GPS_rvbuff[77];
    GPS_rvbuff[77] = GPS_rvbuff[78];
    GPS_rvbuff[78] = GPS_rvbuff[79];
    GPS_rvbuff[79] = GPS_rvbuff[80];
    
    GPS_rvbuff[80] = GPS_rvbuff[81];
    GPS_rvbuff[81] = GPS_rvbuff[82];
    GPS_rvbuff[82] = GPS_rvbuff[83];
    GPS_rvbuff[83] = GPS_rvbuff[84];
    GPS_rvbuff[84] = GPS_rvbuff[85];
    GPS_rvbuff[85] = GPS_rvbuff[86];
    GPS_rvbuff[86] = GPS_rvbuff[87];
    GPS_rvbuff[87] = GPS_rvbuff[88];
    GPS_rvbuff[88] = GPS_rvbuff[89];
    GPS_rvbuff[89] = GPS_rvbuff[90];
    
    GPS_rvbuff[90] = dat_read;
     
    
    if(( GPS_rvbuff[0]=='$') &&( GPS_rvbuff[1]=='G') &&( GPS_rvbuff[2]=='P') &&( GPS_rvbuff[3]=='R') &&( GPS_rvbuff[4]=='M') &&( GPS_rvbuff[5]=='C') )
    {
    
       NMEA_GPRMC_Analysis(&gpsx,(byte*)GPS_rvbuff);
    
    }

}




 