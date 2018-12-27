//#include "SpiIf_Type.h"
#include "SpiIf.h"

GeneralReturnType SpiIf_WriteReadByte(uByte SpiIf_Id,uByte cmd,uByte *Rd_DataPtr)
{
    uByte i;
    uByte tmp_wr,tmp_rd;
    GeneralReturnType rslt;
    
    rslt = (GeneralReturnType)ERR_OK;
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }
    

    tmp_wr = cmd; 
    tmp_rd = 0;
    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)(); //clear cs
    CPU_TLE_CS_PutVal(FALSE);
    for(i=0; i<8; i++)
    {
        //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)(); //set sck
        CPU_TLE_SCK_PutVal(TRUE);
        tmp_rd <<= 1;
        if ( (tmp_wr &0x80) == 0x80 )
        {
            //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr)(); //set si
            CPU_TLE_SDI_PutVal(TRUE);
        }
        else
        {
            //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr)();  //clear si
            CPU_TLE_SDI_PutVal(FALSE);
        }
        
        //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
        CPU_TLE_SCK_PutVal(FALSE);
        tmp_wr <<= 1;
        //if (!(*SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr)())
        if(!CPU_TLE_SDO_GetVal())
        {
            tmp_rd |= 0x01;
        }
    }
    if(Rd_DataPtr != NULL) 
    {  
        *Rd_DataPtr = tmp_rd;
    }
    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)(); //set cs
    CPU_TLE_CS_PutVal(TRUE);

    return rslt;
    
    
    
  /*  
    uByte i;
    uByte tmp_wr,tmp_rd;
    GeneralReturnType rslt;
    const sSpiIf_PropertyType *SpiIf_PropPtr;
    const sSpiIf_OperationType *SpiIf_OperaPtr;
    
    rslt = (GeneralReturnType)ERR_OK;
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }
    SpiIf_PropPtr = GET_SPIPROPERTY(SpiIf_Id); //point to property
    SpiIf_OperaPtr = GET_SPIOPERATION(SpiIf_Id);  //point to operation
    
    if((uByte)(SpiIf_PropPtr->SpiIf_IdNum) == SpiIf_Id)   //match the spi number
    {
        if((SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr != NULL)) 
        {
            tmp_wr = cmd; 
            tmp_rd = 0;
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)(); //clear cs
            
            for(i=0; i<8; i++)
            {
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)(); //set sck
                tmp_rd <<= 1;
                if ( (tmp_wr &0x80) == 0x80 )
                {
                    (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr)(); //set si
                }
                else
                {
                    (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr)();  //clear si
                }
                
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
                tmp_wr <<= 1;
                if (!(*SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr)())
                {
                    tmp_rd |= 0x01;
                }
            }
            if(Rd_DataPtr != NULL) 
            {  
                *Rd_DataPtr = tmp_rd;
            }
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)(); //set cs
        }
        else 
        {
            rslt = (GeneralReturnType)ERR_IO_FUNCTION_PTR_NULL;  //the function pointer is null
        }
    } 
    else 
    {
        rslt = (GeneralReturnType)ERR_IO_ID_NUM_NOTMATCH;   //the id number is not match
    }

    return rslt;
  */  
 
}

GeneralReturnType SpiIf_WriteByte(uByte SpiIf_Id,uByte cmd)
{
        uByte i;
    uByte tmp_wr;
    GeneralReturnType rslt;
    
    rslt = (GeneralReturnType)ERR_OK;
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }

    tmp_wr = cmd; 
    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)(); //clear cs
    CPU_TLE_CS_PutVal(FALSE);
    for(i=0; i<8; i++)
    {
        //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)(); //set sck
        CPU_TLE_SCK_PutVal(TRUE);
        if ( (tmp_wr &0x80) == 0x80 )
        {
            //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr)(); //set si
            CPU_TLE_SDI_PutVal(TRUE);
        }
        else
        {
            //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr)();  //clear si
            CPU_TLE_SDI_PutVal(FALSE);
        }
        
        //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
        CPU_TLE_SCK_PutVal(FALSE);
        tmp_wr <<= 1;
        
    }
    
    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)(); //set cs
    CPU_TLE_CS_PutVal(TRUE);

    return rslt;
   
   
   
   /* 
    uByte i;
    uByte tmp_wr;
    GeneralReturnType rslt;
    const sSpiIf_PropertyType *SpiIf_PropPtr;
    const sSpiIf_OperationType *SpiIf_OperaPtr;
    
    rslt = (GeneralReturnType)ERR_OK;
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }
    SpiIf_PropPtr = GET_SPIPROPERTY(SpiIf_Id); //point to property
    SpiIf_OperaPtr = GET_SPIOPERATION(SpiIf_Id);  //point to operation
    
    if((uByte)(SpiIf_PropPtr->SpiIf_IdNum) == SpiIf_Id)   //match the spi number
    {
        if((SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr != NULL)) 
        {
            tmp_wr = cmd; 
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)(); //clear cs
            
            for(i=0; i<8; i++)
            {
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)(); //set sck
                if ( (tmp_wr &0x80) == 0x80 )
                {
                    (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoSetValPtr)(); //set si
                }
                else
                {
                    (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSi.SpiIf_IoClrValPtr)();  //clear si
                }
                
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
                tmp_wr <<= 1;
                
            }
            
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)(); //set cs
        }
        else 
        {
            rslt = (GeneralReturnType)ERR_IO_FUNCTION_PTR_NULL;  //the function pointer is null
        }
    } 
    else 
    {
        rslt = (GeneralReturnType)ERR_IO_ID_NUM_NOTMATCH;   //the id number is not match
    }

    return rslt;
  */  
  
}

