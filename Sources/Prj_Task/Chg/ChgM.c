// ChgM.c

#include "ChgM.h"

#ifdef USE_CHGM_B
#include"ChgM-B.h"
#include"GbChgM-B.h"
#endif

/* table for charge manage */
const sChg_CurTableType  DcChg_TemperCurTable[T_SEC] =
{
    {40,  0}, {45,  80}, {55, 80}, {85, 80},    /* Temper: unit: 1'C, Offset -40; Current: unit: 0.01C */
    {92, 0}, {100, 0}, {110, 0}, {250, 0}
};

const sChg_CurTableType AcChg_TemperCurTable[T_SEC] =
{
    {40,  0}, {50,  200}, {60, 400}, {75, 400},   /* Temper: unit: 1'C, Offset -40; Current: unit: 0.1A */
    {85, 100},{90, 0}, {110, 0}, {250, 0}
};




/* Module initial */
void ChgM_Init(void)
{
    /* ChgM module initial */
    ChgM_ModuleInit();
    
#ifdef ECU_M_ENABLE_GBCHGIF
    /* GB charge module init, if used */
    GbChgM_ModuleInit();
#endif        
}

/* Module count call back */
void ChgM_MainCountCbk(void)
{
    /* Count every 5ms */
    ChgM_ModuleCountCbk();
    
    /* GB charge control counter */
#ifdef ECU_M_ENABLE_GBCHGIF
    GbChgM_ModuleCountCbk();
#endif
}

/* Main call back */
void ChgM_MainCbk(void)
{
    ChgM_ModuleCbk();
    /* GB charge control */
#ifdef ECU_M_ENABLE_GBCHGIF
    GbChgM_ModuleCbk();
#endif
}
