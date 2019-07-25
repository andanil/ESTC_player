#include "player.h"

static volatile uint8_t pause = AUDIO_MUTE_OFF, change_song = 0, volume = 50;
static volatile uint8_t number_of_songs = 0;
uint8_t isWAVFile(FILINFO fileInfo);

void Player_VolumeUp(void)
{
  volume += VolumeStep;
  Codec_SetVolume(VOLUME_CONVERT(volume));
}

void Player_VolumeDown(void)
{
  volume -= VolumeStep;
  Codec_SetVolume(VOLUME_CONVERT(volume));
}

void Player_Toggle(void)
{
  if (pause == AUDIO_MUTE_OFF)
  {
    pause = AUDIO_MUTE_ON;
    Codec_WriteRegister(CODEC_MAP_PWR_CTRL2, CODEC_MUTE_ON);
    Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_DOWN);
    NVIC_DisableIRQ(DMA1_Stream7_IRQn);
    SetPauseLight();
  }
  else
  {
    pause = AUDIO_MUTE_OFF;
    Codec_WriteRegister(CODEC_MAP_PWR_CTRL2, CODEC_HEADPHONE_DEVICE);
    Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_ON);
    NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    SetPlayLight();
  }
}

void Player_ChangeSong(void)
{
  change_song = 1;
}

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

uint8_t OpenDir(struct List *first, struct List *last,  FRESULT fresult, char *path)
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
    Codec_SetVolume(VOLUME_CONVERT(volume));
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

void PlayByteArray(uint16_t *begin_pos, uint32_t size)
{
  Codec_SetVolume(VOLUME_CONVERT(volume));
  SetPlayLight();
  Init_DMA_ForByteArray(begin_pos, size);
}