// CellvIf.c
#include "CellvIf_Type.h"
#include "CellvIf.h"
#include "Ecu.h"

// redefine ltc pin logic by hardware
/*#define CELLV_LTC_SDO(pDevice)       ((*(pDevice->SdoGetValPtr))())
#define CELLV_LTC_CS_SET(pDevice)  ((*(pDevice->CsPutValPtr))(FALSE))
#define CELLV_LTC_CS_CLR(pDevice)  ((*(pDevice->CsPutValPtr))(TRUE))
#define CELLV_LTC_SCK_SET(pDevice) ((*(pDevice->SckPutValPtr))(FALSE))
#define CELLV_LTC_SCK_CLR(pDevice) ((*(pDevice->SckPutValPtr))(TRUE))
#define CELLV_LTC_SDI_SET(pDevice) ((*(pDevice->SdiPutValPtr))(FALSE))
#define CELLV_LTC_SDI_CLR(pDevice) ((*(pDevice->SdiPutValPtr))(TRUE))
*/

#define BUSRATE_24MHZ
//#define BUSRATE_12MHZ
#ifdef BUSRATE_24MHZ
#define SCK_DELAY()  {asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP;asm NOP;}
#define CS_DELAY()  {asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;}//asm NOP; asm NOP;asm NOP; asm NOP;asm NOP; asm NOP;asm NOP;asm NOP;}
#define SDI_DELAY() {asm NOP; asm NOP;asm NOP; }
#endif

#ifdef BUSRATE_12MHZ
#define SCK_DELAY()  
#define CS_DELAY()  
#define SDI_DELAY() 
#endif

#define CELLV_LTC_SDO(pDevice)     ((*(pDevice->SdoGetValPtr))())
#define CELLV_LTC_CS_SET(pDevice) { ((*(pDevice->CsPutValPtr))(FALSE));CS_DELAY();}
#define CELLV_LTC_CS_CLR(pDevice)  ((*(pDevice->CsPutValPtr))(TRUE))
#define CELLV_LTC_SCK_SET(pDevice) {(clrReg8Bits(PTS, 0x40U));SCK_DELAY();}
#define CELLV_LTC_SCK_CLR(pDevice) {(setReg8Bits(PTS, 0x40U));SCK_DELAY();}
#define CELLV_LTC_SDI_SET(pDevice) {(clrReg8Bits(PTS, 0x20U));SDI_DELAY();}
#define CELLV_LTC_SDI_CLR(pDevice) {(setReg8Bits(PTS, 0x20U));SDI_DELAY();}


// define 5 Ltc6803 SDO
#define CELLV_LTC_SDO_PIN(port,mask) ((bool)((port)&(mask))) // direct use pin
#define CELLV_LTC_SDO_PIN_0()  CELLV_LTC_SDO_PIN(PTS,0x10U)
#define CELLV_LTC_SDO_PIN_1()  CELLV_LTC_SDO_PIN(PTS,0x04U)
#define CELLV_LTC_SDO_PIN_2()  CELLV_LTC_SDO_PIN(PTS,0x01U)
#define CELLV_LTC_SDO_PIN_3()  CELLV_LTC_SDO_PIN(PT0AD,0x40U)
#define CELLV_LTC_SDO_PIN_4()  CELLV_LTC_SDO_PIN(PT0AD,0x10U)


sCellvIf_CfgTYPE   CellvIf_Config[REAL_CELLV_IC_MODULE_NUM];

static uByte DeviveCalCrc(uByte *BufferPtr, uByte length)
{
    uByte lby_i;
    uByte lby_crc;
    uByte lby_len;
    uByte *lby_buff_ptr;

    __ASSERT(BufferPtr != NULL);
    
    lby_crc = 0x41;
    lby_len = length;
    lby_buff_ptr = BufferPtr;

    while(lby_len-- != 0)
    {
        for(lby_i = 0x80; lby_i != 0; lby_i = lby_i >> 1)
        {
            if((lby_crc & 0x80) != 0)
            {
                lby_crc = lby_crc << 1;
                lby_crc ^= 0x07;
            }
            else
            {
                lby_crc = lby_crc << 1;
            }
            if((*lby_buff_ptr & lby_i) != 0)
            {
                lby_crc ^= 0x07;
            }
        }
        lby_buff_ptr++;
    }

    return lby_crc;
}


