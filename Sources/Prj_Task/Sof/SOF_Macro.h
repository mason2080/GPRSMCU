// SOF.h
#ifndef SOF_MACRO_H
#define SOF_MACRO_H

#include "GeneralTypeDef.h"

/* select solution */ 
//#define USE_SOF_B
#define USE_SOF_A

/* sub function enable macro */
#define SOF_FUN_ENABLE      1
#define SOF_FUN_DISABLE     0

// select function
#define SOF_DCH_CUR_VOLT_MODIFY   SOF_FUN_DISABLE
#define SOF_DCH_CUR_ERR_MODIFY    SOF_FUN_DISABLE
#define SOF_FD_CUR_VOLT_MODIFY    SOF_FUN_DISABLE
#define SOF_FD_CUR_ERR_MODIFY     SOF_FUN_DISABLE
#define SOF_PLUSE_CUR_MODIFY      SOF_FUN_DISABLE
#define SOF_CALC_MAX_POWER        SOF_FUN_DISABLE
#define SOF_USER_FUN              SOF_FUN_DISABLE
/* end of sub function enable macro */
 
// define for array
#define SOF_MAX_T_POINT      7
#define SOF_MAX_SOC_POINT    6
#define SOF_MAX_VOLT_POINT   4


// define for pluse current last time
#define SOF_PULSE_DISCHARGE_CUR_TIME    100   //10S
#define SOF_PULSE_FEEDBACK_CUR_TIME     100   //10S

// use for sof self_adaptation
#define SOF_MINV_SELF_CALC_CUR 3000  // 3V, unit 1mV

// use for smooth permit current or power
#define SOF_CONST_DCHCUR_SMOOTH_UNIT       1    // 0.1A/10ms, unit 0.1A
#define SOF_CONST_FEEDBACKCUR_SMOOTH_UNIT  1    // 0.1A/10ms, unit 0.1A
#define SOF_PULSE_DCHCUR_SMOOTH_UNIT       1    // 0.1A/10ms, unit 0.1A
#define SOF_PULSE_FEEDBACKCUR_SMOOTH_UNIT  1    // 0.1A/10ms, unit 0.1A

#define SOF_CONST_DCHPOW_SMOOTH_UNIT       1    // 0.1kw/10ms, unit 0.1kw
#define SOF_CONST_FEEDBACKPOW_SMOOTH_UNIT  1    // 0.1kw/10ms, unit 0.1kw
#define SOF_PULSE_DCHPOW_SMOOTH_UNIT       1    // 0.1kw/10ms, unit 0.1kw
#define SOF_PULSE_FEEDBACKPOW_SMOOTH_UNIT  1    // 0.1kw/10ms, unit 0.1kw

// use for error modify
/* for discharge cur modify */
#define SOF_MODIFY_CELLV_LOW_L1    50
#define SOF_MODIFY_CELLV_LOW_L2    0
#define SOF_MODIFY_SUMV_LOW_L1     50
#define SOF_MODIFY_SUMV_LOW_L2     0
#define SOF_MODIFY_DCHCUR_HIGH_L1  90
#define SOF_MODIFY_DCHCUR_HIGH_L2  80

/* for feedback cur modify */
#define SOF_MODIFY_CELLV_HIGH_L1   50
#define SOF_MODIFY_CELLV_HIGH_L2   0
#define SOF_MODIFY_SUMV_HIGH_L1    50
#define SOF_MODIFY_SUMV_HIGH_L2    0
#define SOF_MODIFY_FDCUR_HIGH_L1   90
#define SOF_MODIFY_FDCUR_HIGH_L2   80

/* for both discharge and feedback cur modify */
#define SOF_MODIFY_DETVOLT_L1      90
#define SOF_MODIFY_DETVOLT_L2      80
#define SOF_MODIFY_DETTEMP_L1      90
#define SOF_MODIFY_DETTEMP_L2      80
#define SOF_MODIFY_INTCAN_ERR      0

/* define struct for macro and config parameter */
typedef struct
{
    uByte Sof_Max_Soc_Point;
    uByte Sof_Max_T_Point;
    uByte Sof_Max_Volt_Point;
    
    uByte Sof_Dch_Cur_Volt_Modify;
    uByte Sof_Dch_Cur_Err_Modify;
    uByte Sof_Fd_Cur_Volt_Modify;
    uByte Sof_Fd_Cur_Err_Modify;
    uByte Sof_Pulse_Cur_Modify;
    uByte Sof_Calc_Max_Power;
    uByte Sof_User_Fun;
} sSOF_MacroType;

typedef struct
{
    uWord Sof_Minv_Self_Calc_Cur;
    
    uWord Sof_Const_DchCur_Smooth_Unit;
    uWord Sof_Const_FeedbackCur_Smooth_Unit;
    uWord Sof_Pulse_DchCur_Smooth_Unit;
    uWord Sof_Pulse_FeedbackCur_Smooth_Unit;
    
    uWord Sof_Const_DchPow_Smooth_Unit;
    uWord Sof_Const_FeedbackPow_Smooth_Unit;
    uWord Sof_Pulse_DchPow_Smooth_Unit;
    uWord Sof_Pulse_FeedbackPow_Smooth_Unit;
    
    uByte Sof_Modify_CellvLow_L1;
    uByte Sof_Modify_CellvLow_L2;
    uByte Sof_Modify_SumvLow_L1;
    uByte Sof_Modify_SumvLow_L2;
    uByte Sof_Modify_DchCur_L1;
    uByte Sof_Modify_DchCur_L2;
    
    uByte Sof_Modify_CellvHigh_L1;
    uByte Sof_Modify_CellvHigh_L2;
    uByte Sof_Modify_SumvHigh_L1;
    uByte Sof_Modify_SumvHigh_L2;
    uByte Sof_Modify_FdCur_L1;
    uByte Sof_Modify_FdCur_L2;
    
    uByte Sof_Modify_DetVolt_L1;
    uByte Sof_Modify_DetVolt_L2;
    uByte Sof_Modify_DetTemp_L1;
    uByte Sof_Modify_DetTemp_L2;
    uByte Sof_Modify_IntCanErr;
    uByte Sof_Pulse_Discharge_Cur_Time;
    uByte Sof_Pulse_FeedBack_Cur_Time; 
          
} sSOF_ConfigParaType;

/* end of define struct for macro and config parameter */

/* define struct for volt modify */
typedef struct
{ 
    uWord volt;
    uByte k;    
}sSOF_VoltLmtType;

extern const sSOF_VoltLmtType VoltLowArray[SOF_MAX_VOLT_POINT];
extern const sSOF_VoltLmtType VoltHighArray[SOF_MAX_VOLT_POINT];
extern const sByte DchTemperArray[SOF_MAX_T_POINT];
extern const sByte FbTemperArray[SOF_MAX_T_POINT];
extern const uWord SocArray[SOF_MAX_SOC_POINT];
extern const uWord CnstDisCurArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord CnstFeedbackCurArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord PulseDisCurArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord PulseFeedbackCurArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord CnstDisPowArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord CnstFeedbackPowArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord PulseDisPowArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const uWord PulseFeedbackPowArray[SOF_MAX_SOC_POINT][SOF_MAX_T_POINT];
extern const sSOF_MacroType SOF_Macro;
extern const sSOF_ConfigParaType SOF_ConfigPara;


#endif /* SOF_MACRO_H */