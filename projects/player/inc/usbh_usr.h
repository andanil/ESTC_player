#ifndef USH_USR_H
#define USH_USR_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "usbh_core.h"
#include <stdio.h>
#include "usbh_msc_core.h"
#include "actions_indication.h"
#include "player.h"

#define USH_USR_FS_INIT    ((uint8_t)0x00)
#define USH_USR_AUDIO      ((uint8_t)0x01)

extern  USBH_Usr_cb_TypeDef USR_Callbacks;

void USBH_USR_Init(void);
void USBH_USR_DeviceAttached(void);
void USBH_USR_ResetDevice(void);
void USBH_USR_DeviceDisconnected (void);
void USBH_USR_OverCurrentDetected (void);
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed); 
void USBH_USR_Device_DescAvailable(void *);
void USBH_USR_DeviceAddressAssigned(void);
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc);
void USBH_USR_Manufacturer_String(void *);
void USBH_USR_Product_String(void *);
void USBH_USR_SerialNum_String(void *);
void USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
int USBH_USR_MSC_Application(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceNotSupported(void);
void USBH_USR_UnrecoveredError(void);

#ifdef __cplusplus
}
#endif

#endif /*USH_USR_H*/