static void WriteByteToDevice(eCellvIf_DeviceIdType DeviceId, uByte Wr_Data)
{
    uByte i;
    uByte lby_WrDataBuff;
    const Cellv_DeviceOperationType * pDevice;

    pDevice = GET_DEVICE_HANDLE(DeviceId);

//    __ASSERT(pDevice != NULL);

    lby_WrDataBuff = Wr_Data;

    for(i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        if((lby_WrDataBuff & 0x80) == 0x80)
        {
            CELLV_LTC_SDI_SET(pDevice);
        }
        else
        {
            CELLV_LTC_SDI_CLR(pDevice);
        }

        CELLV_LTC_SCK_SET(pDevice);

        asm NOP;
        asm NOP;

        lby_WrDataBuff = lby_WrDataBuff << 1;
    }

}

static void ReadByteFromDevice_0(uByte *Rd_DataPtr)
{
    uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;
    pDevice = GET_DEVICE_HANDLE(0);

//    __ASSERT(pDevice != NULL);
//    __ASSERT(Rd_DataPtr != NULL);
    
  /*  if(NULL == Rd_DataPtr)
    {
        return;
    } */

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO_PIN_0())//if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;
}

static void ReadByteFromDevice_1(uByte *Rd_DataPtr)
{
    uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;
    pDevice = GET_DEVICE_HANDLE(1);

//    __ASSERT(pDevice != NULL);
//    __ASSERT(Rd_DataPtr != NULL);
    
   /* if(NULL == Rd_DataPtr)
    {
       return;
    }  */

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO_PIN_1())//if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;
}

static void ReadByteFromDevice_2(uByte *Rd_DataPtr)
{
    uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;
    pDevice = GET_DEVICE_HANDLE(2);

//    __ASSERT(pDevice != NULL);
//    __ASSERT(Rd_DataPtr != NULL);
    
  /*  if(NULL == Rd_DataPtr)
    {
       return;
    } */

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO_PIN_2())//if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;
}

static void ReadByteFromDevice_3(uByte *Rd_DataPtr)
{
    uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;
    pDevice = GET_DEVICE_HANDLE(3);

//    __ASSERT(pDevice != NULL);
//    __ASSERT(Rd_DataPtr != NULL);
    
   /* if(NULL == Rd_DataPtr)
    {
        return;
    } */

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO_PIN_3())//if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;
}

static void ReadByteFromDevice_4(uByte *Rd_DataPtr)
{
    uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;
    pDevice = GET_DEVICE_HANDLE(4);

//    __ASSERT(pDevice != NULL);
//    __ASSERT(Rd_DataPtr != NULL);
    
   /* if(NULL == Rd_DataPtr)
    {
        return;
    } */

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO_PIN_4())//if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;
}


const static void (* ReadByteFuncPtr[5])(uByte *Rd_DataPtr) = 
{
    ReadByteFromDevice_0,
    ReadByteFromDevice_1,
    ReadByteFromDevice_2,
    ReadByteFromDevice_3,
    ReadByteFromDevice_4
};

static void ReadByteFromDevice(eCellvIf_DeviceIdType DeviceId, uByte *Rd_DataPtr)
{
    /*uByte i;
    uByte lby_RdDataBuff = 0;
    const Cellv_DeviceOperationType * pDevice;*/
    (*(ReadByteFuncPtr + DeviceId))(Rd_DataPtr);
   
   /* pDevice = GET_DEVICE_HANDLE(DeviceId);

    __ASSERT(pDevice != NULL);

    for (i = 0; i < 8; i++)
    {
        CELLV_LTC_SCK_CLR(pDevice);

        lby_RdDataBuff = lby_RdDataBuff << 1;

        CELLV_LTC_SCK_SET(pDevice);

        if(CELLV_LTC_SDO(pDevice))
        {
            lby_RdDataBuff |= 0x01;
        }
        else
        {
            lby_RdDataBuff &= 0xFE;
        }
    }

    *Rd_DataPtr = lby_RdDataBuff;*/ 
}


