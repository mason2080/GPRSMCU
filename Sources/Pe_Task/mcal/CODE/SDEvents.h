/** ###################################################################
**     Filename  : SDEvents.h
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

#ifndef __SDEvents_H
#define __SDEvents_H
/* MODULE SDEvents */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "wdi.h"
#include "Timer.h"
#include "CAN0.h"
#include "CAN1.h"
#include "CAN4.h"
#include "CPU_CHG_S.h"
#include "CPU_CHG_P.h"
#include "ISO_CTRL_POS.h"
#include "ISO_CTRL_NEG.h"
#include "ISO_POW.h"
#include "CPUDI1.h"
#include "CPUDI2.h"
#include "CPUDI3.h"
#include "CPUDI4.h"
#include "OV_Power.h"
#include "UV_Power.h"
#include "CPU_SUMV_SDA.h"
#include "CPU_SUMV_SCL.h"
#include "CPU_SUMV_EXT_SDA.h"
#include "CPU_SUMV_EXT_SCL.h"
#include "CPU_SUMV_EXT_RDY.h"
#include "SUMV_EXT_POW.h"
#include "ExternFlash_Rst.h"
#include "ExternFlash_Cs.h"
#include "ExternFlash_Sck.h"
#include "ExternFlash_Si.h"
#include "ExternFlash_So.h"
#include "ExternTimer_Int.h"
#include "ExternTimer_Scl.h"
#include "ExternTimer_Sda.h"
#include "PORTA0.h"
#include "PORTA1.h"
#include "PORTA2.h"
#include "CON1.h"
#include "Slaver_Pow_Ctrl.h"
#include "CPU_RLY_POS.h"
#include "CPU_RLY_NEG.h"
#include "CPU_RLY_PRE.h"
#include "CPU_RLY_CHG.h"
#include "CPU_RLY_CTRL.h"
#include "RELAY_DIG_ST34.h"
#include "RELAY_DIG_ST12.h"
#include "AI1.h"
#include "AI2.h"
#include "CHG_CAN_POW.h"
#include "CPUCAN_CAR_EN.h"
#include "CPUCAN_INT_EN.h"
#include "CPUCSO.h"
#include "CPUCSI.h"
#include "CPUCSCK.h"
#include "CPUCCS.h"
#include "ExtEE_Sck.h"
#include "ExtEE_Sda.h"
#include "CPU_CAR_CAN_STB.h"
#include "CPU_CAR_CAN_ERR.h"
#include "ExtEE_Wp.h"
#include "AC_CP_IOC.h"
#include "RELAY_CHG.h"
#include "RELAY_PRE.h"
#include "RELAY_NEG.h"
#include "RELAY_POS.h"
#include "CPUDO2.h"
#include "CPUDO1.h"
#include "CPU_RLY_HEAT.h"
#include "CPU_RLY_FAN.h"
#include "CPUEE.h"
#include "AM2_GPRS.h"
#include "AM1_SD.h"
#include "AM3_GPS.h"

#pragma CODE_SEG DEFAULT

void AM3_GPS_OnError(void);
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

void AM3_GPS_OnRxChar(void);
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

void AM3_GPS_OnTxChar(void);
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


/* END SDEvents */
#endif /* __SDEvents_H*/

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
