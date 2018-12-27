/** ###################################################################
**     Filename  : GPRSEvents.c
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
**         AM2_GPRS_OnRxChar - void AM2_GPRS_OnRxChar(void);
**         AM2_GPRS_OnTxChar - void AM2_GPRS_OnTxChar(void);
**
** ###################################################################*/
/* MODULE GPRSEvents */


#include "Cpu.h"
#include "Events.h"
#include "TimerEvents.h"
#include "CAN0Events.h"
#include "CAN1Events.h"
#include "CAN4Events.h"
#include "AC_CP_IOCEvents.h"
#include "GPRSEvents.h"
#include "CH376Events.h"
#include "GPSEvents.h"
#include "GPRS.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

#pragma CODE_SEG DEFAULT

/*
** ===================================================================
**     Event       :  AM2_GPRS_OnRxChar (module GPRSEvents)
**
**     Component   :  AM2_GPRS [AsynchroSerial]
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
void AM2_GPRS_OnRxChar(void)
{
GPRS_RxApp();
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AM2_GPRS_OnTxChar (module GPRSEvents)
**
**     Component   :  AM2_GPRS [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AM2_GPRS_OnTxChar(void)
{
GPRS_TxApp();
  /* Write your code here ... */
}

/* END GPRSEvents */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