GeneralReturnType CellvIf_Init(eCellvIf_DeviceIdType DeviceId)
{
    uByte *p_char;
    uByte i, j, crc, pec;
    uByte buff[8];
    GeneralReturnType rslt;
    uByte by_cfg;
    const Cellv_DeviceOperationType * pDevice;
    
    if(DeviceId >= REAL_CELLV_IC_MODULE_NUM)
    {
        return ERR_CELLV_IDOUTRANG; 
    }

    pDevice = GET_DEVICE_HANDLE(DeviceId);

    __ASSERT(pDevice != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
    /* set PAD7 as input pin */
   // setReg16Bits(ATDDIEN, 0x0080U);  set in pe
    
    CellvIf_Config[DeviceId].cfg = 0x91;  
    CellvIf_Config[DeviceId].dcc_l = 0x00;  
    CellvIf_Config[DeviceId].dcc_h_mci_l = 0x00;  
    CellvIf_Config[DeviceId].mci_h = 0x00;  
    CellvIf_Config[DeviceId].vuv = 0x00;  
    CellvIf_Config[DeviceId].vov = 0xFF;  
                                          
    for(i = 0; i < 3; i++)
    {
        // Write config information to LTC6803
        CELLV_LTC_CS_CLR(pDevice);

        by_cfg = WRCFG;
        pec = DeviveCalCrc(&by_cfg, 1);

        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);

        p_char = (uByte *)(&CellvIf_Config[DeviceId]);
        pec = DeviveCalCrc(p_char, 6);
        crc = pec;

        for(j = 0; j < 6; j++)
        {
            WriteByteToDevice(DeviceId, *p_char++);
        }

        WriteByteToDevice(DeviceId, pec);

        CELLV_LTC_CS_SET(pDevice);
        // end of Write config information to LTC6803
        
        // Read config information from LTC6803
        CELLV_LTC_CS_CLR(pDevice);

        by_cfg = RDCFG;
        pec = DeviveCalCrc(&by_cfg, 1);
        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);

        for(j = 0; j < 7; j++)
        {
            ReadByteFromDevice(DeviceId, &buff[j]);
        }

        CELLV_LTC_CS_SET(pDevice);
        // end of Read config information from LTC6803

        if (crc == buff[6])
        {
            break;
        }
    }

    // Check initial operation whether is OK
    if (i >= 3)
    {
        rslt = (GeneralReturnType)ERR_CELLV_INIT;
    }

    return rslt;
}

GeneralReturnType CellvIf_StartCnvt(eCellvIf_DeviceIdType DeviceId,  uByte CnvtMode)
{
    uByte i, pec;
    uWord rslt;
    uByte by_cfg;
    const Cellv_DeviceOperationType * pDevice;

    if(DeviceId >= REAL_CELLV_IC_MODULE_NUM)
    {
        return ERR_CELLV_IDOUTRANG; 
    }
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);

    __ASSERT(pDevice != NULL);

    rslt = (GeneralReturnType)ERR_OK;

    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);

    for(i = 0; i < 3; i++)
    {
        // normal mode
        if (AD_CNVT_MODE_NORMAL == CnvtMode)
        {
            by_cfg = STCVAD;
            pec = DeviveCalCrc(&by_cfg, 1);
            WriteByteToDevice(DeviceId, by_cfg);
            WriteByteToDevice(DeviceId, pec);

            if (!CELLV_LTC_SDO(pDevice))
            {
                break;
            }
        }
        // diagnostice mode
        else if (AD_CNVT_MODE_DIAG == CnvtMode)
        {
            by_cfg = STCVDC;
            pec = DeviveCalCrc(&by_cfg, 1);
            WriteByteToDevice(DeviceId, by_cfg);
            WriteByteToDevice(DeviceId, pec);

            if (!CELLV_LTC_SDO(pDevice))
            {
                break;
            }
        }
        else
        {
            i = 3;  // to indicate operation error
        }
    }

    // release IC
    CELLV_LTC_CS_SET(pDevice);

    // Check initial operation whether is OK
    if (i >= 3)
    {
        rslt = (GeneralReturnType)ERR_CELLV_START;
    }

    return rslt;
}

