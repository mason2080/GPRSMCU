/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : CPU_RS485.h
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : AsynchroSerial
**     Version   : Component 02.494, Driver 01.22, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2015-11-28, 9:51
**     Abstract  :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings  :
**         Serial channel              : SCI3
**
**         Protocol
**             Init baud rate          : 9600baud
**             Width                   : 8 bits
**             Stop bits               : 1
**             Parity                  : none
**             Breaks                  : Disabled
**             Input buffer size       : 0
**             Output buffer size      : 0
**
**         Registers
**             Input buffer            : SCI3DRL   [$00C7]
**             Output buffer           : SCI3DRL   [$00C7]
**             Control register        : SCI3CR1   [$00C2]
**             Mode register           : SCI3CR2   [$00C3]
**             Baud setting reg.       : SCI3BD    [$00C0]
**             Special register        : SCI3SR1   [$00C4]
**
**         Input interrupt
**             Vector name             : Vsci3
**             Priority                : 4
**
**         Output interrupt
**             Vector name             : Vsci3
**             Priority                : 4
**
**         Used pins:
**         ----------------------------------------------------------
**           Function | On package           |    Name
**         ----------------------------------------------------------
**            Input   |     118              |  PM6_RxCAN3_RxCAN4_RxD3
**            Output  |     117              |  PM7_TxCAN3_TxCAN4_TxD3
**         ----------------------------------------------------------
**
**
**
**     Contents  :
**         Enable          - byte CPU_RS485_Enable(void);
**         Disable         - byte CPU_RS485_Disable(void);
**         EnableEvent     - byte CPU_RS485_EnableEvent(void);
**         DisableEvent    - byte CPU_RS485_DisableEvent(void);
**         RecvChar        - byte CPU_RS485_RecvChar(CPU_RS485_TComData *Chr);
**         SendChar        - byte CPU_RS485_SendChar(CPU_RS485_TComData Chr);
**         GetCharsInRxBuf - word CPU_RS485_GetCharsInRxBuf(void);
**         GetCharsInTxBuf - word CPU_RS485_GetCharsInTxBuf(void);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __CPU_RS485
#define __CPU_RS485

/* MODULE CPU_RS485. */

#include "Cpu.h"



#ifndef __BWUserType_CPU_RS485_TError
#define __BWUserType_CPU_RS485_TError
  typedef union {
    byte err;
    struct {
      /*lint -save  -e46 Disable MISRA rule (6.4) checking. */
      bool OverRun  : 1;               /* Overrun error flag */
      bool Framing  : 1;               /* Framing error flag */
      bool Parity   : 1;               /* Parity error flag */
      bool RxBufOvf : 1;               /* Rx buffer full error flag */
      bool Noise    : 1;               /* Noise error flag */
      bool Break    : 1;               /* Break detect */
      bool LINSync  : 1;               /* LIN synchronization error */
      bool BitError  : 1;              /* Bit error flag - mismatch to the expected value happened. */
      /*lint -restore Enable MISRA rule (6.4) checking. */
    } errName;
  } CPU_RS485_TError;                  /* Error flags. For languages which don't support bit access is byte access only to error flags possible. */
#endif

#ifndef __BWUserType_CPU_RS485_TComData
#define __BWUserType_CPU_RS485_TComData
  typedef byte CPU_RS485_TComData ;    /* User type for communication. Size of this type depends on the communication data witdh. */
#endif

#pragma DATA_SEG CPU_RS485_DATA        /* Select data segment "CPU_RS485_DATA" */
#pragma CODE_SEG CPU_RS485_CODE


extern volatile bool CPU_RS485_EnEvent; /* Enable/Disable events */


byte CPU_RS485_Enable(void);
/*
** ===================================================================
**     Method      :  CPU_RS485_Enable (component AsynchroSerial)
**
**     Description :
**         Enables the component - it starts the send and receive
**         functions. Events may be generated
**         ("DisableEvent"/"EnableEvent").
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte CPU_RS485_Disable(void);
/*
** ===================================================================
**     Method      :  CPU_RS485_Disable (component AsynchroSerial)
**
**     Description :
**         Disables the component - it stops the send and receive
**         functions. No events will be generated.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

#define CPU_RS485_EnableEvent() (CPU_RS485_EnEvent = TRUE, ERR_OK) /* Set the flag "events enabled" */
/*
** ===================================================================
**     Method      :  CPU_RS485_EnableEvent (component AsynchroSerial)
**
**     Description :
**         Enables the events. This method is available if the
**         interrupt service/event property is enabled and at least one
**         event is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

#define CPU_RS485_DisableEvent() (CPU_RS485_EnEvent = FALSE, ERR_OK) /* Set the flag "events disabled" */
/*
** ===================================================================
**     Method      :  CPU_RS485_DisableEvent (component AsynchroSerial)
**
**     Description :
**         Disables the events. This method is available if the
**         interrupt service/event property is enabled and at least one
**         event is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte CPU_RS485_RecvChar(CPU_RS485_TComData *Chr);
/*
** ===================================================================
**     Method      :  CPU_RS485_RecvChar (component AsynchroSerial)
**
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/

byte CPU_RS485_SendChar(CPU_RS485_TComData Chr);
/*
** ===================================================================
**     Method      :  CPU_RS485_SendChar (component AsynchroSerial)
**
**     Description :
**         Sends one character to the channel. If the component is
**         temporarily disabled (Disable method) SendChar method only
**         stores data into an output buffer. In case of a zero output
**         buffer size, only one character can be stored. Enabling the
**         component (Enable method) starts the transmission of the
**         stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/

word CPU_RS485_GetCharsInRxBuf(void);
/*
** ===================================================================
**     Method      :  CPU_RS485_GetCharsInRxBuf (component AsynchroSerial)
**
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/

word CPU_RS485_GetCharsInTxBuf(void);
/*
** ===================================================================
**     Method      :  CPU_RS485_GetCharsInTxBuf (component AsynchroSerial)
**
**     Description :
**         Returns the number of characters in the output buffer. This
**         method is available only if the transmitter property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the output
**                           buffer.
** ===================================================================
*/

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void CPU_RS485_Interrupt(void);
#pragma CODE_SEG CPU_RS485_CODE
/*
** ===================================================================
**     Method      :  CPU_RS485_Interrupt (component AsynchroSerial)
**
**     Description :
**         The method services the receive interrupt of the selected 
**         peripheral(s) and eventually invokes the component's event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


void CPU_RS485_Init(void);
/*
** ===================================================================
**     Method      :  CPU_RS485_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and internal 
**         variables of the component. The method is called automatically 
**         as a part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


#pragma DATA_SEG DEFAULT               /* Select data segment "DEFAULT" */
#pragma CODE_SEG DEFAULT
/* END CPU_RS485. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/

#endif /* ifndef __CPU_RS485 */