GeneralReturnType SpiIf_ReadByte(uByte SpiIf_Id,uByte *SpiIf_ValuePtr)
{
   uByte i;
    uByte tmp_rd;
    GeneralReturnType rslt;
    
    rslt = (GeneralReturnType)ERR_OK;
    
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }
    
    if(NULL == SpiIf_ValuePtr) 
    {
        rslt = (GeneralReturnType)ERR_IO_VALUE_PTR_NULL;
        return rslt;
    }
    

    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)();  //clear cs
     CPU_TLE_CS_PutVal(FALSE);
    tmp_rd = 0;
    for(i=0; i<8; i++)
    {
       // (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)();   //set sck
        CPU_TLE_SCK_PutVal(TRUE);
        tmp_rd <<= 1;
        
        //if((*SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr)())//read so
        if(CPU_TLE_SDO_GetVal())
        {
          tmp_rd |= 0x01;
        } 
        //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
         CPU_TLE_SCK_PutVal(FALSE);
    }
    *SpiIf_ValuePtr = tmp_rd;
    //(*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)();  //set cs
     CPU_TLE_CS_PutVal(TRUE);

    return rslt;
   
   
  /* 
    uByte i;
    uByte tmp_rd;
    GeneralReturnType rslt;
    const sSpiIf_PropertyType *SpiIf_PropPtr;   //point to property
    const sSpiIf_OperationType *SpiIf_OperaPtr; //point to operation
    
    rslt = (GeneralReturnType)ERR_OK;
    
    if(SpiIf_Id >= SPIIF_MAX_NUM) 
    {
        rslt = (GeneralReturnType)ERR_IO_RANGE;
        return rslt;  
    }
    
    if(NULL == SpiIf_ValuePtr) 
    {
        rslt = (GeneralReturnType)ERR_IO_VALUE_PTR_NULL;
        return rslt;
    }
    
    SpiIf_PropPtr = GET_SPIPROPERTY(SpiIf_Id);
    SpiIf_OperaPtr = GET_SPIOPERATION(SpiIf_Id);
    
    if((uByte)(SpiIf_PropPtr->SpiIf_IdNum) == SpiIf_Id)   //match the spi number
    { 
        if((SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr != NULL)
        && (SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr != NULL)) 
        {  
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoClrValPtr)();  //clear cs
            tmp_rd = 0;
            for(i=0; i<8; i++)
            {
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoSetValPtr)();   //set sck
                tmp_rd <<= 1;
                
                if((*SpiIf_OperaPtr->SpiIf_IoInPutOperationSo.SpiIf_IoGetValPtr)())//read so
                {
                  tmp_rd |= 0x01;
                } 
                (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationSck.SpiIf_IoClrValPtr)(); //clear sck
            }
            *SpiIf_ValuePtr = tmp_rd;
            (*SpiIf_OperaPtr->SpiIf_IoOutPutOperationCs.SpiIf_IoSetValPtr)();  //set cs
        }
        else 
        {
            rslt = (GeneralReturnType)ERR_IO_FUNCTION_PTR_NULL;   //the function pointer is null
        }
    }
    else 
    {
        rslt = (GeneralReturnType)ERR_IO_ID_NUM_NOTMATCH;     //the id number is not match
    }

    return rslt;
  */  
  
} 