GeneralReturnType CellvIf_RdValue(eCellvIf_DeviceIdType DeviceId, uWord *Rd_DataBuffPtr, uByte *LengthPtr)
{
    uByte i, j, pec;
    uByte buff[20];
    uWord volt;
    GeneralReturnType rslt;
    uByte by_cfg;
    const Cellv_DeviceOperationType * pDevice;

    if(DeviceId >= REAL_CELLV_IC_MODULE_NUM)
    {
        return ERR_CELLV_IDOUTRANG; 
    }
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);

    __ASSERT(pDevice != NULL);
    __ASSERT(Rd_DataBuffPtr != NULL);
    __ASSERT(LengthPtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
    /* check ADC whether is working */
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);
    
    by_cfg = PLADC;
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);

    if(!CELLV_LTC_SDO(pDevice))
    {
        ECU_DELAYMS(1);  //delay 1ms

        if (!CELLV_LTC_SDO(pDevice))
        {
            rslt = (GeneralReturnType)ERR_BUSY_CELLV;

            return rslt;
        }
    }

    // release IC
    CELLV_LTC_CS_SET(pDevice);
    /* end of check ADC */

    /* read voltage */
    for(i = 0; i < 3; i++)
    {
        // selcect IC
        CELLV_LTC_CS_CLR(pDevice);

        /* read cell voltage */
        by_cfg = RDCV;
        pec = DeviveCalCrc(&by_cfg, 1);
        WriteByteToDevice(DeviceId, RDCV);
        WriteByteToDevice(DeviceId, pec);

        for(j=0; j<19; j++)
        {
            ReadByteFromDevice(DeviceId, &buff[j]);
        }

        // release IC
        CELLV_LTC_CS_SET(pDevice);

        // check the data whether all are 0xFF
        pec = 0xFF;

        for(j = 0; j < 18; j++)
        {
            pec &= buff[j];
        }

        if(0xFF == pec)
        {
            continue;
        }

        // check the data whether all are 0x00
        pec = 0x00;

        for(j = 0; j < 18; j++)
        {
            pec |= buff[j];
        }

        if(0x00 == pec)
        {
            continue;
        }

        // check the CRC whether is OK?
        pec = DeviveCalCrc(buff, 18);

        if(pec == buff[18])
        {
            break;
        }
    }

    // Check read operation whether is OK
    if (i >= 3)
    {
        rslt = (GeneralReturnType)ERR_CELLV_RDCV;
    }
    else
    {
        // data result format translate
        j = 0;

        for (i = 0; i < 12; i++)
        {
            if (0 == (i % 2))
            {
                volt = buff[j];
                j++;
                volt += (((uWord)(buff[j] & 0x0f)) << 8);

                if (volt >= 0x200)
                {
                    volt -= 0x200;   //6803,ad offset = 0x200
                }

                volt *= 15;          // 1bit = 1.5mV, here voltage unit is 0.1mV, so need mux with 15
            }
            else
            {
                volt = (buff[j] & 0xf0) >> 4;
                j++;
                volt += (((uWord)buff[j]) << 4);
                j++;

                if (volt >= 0x200)
                {
                    volt -= 0x200;   //6803,ad offset = 0x200
                }

                volt *= 15;          // 1bit = 1.5mV, here voltage unit is 0.1mV, so need mux with 15
            }
            
            if(NULL != Rd_DataBuffPtr)
            {
                *Rd_DataBuffPtr = volt;
                Rd_DataBuffPtr++;
            }
        }

        if(NULL != LengthPtr)
        {
            *LengthPtr = 12;
        }
    }

    return rslt;
}

/*GeneralReturnType CellvIf_GetDeviceInfo(eCellvIf_DeviceIdType DeviceId, sDeviceInfo *DevicePtr)
{
    uByte rslt = 0;
    (void)rslt;
    (void)DeviceId;
    (void)DevicePtr; // prevent C5703:Parameter declared but no referenced

    // rslt = Cellv_Handle[IdNum].Cellv_GetDeviceInfoFun(DataPtr);

    return rslt;
} */

#if( 1 )


/*
Function name:  CellvIf_ConfigRegDiag
Description:    diagnostic the IC's configuration register
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
*/
static GeneralReturnType CellvIf_ConfigRegDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr)
{
    uByte i, pec;
    uByte buff[20];
    GeneralReturnType rslt;
    uByte by_cfg;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;

  /* if(NULL == ErrCodePtr)
   {
     return ERR_CELLV_NULL_POINTER; 
   } */

    // Config register statue check
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);    //Clear the CS level to low. This is reverse logic;
    
    by_cfg = RDCFG;                    // Read config information from LTC6803
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    for(i = 0; i < 7; i++)
    {
        ReadByteFromDevice(DeviceId, &buff[i]);
    }
    
    if(  (buff[0] != CellvIf_Config[DeviceId].cfg)
       ||(buff[1] != CellvIf_Config[DeviceId].dcc_l)
       ||(buff[2] != CellvIf_Config[DeviceId].dcc_h_mci_l)
       ||(buff[3] != CellvIf_Config[DeviceId].mci_h)
       ||(buff[4] != CellvIf_Config[DeviceId].vuv)
       ||(buff[5] != CellvIf_Config[DeviceId].vov) )
    {                                   //check config register is right?
        *ErrCodePtr = ERR_CELLV_CFG;
        rslt = (GeneralReturnType)ERR_CELLV_CFG;
    }
    else
    {
        *ErrCodePtr = ERR_OK;
    }
    // release IC
    CELLV_LTC_CS_SET(pDevice);   //Set the CS level to High. This is reverse logic;
    
    return rslt;
}

