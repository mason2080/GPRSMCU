/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : AM1_SD.h
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : AsynchroSerial
**     Version   : Component 02.494, Driver 01.22, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/10/10, 15:57
**     Abstract  :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings  :
**         Serial channel              : SCI6
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
**             Input buffer            : SCI6DRL   [$0337]
**             Output buffer           : SCI6DRL   [$0337]
**             Control register        : SCI6CR1   [$0332]
**             Mode register           : SCI6CR2   [$0333]
**             Baud setting reg.       : SCI6BD    [$0330]
**             Special register        : SCI6SR1   [$0334]
**
**         Input interrupt
**             Vector name             : Vsci6
**             Priority                : 4
**
**         Output interrupt
**             Vector name             : Vsci6
**             Priority                : 4
**
**         Used pins:
**         ----------------------------------------------------------
**           Function | On package           |    Name
**         ----------------------------------------------------------
**            Input   |     64               |  PH0_KWH0_MISO1_RxD6
**            Output  |     63               |  PH1_KWH1_MOSI1_TxD6
**         ----------------------------------------------------------
**
**
**
**     Contents  :
**         RecvChar - byte AM1_SD_RecvChar(AM1_SD_TComData *Chr);
**         SendChar - byte AM1_SD_SendChar(AM1_SD_TComData Chr);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __AM1_SD
#define __AM1_SD

/* MODULE AM1_SD. */

#include "Cpu.h"



#ifndef __BWUserType_AM1_SD_TError
#define __BWUserType_AM1_SD_TError
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
  } AM1_SD_TError;                     /* Error flags. For languages which don't support bit access is byte access only to error flags possible. */
#endif

#ifndef __BWUserType_AM1_SD_TComData
#define __BWUserType_AM1_SD_TComData
  typedef byte AM1_SD_TComData ;       /* User type for communication. Size of this type depends on the communication data witdh. */
#endif

#pragma DATA_SEG AM1_SD_DATA           /* Select data segment "AM1_SD_DATA" */
#pragma CODE_SEG AM1_SD_CODE



byte AM1_SD_RecvChar(AM1_SD_TComData *Chr);
/*
** ===================================================================
**     Method      :  AM1_SD_RecvChar (component AsynchroSerial)
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

byte AM1_SD_SendChar(AM1_SD_TComData Chr);
/*
** ===================================================================
**     Method      :  AM1_SD_SendChar (component AsynchroSerial)
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

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void AM1_SD_Interrupt(void);
#pragma CODE_SEG AM1_SD_CODE
/*
** ===================================================================
**     Method      :  AM1_SD_Interrupt (component AsynchroSerial)
**
**     Description :
**         The method services the receive interrupt of the selected 
**         peripheral(s) and eventually invokes the component's event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


void AM1_SD_Init(void);
/*
** ===================================================================
**     Method      :  AM1_SD_Init (component AsynchroSerial)
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
/* END AM1_SD. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/

#endif /* ifndef __AM1_SD */