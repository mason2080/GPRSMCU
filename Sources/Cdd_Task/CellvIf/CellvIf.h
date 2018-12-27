// CellvIf.h
#ifndef CELLVIF_H
#define CELLVIF_H

#include "CellvIf_Type.h"

/* Add define below */
#define WRCFG   0x01      //write configuration register group
#define RDCFG   0x02      //read configuration register group
#define RDCV    0x04      //read cell voltage register group
#define RDFLG   0x06      //read flag register group
#define RDTMP   0x08      //read temperature register group
#define STCVAD  0x10      //start cell voltage A/D conversions and poll status
#define STOWAD  0x20      //start open wire A/D conversions and poll status
#define STTMPAD 0x30      //start temperature A/D conversions and poll status
#define PLADC   0x40      //poll A/D converter status
#define PLINT   0x50      //poll interrupt status
#define STCVDC  0x60      //start cell voltage A/D conversions and poll status,
                          //with discharge permitted
#define STOWDC  0x70      //start open wire A/D conversions and poll status,
                          //with discharge permitted
#define DAGN    0x52      //start diagnostic
#define RDDGNR  0x54      //read diagnostic register
#define CLEARAD 0x1D      //clear all voltage ADC result register to 0xFF
#define TESTAD1 0x1E
#define TESTAD2 0x1F 

/* define hardware diagnostic */
#define CELLV_DIAG_MODE_CONFIG       0x01
#define CELLV_DIAG_MODE_OPENWIRE     0x02
#define CELLV_DIAG_MODE_BALANCE      0x03
#define CELLV_DIAG_MODE_ADC          0x04
#define CELLV_DIAG_MODE_SELFTEST     0x05
#define CELLV_DIAG_MODE_REF          0x06
#define CELLV_DIAG_MODE_GPIO         0x07


/* define balance control commad */
#define DCH_BALANCE_OFF   0x10
#define DCH_BALANCE_ON    0x11

#define CHG_BALANCE_OFF   0x20
#define CHG_BALANCE_ON    0x21


#define IO_HIGH_LEVEL    1
#define IO_LOW_LEVEL     0  

#define MUXFAIL          0x20

#define AD_CNVT_MODE_NORMAL          0
#define AD_CNVT_MODE_DIAG            1       


#define WIRE_NUM_0                   0x1000
#define WIRE_NUM_1                   0x0001
#define WIRE_NUM_2                   0x0002
#define WIRE_NUM_3                   0x0004
#define WIRE_NUM_4                   0x0008
#define WIRE_NUM_5                   0x0010
#define WIRE_NUM_6                   0x0020
#define WIRE_NUM_7                   0x0040
#define WIRE_NUM_8                   0x0080
#define WIRE_NUM_9                   0x0100
#define WIRE_NUM_10                  0x0200
#define WIRE_NUM_11                  0x0400
#define WIRE_NUM_12                  0x0800

#define CELL_NUM_1                   0x0001
#define CELL_NUM_2                   0x0002
#define CELL_NUM_3                   0x0004
#define CELL_NUM_4                   0x0008
#define CELL_NUM_5                   0x0010
#define CELL_NUM_6                   0x0020
#define CELL_NUM_7                   0x0040
#define CELL_NUM_8                   0x0080
#define CELL_NUM_9                   0x0100
#define CELL_NUM_10                  0x0200
#define CELL_NUM_11                  0x0400
#define CELL_NUM_12                  0x0800

#define WIRE_OPEN_DELT_THRED         2000   //200mV
#define WIRE_OPEN_MAX_THRED          53750  //5.375V

#define BALANCE_FAULT_DELT_VOLT      2000   //200mV
#define BALANCE_FAULT_VOLT_THRD      30000  //3V 

#define REF_MAX_VALUE                29000
#define REF_MIN_VALUE                21000

typedef enum
{
  CELLVIF_LTC6803_1,
  CELLVIF_LTC6803_2,
  CELLVIF_LTC6803_3,
  CELLVIF_LTC6803_4,
  CELLVIF_LTC6803_5,
  CELLVIF_LTC6803_6,
  CELLVIF_LTC6803_7,
  CELLVIF_LTC6803_8,
  CELLVIF_LTC6803_9,
  CELLVIF_LTC6803_10,
}eCellvIf_DeviceIdType;

typedef struct
{
    uByte  cfg;
    uByte  dcc_l;
    uByte  dcc_h_mci_l;
    uByte  mci_h;
    uByte  vuv;
    uByte  vov;
}sCellvIf_CfgTYPE;  

extern sCellvIf_CfgTYPE   CellvIf_Config[REAL_CELLV_IC_MODULE_NUM];
              

GeneralReturnType CellvIf_Init(eCellvIf_DeviceIdType DeviceId);
GeneralReturnType CellvIf_StartCnvt(eCellvIf_DeviceIdType DeviceId,  uByte CnvtMode);
GeneralReturnType CellvIf_RdValue(eCellvIf_DeviceIdType DeviceId, uWord *Rd_DataBuffPtr,uByte *LengthPtr);
GeneralReturnType CellvIf_GetDeviceInfo(eCellvIf_DeviceIdType DeviceId, sDeviceInfo *DevicePtr);
GeneralReturnType CellvIf_HwDiag(eCellvIf_DeviceIdType DeviceId, uByte HwDiagMode, uByte CellNum, uWord *ErrCodePtr, uWord *FaultPosPtr);
GeneralReturnType CellvIf_CtrlBalanceSw(eCellvIf_DeviceIdType DeviceId, uByte Row, uByte Cmd);

#endif
