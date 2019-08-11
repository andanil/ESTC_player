#include "actions_indication.h"
#include "player.h"

extern uint16_t AUDIO_SAMPLE[];
extern size_t size;

int main(void)
{
  SystemInit(); 
  Indic_Init(500); 
  PeriphInit(I2S_AudioFreq_48k);
  PlayByteArray(AUDIO_SAMPLE, 990000);
  while(1){}
}
