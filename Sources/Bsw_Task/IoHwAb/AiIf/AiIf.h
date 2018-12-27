#ifndef AIIF_H
#define AIIF_H

#include "GeneralTypeDef.h"
#include "Ecu.h"
#include "IoIf_Type.h"
#include "Ai1.h"
#include "Ai2.h"


#define  CPU_ADC_ACCURACY 4096      // 10bit AD

#define  ADC_VOLT_CRITERION  5000    // 5V

#define ENABLE 1
#define DISABLE 0

#define AIIF_MAX_CHANNEL_NUM  17
#define AIIF_MAX_AI_NUM    2

//通道号与PE生成时选择的顺序有关，禁止改动 AN0~AN15--AI1  AN16~AN23---AI2//
typedef enum 
{
   AIIF_POW_VOLT_NO,           // channel0   AN03   AI1
   AIIF_CPUAI1_NO,             // channel1   AN10   AI1
   AIIF_CPUAI_DC_P2_NO,        // channel2   AN01   AI1
   AIIF_CPUAI_AC_P3_NO,        // channel3   AN09   AI1
   AIIF_CPUAI2_NO,             // channel4   AN02   AI1
   AIIF_CPUAI_CAN_INH_NO,      // channel5   AN11   AI1
   AIIF_CPUAI_OFFBD_CHG_P_NO,  // channel6   AN04   AI1
   AIIF_CPUAI_ONBD_CHG_P_NO,   // channel7   AN12   AI1
   AIIF_CPUAI_K15_NO,          // channel8   AN05   AI1
   AIIF_CPUAI_ALARM_NO,        // channel9   AN13   AI1
   AIIF_ISO_NEG_NO,            // channel10  AN06   AI1
   AIIF_ISO_POS_NO,            // channel11  AN07   AI1
   AIIF_ISO_NEG1_NO,           // channel12  AN14   AI1
   AIIF_ISO_POS1_NO,           // channel13  AN15   AI1
   AIIF_SLAVER_POW_AD_NO,      // channel14  AN08   AI1
   AIIF_CPUDO2_AD_NO,          // channel15  AN16   AI2
   AIIF_CPUDO1_AD_NO,          // channel16  AN17   AI2
   
}eAiIf_ChannelIdNumType;



GeneralReturnType AiIf_MeasureChannel(uByte AiIf_ChannelId,uWord *AiIf_AdValuePtr); 

#endif /* AIIF_H */