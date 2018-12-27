// GeneralTypeDef.h
#ifndef GENERALYTPEDEF_H
#define GENERALYTPEDEF_H

#include "PE_Types.h"
#include "PE_Error.h"
#include <stddef.h>
#include <stdtypes.h>
#include "Cpu.h"
#include "EcuDebug.h"

#define uByte byte
#define uWord word
#define uLWord dword
typedef uWord GeneralReturnType;

#pragma DATA_SEG DEFAULT   

extern volatile byte CCR_reg;

#define ERR_MODULE_CODE_SIZE  0xFFU  // module have 256 error value
#define ERR_SUBMODULE_CODE_SIZE  0x1FU // submodule have 32 error value
/* define error code base for all module */
/*
 *  Bsw Error From 0x100 To 0x1FF
 */ 
#define ERR_BSW_BASE 0x100U
#define ERR_BSW_MAX  (ERR_BSW_BASE + ERR_MODULE_CODE_SIZE)
// For CanIf CanCom CanTp 0x100~0x11F
#define ERR_BSW_CAN_BASE ERR_BSW_BASE
#define ERR_BSW_CAN_MAX  (ERR_BSW_BASE + ERR_SUBMODULE_CODE_SIZE) 
// For Nvm EeIf FlashIf  0x120~13F
#define ERR_BSW_NVM_BASE (ERR_BSW_CAN_MAX + 1U)
#define ERR_BSW_NVM_MAX  (ERR_BSW_NVM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For IoHwAb    0x140~15F
#define ERR_BSW_IOHWAB_BASE (ERR_BSW_NVM_MAX + 1U)
#define ERR_BSW_IOHWAB_MAX  (ERR_BSW_IOHWAB_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved    0x160~ERR_BSW_MAX
#define ERR_BSW_RSV (ERR_BSW_IOHWAB_MAX + 1)
#if (ERR_BSW_RSV > ERR_BSW_MAX)
#error "Error Code overflow ERR_BSW_MAX, must less than ERR_BSW_MAX"
#endif
#define ERR_BSW_RSV_MAX  ERR_BSW_MAX 
/*
 *  CDD Error From  0x200 To 0x2FF
 */
#define ERR_CDD_BASE 0x200U
#define ERR_CDD_MAX  (ERR_CDD_BASE + ERR_MODULE_CODE_SIZE)
// For  CellvIf 0x200~0x21F
#define ERR_CDD_CELLVIF_BASE ERR_CDD_BASE
#define ERR_CDD_CELLVIF_MAX (ERR_CDD_BASE + ERR_SUBMODULE_CODE_SIZE)
// For TemperIf 0x220~0x23F
#define ERR_CDD_TEMPERIF_BASE (ERR_CDD_CELLVIF_MAX + 1U)
#define ERR_CDD_TEMPERIF_MAX (ERR_CDD_TEMPERIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For SumvIf   0x240~0x25F
#define ERR_CDD_SUMVRIF_BASE (ERR_CDD_TEMPERIF_MAX + 1U)
#define ERR_CDD_SUMVIF_MAX (ERR_CDD_SUMVRIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For CurrentIf 0x260~0x27F
#define ERR_CDD_CURRENTIF_BASE (ERR_CDD_SUMVIF_MAX + 1U)
#define ERR_CDD_CURRENTIF_MAX (ERR_CDD_CURRENTIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For IsoIf     0x280~0x29F
#define ERR_CDD_ISOIF_BASE (ERR_CDD_CURRENTIF_MAX + 1U)
#define ERR_CDD_ISOIF_MAX (ERR_CDD_ISOIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For ClockIf 0x2A0~0x2BF
#define ERR_CDD_CLOCKIF_BASE (ERR_CDD_ISOIF_MAX + 1)
#define ERR_CDD_CLOCKIF_MAX (ERR_CDD_CLOCKIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For BalIf 0x2C0~0x2DF
#define ERR_CDD_BALIF_BASE (ERR_CDD_CLOCKIF_MAX + 1U)
#define ERR_CDD_BALIF_MAX (ERR_CDD_BALIF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved 0x2E0~0x2FF
#define ERR_CDD_RSV (ERR_CDD_BALIF_MAX + 1U)
#if (ERR_CDD_RSV > ERR_CDD_MAX)
#error "Error Code overflow ERR_CDD_MAX, must less than ERR_CDD_MAX"
#endif
#define ERR_CDD_RSV_MAX ERR_CDD_MAX
/*
 * Sch Error From 0x300 To 0x3FF
 */
