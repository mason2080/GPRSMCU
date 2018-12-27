/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : RELAY_DIG_ST34.c
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : BitIO
**     Version   : Component 02.075, Driver 03.14, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/7/12, 10:32
**     Abstract  :
**         This component "BitIO" implements an one-bit input/output.
**         It uses one bit/pin of a port.
**         Note: This component is set to work in Input direction only.
**         Methods of this component are mostly implemented as a macros
**         (if supported by target language and compiler).
**     Settings  :
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       85            |  PD6_DATA6
**             ----------------------------------------------------
**
**         Port name                   : D
**
**         Bit number (in port)        : 6
**         Bit mask of the port        : $0040
**
**         Initial direction           : Input (direction cannot be changed)
**         Initial output value        : 0
**         Initial pull option         : off
**
**         Port data register          : PORTD     [$0005]
**         Port control register       : DDRD      [$0007]
**
**         Optimization for            : speed
**     Contents  :
**         GetVal - bool RELAY_DIG_ST34_GetVal(void);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE RELAY_DIG_ST34. */

#include "RELAY_DIG_ST34.h"
  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"

#pragma DATA_SEG RELAY_DIG_ST34_DATA   /* Select data segment "RELAY_DIG_ST34_DATA" */
#pragma CODE_SEG RELAY_DIG_ST34_CODE
#pragma CONST_SEG RELAY_DIG_ST34_CONST /* Constant section for this module */
/*
** ===================================================================
**     Method      :  RELAY_DIG_ST34_GetVal (component BitIO)
**
**     Description :
**         This method returns an input value.
**           a) direction = Input  : reads the input value from the
**                                   pin and returns it
**           b) direction = Output : returns the last written value
**         Note: This component is set to work in Input direction only.
**     Parameters  : None
**     Returns     :
**         ---             - Input value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)

** ===================================================================
*/
/*
bool RELAY_DIG_ST34_GetVal(void)

**  This method is implemented as a macro. See RELAY_DIG_ST34.h file.  **
*/


/* END RELAY_DIG_ST34. */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