/*
Function name:  CellvIf_OpenWireDiag
Description:    diagnostic the IC's cell voltage wire
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
                FaultPosPtr: return which wire is open
*/
static GeneralReturnType CellvIf_OpenWireDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr, uWord *FaultPosPtr)
{

    uByte i, pec;
    uWord volt_buff_a[12], volt_buff_b[12], fault_pos;
    GeneralReturnType rslt;
    uByte by_cfg;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    __ASSERT(FaultPosPtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
 /*  if((NULL == ErrCodePtr) || (NULL == FaultPosPtr))
   {
       return ERR_CELLV_NULL_POINTER; 
   } */

    // Open Wire Diagnostic
    *ErrCodePtr = 0;
    *FaultPosPtr = 0;
    
    // step 1, read volt using STCAD
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);         //Clear the CS level to low. This is reverse logic;

    by_cfg = STCVAD;                        //start AD convert and read by using normal command
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);

    if (IO_HIGH_LEVEL == (*(pDevice->SdoGetValPtr))())
    {                                         //To check device response the AD start command
        rslt |= (GeneralReturnType)ERR_CELLV_START;
        
        // release IC
        CELLV_LTC_CS_SET(pDevice);  

        return rslt; 
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);  
   
    ECU_DELAYMS(15);  //delay 15ms
    
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice); 

    i = 0;
    if(ERR_OK != CellvIf_RdValue(DeviceId, volt_buff_a, &i))
    {
        rslt |= (GeneralReturnType)ERR_CELLV_RDCV;
        
        // release IC
        CELLV_LTC_CS_SET(pDevice);  

        return rslt; 
    }
    
    // step 2, read volt using STOWCAD
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);          //start open wire AD convert and read device's AD 

    by_cfg = STOWAD;
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);

    if (IO_HIGH_LEVEL == (*(pDevice->SdoGetValPtr))())
    {
        rslt |= (GeneralReturnType)ERR_CELLV_START;            //To check device response the AD start command
        
        // release IC
        CELLV_LTC_CS_SET(pDevice);  

        return rslt; 
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);            //Set the CS level to high. This is reverse logic;
   
    ECU_DELAYMS(15);  //delay 15ms
    
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);         //Clear the CS level to low. This is reverse logic;

    i = 0;
    if(ERR_OK != CellvIf_RdValue(DeviceId, volt_buff_b, &i))
    {
        rslt |= (GeneralReturnType)ERR_CELLV_RDCV;
        
        // release IC
        CELLV_LTC_CS_SET(pDevice);   // end of config register check

        return rslt; 
    }
    // release IC
    CELLV_LTC_CS_SET(pDevice);        //Set the CS level to high. This is reverse logic;

    
    // step3 check voltage of buff_a and buff_b whether is same?
    fault_pos = 0;
    
    if(volt_buff_b[0] < 0)
    {
        fault_pos |= WIRE_NUM_0;
    }
    
    if(volt_buff_b[11] < 0)
    {
        fault_pos |= WIRE_NUM_12;
    }
    
    for(i = 0; i < 10; i++)
    {
        if ( (volt_buff_b[i+1] > (volt_buff_a[i+1] + WIRE_OPEN_DELT_THRED)) 
        || (volt_buff_b[i+1] >= WIRE_OPEN_MAX_THRED) ) 
        {
            fault_pos |= (0x0001<<i);        //record the fault cell postion
        }
    } 
    
    *FaultPosPtr = fault_pos;
    
    return rslt;
    
    // end of open wire diagnostic
}

/*
Function name:  CellvIf_AdcDiag
Description:    diagnostic the IC's device's ADC
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
*/
static GeneralReturnType CellvIf_AdcDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr)
{

    uByte by_cfg;
    uByte i, j, crc, pec;
    uByte buff[20];
    GeneralReturnType rslt;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);

    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
   /* if(NULL == ErrCodePtr) 
    {
       return ERR_CELLV_NULL_POINTER;
    }*/

    // IC's ADC Diagnostic, using clear command
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);     //Clear the CS level to low. This is reverse logic;
    
    by_cfg = CLEARAD;                   //Send clear command to device
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);   //Set the CS level to high. This is reverse logic;
    
    ECU_DELAYMS(1);
                                        //Wait the device compelet the clear operation for 1ms
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);    //Clear the CS level to low. This is reverse logic;

    /* read cell voltage */
    by_cfg = RDCV;
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    for(i = 0; i < 19; i++)
    {
        ReadByteFromDevice(DeviceId, &buff[i]);
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice); 
    
    // check the data whether all are 0xFF
    pec = 0xFF;
    
    for(i = 0; i < 18; i++)
    {
        pec &= buff[i];
    }
    
    if(0xFF != pec)
    {
        rslt |= (GeneralReturnType)ERR_CELLV_NOT_MEASURE;
    }
    
    *ErrCodePtr = (uWord)rslt;
    
    return rslt;
}

