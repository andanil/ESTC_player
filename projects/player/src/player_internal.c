#include "player_internal.h"
#include "actions_indication.h"

void CodecI2S_Init(uint32_t AudioFreq);
void Codec_CS42L22Init(void);
void CodecGPIO_Init(void);
void CodecI2C_Init(void);
void Init_DMA(void);

static uint16_t* start_pos;
static volatile uint16_t* current_pos;
static uint32_t AudioTotalSize; 
static volatile uint32_t AudioRemSize;

DMA_InitTypeDef  DMA_InitStructure;

void Codec_SetVolume(uint8_t Volume)
{
  if (Volume > 0xE6)
  {
    Codec_WriteRegister(CODEC_MAP_MASTER_A_VOL, Volume - 0xE7);
    Codec_WriteRegister(CODEC_MAP_MASTER_B_VOL, Volume - 0xE7);
  }
  else
  {
    Codec_WriteRegister(CODEC_MAP_MASTER_A_VOL, Volume + 0x19);
    Codec_WriteRegister(CODEC_MAP_MASTER_B_VOL, Volume + 0x19);
  }
}

uint8_t DMA_Read_Send(FIL file, FRESULT fresult, uint16_t *begin_pos, volatile ITStatus it_status, uint32_t DMA_FLAG, uint8_t change_song)
{
  UINT read_bytes;
  it_status = RESET;
  while(it_status == RESET)
  {
	it_status = DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG);
  }
  fresult = f_read (&file,begin_pos,1024*2,&read_bytes);
  DMA_ClearFlag(DMA1_Stream5, DMA_FLAG);
  if(fresult != FR_OK)
  {
	return 0;
  }
  if(read_bytes<1024*2||change_song!=0)
  {
	return 0;
  }
  return 1;
}

uint8_t Codec_ReadRegister(uint32_t RegisterAddr)
{
  uint8_t receivedByte = 0;
  while (I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY)) { }
  I2C_GenerateSTART(CODEC_I2C, ENABLE); 
  while (!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_SB)) { }
  I2C_Send7bitAddress(CODEC_I2C, CODEC_I2C_ADDRESS, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) { }
  I2C_SendData(CODEC_I2C, RegisterAddr); 
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) { }
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);
  while (I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY)) { }
  I2C_AcknowledgeConfig(CODEC_I2C, DISABLE);
  I2C_GenerateSTART(CODEC_I2C, ENABLE);
 while (!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_SB)) { }
  I2C_Send7bitAddress(CODEC_I2C, CODEC_I2C_ADDRESS, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) { }
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) { }
  receivedByte = I2C_ReceiveData(CODEC_I2C);
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);
  return receivedByte;
}

uint8_t Codec_WriteRegister(uint32_t RegisterAddr, uint32_t RegisterValue)
{
  uint8_t result = 1;

  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY)) { }
  I2C_GenerateSTART(CODEC_I2C, ENABLE);
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT)) { }               // Test on EV5 and clear it 
  I2C_Send7bitAddress(CODEC_I2C, CODEC_I2C_ADDRESS, I2C_Direction_Transmitter);      // Transmit the slave address and enable writing operation 
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) { } // Test on EV6 and clear it 
  I2C_SendData(CODEC_I2C, RegisterAddr);                                             // Transmit the first address for write operation 
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) { }         // Test on EV8 and clear it 
  __disable_irq();                                                                   // Disable the interrupts mechanism to prevent the I2C communication from corruption 
  I2C_SendData(CODEC_I2C, RegisterValue);                                            // Prepare the register value to be sent 
  while(!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF)) { }                             // Wait till all data have been physically transferred on the bus 
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);                                               // End the configuration sequence 
  __enable_irq();                                                                    // Re-enable the interrupt mechanism 
  return result;                                                                     // Return the verifying value: 1 (Passed) or 0 (Failed) 
}

void Codec_CS42L22Init(void)
{
  uint32_t delaycount;
  uint8_t regValue = 0xFF;
  GPIO_SetBits(GPIOD, CODEC_RESET_PIN);
  delaycount = 1000000;
  while (delaycount > 0)
  {
	delaycount--;
  }

  Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_DOWN);
  //begin initialization 
  Codec_WriteRegister(0x00, 0x99);
  Codec_WriteRegister(0x47, 0x80);
  regValue = Codec_ReadRegister(0x32);
  Codec_WriteRegister(0x32, regValue | 0x80);
  regValue = Codec_ReadRegister(0x32);
  Codec_WriteRegister(0x32, regValue & (~0x80));
  Codec_WriteRegister(0x00, 0x00);

  Codec_WriteRegister(CODEC_MAP_PWR_CTRL2, CODEC_HEADPHONE_DEVICE);
  Codec_WriteRegister(CODEC_MAP_PLAYBACK_CTRL1, 0x70);
  Codec_WriteRegister(CODEC_MAP_CLK_CTRL, AUTO_CLOCK_DETECT);
  Codec_WriteRegister(CODEC_MAP_IF_CTRL1, 0x04);
  Codec_WriteRegister(CODEC_MAP_PWR_CTRL1, CODEC_POWER_ON);
  Codec_WriteRegister(CODEC_MAP_ANALOG_SET, 0x00);
  Codec_WriteRegister(CODEC_MAP_MISC_CTRL, 0x04);
  Codec_WriteRegister(CODEC_MAP_LIMIT_CTRL1, 0x00);
  Codec_WriteRegister(CODEC_MAP_TONE_CTRL, 0x0F);
  Codec_WriteRegister(CODEC_MAP_PCMA_VOL, 0x0A);
  Codec_WriteRegister(CODEC_MAP_PCMB_VOL, 0x0A);
}