#define ERR_SCH_BASE 0x300U
#define ERR_SCH_MAX (ERR_SCH_BASE + ERR_MODULE_CODE_SIZE)
/*
 * Datalayer Error From 0x400 To 0x4FF
 */
#define ERR_DATALAYER_BASE 0x400U
#define ERR_DATALAYER_MAX  (ERR_DATALAYER_BASE + ERR_MODULE_CODE_SIZE)

/*
 * For reserved
 */
#define ERR_RSV_500_7FFF 0x500U
#if (ERR_DATALAYER_MAX > ERR_RSV_500_7FFF)
#error "Error Code overflow ERR_RSV_500_7FFF, need modify ERR_RSV_500_7FFF!"
#endif
#define ERR_RSV_500_7FFF_MAX 0x3FFFU
/*
 *  Application Error From 0x4000 To 0x7FFF
 */
#define ERR_APP_BASE 0x4000U
#define ERR_APP_MAX 0x7FFFU
/*
 *  Actor Error From 0x4000 To 0x40FF
 */
#define ERR_APP_ACTOR_BASE 0x4000U
#define ERR_APP_ACTOR_MAX (ERR_APP_ACTOR_BASE + ERR_MODULE_CODE_SIZE)
// For relay 0x4000~0x401F
#define ERR_APP_ACTOR_RELAY_BASE ERR_APP_ACTOR_BASE
#define ERR_APP_ACTOR_RELAY_MAX (ERR_APP_ACTOR_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserverd 0x4020~0x40FF
#define ERR_APP_ACTOR_RSV (ERR_APP_ACTOR_RELAY_MAX + 1U)
#define ERR_APP_ACTOR_RSV_MAX ERR_APP_ACTOR_MAX
/*
 * Sensor Error From 0x4100 To 0x41FF
 */
#define ERR_APP_SENSOR_BASE 0x4100U
#define ERR_APP_SENSOR_MAX (ERR_APP_SENSOR_BASE + ERR_MODULE_CODE_SIZE)
// For CellvDet 0x4100~0x411F
#define ERR_APP_SENSOR_CELLVDET_BASE (ERR_APP_SENSOR_BASE)
#define ERR_APP_SENSOR_CELLVDET_MAX (ERR_APP_SENSOR_CELLVDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For TemperDet 0x4120~0x413F
#define ERR_APP_SENSOR_TEMPERDET_BASE (ERR_APP_SENSOR_CELLVDET_MAX + 1U)
#define ERR_APP_SENSOR_TEMPERDET_MAX (ERR_APP_SENSOR_TEMPERDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For SumvDet 0x4140~0x415F
#define ERR_APP_SENSOR_SUMVDET_BASE (ERR_APP_SENSOR_TEMPERDET_MAX + 1U)
#define ERR_APP_SENSOR_SUMVDET_MAX (ERR_APP_SENSOR_SUMVDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For CurrentDet 0x4160~0x417F
#define ERR_APP_SENSOR_CURRENTDET_BASE (ERR_APP_SENSOR_SUMVDET_MAX + 1U)
#define ERR_APP_SENSOR_CURRENTDET_MAX (ERR_APP_SENSOR_CURRENTDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For IsoDet    0x4180~0x419F
#define ERR_APP_SENSOR_ISODET_BASE (ERR_APP_SENSOR_CURRENTDET_MAX + 1U)
#define ERR_APP_SENSOR_ISODET_MAX (ERR_APP_SENSOR_ISODET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For SupplyPowDet 0x41A0~0x41BF
#define ERR_APP_SENSOR_SUPPLYPOWDET_BASE (ERR_APP_SENSOR_ISODET_MAX + 1U)
#define ERR_APP_SENSOR_SUPPLYPOWDET_MAX (ERR_APP_SENSOR_SUPPLYPOWDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For GBChgDet 0x41C0~0x41DF
#define ERR_APP_SENSOR_GBCHGDET_BASE (ERR_APP_SENSOR_SUPPLYPOWDET_MAX + 1U)
#define ERR_APP_SENSOR_GBCHGDET_MAX (ERR_APP_SENSOR_GBCHGDET_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved 0x41E0~0x41FF
#define ERR_APP_SENSOR_RSV (ERR_APP_SENSOR_GBCHGDET_MAX + 1U)
#if (ERR_APP_SENSOR_RSV > ERR_APP_SENSOR_MAX)
#error "Error Code overflow ERR_APP_SENSOR_MAX, must less than ERR_APP_SENSOR_MAX"
#endif
#define ERR_APP_SENSOR_RSV_MAX ERR_APP_SENSOR_MAX
/*
 * Protocole Error From 0x4200 To 0x42FF
 */
