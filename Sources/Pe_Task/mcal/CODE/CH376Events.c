/** ###################################################################
**     Filename  : CH376Events.c
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : Events
**     Version   : Driver 01.04
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/10/10, 15:50
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         AM1_SD_OnError  - void AM1_SD_OnError(void);
**         AM1_SD_OnRxChar - void AM1_SD_OnRxChar(void);
**         AM1_SD_OnTxChar - void AM1_SD_OnTxChar(void);
**
** ###################################################################*/
/* MODULE CH376Events */


#include "Cpu.h"
#include "Events.h"
#include "TimerEvents.h"
#include "CAN0Events.h"
#include "CAN1Events.h"
#include "CAN4Events.h"
#include "AC_CP_IOCEvents.h"
#include "GPRSEvents.h"
#include "Ch376Events.h"
#include "GPSEvents.h"
#include "CH376.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

#pragma CODE_SEG DEFAULT

/*
** ===================================================================
**     Event       :  AM1_SD_OnError (module CH376Events)
**
**     Component   :  AM1_SD [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM1_SD_OnError(void)
{
  
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AM1_SD_OnRxChar (module CH376Events)
**
**     Component   :  AM1_SD [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM1_SD_OnRxChar(void)
{
      CH376_RxApp(); 
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AM1_SD_OnTxChar (module CH376Events)
**
**     Component   :  AM1_SD [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM1_SD_OnTxChar(void)
{

 CH376_TxApp();
  /* Write your code here ... */
}

/* END CH376Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