void CodecGPIO_Init(void)
{
  GPIO_InitTypeDef PinInitStruct;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

  //Reset pin as GPIO
  PinInitStruct.GPIO_Pin = CODEC_RESET_PIN;
  PinInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  PinInitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  PinInitStruct.GPIO_OType = GPIO_OType_PP;
  PinInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &PinInitStruct);

  // I2C pins
  PinInitStruct.GPIO_Mode = GPIO_Mode_AF;
  PinInitStruct.GPIO_OType = GPIO_OType_OD;
  PinInitStruct.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
  PinInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  PinInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &PinInitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

  // I2S pins
  PinInitStruct.GPIO_Pin = I2S3_SCLK_PIN | I2S3_SD_PIN | I2S3_MCLK_PIN;
  PinInitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOC, &PinInitStruct);
  PinInitStruct.GPIO_Pin = I2S3_WS_PIN;
  GPIO_Init(GPIOA, &PinInitStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  GPIO_ResetBits(GPIOD, CODEC_RESET_PIN);
}

void CodecI2C_Init(void)
{
  I2C_InitTypeDef I2C_InitType;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  I2C_DeInit(CODEC_I2C);
  I2C_InitType.I2C_ClockSpeed = I2C_SPEED;
  I2C_InitType.I2C_Mode = I2C_Mode_I2C;
  I2C_InitType.I2C_OwnAddress1 = CORE_I2C_ADDRESS;
  I2C_InitType.I2C_Ack = I2C_Ack_Enable;
  I2C_InitType.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitType.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_Init(CODEC_I2C, &I2C_InitType);
  I2C_Cmd(CODEC_I2C, ENABLE);
}

void CodecI2S_Init(uint32_t AudioFreq)
{
  I2S_InitTypeDef I2S_InitType;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  SPI_I2S_DeInit(CODEC_I2S);
  I2S_InitType.I2S_AudioFreq = AudioFreq;
  I2S_InitType.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitType.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitType.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitType.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitType.I2S_CPOL = I2S_CPOL_Low;
  I2S_Init(CODEC_I2S, &I2S_InitType);
  RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
  RCC_PLLI2SCmd(ENABLE);
}

void Init_DMA(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);
  DMA_DeInit(AUDIO_I2S_DMA_STREAM);
  SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)0xFFFE;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI3->DR));
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_Init(AUDIO_I2S_DMA_STREAM, &DMA_InitStructure);
  SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
  DMA_ITConfig(AUDIO_I2S_DMA_STREAM, DMA_IT_TC, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA_Play(uint16_t *begin_pos, uint32_t size)
{
  AudioTotalSize = size;
  AudioRemSize = (size/2) - DMA_MAX(AudioTotalSize);
  current_pos = begin_pos + DMA_MAX(AudioTotalSize);
  start_pos = begin_pos;

  DMA_InitStructure.DMA_BufferSize = (uint32_t)(DMA_MAX(AudioRemSize));
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)current_pos;
  DMA_Init(AUDIO_I2S_DMA_STREAM, &DMA_InitStructure);
  DMA_Cmd(AUDIO_I2S_DMA_STREAM, ENABLE);
  if ((CODEC_I2S->I2SCFGR & I2S_ENABLE_MASK) == 0)
  {
    I2S_Cmd(CODEC_I2S, ENABLE);
  }
}

void DMA1_Stream7_IRQHandler(void)
{
  SetCommandLight(); 
  if(DMA_GetFlagStatus(AUDIO_I2S_DMA_STREAM, DMA_FLAG_TCIF7) != RESET)
  {        
    if (AudioRemSize > 0) // Check if the end of file has been reached
    {   
      while (DMA_GetCmdStatus(AUDIO_I2S_DMA_STREAM) != DISABLE) 
      {}    
      DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, DMA_FLAG_TCIF7);        
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)current_pos;
      DMA_InitStructure.DMA_BufferSize = (uint32_t)(DMA_MAX(AudioRemSize));      
      DMA_Init(AUDIO_I2S_DMA_STREAM, &DMA_InitStructure);    
      DMA_Cmd(AUDIO_I2S_DMA_STREAM, ENABLE);         
      current_pos += DMA_MAX(AudioRemSize);             
      AudioRemSize -= DMA_MAX(AudioRemSize);   
    }
    else
    {   
      DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);      
      DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, DMA_FLAG_TCIF7);
      Player_TransferComplete_CallBack(start_pos, AudioTotalSize);
    }
  }
}

void PeriphInit(uint32_t AudioFreq)
{
  CodecGPIO_Init();
  CodecI2C_Init();
  Codec_CS42L22Init();
  CodecI2S_Init(AudioFreq);
  Init_DMA();
}
