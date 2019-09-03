#include "actions_indication.h"
#include "usbh_usr.h"

USB_OTG_CORE_HANDLE USB_OTG_Core;
USBH_HOST USB_Host;

#ifdef MEDIA_IntFLASH
extern uint16_t AUDIO_SAMPLE[];
extern size_t size;
#elif defined MEDIA_USB_KEY 
USBH_HOST USB_Host;
#endif 

int main(void)
{
  SystemInit(); 
  Indic_Init(500); 
#ifdef MEDIA_IntFLASH
USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
  PeriphInit(I2S_AudioFreq_48k);
  PlayByteArray(AUDIO_SAMPLE, 990000);
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
  }
  while(1){}
#elif defined MEDIA_USB_KEY 
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
  }
#endif
}
