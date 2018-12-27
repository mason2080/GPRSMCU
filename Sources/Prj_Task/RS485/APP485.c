#include "APP485.h"
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


#define GET_HIGH_BYTE(val) ((uByte)(val >> 8))
#define GET_LOW_BYTE(val)   ((uByte)(val))

#define GET_HH_WORD(val)  ((uByte) (0x30|((val >> 12)&0x0f)))
#define GET_HL_WORD(val)  ((uByte) (0x30|((val >> 8)&0x0f)))
#define GET_LH_WORD(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_WORD(val)  ((uByte) (0x30|(val & 0x0f)))

#define GET_LH_BYTE(val)  ((uByte) (0x30|((val >> 4)&0x0f)))
#define GET_LL_BYTE(val)  ((uByte) (0x30|(val & 0x0f)))

uWord App485_cnt;
uByte rs485_rvnum;
uByte rs485_rvbuff[30];
uByte rs485_fullrv_flag;
uByte rs485_data_type;
word crc;
uWord uAddr,ucount;

uByte rs485_ttrbuff[200];	//485串口接收缓存
uByte rs485_trnum;			  //485串口发送计数器         rs485_trbuff
uByte rs485_trlen;
uByte rs485_rvlen;
uByte rs485_tx_busy;
Byte rs485_cmd[30];
Byte dat_len,receive_head,receive_flag;

void delay_time( Word time) 
{
    while(time--) 
    {
       asm nop;
       asm nop;
       WdgIf_Trig();         
       
    }
}
void load_rs485_data(void);
void CPU485_TxAppi(uWord iuAddr,uWord iucount);
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
       
       if(wCheck == 1)
       {
          wCRCIn = wCRCIn ^ 0xa001;//0x8005;//
       }
       wCRCIn = wCRCIn & 0xffff;
    }
    return wCRCIn;
}
//****************************************************************//
const Word wCRCTalbeAbs[] =
{
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400,
};

Word CRC16_2(Byte* pchMsg, Word wDataLen)
{
        Word wCRC = 0xFFFF;
        Word i;
        Byte chChar;
        for (i = 0; i < wDataLen; i++)
        {
                chChar = *pchMsg++;
                wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
                wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
        }
        return wCRC;
}
//****************************************************************//
///////////////////////////////////////////////////////////////
void APP485_Init(void)

{
     App485_cnt = 0; 
    CPU_RS485_Enable();
    CPU_RS485_EnableEvent();
    //setReg8(DDR1AD1, 0xFF);
    //setReg8Bits(DDR1AD1, 0x40U);  //DDR0AD1 
       
    rs485_data_type = 0;
}

/* module count call back */
void APP485_MainCountCbk(void)
{
    (App485_cnt < 65535) ? (App485_cnt++) : (App485_cnt = 65535);
}

void CPU485_TxApp(uByte i_length)//rs485_send_dat(void)
{
   uByte i_dat;

   CPU_RS485_DR_PutVal(0);
   rs485_trnum = 0;
   delay_time(15425) ;
   
   
   for(i_dat = 0;i_dat <= i_length;i_dat++) 
   { 
     delay_time(14625) ;
     
     CPU_RS485_SendChar(rs485_ttrbuff[i_dat]);      
   }
   Crc_16_CheckModbus(&rs485_ttrbuff[0],i_length + 1,&crc);
   //receive_flag = 0;
   delay_time(12625) ;
   CPU_RS485_DR_PutVal(1);
 }
void CPU485_TxAppi(uWord iuAddr,uWord iucount)//rs485_send_dat(void)
{
   uByte i_dat;

   CPU_RS485_DR_PutVal(0);
   rs485_trnum = 0;
   delay_time(15425) ;
   
   
   /*for(i_dat = 0;i_dat <= 3;i_dat++) 
   { 
     delay_time(14625) ;
     
     CPU_RS485_SendChar(rs485_ttrbuff[i_dat]);      
   }*/
   //Crc_16_CheckModbus(&rs485_ttrbuff[0],4,&crc);
   for(i_dat = (uByte)iuAddr;i_dat <= (uByte)iucount;i_dat++)//(i_dat = 0;i_dat <= 7;i_dat++) 
   { 
     delay_time(14625) ;
     
     CPU_RS485_SendChar(rs485_ttrbuff[i_dat + 4]); 
   }
   Crc_16_CheckModbus(&rs485_ttrbuff[(uByte)(uAddr+4)],(uByte)(ucount + 5),&crc);      
   CPU_RS485_SendChar(GET_LOW_BYTE(crc)); 
   CPU_RS485_SendChar(GET_HIGH_BYTE(crc));

   //receive_flag = 0;
   delay_time(12625) ;
   CPU_RS485_DR_PutVal(1);
 }
