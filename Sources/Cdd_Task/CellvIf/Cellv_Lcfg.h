//Cellv_Lcfg.h
#ifndef CELLV_LCFG_H
#define CELLV_LCFG_H
          
#include "GeneralTypeDef.h"
#include "Ecu.h"
// Add define below

// Identify nubmer property
//#define REAL_CELLV_IC_MODULE_NUM    5
//#define MAX_CELLV_NUM_PER_IC       12

typedef struct
{
 const void (* CsPutValPtr)(bool val);
 const void (* SckPutValPtr)(bool val); 
 const bool (* SdoGetValPtr)(void);
 const void (* SdiPutValPtr)(bool val);
}Cellv_DeviceOperationType;


extern const Cellv_DeviceOperationType Cellv_DeviceOperation[];

#define GET_DEVICE_HANDLE(id) (&(Cellv_DeviceOperation[id]))

#endif