#define ERR_PROTOCOL_BASE 0x4200U
#define ERR_PROTOCOL_MAX (ERR_PROTOCOL_BASE + ERR_MODULE_CODE_SIZE )
// For IncCAN 0x4200~0x421F
#define ERR_PROTOCOL_INTCAN_BASE ERR_PROTOCOL_BASE
#define ERR_PROTOCOL_INTCAN_MAX (ERR_PROTOCOL_BASE + ERR_SUBMODULE_CODE_SIZE)
// For CarCAN 0x4220~0x423F
#define ERR_PROTOCOL_CARCAN_BASE (ERR_PROTOCOL_INTCAN_MAX + 1U)
#define ERR_PROTOCOL_CARCAN_MAX (ERR_PROTOCOL_CARCAN_BASE + ERR_SUBMODULE_CODE_SIZE)
// For ChgCAN 0x4240~0x425F
#define ERR_PROTOCOL_CHGCAN_BASE (ERR_PROTOCOL_CARCAN_MAX + 1U)
#define ERR_PROTOCOL_CHGCAN_MAX (ERR_PROTOCOL_CHGCAN_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved 0x4260~0x42FF
#define ERR_PROTOCOL_RSV (ERR_PROTOCOL_CHGCAN_MAX + 1U)
#if (ERR_PROTOCOL_RSV > ERR_PROTOCOL_MAX)
#error "Error Code overflow ERR_PROTOCOL_MAX, must less than ERR_PROTOCOL_MAX"
#endif
#define ERR_PROTOCOL_RSVN_MAX ERR_PROTOCOL_MAX
/*
 * Data Analysis and Soc Sof Soh IntRes 
 * Error Form 0x4300~0x44FF
 */
#define ERR_DATA_BASE 0x4300U
#define ERR_DATA_MAX (ERR_DATA_BASE + (2 * ERR_MODULE_CODE_SIZE))
// For SystemSt 0x4300~0x431F
#define ERR_DATA_SYSTEMST_BASE ERR_DATA_BASE
#define ERR_DATA_SYSTEMST_MAX (ERR_DATA_SYSTEMST_BASE + ERR_SUBMODULE_CODE_SIZE)
// For ErrorAlarm 0x4320~0x433F
#define ERR_DATA_ERRORALARM_BASE (ERR_DATA_SYSTEMST_MAX + 1U)
#define ERR_DATA_ERRORALARM_MAX (ERR_DATA_ERRORALARM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For EE 0x4340~0x435F
#define ERR_DATA_EE_BASE (ERR_DATA_ERRORALARM_MAX + 1U)
#define ERR_DATA_EE_MAX (ERR_DATA_EE_BASE + ERR_SUBMODULE_CODE_SIZE)
// For DataStore 0x4360~0x437F
#define ERR_DATA_DATASTORE_BASE (ERR_DATA_EE_MAX + 1U)
#define ERR_DATA_DATASTORE_MAX (ERR_DATA_DATASTORE_BASE + ERR_SUBMODULE_CODE_SIZE)
// For soc 0x4380~0x439F
#define ERR_DATA_SOC_BASE (ERR_DATA_DATASTORE_MAX + 1U)
#define ERR_DATA_SOC_MAX (ERR_DATA_SOC_BASE + ERR_SUBMODULE_CODE_SIZE)
// For soh  0x43A0~0x43BF
#define ERR_DATA_SOH_BASE (ERR_DATA_SOC_MAX + 1U)
#define ERR_DATA_SOH_MAX (ERR_DATA_SOH_BASE + ERR_SUBMODULE_CODE_SIZE)
// For sof 0x43C0~0x43Df
#define ERR_DATA_SOF_BASE (ERR_DATA_SOH_MAX + 1U)
#define ERR_DATA_SOF_MAX (ERR_DATA_SOF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For IntRes 0x43E0~0x43FF
#define ERR_DATA_INTRES_BASE (ERR_DATA_SOF_MAX + 1U)
#define ERR_DATA_INTRES_MAX (ERR_DATA_INTRES_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved 0x4380~0x43FF
#define ERR_DATA_RSV (ERR_DATA_INTRES_MAX + 1U)
#if (ERR_DATA_RSV > ERR_DATA_MAX)                                      
#error "Error Code overflow ERR_DATA_MAX, must less than ERR_DATA_MAX"
#endif
#define ERR_DATA_RSV_MAX ERR_DATA_MAX
/*
 * Manage Error From 0x4500 To 0x45FF
 */
