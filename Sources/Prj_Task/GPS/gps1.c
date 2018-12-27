#include "gps1.h"
//#include "GeneralTypeDef.h"

//#define RX_BUFFER_SIZE		1024		
byte GPS_BUF[RX_BUFFER_SIZE];				
uWord GPSMSG_FLAG = 0;							
uWord GPSMSG_INDEX = 0;
byte GPS_DEBUG = 0; 

						  
byte NMEA_Comma_Pos(byte *buf,byte cx)
{	 		    
	byte *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

dword NMEA_Pow(byte m,byte n)
{
	dword result=1;	 
	while(n--)result*=m;    
	return result;
}

int NMEA_Str2num(byte *buf,byte*dx)
{
	byte *p=buf;
	dword ires=0,fres=0;
	byte ilen=0,flen=0,i;
	byte mask=0;
	int res;
	while(1)
	{
		if(*p=='-'){mask|=0X02;p++;}
		if(*p==','||(*p=='*'))break;
		if(*p=='.'){mask|=0X01;p++;}
		else if(*p>'9'||(*p<'0'))	
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;
	for(i=0;i<ilen;i++)
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	
	*dx=flen;	 		
	for(i=0;i<flen;i++)	
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  

//GPRMC
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,byte *buf)
{
	byte *p1,dx;			 
	byte posx;     
	dword temp;	   
	dword rs;  
	p1=(byte*)strstr((const char *)buf,"GPRMC");
	posx=NMEA_Comma_Pos(p1,1);								
	if(posx!=0XFF)
	{
		//temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 
		//gpsx->utc.hour=temp/10000;
		//gpsx->utc.min=(temp/100)%100;
		//gpsx->utc.sec=temp%100;	
		
		gpsx->utc.time[0]= *(p1+posx);	
		gpsx->utc.time[1]= *(p1+posx+1);	
		gpsx->utc.time[2]= *(p1+posx+2);	
		gpsx->utc.time[3]= *(p1+posx+3);	
		gpsx->utc.time[4]= *(p1+posx+4);	
		gpsx->utc.time[5]= *(p1+posx+5);	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								
	if(posx!=0XFF)
	{
		gpsx->latitude1[0]= *(p1+posx);	
		gpsx->latitude1[1]= *(p1+posx+1);	
		gpsx->latitude1[2]= *(p1+posx+2);	
		gpsx->latitude1[3]= *(p1+posx+3);	
		gpsx->latitude1[4]= *(p1+posx+4);	
		gpsx->latitude1[5]= *(p1+posx+5);
  	gpsx->latitude1[6]= *(p1+posx+6);	
		gpsx->latitude1[7]= *(p1+posx+7);	
		gpsx->latitude1[8]= *(p1+posx+8);	
		gpsx->latitude1[9]= *(p1+posx+9);	
			 
		//temp=NMEA_Str2num(p1+posx,&dx);		 	 
		//gpsx->latitude=temp/NMEA_Pow(10,dx+2);	
		//rs=temp%NMEA_Pow(10,dx+2);			
		//gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
	}
	posx=NMEA_Comma_Pos(p1,4);							
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								
	if(posx!=0XFF)
	{												  
		//temp=NMEA_Str2num(p1+posx,&dx);		 	 
		//gpsx->longitude=temp/NMEA_Pow(10,dx+2);
		//rs=temp%NMEA_Pow(10,dx+2);					 
		//gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
		
		gpsx->longitude1[0]= *(p1+posx);	
		gpsx->longitude1[1]= *(p1+posx+1);	
		gpsx->longitude1[2]= *(p1+posx+2);	
		gpsx->longitude1[3]= *(p1+posx+3);	
		gpsx->longitude1[4]= *(p1+posx+4);	
		gpsx->longitude1[5]= *(p1+posx+5);
  	gpsx->longitude1[6]= *(p1+posx+6);	
		gpsx->longitude1[7]= *(p1+posx+7);	
		gpsx->longitude1[8]= *(p1+posx+8);	
		gpsx->longitude1[9]= *(p1+posx+9);
		gpsx->longitude1[10]= *(p1+posx+10);	
	}
	posx=NMEA_Comma_Pos(p1,6);						
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);
			 
	posx=NMEA_Comma_Pos(p1,9);							
	if(posx!=0XFF)
	{
		//temp=NMEA_Str2num(p1+posx,&dx);		 			
		//gpsx->utc.date=temp/10000;
		//gpsx->utc.month=(temp/100)%100;
		//gpsx->utc.year=2000+temp%100;	 
		
		gpsx->utc.date1[0]= *(p1+posx);	
		gpsx->utc.date1[1]= *(p1+posx+1);	
		gpsx->utc.date1[2]= *(p1+posx+2);	
		gpsx->utc.date1[3]= *(p1+posx+3);	
		gpsx->utc.date1[4]= *(p1+posx+4);	
		gpsx->utc.date1[5]= *(p1+posx+5);	 
	} 

}

//GPGSV
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,byte *buf)
{
	byte *p,*p1,dx;
	byte len,i,j,slx=0;
	byte posx;   	 
	p=buf;
	p1=(byte*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';								
	posx=NMEA_Comma_Pos(p1,3); 			
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(byte*)strstr((const char *)p,"$GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	
			else break;
			slx++;	   
		}   
 		p=p1+1;
	}   
}

//GPGGA
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,byte *buf)
{
	byte *p1,dx;			 
	byte posx;    
	p1=(byte*)strstr((const char *)buf,"$GPGGA");
	posx=NMEA_Comma_Pos(p1,6);						
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);							
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);							
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}

//GPGSA
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,byte *buf)
{
	byte *p1,dx;			 
	byte posx; 
	byte i;   
	p1=(byte*)strstr((const char *)buf,"$GPGSA");
	posx=NMEA_Comma_Pos(p1,2);						
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
	for(i=0;i<12;i++)							
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);							
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);								
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);								
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}

//GPVTG
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,byte *buf)
{
	byte *p1,dx;			 
	byte posx;    
	p1=(byte*)strstr((const char *)buf,"$GPVTG");							 
	posx=NMEA_Comma_Pos(p1,7);							
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		
	}
}  



void GPSMSG_Analysis(nmea_msg *gpsx, byte *buf)
{
		NMEA_GPRMC_Analysis(gpsx,buf);	
		NMEA_GPGSV_Analysis(gpsx,buf);	
		NMEA_GPGGA_Analysis(gpsx,buf);	
		NMEA_GPGSA_Analysis(gpsx,buf);	
		NMEA_GPVTG_Analysis(gpsx,buf);	
}

