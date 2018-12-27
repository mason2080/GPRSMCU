//CellvIf_Type.h
#ifndef CELLVIF_TYPE_H
#define CELLVIF_TYPE_H

// Add define below
#include "Cellv_Lcfg.h"


#define  ERR_CRC_CELLV   ERR_CDD_CELLVIF_BASE               //CRC error 
#define  ERR_WR_CELLV    (ERR_CDD_CELLVIF_BASE + 1)         //write error
#define  ERR_BUSY_CELLV  (ERR_CDD_CELLVIF_BASE + 2)         //busy
#define  ERR_CFG_CELLV   (ERR_CDD_CELLVIF_BASE + 3)         //configuration register error
#define  ERR_DCC_LTC     (ERR_CDD_CELLVIF_BASE + 4)         //discharge cell register error
#define  ERR_MCI_LTC     (ERR_CDD_CELLVIF_BASE + 5)         //mask cell interrupt error
#define  ERR_VUV_LTC     (ERR_CDD_CELLVIF_BASE + 6)         //undervoltage comparison voltage error
#define  ERR_VOV_LTC     (ERR_CDD_CELLVIF_BASE + 7)         //overvoltage comparison voltage error
#define  ERR_TYPE_BLC    (ERR_CDD_CELLVIF_BASE + 8)         //type of balance is error
#define  ERR_NOT_INIT    (ERR_CDD_CELLVIF_BASE + 9)  
#define  ERR_UNEXPECT     (ERR_CDD_CELLVIF_BASE + 10)       //unexpected error

#define  ERR_CELLV_INIT  (ERR_CDD_CELLVIF_BASE + 11)
#define  ERR_CELLV_START (ERR_CDD_CELLVIF_BASE + 12)
#define  ERR_CELLV_RDCV  (ERR_CDD_CELLVIF_BASE + 13)
#define  ERR_CELLV_CFG   (ERR_CDD_CELLVIF_BASE + 14)
#define  ERR_CELLV_NOT_MEASURE (ERR_CDD_CELLVIF_BASE + 15)
#define  ERR_CELLV_SLEFTEST    (ERR_CDD_CELLVIF_BASE + 16)
#define  ERR_CELLV_MUXFAIL     (ERR_CDD_CELLVIF_BASE + 17)
#define  ERR_CELLV_VREF_FAIL   (ERR_CDD_CELLVIF_BASE + 18)
#define  ERR_CELLV_GPIO        (ERR_CDD_CELLVIF_BASE + 19)
#define  ERR_BALANCE_DCH       (ERR_CDD_CELLVIF_BASE + 20)
#define  ERR_BALANCE_DIAG      (ERR_CDD_CELLVIF_BASE + 21)
#define  ERR_CELLV_MUX         (ERR_CDD_CELLVIF_BASE + 22)
#define  ERR_CELLV_VREF        (ERR_CDD_CELLVIF_BASE + 23)
#define  ERR_CELLV_DIAG        (ERR_CDD_CELLVIF_BASE + 24)
#define  ERR_CELLV_NULL_POINTER  (ERR_CDD_CELLVIF_BASE + 25)

#define  ERR_CELLV_IDOUTRANG   ERR_UNEXPECT//(ERR_CDD_CELLVIF_BASE + 25)
#define  ERR_CELLV_ROWOUTRANG   (ERR_CDD_CELLVIF_BASE + 26)
#define  ERR_CELLV_BALANCE_CMD   (ERR_CDD_CELLVIF_BASE + 27)
#define  MAX_CELLV_NUM_PER_CHIP  12

#if (ERR_CELLV_GPIO > ERR_CDD_CELLVIF_MAX)
#error "Error code over "ERR_CDD_CELLVIF_MAX" !"
#endif


typedef struct
{
    unsigned char DeviceType;   // IIC type, e.g LTC6802 -- 1; LTC6803-1 -- 2; LTC6804-1 -- 3;
    unsigned char DrvSwVesion;  // device software driver seri numeber
}sDeviceInfo;  

#endif
