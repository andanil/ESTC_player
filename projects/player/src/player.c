#include "player.h"

volatile uint8_t State, pause = AUDIO_MUTE_OFF, change_song = 0, volume = 50;
volatile uint8_t number_of_songs = 0;
uint8_t isWAVFile(FILINFO fileInfo);

uint8_t isWAVFile(FILINFO fileInfo)
{
  uint8_t i = 0;
  for (i = 0; i < 20; i++)
  {
    if(fileInfo.fname[i] == '.')
    {
      if(fileInfo.fname[i+1]=='W' && fileInfo.fname[i+2]=='A' && fileInfo.fname[i+3]=='V')
      {
        return 1;
      }
    }
  }
  return 0;
}

uint8_t OpenDir(struct List *first, struct List *last,  FRESULT fresult, const char *path)
{
  DIR Dir;
  FILINFO fileInfo;
  fresult = f_opendir(&Dir, path);
  if(fresult != FR_OK)
  {
	return 0;
  }
  number_of_songs=0;
  while(1)
  {
    fresult = f_readdir(&Dir, &fileInfo);
    if(fresult != FR_OK)
    {
	 return 0;
    }
    if(!fileInfo.fname[0])
    {
	 return 1;
    }
    if(isWAVFile(fileInfo)==1)
    {
	  if(number_of_songs==0)
	  {
		first=last=add(last, fileInfo);
	  }
	  else
	  {
		last=add(last, fileInfo);
	  }
	  number_of_songs++;
    }
  }
}

void PlayFile(struct List *song, FRESULT fresult)
{
  struct List *temporary_song=song;
  UINT read_bytes;
  FIL file;
  if( f_open( &file, temporary_song->file.fname, FA_READ) == FR_OK )
  {
    fresult = f_lseek(&file, 44);
    volatile ITStatus it_status;
    change_song=0;
    while(1)
    {
      if (DMA_Read_Send(fresult,0, it_status, read_bytes, DMA_FLAG_HTIF5, change_song)==0)
      {
	    break;
      }
      if (DMA_Read_Send(fresult, 1024, it_status, read_bytes, DMA_FLAG_TCIF5, change_song)==0)
      {
	    break;
      }
    }
    fresult = f_close(&file);
  }
}

void TIM5_Init(void)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  tim_struct.TIM_Period = 8400-1;
  tim_struct.TIM_Prescaler = 3000-1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &tim_struct);
  TIM_Cmd(TIM5, DISABLE);

  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = TIM5_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
} 

void TIM5_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM5, TIM_IT_Update)!= RESET)
  {		
    switch (State) 
    {
      case AUDIO_PAUSE:
        if (pause == AUDIO_MUTE_OFF)
        {
        	pause = AUDIO_MUTE_ON;
        	Codec_WriteRegister(CODEC_MAP_PWR_CTRL2, CODEC_MUTE_ON);
        	Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_DOWN);
        	SetPauseLight();
        }
        else
        {
        	pause = AUDIO_MUTE_OFF;
        	Codec_WriteRegister(CODEC_MAP_PWR_CTRL2, CODEC_HEADPHONE_DEVICE);
        	Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_ON);
        	SetPlayLight();
        }
        break;
      case AUDIO_NEXT:
        change_song = 1;
        break;
      case AUDIO_VOLUME_UP:
        volume += VolumeStep;
        Codec_SetVolume(volume);
        break;
      case AUDIO_VOLUME_DOWN:
        volume -= VolumeStep;
        Codec_SetVolume(volume);
        break;
    }
    State = -1;
    TIM_Cmd(TIM5, DISABLE);
    TIM_SetCounter(TIM5, 0);
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
  }
}