// SumvIf_Type.h 
#ifndef SUMVIF_TYPE_H
#define SUMVIF_TYPE_H

#include "GeneralTypeDef.h"

/* configuration of registers of ads1115 */
#define SUMVIF_WRADDR      0x90 
#define SUMVIF_RDADDR      0x91 // 

/* AD value normal */
#define SUMV_ZERO_DRIFT_THRED     10     
#define SUMV_OUT_RANGE_THRED      0x7000                                                                                  

/* register address of ads1115 */
#define SUMVIF_CONV_RSLT_ADDR     0x00 // conversion result register
#define SUMVIF_CONGFIG_ADDR       0x01 // congfig register
#define SUMVIF_LO_THRESH_ADDR     0x02 // low  thresh register
#define SUMVIF_HI_THRESH_ADDR     0x03 // high thresh register  

/* config for sumv detect */
#define CONTINUE_CONV_AIN0_128SPS  0x4083 //FS:6.144V
#define CONTINUE_CONV_AIN1_128SPS  0x5083 //FS:6.144V
#define CONTINUE_CONV_AIN2_128SPS  0x6083 //FS:6.144V
#define CONTINUE_CONV_AIN3_128SPS  0x7083 //FS:6.144V

#define CONTINUE_CONV_P0N1_128SPS  0x0483 //FS:2.048V
#define CONTINUE_CONV_P0N3_128SPS  0x1083 //FS:6.144V
#define CONTINUE_CONV_P1N3_128SPS  0x2083 //FS:6.144V
#define CONTINUE_CONV_P2N3_128SPS  0x3083 //FS:6.144V

#define SUMV_CONTINUE_CONV_AIN1_128SPS     0x5883 //FS:0.512V, for inner sumv and ext sumv
#define SUMVBAK_CONTINUE_CONV_AIN2_128SPS  0x6883 //FS:0.512V, for inner sumv backup
  
#endif /* SUMVIF_TYPE_H */ 