/* main call back */
void APP485_MainCbk(void)
{
    uWord wtmp;
    //CPU485_RxApp();
    load_rs485_data();
    if(App485_cnt >= 200) 
  	{
  		  App485_cnt = 0;
  		  rs485_rvnum = 0;
  		  receive_head = 3;
  		  //receive_flag = 1;
  	} /*
    CPU_RS485_DR_PutVal(1);
    if((receive_flag == 1))// && (receive_head == 0)) 
    {
       rs485_ttrbuff[0] = 0x03;//60;
       rs485_ttrbuff[1] = 0x03;//24;
       rs485_ttrbuff[2] = 0x03;//81;
       rs485_ttrbuff[3] = 0x03;//08;
       Crc_16_CheckModbus(&rs485_ttrbuff[0],5,&crc);
       rs485_ttrbuff[5] = GET_LOW_BYTE(crc);
       rs485_ttrbuff[6] = GET_HIGH_BYTE(crc);
       rs485_ttrbuff[7] = 7;
       CPU485_TxApp();
       receive_flag = 0;
    } 
    if((receive_flag == 2) && (receive_head == 0)) 
    {
       rs485_ttrbuff[0] = 0x01;
       rs485_ttrbuff[1] = 0x03;//function code;
       wtmp = 0x0000;
       rs485_ttrbuff[2] = GET_HIGH_BYTE(wtmp);//0x00;
       rs485_ttrbuff[3] = GET_LOW_BYTE(wtmp);//0x00;
       wtmp = 0x0017;
       rs485_ttrbuff[4] = GET_HIGH_BYTE(wtmp);//0x00;
       rs485_ttrbuff[5] = GET_LOW_BYTE(wtmp);//0x0f;
       wtmp = CRC16_2(&rs485_ttrbuff,6);//Crc_16_CheckModbus(&rs485_ttrbuff,6,&crc);
       rs485_ttrbuff[6] = GET_LOW_BYTE(wtmp);//crc);//0x31;  //crc校验低字节
       rs485_ttrbuff[7] = GET_HIGH_BYTE(wtmp);//crc);//0x30;  //crc校验高字节
       CPU485_TxApp();  
       receive_flag = 0;
       //receive_head = 1;
    }
    if(rs485_fullrv_flag)      //40*5=200MS
    { 
     switch(rs485_cmd[3]) 
     {
        case 0x01:
          rs485_ttrbuff[0]=0x0a;
          rs485_ttrbuff[1]=0x55;
          rs485_ttrbuff[2]=0x8; //装载要发多少个字节//
          rs485_ttrbuff[3]=0x51;
          rs485_ttrbuff[4]=0x52;
          rs485_ttrbuff[5]=0x53;
          rs485_ttrbuff[6]=0x54;
          rs485_ttrbuff[7]=0x55;        
          rs485_ttrbuff[8]=0x56;
          CPU485_TxApp();//rs485_send_dat();                
          break; 
       case 0x02:
          rs485_ttrbuff[0]=0x0a;
          rs485_ttrbuff[1]=0x55;
          rs485_ttrbuff[2]=0x8;
          rs485_ttrbuff[3]=0x58;
          rs485_ttrbuff[4]=0x59;
          rs485_ttrbuff[5]=0x63;
          rs485_ttrbuff[6]=0x84;
          rs485_ttrbuff[7]=0x95;        
          rs485_ttrbuff[8]=0xa6;
          CPU485_TxApp();//rs485_send_dat();
          break;
     }
     rs485_fullrv_flag = 0; 
    } */
    if((receive_flag == 1)&&(receive_head == 3))
    { 
        receive_head = 0;
        crc = 0; 
        //CPU485_TxApp(3);
        //CPU485_TxAppi(uAddr,ucount);
        CPU485_TxAppi(0x0000,0x0002);
        //receive_flag = 0;
    }
    
    //Crc_16_CheckModbus(&rs485_ttrbuff[uAddr],p,&crc);
        
    
}
void CPU485_RxApp(void)
{
    Byte dat_read,p,btmp;
    Word wtmp;    
		//rs485_rvnum++;     		 	         //个数加1
    CPU_RS485_RecvChar(&dat_read);
    if(receive_head==1) 
  	{
  	   rs485_cmd[3]= 0x01; 
  		 //rs485_rvnum++;
  		 //rs485_cmd[2+rs485_rvnum]= 0x01;//dat_read;
  		 //if(rs485_rvnum>=3)//(rs485_cmd[2]-1)) 
  		 //{
  		    rs485_fullrv_flag = 1;
  		    receive_flag = 2;
  		    receive_head = 0;
  		 //   memset(rs485_rvbuff,0,sizeof(rs485_rvbuff));
  		 //}
  		      
  	}
    else 
  	{
      rs485_rvbuff[0] = rs485_rvbuff[1];
      rs485_rvbuff[1] = rs485_rvbuff[2];
      rs485_rvbuff[2] = rs485_rvbuff[3];
      rs485_rvbuff[3] = rs485_rvbuff[4];
      rs485_rvbuff[4] = rs485_rvbuff[5];
      rs485_rvbuff[5] = rs485_rvbuff[6];
      rs485_rvbuff[6] = rs485_rvbuff[7];
      rs485_rvbuff[7] = dat_read;/*rs485_rvbuff[8];
      rs485_rvbuff[8] = rs485_rvbuff[9];
      rs485_rvbuff[9] = rs485_rvbuff[10];
      rs485_rvbuff[10] = rs485_rvbuff[11];
      rs485_rvbuff[11] = rs485_rvbuff[12];
      rs485_rvbuff[12] = rs485_rvbuff[13];
      rs485_rvbuff[13] = rs485_rvbuff[14];
      rs485_rvbuff[14] = rs485_rvbuff[15];
      rs485_rvbuff[15] = rs485_rvbuff[16];
      rs485_rvbuff[16] = rs485_rvbuff[17];
      rs485_rvbuff[17] = rs485_rvbuff[18];
      rs485_rvbuff[18] = rs485_rvbuff[19];
      rs485_rvbuff[19] = rs485_rvbuff[20];
      rs485_rvbuff[20] = rs485_rvbuff[21];
      rs485_rvbuff[21] = rs485_rvbuff[22];      
      rs485_rvbuff[22] = dat_read;*/    
            
  	  if(rs485_rvbuff[1] == 0x03)//((rs485_rvbuff[0] == 0x01) && (rs485_rvbuff[1] == 0x03))//&& (rs485_rvbuff[18] == 0x0f)&& (rs485_rvbuff[19] == 0x0f))// && (rs485_rvbuff[2] == 0x0f))	//命令待定，协议不明确，待修改       word crc;
  		{
  		    Crc_16_CheckModbus(&rs485_rvbuff,7,&crc);
  		    rs485_cmd[0] = rs485_rvbuff[0];
  		    /*btmp = rs485_rvbuff[19];
  		    if((btmp & 0x02) == 0x02)// || ((btmp & 0x01) == 0x00))
  		    {
  		        flag_inverter_err = 1;
  		    } 
  		    else
  		    {
  		        flag_inverter_err = 0;
  		    }
  		    if(((btmp & 0x04) == 0x04) || ((btmp & 0x80) == 0x80))
  		    {
  		        flag_Power_err = 1;
  		    } 
  		    else
  		    {
  		        flag_Power_err = 0;
  		    }*/
  		    uAddr = rs485_rvbuff[2]<<8 + rs485_rvbuff[3];
  		    ucount = rs485_rvbuff[4]<<8 + rs485_rvbuff[5];
  		    rs485_data_type = 0;
  		 
		      //rs485_fullrv_flag = 1;
		      //rs485_rvnum=0; 
		      receive_flag = 1;
  			  //receive_head=12;  
  		 }
  		else if(rs485_rvbuff[1] != 0x03)// && (rs485_rvbuff[2] == 0x0f))	//命令待定，协议不明确，待修改       word crc;
  		{  
  		    receive_flag = 0;
  		}
  		else if((rs485_rvbuff[0] == 0x01) && (rs485_rvbuff[1] == 0x83))// && (rs485_rvbuff[2] == 0x0f))	//命令待定，协议不明确，待修改       word crc;
  		{
  		    //rs485_rvnum=rs485_rvbuff[3];
  		    //rs485_cmd[0]= rs485_rvbuff[0];
  		    //rs485_cmd[1]= rs485_rvbuff[1];
  		    //rs485_cmd[2]= dat_read;
  		 
		      //rs485_fullrv_flag = 1;
		      //rs485_rvnum=0; 
		      //receive_flag = 0;
  			  //receive_head=12;  
  		 }
  		 
  	}
}
/*
void CPU485_TxApp(void) 
{
    rs485_trnum++;
    if(rs485_trnum <= rs485_trlen)
    {		
        asm nop;
        asm nop;
        CPU_RS485_SendChar(rs485_trbuff[rs485_trnum]);					                     //延时1mS,等待与其通信的设备相应处理
    }
    else
    {
        //CPU_RS485_SendChar(rs485_trbuff[rs485_trnum]);
        rs485_trnum=0;
        rs485_rvnum=0;
        //CPU_RS485_DR_PutVal(0);
        rs485_tx_busy = 0;
        
    }   
}
 */
 
 
 #if 1