/*
Function name:  CellvIf_BalanceCircuitDiag
Description:    diagnostic the IC's inactive balance circuit
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                CellNum :   tell function how mang cell need to be diagnosticed
                ErrCodePtr: error code of function.
                FaultPosPtr: return which wire is open
*/
static GeneralReturnType CellvIf_BalanceCircuitDiag(eCellvIf_DeviceIdType DeviceId, uByte CellNum, uWord *ErrCodePtr, uWord *FaultPosPtr)
{
    // Balance circuit open or short Diagnostic
    uByte i,j,length = 0;
    GeneralReturnType rslt;
    uWord fault_pos = 0;
    uWord tmp_wd_volt[MAX_CELLV_NUM_PER_IC];
    uWord tmp_wd_volt_bak[MAX_CELLV_NUM_PER_IC];
    
    __ASSERT(ErrCodePtr != NULL);
    __ASSERT(FaultPosPtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;

    /*if((NULL == ErrCodePtr) || (NULL == FaultPosPtr))
    {
      return ERR_CELLV_NULL_POINTER;
    } */
    
    i = CellNum;
    tmp_wd_volt_bak[i] = 0;
    tmp_wd_volt[i] = 0;
     
    (void)CellvIf_StartCnvt(DeviceId, AD_CNVT_MODE_NORMAL);
    
    ECU_DELAYMS(15);
    
    if ( ERR_OK == CellvIf_RdValue(DeviceId, tmp_wd_volt_bak, &length) ) 
    {
        (void)CellvIf_CtrlBalanceSw(DeviceId, i, DCH_BALANCE_ON);    // enable all balance on
        
        (void)CellvIf_StartCnvt(DeviceId, AD_CNVT_MODE_DIAG);
        
        ECU_DELAYMS(20);
        
        if ( ERR_OK == CellvIf_RdValue(DeviceId, tmp_wd_volt, &length) ) 
        {
            if ( ( (tmp_wd_volt[i] + BALANCE_FAULT_DELT_VOLT) > tmp_wd_volt_bak[i] )
                 && (tmp_wd_volt[i] >= BALANCE_FAULT_VOLT_THRD) )
            {
                fault_pos |= (0x0001<<i);
            }
        }
        
        CellvIf_CtrlBalanceSw(DeviceId, i, DCH_BALANCE_OFF);    // enable all balance on
        
        if (0 != fault_pos)
        {
            *ErrCodePtr = ERR_BALANCE_DCH; 
            
            *FaultPosPtr = fault_pos;
        }
        else
        {
            *ErrCodePtr = 0;
        }
    }
    else
    {
        rslt |= (GeneralReturnType)ERR_BALANCE_DIAG;
    }
    
    return rslt;
}

/*
Function name:  CellvIf_DeviceSelfTestDiag
Description:    diagnostic the IC's logic block
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
*/
static GeneralReturnType CellvIf_DeviceSelfTestDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr)
{

    uByte by_cfg;
    uByte i, j, crc, pec;
    uByte buff[20];
    GeneralReturnType rslt;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
   /* if(NULL == ErrCodePtr)
    {
      return ERR_CELLV_NULL_POINTER;
    } */

    // IC's ADC convert logic Diagnostic, using self test command
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);    //Clear the CS level to low. This is reverse logic;
    
    by_cfg = TESTAD1;                 // Set self-test command 
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);   //Set the CS level to high. This is reverse logic;
    
    ECU_DELAYMS(15);                //wait the TEST command finish for 15ms
    
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice); 

    /* read cell voltage */
    by_cfg = RDCV;
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    for(j = 0; j < 19; j++)
    {
        ReadByteFromDevice(DeviceId, &buff[j]);
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice); 
    
    // check the data whether all are 0xFF
    pec = 0xFF;
    
    for(j = 0; j < 18; j++)
    {
        pec &= buff[j];
    }
    
    if(0x55 != pec)
    {
        rslt |= (GeneralReturnType)ERR_CELLV_SLEFTEST;  
    }
    
    *ErrCodePtr = (uWord)rslt;
    
    return rslt;
}

