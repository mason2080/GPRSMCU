#ifndef APPCH376_H
#define APPCH376_H


#include "GeneralTypeDef.h"

extern uByte flag_Power_err;
//extern uByte flag_inverter_err;

extern void CH376_Init(void);
extern void CH376_MainCbk(void);
extern void CH376_RxApp(void);
extern void CH376_TxApp(void);
extern void CH376_CheckExist(void);
extern void CH376_MainCountCbk(void);

extern void CH376_CLOSE_FILE(void);

typedef enum {Idle,
              CHECK_EXIST,
              SET_USB_MODE,
              DISK_CONNECT,
              DISK_MOUNT,
              SET_FILE_NAME,
              CREATE_DIR,
              WRITE_VAR32,
              WRITE_CURRENT_CLUST,
              DIR_CREATE,
              FILE_CREATE,
              SET_DIR_NAME,
              DIR_OPEN,
              FILE_OPEN,
              SET_SIZE,
              GET_STATUS,
              SaveData,
              CLOSE_FILE,
              WRITE_BYTE,
              WR_REQ_DATA ,
              BYTE_LOCATE,
              UPDATE_LEN,
              BYTE_WR_GO,
              
              ENUM_FILE,
              DELETE_FILE,
              }CH376_Step;
              
extern CH376_Step ch376_Step;

#endif
