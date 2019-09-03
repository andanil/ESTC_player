#include "player.h"

static volatile uint8_t pause = AUDIO_MUTE_OFF, change_song = 0, volume = 50;
static volatile uint8_t number_of_songs = 0;

static ErrorCode file_status = Unvalid_RIFF_ID;
static uint8_t speech_data_offset = 0x00;
uint16_t buffer1[_MAX_SS] ={0x00};
uint16_t buffer2[_MAX_SS] ={0x00};
static uint32_t data_length = 0;
WAVE_FormatTypeDef wave_format;
static uint32_t XferCplt = 0;
static uint8_t buffer_switch = 1;
static FIL file;
UINT read_bytes;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
uint8_t isWAVFile(FILINFO fileInfo);

void Player_Stop(void)
{
  DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);
  DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, DMA_FLAG_TCIF7);
  I2S_Cmd(CODEC_I2S, DISABLE);
}

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

void PlayFile(char *file_name)
{
  buffer_switch = 1;
  if (f_open(&file, file_name, FA_READ) != FR_OK)
  {
      SetErrorLight();
  }
  else
  {  
    f_read (&file, buffer1, _MAX_SS, &read_bytes);
    file_status = WaveParsing(&speech_data_offset, buffer1);
    if (file_status == Valid_WAVE_File)  
    {
        data_length = wave_format.DataSize;
    }
    else 
    {
      while(1)
      {
        SetErrorLight();
      }
    }
    PeriphInit(wave_format.SampleRate);
    SetPlayLight(); 
    f_lseek(&file, speech_data_offset);
    f_read (&file, buffer1, _MAX_SS, &read_bytes); 
    f_read (&file, buffer2, _MAX_SS, &read_bytes);
    DMA_Play(buffer1, _MAX_SS);
    buffer_switch = 1;
    XferCplt = 0;
    while(data_length != 0)
    { 
      while(XferCplt == 0);
      XferCplt = 0;

      if(buffer_switch == 0)
      {
        DMA_Play(buffer1, _MAX_SS);
        f_read (&file, buffer2, _MAX_SS, &read_bytes);
        buffer_switch = 1;
      }
      else 
      {   
        DMA_Play(buffer2, 990000);
        f_read (&file, buffer1, _MAX_SS, &read_bytes);
        buffer_switch = 0;
      } 
    }
  }
  //Player_Stop();
}

void PlayByteArray(uint16_t *begin_pos, uint32_t size)
{
  Codec_SetVolume(VOLUME_CONVERT(volume));
  SetPlayLight();
  DMA_Play(begin_pos, size);
}

void Player_TransferComplete_CallBack(uint16_t *begin_pos, uint32_t size)
{
#ifdef MEDIA_IntFLASH
  DMA_Play(begin_pos, size); 
#elif defined MEDIA_USB_KEY 
  XferCplt = 1;
  if (data_length) data_length -= _MAX_SS;
  if (data_length < _MAX_SS) data_length = 0;
#endif 
}