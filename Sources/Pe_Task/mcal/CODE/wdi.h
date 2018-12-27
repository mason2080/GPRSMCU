/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : wdi.h
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : BitIO
**     Version   : Component 02.075, Driver 03.14, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/7/12, 10:32
**     Abstract  :
**         This component "BitIO" implements an one-bit input/output.
**         It uses one bit/pin of a port.
**         Note: This component is set to work in Output direction only.
**         Methods of this component are mostly implemented as a macros
**         (if supported by target language and compiler).
**     Settings  :
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       37            |  PB5_ADDR5_IVD5
**             ----------------------------------------------------
**
**         Port name                   : B
**
**         Bit number (in port)        : 5
**         Bit mask of the port        : $0020
**
**         Initial direction           : Output (direction cannot be changed)
**         Initial output value        : 0
**         Initial pull option         : off
**
**         Port data register          : PORTB     [$0001]
**         Port control register       : DDRB      [$0003]
**
**         Optimization for            : speed
**     Contents  :
**         GetVal - bool wdi_GetVal(void);
**         PutVal - void wdi_PutVal(bool Val);
**         ClrVal - void wdi_ClrVal(void);
**         SetVal - void wdi_SetVal(void);
**         NegVal - void wdi_NegVal(void);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef wdi_H_
#define wdi_H_

/* MODULE wdi. */

  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"

#pragma CODE_SEG wdi_CODE
/*
** ===================================================================
**     Method      :  wdi_GetVal (component BitIO)
**
**     Description :
**         This method returns an input value.
**           a) direction = Input  : reads the input value from the
**                                   pin and returns it
**           b) direction = Output : returns the last written value
**         Note: This component is set to work in Output direction only.
**     Parameters  : None
**     Returns     :
**         ---             - Input value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)

** ===================================================================
*/
#define wdi_GetVal() ( \
    (bool)((getReg8(PORTB) & 0x20U))   /* Return port data */ \
  )

/*
** ===================================================================
**     Method      :  wdi_PutVal (component BitIO)
**
**     Description :
**         This method writes the new output value.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Val             - Output value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)
**     Returns     : Nothing
** ===================================================================
*/
void wdi_PutVal(bool Val);

/*
** ===================================================================
**     Method      :  wdi_ClrVal (component BitIO)
**
**     Description :
**         This method clears (sets to zero) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define wdi_ClrVal() ( \
    (void)clrReg8Bits(PORTB, 0x20U)    /* PB5=0x00U */ \
  )

/*
** ===================================================================
**     Method      :  wdi_SetVal (component BitIO)
**
**     Description :
**         This method sets (sets to one) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define wdi_SetVal() ( \
    (void)setReg8Bits(PORTB, 0x20U)    /* PB5=0x01U */ \
  )

/*
** ===================================================================
**     Method      :  wdi_NegVal (component BitIO)
**
**     Description :
**         This method negates (inverts) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define wdi_NegVal() ( \
    (void)invertReg8Bits(PORTB, 0x20U) /* PB5=invert */ \
  )

#pragma CODE_SEG DEFAULT

/* END wdi. */
#endif /* #ifndef __wdi_H_ */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
