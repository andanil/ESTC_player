#include "actions_indication.h"
#include "player.h"

extern uint16_t AUDIO_SAMPLE[];
extern size_t size;

int main(void)
{
  SystemInit(); 
  Indic_Init(1400-1, 30000-1); 
  PeriphInit(I2S_AudioFreq_48k);
  TIM5_Init();
  PlayByteArray(AUDIO_SAMPLE, 990000);
  while(1){}
}
