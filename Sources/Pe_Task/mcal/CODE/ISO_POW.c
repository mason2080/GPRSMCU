/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : ISO_POW.c
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
**                       10            |  PK0_ADDR16_IQSTAT0
**             ----------------------------------------------------
**
**         Port name                   : K
**
**         Bit number (in port)        : 0
**         Bit mask of the port        : $0001
**
**         Initial direction           : Output (direction cannot be changed)
**         Initial output value        : 1
**         Initial pull option         : off
**
**         Port data register          : PORTK     [$0032]
**         Port control register       : DDRK      [$0033]
**
**         Optimization for            : speed
**     Contents  :
**         PutVal - void ISO_POW_PutVal(bool Val);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE ISO_POW. */

#include "ISO_POW.h"
  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"

#pragma DATA_SEG ISO_POW_DATA          /* Select data segment "ISO_POW_DATA" */
#pragma CODE_SEG ISO_POW_CODE
#pragma CONST_SEG ISO_POW_CONST        /* Constant section for this module */
/*
** ===================================================================
**     Method      :  ISO_POW_PutVal (component BitIO)
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
void ISO_POW_PutVal(bool Val)
{
  if (Val) {
    setReg8Bits(PORTK, 0x01U);         /* PK0=0x01U */
  } else { /* !Val */
    clrReg8Bits(PORTK, 0x01U);         /* PK0=0x00U */
  } /* !Val */
}


/* END ISO_POW. */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
