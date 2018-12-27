// HvM.h
#ifndef HVM_B_H
#define HVM_B_H

#include "GeneralTypeDef.h"
#include "Version_Type.h" 
#include "HvM_Macro.h" 

#define FLAG_OK 1
#define FLAG_FAIL 0


typedef union
{
    uWord wd;

    struct
    {
        uByte SumVolt       : 1;
        uByte BmsCanErr     : 1;
        uByte MinVolt       : 1;
        uByte IsoBat        : 1;

        uByte IsoNeg        : 1;
        uByte IsoPos        : 1;
        uByte NegPaste      : 1;
        uByte NegInvalid    : 1;

        uByte PrePaste      : 1;
        uByte PreInvalid    : 1;
        uByte PosPaste      : 1;
        uByte PosInvalid    : 1;

        uByte PreCircuitOff : 1;
        uByte LoadShort     : 1;
        uByte LoadOff       : 1;
        uByte ChgLink       : 1;
    } bit;

}uHvM_ErrCodeType;

typedef struct
{
    uHvM_ErrCodeType code;
}
sHvM_ErrType;

typedef struct
{
    uByte Cmd;
    uByte Life;
    uWord VcuSumV;
}sHvM_VcuInfoType;

//extern sHvM_VcuInfoType HvM_VcuInfo;
//extern sHvM_ErrType     HvM_Err;
extern uByte flag_inverter_err;
extern uByte flag_Chg_err;
extern uByte flag_Dhg_err;
//extern uByte flag_Paste_err;
extern uByte flag_PosPaste;

extern void HvM_ModuleCbk(void);
extern void HvM_ModuleCountCbk(void);
extern void HvM_UpdateVcuInfo(sHvM_VcuInfoType *p_vcu);
extern ModuleID_Type HvM_B_GetVesionModuleVer(sVersion_CodeType * VerPtr);
extern uByte Get_HvM_Ctrl_Step(void);         

#endif /* HVM_H */