/*
Function name:  CellvIf_RefDiag
Description:    diagnostic the IC's REF, you can use it check device's accuracy
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
*/
static GeneralReturnType CellvIf_RefDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr)
{
    uByte by_cfg;
    uByte i, pec;
    uByte buff[3];
    uWord ltmp_volt;
    GeneralReturnType rslt;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
   /* if(NULL == ErrCodePtr)
    {
      return ERR_CELLV_NULL_POINTER;
    } */

    // IC's ADC Peripherals are working Diagnostic, using REF2 and diagnostic command
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);    //Clear the CS level to low. This is reverse logic;
    
    by_cfg = DAGN;                     // Set diagnostic-test command 
    pec = DeviveCalCrc(&by_cfg, 1);
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);   //Set the CS level to high. This is reverse logic;
    
    ECU_DELAYMS(20);
    
    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);    //Clear the CS level to low. This is reverse logic;

    /* read Diagnostic register */
    by_cfg = RDDGNR;
    pec = DeviveCalCrc(&by_cfg, 1);      //read diagnostic reg and check
    WriteByteToDevice(DeviceId, by_cfg);
    WriteByteToDevice(DeviceId, pec);
    
    for(i = 0; i < 3; i++)
    {
        ReadByteFromDevice(DeviceId, &buff[i]);
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);   //Set the CS level to high. This is reverse logic;
    
    // check the data whether all are 0xFF
    pec = 0xFF;
    
    if ( (buff[1] & MUXFAIL) == MUXFAIL )
    {
        rslt |= ERR_CELLV_MUX; 
    }
    
    ltmp_volt = buff[1] & 0x0F;
    ltmp_volt <<= 8;
    ltmp_volt += buff[0];
    
    if (ltmp_volt >= 0x200)
    {
        ltmp_volt -= 0x200;  
    }   
    ltmp_volt *= 15;
    
    if ( (ltmp_volt < REF_MIN_VALUE) || (ltmp_volt > REF_MAX_VALUE) ) 
    {
        rslt |= (GeneralReturnType)ERR_CELLV_VREF; 
    }
    
    *ErrCodePtr = (uWord)rslt;
     
    return rslt;
}

/*
Function name:  CellvIf_GpioDiag
Description:    diagnostic the IC's logic using GPIO operation
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                ErrCodePtr: error code of function.
*/
static GeneralReturnType CellvIf_GpioDiag(eCellvIf_DeviceIdType DeviceId, uWord *ErrCodePtr)
{
    uByte by_cfg;
    uByte *p_char;
    uByte i, j, crc, pec;
    uByte buff[20];
    GeneralReturnType rslt;
    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);
    
    __ASSERT(pDevice != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    rslt = (GeneralReturnType)ERR_OK;
    
   /* if(NULL == ErrCodePtr)
    {
      return ERR_CELLV_NULL_POINTER;
    }*/
    
    // IC's Logic Work Diagnostic, using GPIO1 and GPIO2 setting logic
    
    CellvIf_Config[DeviceId].cfg = 0xB0; // set GPIO2 = 0; GPIO1 = 1;

    // selcect IC
    CELLV_LTC_CS_CLR(pDevice);     //Clear the CS level to low. This is reverse logic;
    
    for(i = 0; i < 3; i++)
    {
        // Write config information to LTC6803
        by_cfg = WRCFG;
        pec = DeviveCalCrc(&by_cfg, 1);
        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);
        
        p_char = (uByte *)(&CellvIf_Config[DeviceId]);
        crc = DeviveCalCrc(p_char, 6);
        for(j = 0; j < 6; j++)
        {
            WriteByteToDevice(DeviceId, *p_char++);
        }
        WriteByteToDevice(DeviceId, pec);
        
        // Read config information from LTC6803
        by_cfg = RDCFG;
        pec = DeviveCalCrc(&by_cfg, 1);
        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);
        
        for(j = 0; j < 7; j++)
        {
            ReadByteFromDevice(DeviceId, &buff[j]);
        }
        
        if (crc == buff[6])
        {
            break;
        }
    }
    
    if (0x20  ==  (buff[0] & 0x20)) // check GPIO2 == 0
    {
        rslt |= (GeneralReturnType)ERR_CELLV_GPIO;     //if not, GPIO error
    }
    
    // release IC
    CELLV_LTC_CS_SET(pDevice);    //Clear the CS level to low. This is reverse logic;
    
    *ErrCodePtr = (uWord)rslt;
    
    return rslt;
}