void load_rs485_data(void) 
{  
  word wtmp,wtmp1,temp1,lrc,bat_volt[60],crc;                              
  //Byte time[6];
  Byte p;//,batstar,batsum,bmuno;
  Byte i,j,bmu_bat_num, cnt;
  //Byte btemp;
  //uLWord bms_hd_err,k;  

  p = 0;
  
  rs485_ttrbuff[p++] = rs485_cmd[0];//0x3A;                     //start fun 
  //lrc = 2; 
  rs485_ttrbuff[p++] = 0x03;//0x30;
  rs485_ttrbuff[p++] = GET_HIGH_BYTE(ucount);
  rs485_ttrbuff[p++] = GET_LOW_BYTE(ucount);                 
  
 
  rs485_data_type=0;    
  if(rs485_data_type<1) 
  {    
    ////data1  
      wtmp = Bat_GetSumV(INT_SUMV)*4/10;   //1V
      //lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      wtmp =Bat_GetSOC()*2/10;
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
    ////data5
      wtmp =Bat_GetCurrent(AVGCUR);//*4 + 1000;
      if(wtmp >= 30000) 
      {
          wtmp -= 30000;
          wtmp *= 4;
          wtmp += 1000;                  //1A +500
      } 
      else
      {
          wtmp = 30000 - wtmp;
          wtmp *= 4;
          wtmp = 1000 -wtmp;
      }
      //lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
     ////data7 
      if( (Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= ERR_LEVEL_TWO)   //iso err 2
          ||(Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= ERR_LEVEL_TWO)
          || (Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH) >= ERR_LEVEL_TWO)
          ||(Err_GetBatErrLevel(ERR_BAT_SUMV_HIGH) >= ERR_LEVEL_TWO)      //sumv high 2
          ||(Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH) >= ERR_LEVEL_TWO)     //cellv high 2
          ||(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG) >= ERR_LEVEL_TWO)   //cur high 2
          ||(Err_GetBatErrLevel(ERR_BAT_T_HIGH) >= ERR_LEVEL_TWO) 
          ||(Err_GetBatErrLevel(ERR_BAT_T_LOW) >= ERR_LEVEL_TWO) ) 
      {
          wtmp = 0x02;
      } 
      else if( (Err_GetBatSysErrLevel(ERR_BATSYS_ISO_ERR) >= ERR_LEVEL_ONE)   //iso err 2
          ||(Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= ERR_LEVEL_ONE)
          || (Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH) >= ERR_LEVEL_ONE)
          ||(Err_GetBatErrLevel(ERR_BAT_SUMV_HIGH) >= ERR_LEVEL_ONE)      //sumv high 2
          ||(Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH) >= ERR_LEVEL_ONE)     //cellv high 2
          ||(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG) >= ERR_LEVEL_ONE)   //cur high 2
          ||(Err_GetBatErrLevel(ERR_BAT_T_HIGH) >= ERR_LEVEL_ONE) 
          ||(Err_GetBatErrLevel(ERR_BAT_T_LOW) >= ERR_LEVEL_ONE) ) 
      {
          wtmp = 0x01;
      }
      else
      {
          wtmp = 0x00;
      }
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
     ////data9 
      wtmp =Bat_GetMaxT()*2;
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      wtmp =Bat_GetMinT()*2;
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      wtmp =Bat_GetMaxV();
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      wtmp =Bat_GetMinV();
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      ////data18
      wtmp = 0;
      wtmp1 = Bat_GetSumV(INT_SUMV);
      if(wtmp1 < 1920*84/100) 
      {
         wtmp =0x0010;
      }
      wtmp1 = Bat_GetSumV(INT_SUMV);
      if(wtmp1 > 1920*108/100) 
      {
         wtmp =0x0020;
      }
      if(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_DCH) >= ERR_LEVEL_ONE) 
      {
         wtmp |=0x0100; 
      }
      if(flag_PosPaste == 1) 
      {
         wtmp |=0x0200; 
      }
      
      if(Err_GetBatErrLevel(ERR_BAT_CUR_HIGH_CHG) >= ERR_LEVEL_ONE) 
      {
         wtmp |=0x0400; 
      }
      if(Err_GetBatErrLevel(ERR_BAT_CELLV_LOW) >= ERR_LEVEL_ONE)
      {
         wtmp |=0x0800; 
      }
      if(Err_GetBatErrLevel(ERR_BAT_CELLV_HIGH) >= ERR_LEVEL_ONE)
      {
         wtmp |=0x1000; 
      }
      if(Err_GetBatErrLevel(ERR_BAT_T_LOW) >= ERR_LEVEL_ONE)
      {
         wtmp |=0x2000; 
      }
      if(Err_GetBatErrLevel(ERR_BAT_T_HIGH) >= ERR_LEVEL_ONE)
      {
         wtmp |=0x4000; 
      }
      if(Err_GetBatErrLevel(ERR_BAT_DELTT_ALLPACK) >= ERR_LEVEL_ONE)
      {
         wtmp |=0x8000; 
      }
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      ////data21
      wtmp1 = Bat_GetMinV();
      if(wtmp1 <= 2500)
      {
          wtmp =0x0001;
      }
      rs485_ttrbuff[p++] = GET_HIGH_BYTE(wtmp);
      rs485_ttrbuff[p++] = GET_LOW_BYTE(wtmp);
      ////data23
      //for(i=0;i<Bat_GetBmuNum();i++) 
      //{
         //bmu_bat_num = (uByte)Bat_GetPackSerialNum(1);       //i,j,bmu_bat_num, bat_volt[60],cnt,
         for(j=0;j<60;j++)   //bmu_bat_num
         {  
            bat_volt[j]=(uWord)(Bat_GetCellV(1,j+1)); 
            rs485_ttrbuff[p++] = GET_HIGH_BYTE(bat_volt[j]);
            rs485_ttrbuff[p++] = GET_LOW_BYTE(bat_volt[j]);  
         }
      //}
      /*for(i=0;i<60;i++) //Bat_GetBmuNum()         //
      {
         rs485_ttrbuff[p+(i++)] = GET_HIGH_BYTE(bat_volt[i++]);
         rs485_ttrbuff[p+(i++)] = GET_LOW_BYTE(bat_volt[i++]);
      }*/
      ////data83
      for(i=1;i<8;i++) //Bat_GetBmuNum()         //
      {
         //Bat_GetTemper(1, i) ;
         rs485_ttrbuff[p++] = GET_HIGH_BYTE(Bat_GetTemper(1, i));
         rs485_ttrbuff[p++] = GET_LOW_BYTE(Bat_GetTemper(1, i));
      }
      
      
      //Crc_16_CheckModbus(&rs485_ttrbuff[uAddr],p,&crc);
      //rs485_ttrbuff[p++] = GET_LOW_BYTE(crc);//0x31;  //crc校验低字节
      //rs485_ttrbuff[p++] = GET_HIGH_BYTE(crc);//0x30;  //crc校验高字节 

  
  } 	     
    	
}

