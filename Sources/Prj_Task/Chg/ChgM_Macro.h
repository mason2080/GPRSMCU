// ChgM_Macro.h
#ifndef CHGM_MACRO_H
#define CHGM_MACRO_H

#include "GeneralTypeDef.h"
#include "SchIf.h"
#include "BmsSysSt.h"

// select solution
#define USE_CHGM_B

/* config time count */
#define CHGM_CYCLE  TM_MS(50)

#define TIME_1S     1000
#define TIME_2S     2000
#define TIME_2S5    2500
#define TIME_3S     3000
#define TIME_5S     5000
#define TIME_10S    10000
#define TIME_30S    30000
#define TIME_1MIN   60000
#define TIME_3MIN   180000
#define TIME_5MIN   300000

#define CHGM_CNT_WAIT_READY    (TIME_2S5 / (CHGM_CYCLE * 5))
#define CHGM_CNT_CAN_ERR       (TIME_10S / (CHGM_CYCLE * 5))
#define CHGM_CNT_CHG_ERR       (TIME_2S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_STEP_CUR      (TIME_2S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_DOWN_CUR      (TIME_2S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_RLY_OFF       (TIME_1S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_RLY_ON        (TIME_1S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_CALC_CUR      (TIME_3S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_WAIT_STOP     (TIME_5S /  (CHGM_CYCLE * 5))

#define CHGM_CNT_RLY_OFF30     (TIME_30S /  (CHGM_CYCLE * 5))
#define CHGM_CNT_CHG_1MIN      (TIME_1MIN /  (CHGM_CYCLE * 5))
#define CHGM_CNT_CHG_3MIN      (TIME_3MIN /  (CHGM_CYCLE * 5))
#define CHGM_CNT_START_TIME    (TIME_5MIN /  (CHGM_CYCLE * 5))
/* end of config time count */


/* config funciton switch */
//#define CHG_DET_LIMIT_T    /* need detect limit temper when judge charge enable */
//#define CHG_DET_LOW_V      /* need detect cellv low and sumv low when judge charge enable */
//#define CHG_DET_RLY      /* need detect relay status when judge charge enable  */
//#define CHG_DET_CHG_CAN    /* need detect charge CAN connect when judge charge enable */
//#define CHG_DET_GB_PLUG_CUR /* need detect GB charge current */
#define CHG_CTL_STOP_CHG_CUR
//#define CHG_DET_CHARGER_OUT_CUR    /* need detect charger's max output current when calc pmt current */
//#define CHG_RELAY_CTRL
#define CHG_CALC_CUR_MODE1
//#define CHG_CALC_CUR_MODE2
/* end of config funciton switch */


/* config limit error parameter */
#define CHGM_ERR_MAX_CELLV      3720//3680     /* 3.8V */
#define CHGM_ERR_MIN_CELLV      2250   /* 1V, not use now, read from EE */

#define CHGM_ERR_MAX_SUMV       2240//2210     /* 390V */
#define CHGM_ERR_MIN_SUMV       1000     /* 100V */

//#define CHGM_ERR_MAX_TEMPER     100    /* 60'C, not use now, read from EE */
//#define CHGM_ERR_MIN_TEMPER     30     /* -10'C, not use now, read from EE */
/* end of config limit error parameter */


/* config down current parameter */
#define MINV_DOWN_CC_CUR   2000  /* 2.0V */
#define MAXV_DOWN_CC_CUR   3600  /* 3.6V */

#define AC_CUR_STEP  20  /* cur step 2A */
/* end of config down current parameter */


#define CHGM_JUDGE_CUR          300                  /* 30 A*/
#define CHGM_ERR_MAX_CUR        (OFFSET_CUR - 600)   /* 45A */
#define CHGM_DET_START_CHG_CUR  (OFFSET_CUR - 20)    /* 2A */

/*========================================================================
 Define for GB Charge
======================================================================== */
/* define the charge mode */
#define NO_CHG_CON        0				  //No charge connector
#define AC_CHG_MODE1_B    1					//ac charge mode1 connection type B
#define AC_CHG_MODE2_B    2					//ac charge mode2 connection type B
#define AC_CHG_MODE3_A    3					//ac charge mode3 connection type A
#define AC_CHG_MODE3_B    4					//ac charge mode3 connection type B
#define AC_CHG_MODE3_C    5					//ac charge mode3 connection type C
#define AC_CHG_MODE       6					//ac charge mode
#define DC_CHG_MODE       7					//dc charge mode
#define AC_CHG_CON_ERR    8         //ac charge connet error
#define AC_CHG_LOCK_ERR   9
#define CHG_MODE_CONFLICT 10


/* config CC and CC2 volt range  */
#define CC_V_L_16AMODE ((BmsSys_GetK30Volt() * 17 / 42) - 8)
#define CC_V_H_16AMODE ((BmsSys_GetK30Volt() * 17 / 42) + 8)

#define CC_V_L_32AMODE ((BmsSys_GetK30Volt() * 11 / 61) - 6)
#define CC_V_H_32AMODE ((BmsSys_GetK30Volt() * 11 / 61) + 6)

#define CC2_V_L        ((BmsSys_GetK30Volt() / 2) - 10)
#define CC2_V_H        ((BmsSys_GetK30Volt() / 2) + 10)
#define CON_CONFLICT_V ((BmsSys_GetK30Volt() * 3) / 4)

/* end of config CC and CC2 volt range */

typedef struct
{
    uByte    t;		   /* control variable, lby_max_t or lby_min_t, unit -40C  */
    uWord    Cur;		 /* pmt charge current which calc by temper, unit: 0.01C for Dc chg, 0.1 for ac chg*/
} sChg_CurTableType;

#define T_SEC  8

extern const sChg_CurTableType DcChg_TemperCurTable[T_SEC];
extern const sChg_CurTableType AcChg_TemperCurTable[T_SEC];

#endif /* CHGM_MACRO_H */