/*
Function name:  CellvIf_HwDiag
Description:    diagnostic interface for the IC 
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                HwDiagMode: chose different diagnostic mode
                CellNum: tell the function how many cells to be diagnostic
                ErrCodePtr: error code of function.
                FaultPosPtr: return diagnostic result 
*/
GeneralReturnType CellvIf_HwDiag(eCellvIf_DeviceIdType DeviceId, uByte HwDiagMode, uByte CellNum, uWord *ErrCodePtr, uWord *FaultPosPtr)
{
    uWord rslt = 0;
    
    __ASSERT(FaultPosPtr != NULL);
    __ASSERT(ErrCodePtr != NULL);
    
    switch(HwDiagMode)
    {
        case CELLV_DIAG_MODE_CONFIG:
        {
             if(ERR_OK != CellvIf_ConfigRegDiag(DeviceId, ErrCodePtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_OPENWIRE:
        {
             if(ERR_OK != CellvIf_OpenWireDiag(DeviceId, ErrCodePtr, FaultPosPtr))
             {
                 rslt |=(GeneralReturnType) ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_BALANCE:
        {
             if(ERR_OK != CellvIf_BalanceCircuitDiag(DeviceId, CellNum, ErrCodePtr, FaultPosPtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_ADC:
        {
             if(ERR_OK != CellvIf_AdcDiag(DeviceId, ErrCodePtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_SELFTEST:
        {
             if(ERR_OK != CellvIf_DeviceSelfTestDiag(DeviceId, ErrCodePtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_REF:
        {
             if(ERR_OK != CellvIf_RefDiag(DeviceId, ErrCodePtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        case CELLV_DIAG_MODE_GPIO:
        {
             if(ERR_OK != CellvIf_GpioDiag(DeviceId, ErrCodePtr))
             {
                 rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             }
             
             break;
        }
        default :
        {
             rslt |= (GeneralReturnType)ERR_CELLV_DIAG;
             
             break;
        }
    }
    
    return rslt;
}

/*
Function name:  CellvIf_CtrlBalanceSw
Description:    operate the inactive balance switch
Input Parameter:
                DeviceId:   the measure IC's encode, the first ic is 0, next is 1, and so on
                Row: the No. of cell to be operate
                Cmd: ON or OFF command for operation, for using CHG and DCH
*/
GeneralReturnType CellvIf_CtrlBalanceSw(eCellvIf_DeviceIdType DeviceId, uByte Row, uByte Cmd)
{

    uByte lby_num;
    uByte *p_char;
    uByte i, j, crc, pec;
    uWord rslt = 0;
    uByte by_cfg;
    uByte buff[7];

    const Cellv_DeviceOperationType * pDevice;
    
    pDevice = GET_DEVICE_HANDLE(DeviceId);   //find device's operation function Handle or Pointer

    __ASSERT(pDevice != NULL);
    
    if (Row < 8)                             //According the parameter, operate the channle of deveice to ON or OFF the balance.
    {                                        //and to update the congfig register of device.
        lby_num = Row;

        if (DCH_BALANCE_ON == Cmd)
        {
            CellvIf_Config[DeviceId].dcc_l = CellvIf_Config[DeviceId].dcc_l | (0x01 << lby_num);
        }
        else
        {
            CellvIf_Config[DeviceId].dcc_l = CellvIf_Config[DeviceId].dcc_l & (~(0x01 << lby_num));
        }

    }
    else if (Row < 16)
    {
        lby_num = Row - 8;

        if (DCH_BALANCE_ON == Cmd)
        {
            CellvIf_Config[DeviceId].dcc_h_mci_l = CellvIf_Config[DeviceId].dcc_h_mci_l | (0x01 << lby_num);
        }
        else
        {
            CellvIf_Config[DeviceId].dcc_h_mci_l = CellvIf_Config[DeviceId].dcc_h_mci_l & (~(0x01 << lby_num));
        }
    }
    else
    {
        return (GeneralReturnType)ERR_BALANCE_DCH;
    }
    
    for(i = 0; i < 3; i++)
    {
        // Write config information to LTC6803
        CELLV_LTC_CS_CLR(pDevice);

        by_cfg = WRCFG;
        pec = DeviveCalCrc(&by_cfg, 1);

        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);

        p_char = (uByte *)(&CellvIf_Config[DeviceId]);
        pec = DeviveCalCrc(p_char, 6);
        crc = pec;

        for(j = 0; j < 6; j++)
        {
            WriteByteToDevice(DeviceId, *p_char++);
        }

        WriteByteToDevice(DeviceId, pec);

        CELLV_LTC_CS_SET(pDevice);
        // end of Write config information to LTC6803

        // Read config information from LTC6803
        CELLV_LTC_CS_CLR(pDevice);

        by_cfg = RDCFG;
        pec = DeviveCalCrc(&by_cfg, 1);
        WriteByteToDevice(DeviceId, by_cfg);
        WriteByteToDevice(DeviceId, pec);

        for(j = 0; j < 7; j++)
        {
            ReadByteFromDevice(DeviceId, &buff[j]);
        }

        CELLV_LTC_CS_SET(pDevice);
        // end of Read config information from LTC6803

        if (crc == buff[6])
        {
            break;
        }
    }
    
    
    // Check initial operation whether is OK
    if (j >= 3)
    {
        rslt = (GeneralReturnType)ERR_BALANCE_DCH;           //If read AD fail count no smaller than 3, return cellv balance error code.
    }
    
    return rslt;
}


#endif