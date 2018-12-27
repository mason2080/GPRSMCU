// HvM.h

#ifndef HVM_H
#define HVM_H

#include "HvM_Macro.h"

#ifdef USE_HVM_A
#include "HvM-A.h"
#endif

#ifdef USE_HVM_B
#include "HvM-B.h"
#endif


extern void HvM_Init(void);
extern void HvM_MainCountCbk(void);
extern void HvM_MainCbk(void);
extern ModuleID_Type HvM_GetVesionModuleVer(sVersion_CodeType * VerPtr);
         
#endif /* HVM_H */

