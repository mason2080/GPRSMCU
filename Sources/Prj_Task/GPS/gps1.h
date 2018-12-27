#ifndef _GPS1_H
#define _GPS1_H

#include "GeneralTypeDef.h"
//#include "math.h"
//#include "stdio.h"
#include "string.h"
//#include "stm32f10x.h"

#define RX_BUFFER_SIZE		200			




typedef struct
{
	uByte num;				//satellite number
	uByte eledeg;		//elevation in degrees
	word azideg;		//azimuth in degrees to true
	uByte sn;				//SNR in dB
}nmea_slmsg;

//UTC
 typedef struct
{
		word year;		
		byte month;		
		byte date;		
		byte hour;		
		byte min;			
		byte sec;	
		
		
		byte time[6];   //BeiJing Time HH:need Add 8 Hours
		byte date1[6];		
}nmea_utc_time;

//NEMA 0183
 typedef struct
{
		byte svnum;								//satellites in view
		nmea_slmsg slmsg[12];		
		nmea_utc_time utc;			//UTC
		dword latitude;						//latitude * 100000
		byte latitude1[10];				
		byte nshemi;							//North or South
		dword longitude;					//longitude * 100000
	  byte longitude1[11];			
		byte ewhemi;							//East or West
		byte gpssta;							//GPS quality indicator
		byte posslnum;						//Number of satellites in view, 00 - 12
		byte possl[12];						
		byte fixmode;							//mode
		word pdop;								//PDOP in meters
		word hdop;								//HDOP in meters
		word vdop;								//VDOP in meters
		int altitude;						//antenna altitude above/below mean-sea-level * 10
		word speed;							//speed * 1000
		
		
}nmea_msg;

extern nmea_msg gpsx;

void GPSMSG_Analysis(nmea_msg *gpsx, byte *buf);

void NMEA_GPRMC_Analysis(nmea_msg *gpsx,byte *buf);
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,byte *buf);
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,byte *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,byte *buf);
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,byte *buf);

#endif /*_GPS_H*/
