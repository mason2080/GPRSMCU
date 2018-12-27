/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : CPUEE.h
**     Project   : mcal
**     Processor : MC9S12XEP100MAG
**     Component : IntEEPROM
**     Version   : Component 02.200, Driver 01.25, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/7/12, 10:32
**     Abstract  :
**         This device "IntEEPROM" implements internal EEPROM
**     Comment   :
**         The EEPROM array is organized as rows of word (2 bytes), the EEPROM block's
**         erase sector size is 2 rows (2 words). Therefore it is preferable
**         to use word aligned data for writting - methods SetWord and SetLong -
**         with word aligned address.
**         Driver expects that all security options of EEPROM are disabled.
**         If some security option is enabled methods performing write
**         operation (such as SetWord) can return error.
**     Settings  :
**         EEPROM size                 : 32768 byte
**         Initialization:
**              Wait in methods        : Enabled
**              EEPROM clock           : High: 1000 kHz
**
**     Contents  :
**         SetByte   - byte CPUEE_SetByte(CPUEE_TAddress Addr, byte Data);
**         GetByte   - byte CPUEE_GetByte(CPUEE_TAddress Addr, byte *Data);
**         SetWord   - byte CPUEE_SetWord(CPUEE_TAddress Addr, word Data);
**         GetWord   - byte CPUEE_GetWord(CPUEE_TAddress Addr, word *Data);
**         SetLong   - byte CPUEE_SetLong(CPUEE_TAddress Addr, dword Data);
**         GetLong   - byte CPUEE_GetLong(CPUEE_TAddress Addr, dword *Data);
**         Partition - byte CPUEE_Partition(void);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __CPUEE
#define __CPUEE

/* MODULE CPUEE. */

#include "Cpu.h"


#ifndef __BWUserType_CPUEE_TAddress
#define __BWUserType_CPUEE_TAddress
  /* User type for addressing of the EEPROM. Actual type depends on the CPU family. */
  typedef far word * far CPUEE_TAddress; /* Type of address to the EEPROM */
#endif

#pragma CODE_SEG CPUEE_CODE


/* Emulated EEPROM area start address (in the format used by component methods). */
#define CPUEE_EEE_START                 ((CPUEE_TAddress)0x0013F000UL)
#define CPUEE_EEE_START_INT             (0x0013F000UL)
/* Emulated EEPROM area last address (in the format used by component methods). */
#define CPUEE_EEE_END                   ((CPUEE_TAddress)0x0013FFFFUL)
#define CPUEE_EEE_END_INT               (0x0013FFFFUL)
/* Emulated EEPROM area size (in bytes). */
#define CPUEE_EEE_SIZE                  0x1000U
/* Emulated EEPROM area sector size: minimal erasable unit (in bytes). */
#define CPUEE_EEE_SECTOR_SIZE           0x01U
/* EEPROM area start address (in the format used by component methods). */
#define CPUEE_AREA_START                CPUEE_EEE_START
#define CPUEE_AREA_START_INT            CPUEE_EEE_START_INT
/* EEPROM area last address (in the format used by component methods). */
#define CPUEE_AREA_END                  CPUEE_EEE_END
#define CPUEE_AREA_END_INT              CPUEE_EEE_END_INT
/* EEPROM area size (in bytes). */
#define CPUEE_AREA_SIZE                 CPUEE_EEE_SIZE
/* EEPROM area sector size: minimal erasable unit (in bytes). */
#define CPUEE_AREA_SECTOR_SIZE          CPUEE_EEE_SECTOR_SIZE
/* TRUE if the component setting allows clear bits of already programmed flash memory location without destruction of the value in surrounding addresses by sector erase. */
#define CPUEE_ALLOW_CLEAR               (TRUE)
/* Size of programming phrase, i.e. number of bytes that must be programmed at once */
#define CPUEE_PROGRAMMING_PHRASE        (0x02U)

#define EEPROMStart CPUEE_AREA_START   /* [DEPRECATED] Start address of used EEPROM array */
#define EEPROMEnd CPUEE_AREA_END       /* [DEPRECATED] End address of used EEPROM array */

/* Flags parameter of OnError event can contain following error values (values may be OR-ed together) */
/* EEE Erase error */
#define CPUEE_ERASE_ERROR               (0x80U)
/* EEE Program error */
#define CPUEE_PROGRAM_ERROR             (0x40U)
/* EEE Protect violation error */
#define CPUEE_PROTECT_ERROR             (0x10U)
/* EEE error 1 */
#define CPUEE_ERR1_ERROR                (0x08U)
/* EEE error 0 */
#define CPUEE_ERR0_ERROR                (0x04U)
/* Double bit fault detect */
#define CPUEE_DOUBLE_FAULT              (0x02U)
/* Single bit fault detect */
#define CPUEE_SINGLE_FAULT              (0x01U)

byte CPUEE_SetByte(CPUEE_TAddress Addr,byte Data);
/*
** ===================================================================
**     Method      :  CPUEE_SetByte (component IntEEPROM)
**
**     Description :
**         This method writes a given byte to a specified address in
**         EEPROM. The method also sets address pointer for <SetActByte>
**         and <GetActByte> methods (applicable only if these methods
**         are enabled). The pointer is set to address passed as the
**         parameter.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - Address to EEPROM
**         Data            - Data to write
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - the component does not work
**                           in the active speed mode 
**                           - ERR_BUSY - device is busy 
**                           - ERR_VALUE - verification of written data
**                           failed (read value does not match with
**                           written value) 
**                           - ERR_NOTAVAIL - other device-specific
**                           error 
**                           - ERR_RANGE - parameter Addr is out of range
** ===================================================================
*/

