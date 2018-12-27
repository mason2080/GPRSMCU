// HvM.c
#include "HvM.h"

/* Version Get Interface of Version Module  */
ModuleID_Type HvM_GetVesionModuleVer(sVersion_CodeType * VerPtr)
{      
    ModuleID_Type module_id;
    
#ifdef USE_HVM_A
    module_id = HvM_A_GetVesionModuleVer(VerPtr);
#endif
    
#ifdef USE_HVM_B
    module_id = HvM_B_GetVesionModuleVer(VerPtr);
#endif

    return module_id;   
}

/* module initial */
void HvM_Init(void)
{
    
}



/* module count call back */
void HvM_MainCountCbk(void)
{
    HvM_ModuleCountCbk();
}

/* main call back */
void HvM_MainCbk(void)
{
    HvM_ModuleCbk();
}
