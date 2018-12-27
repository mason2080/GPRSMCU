/** ###################################################################
**     Filename  : RS485Events.c
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : Events
**     Version   : Driver 01.04
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2015-11-30, 15:11
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         CPU_RS485_OnError  - void CPU_RS485_OnError(void);
**         CPU_RS485_OnRxChar - void CPU_RS485_OnRxChar(void);
**         CPU_RS485_OnTxChar - void CPU_RS485_OnTxChar(void);
**
** ###################################################################*/
/* MODULE RS485Events */


#include "Cpu.h"
#include "Events.h"
#include "TimerEvents.h"
#include "CAN0Events.h"
#include "CAN1Events.h"
#include "CAN4Events.h"
#include "AC_CP_IOCEvents.h"
#include "RS485Events.h"

#include "APP485.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

#pragma CODE_SEG DEFAULT

/*
** ===================================================================
**     Event       :  CPU_RS485_OnError (module RS485Events)
**
**     Component   :  CPU_RS485 [AsynchroSerial]
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
void CPU_RS485_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  CPU_RS485_OnRxChar (module RS485Events)
**
**     Component   :  CPU_RS485 [AsynchroSerial]
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
void CPU_RS485_OnRxChar(void)
{
  /* Write your code here ... */
  CPU485_RxApp();
}

/*
** ===================================================================
**     Event       :  CPU_RS485_OnTxChar (module RS485Events)
**
**     Component   :  CPU_RS485 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CPU_RS485_OnTxChar(void)
{
  /* Write your code here ... */
  //CPU485_TxApp();
}

/* END RS485Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
