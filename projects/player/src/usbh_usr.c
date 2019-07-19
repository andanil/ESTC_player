#include "usbh_usr.h"

USBH_Usr_cb_TypeDef USR_Callbacks =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};

extern volatile uint8_t result;
FATFS fatfs;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;


void USBH_USR_Init(void)
{

}

void USBH_USR_DeviceAttached(void)
{
  SetFixErrorLight();
}

void USBH_USR_UnrecoveredError (void)
{

}

void USBH_USR_DeviceDisconnected (void)
{
  SetErrorLight();
}

void USBH_USR_ResetDevice(void)
{

}

void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if ((DeviceSpeed != HPRT0_PRTSPD_FULL_SPEED)&&(DeviceSpeed != HPRT0_PRTSPD_LOW_SPEED))
  {
  }
}

void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{

}

void USBH_USR_DeviceAddressAssigned(void)
{

}

void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
    USBH_InterfaceDesc_TypeDef *itfDesc, USBH_EpDesc_TypeDef *epDesc)
{

}

void USBH_USR_Manufacturer_String(void *ManufacturerString)
{

}

void USBH_USR_Product_String(void *ProductString)
{

}

void USBH_USR_SerialNum_String(void *SerialNumString)
{

}

void USBH_USR_EnumerationDone(void)
{
  USB_OTG_BSP_mDelay(500);
  USBH_USR_MSC_Application();
} 

void USBH_USR_DeviceNotSupported(void)
{
  SetErrorLight();
}

USBH_USR_Status USBH_USR_UserInput(void)
{
  return USBH_USR_RESP_OK;
}

void USBH_USR_OverCurrentDetected (void)
{

}

int USBH_USR_MSC_Application(void)
{
  if (f_mount( 0, &fatfs ) != FR_OK ) 
  {
    result = 0;
    SetErrorLight();
  }
  else
  {
    result = 1;
  }
  return 0;
}

void USBH_USR_DeInit(void)
{
}


