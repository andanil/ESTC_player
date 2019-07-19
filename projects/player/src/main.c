#include "actions_indication.h"
#include "usbh_usr.h"
#include "player.h"

USB_OTG_CORE_HANDLE USB_OTG_Core;
USBH_HOST USB_Host;

struct List *first=0, *last=0, *current;
FRESULT fresult;
extern volatile uint8_t number_of_songs;
volatile uint8_t result = -1;

int main(void)
{
  Indic_Init(1400-1, 30000-1); 
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
    if(result == 1)
    {
      if(OpenDir(first, last, fresult, "//")==0 || first==0)
      {
        SetErrorLight();
      }
      else
      {
        last->next = first;
        current = first;
        PeriphInit();
        TIM5_Init();
        while(1)
        {
          SetPlayLight();
          PlayFile(current, fresult);
          current = current->next;
        }
      }
      result = -1;
    }
  }
}