byte CPUEE_GetByte(CPUEE_TAddress Addr,byte *Data);
/*
** ===================================================================
**     Method      :  CPUEE_GetByte (component IntEEPROM)
**
**     Description :
**         This method reads a byte from a specified EEPROM address.
**         The method also sets address pointer for <SetActByte> and
**         <GetActByte> methods (applicable only if these methods are
**         enabled). The pointer is set to address passed as the
**         parameter.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - EEPROM Address
**       * Data            - A pointer to the returned 8-bit data
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_BUSY - device is busy 
**                           - ERR_RANGE - parameter Addr is out of range
** ===================================================================
*/

byte CPUEE_SetWord(CPUEE_TAddress Addr,word Data);
/*
** ===================================================================
**     Method      :  CPUEE_SetWord (component IntEEPROM)
**
**     Description :
**         This method writes a given word (2 bytes) to the specified
**         address in EEPROM. The method also sets address pointer for
**         <SetActByte> and <GetActByte> methods (applicable only if
**         these methods are enabled). The pointer is set to address
**         passed as the parameter + 1.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - Address to EEPROM
**         Data            - Data to write
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - the component does not work
**                           in the active speed mode 
**                           - ERR_BUSY - device is busy 
**                           - ERR_VALUE - verification of written data
**                           failed (read value does not match with
**                           written value) 
**                           - ERR_NOTAVAIL - other device-specific
**                           error 
**                           - ERR_RANGE - selected address out of the
**                           selected address range
** ===================================================================
*/

byte CPUEE_GetWord(CPUEE_TAddress Addr,word *Data);
/*
** ===================================================================
**     Method      :  CPUEE_GetWord (component IntEEPROM)
**
**     Description :
**         This method reads a word (2 bytes) from the specified EEPROM
**         address. The method also sets address pointer for
**         <SetActByte> and <GetActByte> methods (applicable only if
**         these methods are enabled). The pointer is set to address
**         passed as the parameter + 1.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - Address to EEPROM
**       * Data            - Pointer to returned 16-bit data
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_BUSY - device is busy 
**                           - ERR_RANGE - selected address out of the
**                           selected address range
** ===================================================================
*/

byte CPUEE_SetLong(CPUEE_TAddress Addr,dword Data);
/*
** ===================================================================
**     Method      :  CPUEE_SetLong (component IntEEPROM)
**
**     Description :
**         This method writes a given long word (4 bytes) to the
**         specified address in EEPROM. The method also sets address
**         pointer for <SetActByte> and <GetActByte> methods
**         (applicable only if these methods are enabled). The pointer
**         is set to address passed as the parameter + 3.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - Address to EEPROM
**         Data            - Data to write
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - the component does not work
**                           in the active speed mode 
**                           - ERR_BUSY - device is busy 
**                           - ERR_VALUE - verification of written data
**                           failed (read value does not match with
**                           written value) 
**                           - ERR_NOTAVAIL - other device-specific
**                           error 
**                           - ERR_RANGE - selected address out of the
**                           selected address range
** ===================================================================
*/

byte CPUEE_GetLong(CPUEE_TAddress Addr,dword *Data);
/*
** ===================================================================
**     Method      :  CPUEE_GetLong (component IntEEPROM)
**
**     Description :
**         This method reads a long word (4 bytes) from the specified
**         EEPROM address. The method also sets address pointer for
**         <SetActByte> and <GetActByte> methods (applicable only if
**         these methods are enabled). The pointer is set to address
**         passed as the parameter + 3.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Addr            - Address to EEPROM
**       * Data            - A pointer to the returned 32-bit data
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_BUSY - device is busy 
**                           - ERR_RANGE - selected address out of the
**                           selected address range
** ===================================================================
*/

byte CPUEE_Partition(void);
/*
** ===================================================================
**     Method      :  CPUEE_Partition (component IntEEPROM)
**
**     Description :
**         The method checks current partitions of D-Flash and buffer
**         RAM used for Emulated EEPROM and compares it to "D-Flash
**         sector count (DFPART)" and "Buffer RAM sector count for
**         Emulated EEPROM (ERPART)" settings. If the settings equals
**         with current partition, the method just enables EEPROM
**         emulation. If the settings differ, the method proceeds to
**         full partition of D-Flash memory according to ERPART and
**         DFPART settings and enables Emulated EEPROM feature. Warning:
**         All data in D-Flash memory will be erased during full
**         partition process. Full partition command is only provided
**         if the cpu runs in unsecured special mode otherwise the
**         method returns error code (ERR_NOTAVAIL). If "<Partition
**         D-Flash in init>" property is set to yes, this method is
**         automatically called in init.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - the component does not work
**                           in the active speed mode 
**                           - ERR_BUSY - device is busy 
**                           - ERR_NOTAVAIL - command needed to complete
**                           requested operation is not available.
** ===================================================================
*/

void CPUEE_Init(void);
/*
** ===================================================================
**     Method      :  CPUEE_Init (component IntEEPROM)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

#pragma CODE_SEG DEFAULT

/* END CPUEE. */

#endif /* ifndef __CPUEE */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.04 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
