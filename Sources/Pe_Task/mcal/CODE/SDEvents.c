/** ###################################################################
**     Filename  : SDEvents.c
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : Events
**     Version   : Driver 01.04
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/10/10, 16:39
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         AM3_GPS_OnError  - void AM3_GPS_OnError(void);
**         AM3_GPS_OnRxChar - void AM3_GPS_OnRxChar(void);
**         AM3_GPS_OnTxChar - void AM3_GPS_OnTxChar(void);
**
** ###################################################################*/
/* MODULE SDEvents */


#include "Cpu.h"
#include "Events.h"
#include "TimerEvents.h"
#include "CAN0Events.h"
#include "CAN1Events.h"
#include "CAN4Events.h"
#include "AC_CP_IOCEvents.h"
#include "GPRSEvents.h"
#include "CH376Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

#pragma CODE_SEG DEFAULT

/*
** ===================================================================
**     Event       :  AM3_GPS_OnError (module SDEvents)
**
**     Component   :  AM3_GPS [AsynchroMaster]
**     Description :
**         This event is called when a channel error occurs (except the
**         error returned by a given method). The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM3_GPS_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AM3_GPS_OnRxChar (module SDEvents)
**
**     Component   :  AM3_GPS [AsynchroMaster]
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
void AM3_GPS_OnRxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AM3_GPS_OnTxChar (module SDEvents)
**
**     Component   :  AM3_GPS [AsynchroMaster]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM3_GPS_OnTxChar(void)
{
  /* Write your code here ... */
}

/* END SDEvents */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