#endif

 #if 0
//void load_rs485_data(void) 
{  
  word wtmp,temp1,lrc,crc;                              
  Byte time[6];
  Byte p,batstar,batsum,bmuno;
  Byte i,j;
  Byte btemp;
  uLWord bms_hd_err,k;  

  p = 0;
  
  rs485_ttrbuff[p++] = 0x01;//0x3A;                     //start fun 
  //lrc = 2; 
  rs485_ttrbuff[p++] = 0x03;//0x30;
  rs485_ttrbuff[p++] = 0x00;
  rs485_ttrbuff[p++] = 0x00;
  rs485_ttrbuff[p++] = 0x00;
  rs485_ttrbuff[p++] = 0x15;//0x32;
  //lrc += 0x10;
  Crc_16_CheckModbus(&rs485_ttrbuff[0],p,&crc);
  rs485_ttrbuff[p++] = GET_LOW_BYTE(crc);//0x31;  //crc校验低字节
  rs485_ttrbuff[p++] = GET_HIGH_BYTE(crc);//0x30;  //crc校验高字节                   
  
  if(rs485_data_type<1) 
  {    
      wtmp = 0;
  } 
  else if(rs485_data_type<8)
  {
      wtmp = 60 + 12 * (rs485_data_type - 1);
  } 
  else if(rs485_data_type < 10)
  {
      wtmp = 300 + 12 * (rs485_data_type - 8);
  } 
  else if(rs485_data_type < 11)
  {
      wtmp = 400;
  }
  else 
  {
      wtmp = 500;
  }
  
  lrc += wtmp;
  rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
  rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
  rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
  rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
  lrc += 0x0c;
  rs485_ttrbuff[p++] = 0x30;
  rs485_ttrbuff[p++] = 0x30;
  rs485_ttrbuff[p++] = 0x30;
  rs485_ttrbuff[p++] = 0x3c;
  lrc += 0x18;
  rs485_ttrbuff[p++] = 0x31;
  rs485_ttrbuff[p++] = 0x38;
  
//  for(i=15;i<63;i++)
//      rs485_trbuff[i] = 0xff;
      
  if(rs485_data_type<1) 
  {    
    ////data1  
      wtmp = Bat_GetSumV(INT_SUMV) / 10;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data2
      wtmp =Bat_GetCurrent(INSCUR);
      if(wtmp >= 30000) 
      {
          wtmp -= 30000;
          wtmp /= 10;
          wtmp += 500;                  //1A +500
      } 
      else
      {
          wtmp = 30000 - wtmp;
          wtmp /= 10;
          wtmp = 500 -wtmp;
      }
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data3
      wtmp =Bat_GetSOC() / 10;
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data4  
      wtmp =Bat_GetAvgT() + 10;                    //AVT
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data5
      wtmp =Bat_GetMaxT() + 10;
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data6  
      wtmp =Bat_GetMinT() + 10;
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data7
      wtmp =Bat_GetMaxV();
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data8
      wtmp =Bat_GetMaxVTotalNo();
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data9
      wtmp =Bat_GetMinV();
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data10
      wtmp =Bat_GetMinVTotalNo();
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data11
      wtmp = Bat_GetAvgV();
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
    ////data12
      wtmp =0;
      if(flag_onchg_connect == 1)
         wtmp |= 0x01;
      if(7 == Get_HvM_Ctrl_Step())
         wtmp |= 0x02;            
      if(Chg_Mode == 0)
         wtmp |= 0x04;
      if((flag_onchg_err == 0) && (flag_onchg_connect==1))
         wtmp |= 0x8000;
      //
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
  } 
  else if(rs485_data_type<9)
  {
      if(rs485_data_type < 5) 
      {           
         bmuno = 1;
         batstar =  12 * (rs485_data_type - 1);
         batsum = 12;
      } 
      else if(rs485_data_type == 5)
      {
         bmuno = 1;
         batstar =  12 * (rs485_data_type - 1);
         batsum = 12;      
      } 
      
      else if(rs485_data_type < 8)
      {
         bmuno = 2;
         batstar =  12 * (rs485_data_type - 5) + 8;
         batsum = 12;      
      } 
      else
      {
         bmuno = 2;
         batstar =  12 * (rs485_data_type - 5) + 8;
         batsum = 2;            
      }
      for(i=0;i<batsum;i++)
      {        
        ////data
          if(rs485_data_type == 5) 
          {
              if(i<4) 
              {
                  wtmp =Bat_GetCellV(1,batstar + 1 + i); 
              } 
              else
              {
                  wtmp =Bat_GetCellV(2, 1 + i - 4);
              }
          } 
          else
          {            
              wtmp =Bat_GetCellV(bmuno,batstar + 1 + i);                        //
          }
          lrc += wtmp;
          rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
          rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
          rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
          rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
      }
      if(rs485_data_type == 8)
      {        
          for(i=0;i<2;i++)
          {        
            ////data
              wtmp =0xFF;                        //
              lrc += wtmp;
              rs485_ttrbuff[p++] = 0xFF;//GET_HH_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_HL_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_LH_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_LL_WORD(wtmp);
          }
      }
      
  }
  else if(rs485_data_type<10)
  {
      if(rs485_data_type < 10) 
      {           
          for(i=1;i<5;i++)
          {        
            ////data
              wtmp =Bat_GetTemper(1,i);                        //
              lrc += wtmp;
              rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
          }
          for(i=1;i<5;i++)
          {        
            ////data
              wtmp =Bat_GetTemper(2,i);                        //
              lrc += wtmp;
              rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
              rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
          }
          for(i=0;i<4;i++)
          {        
            ////data
              wtmp =0xFF;                        //
              lrc += wtmp;
              rs485_ttrbuff[p++] = 0xFF;//GET_HH_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_HL_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_LH_WORD(wtmp);
              rs485_ttrbuff[p++] = 0xFF;//GET_LL_WORD(wtmp);
          }              
                        
      } 
      /*else
      {
          for(i=5;i<9;i++)
          {        
            ////data
              wtmp =Bat_GetTemper(2,i);                        //
              lrc += wtmp;
              rs485_trbuff[p++] = GET_HH_WORD(wtmp);
              rs485_trbuff[p++] = GET_HL_WORD(wtmp);
              rs485_trbuff[p++] = GET_LH_WORD(wtmp);
              rs485_trbuff[p++] = GET_LL_WORD(wtmp);
          }              
          for(i=0;i<8;i++)
          {        
            ////data
              wtmp =0xFF;                        //
              lrc += wtmp;
              rs485_trbuff[p++] = 0xFF;//GET_HH_WORD(wtmp);
              rs485_trbuff[p++] = 0xFF;//GET_HL_WORD(wtmp);
              rs485_trbuff[p++] = 0xFF;//GET_LH_WORD(wtmp);
              rs485_trbuff[p++] = 0xFF;//GET_LL_WORD(wtmp);
          }              
      }*/
  }
  else if(rs485_data_type<11)
  {
    ////data1  
      wtmp = Bat_GetHisMaxSumV() / 10;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);    

    ////data2  
      wtmp = Bat_GetHisMinSumV() / 10;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data3  
      wtmp = Bat_GetHisMaxChgCur() / 10;   //1A
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data4  
      wtmp = Bat_GetHisMaxDchCur() / 10;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data5  
      wtmp = Bat_GetHisMaxV();   //1MV
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data6  
      wtmp = Bat_GetHisMinV();   //1MV
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data7  
      wtmp = Bat_GetHisMaxT() + 10;   //1C
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data8  
      wtmp = Bat_GetHisMinT() + 10;   //1C
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data9  
      wtmp = 0;   //0
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data10  
      wtmp = 0;   //0
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
      for(i=0;i<2;i++)
      {        
        ////data
          wtmp =0xFF;                        //
          lrc += wtmp;
          rs485_ttrbuff[p++] = 0xFF;//GET_HH_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_HL_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_LH_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_LL_WORD(wtmp);
      }              
  } 
  else
  {
    ////data1  
      wtmp = 0;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);    

    ////data2  
      wtmp = 0;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data3  
      wtmp = 0;   //1A
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);

    ////data4  
      wtmp = 0;   //1V
      lrc += wtmp;
      rs485_ttrbuff[p++] = GET_HH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_HL_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LH_WORD(wtmp);
      rs485_ttrbuff[p++] = GET_LL_WORD(wtmp);
  
      for(i=0;i<8;i++)
      {        
        ////data
          wtmp =0xFF;                        //
          lrc += wtmp;
          rs485_ttrbuff[p++] = 0xFF;//GET_HH_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_HL_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_LH_WORD(wtmp);
          rs485_ttrbuff[p++] = 0xFF;//GET_LL_WORD(wtmp);
      }              

  }
  
  btemp = (uByte)(256-lrc);
  rs485_ttrbuff[p++] = GET_LH_BYTE(btemp);
  rs485_ttrbuff[p++] = GET_LL_BYTE(btemp);
  rs485_ttrbuff[p++] = 0x0D;
  rs485_ttrbuff[p++] = 0x0A;
  rs485_trlen=p;  	     
    	
}

#endif











