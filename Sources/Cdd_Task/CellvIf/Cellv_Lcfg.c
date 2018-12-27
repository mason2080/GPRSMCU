//Cellv_Lcfg.c
#include "Cellv_Lcfg.h"

#include "CPU_SCK.h"
#include "CPU_MOSI.h"

//include mcal file from here
#if(REAL_CELLV_IC_MODULE_NUM > 0)
    #include "CPU_CS1.h"
    #include "CPU_MISO1.h"      
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 1)
    #include "CPU_CS2.h"
    #include "CPU_MISO2.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 2)
    #include "CPU_CS3.h"
    #include "CPU_MISO3.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 3)
    #include "CPU_CS4.h"
    #include "CPU_MISO4.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 4)
    #include "CPU_CS5.h"
    #include "CPU_MISO5.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 5)
    #include "CPU_CS6.h"
    #include "CPU_MISO6.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 6)
    #include "CPU_CS7.h"
    #include "CPU_MISO7.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 7)
    #include "CPU_CS8.h"
    #include "CPU_MISO8.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 8)
    #include "CPU_CS9.h"
    #include "CPU_MISO9.h"
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 9)
    #include "CPU_CS10.h"
    #include "CPU_MISO10.h"
#endif

 
#if(REAL_CELLV_IC_MODULE_NUM > 0)
static bool Cellv_DeviceSdo1(void)
{   
  return  CPU_MISO1_GetVal();
}
#endif

#if(REAL_CELLV_IC_MODULE_NUM > 1)
static bool Cellv_DeviceSdo2(void)
{
  return  CPU_MISO2_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 2)
static bool Cellv_DeviceSdo3(void)
{
  return  CPU_MISO3_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 3)
static bool Cellv_DeviceSdo4(void)
{
   return  CPU_MISO4_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 4)
static bool Cellv_DeviceSdo5(void)
{
   return  CPU_MISO5_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 5)
static bool Cellv_DeviceSdo6(void)
{
   return  CPU_MISO6_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 6)
static bool Cellv_DeviceSdo7(void)
{
   return  CPU_MISO7_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 7)
static bool Cellv_DeviceSdo8(void)
{
  return  CPU_MISO8_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 8)
static bool Cellv_DeviceSdo9(void)
{
  return  CPU_MISO9_GetVal();
}
#endif
#if(REAL_CELLV_IC_MODULE_NUM > 9)
static bool Cellv_DeviceSdo10(void)
{
   return  CPU_MISO10_GetVal();
}
#endif


   

const Cellv_DeviceOperationType Cellv_DeviceOperation[] = 
{
   #if(REAL_CELLV_IC_MODULE_NUM > 0)
   {           
      CPU_CS1_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo1,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
   #if(REAL_CELLV_IC_MODULE_NUM > 1)
   {           
      CPU_CS2_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo2,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
   #if(REAL_CELLV_IC_MODULE_NUM > 2)
   {           
      CPU_CS3_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo3,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 3)
   {           
      CPU_CS4_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo4,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 4)
   {           
      CPU_CS5_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo5,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 5)
   {           
      CPU_CS6_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo6,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 6)
   {           
      CPU_CS7_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo7,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 7)
   {           
      CPU_CS8_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo8,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 8)
   {           
      CPU_CS9_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo9,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
    #if(REAL_CELLV_IC_MODULE_NUM > 9)
   {           
      CPU_CS10_PutVal, // CsOperation
      CPU_SCK_PutVal,// SckOperation
      Cellv_DeviceSdo10,// SdoOperation
      CPU_MOSI_PutVal,// SdiOperation 
   },
   #endif
};
