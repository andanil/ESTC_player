#include "actions_indication.h"
#include "usbh_usr.h"
#include "player.h"

USB_OTG_CORE_HANDLE USB_OTG_Core;
USBH_HOST USB_Host;

int main(void)
{
  SystemInit(); 
  Indic_Init(500); 
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
  }
}