#define ERR_MANAGE_BASE 0x4500U
#define ERR_MANAGE_MAX (ERR_MANAGE_BASE + ERR_MODULE_CODE_SIZE)
// For thm 0x4500~0x451F
#define ERR_MANAGE_THM_BASE ERR_MANAGE_BASE
#define ERR_MANAGE_THM_MAX (ERR_MANAGE_THM_BASE + ERR_SUBMODULE_CODE_SIZE) 
// For chgm 0x4520~0x453F
#define ERR_MANAGE_CHGM_BASE (ERR_MANAGE_THM_MAX + 1U)
#define ERR_MANAGE_CHGM_MAX (ERR_MANAGE_CHGM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For hvm 0x4540~0x455F
#define ERR_MANAGE_HVM_BASE (ERR_MANAGE_CHGM_MAX + 1U)
#define ERR_MANAGE_HVM_MAX (ERR_MANAGE_HVM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For lvm 0x4560~0x457F
#define ERR_MANAGE_LVM_BASE (ERR_MANAGE_HVM_MAX  + 1U)
#define ERR_MANAGE_LVM_MAX (ERR_MANAGE_LVM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For balm 0x4580~0x459F
#define ERR_MANAGE_BALM_BASE (ERR_MANAGE_LVM_MAX + 1U)
#define ERR_MANAGE_BALM_MAX (ERR_MANAGE_BALM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For safem 0x45A0~0x45BF
#define ERR_MANAGE_SAFEM_BASE (ERR_MANAGE_BALM_MAX + 1U)
#define ERR_MANAGE_SAFEM_MAX (ERR_MANAGE_SAFEM_BASE + ERR_SUBMODULE_CODE_SIZE)
// For runmode 0x45C0~0x45DF
#define ERR_MANAGE_RUNMODE_BASE (ERR_MANAGE_SAFEM_MAX + 1U)
#define ERR_MANAGE_RUNMODE_MAX (ERR_MANAGE_RUNMODE_BASE + ERR_SUBMODULE_CODE_SIZE)
// For reserved 0x44E0~0x44FF
#define ERR_MANAGE_RSV (ERR_MANAGE_RUNMODE_MAX + 1U)
#if (ERR_MANAGE_RSV > ERR_MANAGE_MAX)                                      
#error "Error Code overflow ERR_MANAGE_MAX, must less than ERR_MANAGE_MAX"
#endif
#define ERR_MANAGE_RSV_MAX ERR_MANAGE_MAX
/*
 * Config Error From 0x4600 To 0x46FF
 */
#define ERR_CONFIG_BASE 0x4600U
#define ERR_CONFIG_MAX (ERR_CONFIG_BASE + ERR_MODULE_CODE_SIZE)
// For SysConf 0x4600~0x461F
#define ERR_CONFIG_SYSCONF_BASE ERR_CONFIG_BASE
#define ERR_CONFIG_SYSCONF_MAX (ERR_CONFIG_SYSCONF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For EcuConf 0x4620~0x463F
#define ERR_CONFIG_ECUCONF_BASE (ERR_CONFIG_SYSCONF_MAX + 1U)
#define ERR_CONFIG_ECUCONF_MAX (ERR_CONFIG_ECUCONF_BASE + ERR_SUBMODULE_CODE_SIZE)
// For version 0x4640~0x465F
#define ERR_CONFIG_VERSION_BASE (ERR_CONFIG_ECUCONF_MAX + 1U)
#define ERR_CONFIG_VERSION_MAX (ERR_CONFIG_VERSION_BASE + ERR_SUBMODULE_CODE_SIZE)
// For Reserved 0x4660~0x46FF
#define ERR_CONFIG_RSV (ERR_CONFIG_VERSION_MAX + 1U)
#if (ERR_CONFIG_RSV > ERR_CONFIG_MAX)                                      
#error "Error Code overflow ERR_CONFIG_MAX, must less than ERR_CONFIG_MAX"
#endif
#define ERR_CONFIG_RSV_MAX ERR_CONFIG_MAX
/*
 * Reserved For Other
 */
#define ERR_RSV_4700_FFFF 0x4700U
#if (ERR_CONFIG_RSV_MAX > ERR_RSV_4700_FFFF)                                      
#error "Error Code overflow ERR_RSV_4700_FFFF, need modify ERR_RSV_4700_FFFF!"
#endif
#define ERR_RSV_4600_FFFF_MAX 0x7FFFU


#endif