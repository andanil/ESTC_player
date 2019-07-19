#include "periph.h"

void Codec_Init(void);
void Codec_WriteRegisterForInit(uint32_t RegisterAddr, uint32_t RegisterValue);
uint8_t Codec_ReadRegister(uint32_t RegisterAddr);
void Codec_CS42L22Init(void);
void Init_DMA(void);

uint16_t sample_buffer[2048];

void Codec_WriteRegisterForInit(uint32_t RegisterAddr, uint32_t RegisterValue)
{
  while (I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY)) { }
  I2C_GenerateSTART(CODEC_I2C, ENABLE);
  while (!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_SB)) { }
  I2C_Send7bitAddress(CODEC_I2C, CODEC_I2C_ADDRESS, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) { }
  I2C_SendData(CODEC_I2C, RegisterAddr);
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) { }
  I2C_SendData(CODEC_I2C, RegisterValue);
  while(!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF)) { }
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);
}

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

uint8_t DMA_Read_Send(FRESULT fresult, int position, volatile ITStatus it_status, UINT read_bytes, uint32_t DMA_FLAG, uint8_t change_song)
{
  FIL file;
  it_status = RESET;
  while(it_status == RESET)
  {
	it_status = DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG);
  }
  fresult = f_read (&file,&sample_buffer[position],1024*2,&read_bytes);
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
  I2C_SendData(CODEC_I2C, RegisterAddr); //sets the transmitter address
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
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT)) { } // Test on EV5 and clear it 
  I2C_Send7bitAddress(CODEC_I2C, CODEC_I2C_ADDRESS, I2C_Direction_Transmitter);   // Transmit the slave address and enable writing operation 
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) { } // Test on EV6 and clear it 
  I2C_SendData(CODEC_I2C, RegisterAddr);  // Transmit the first address for write operation 
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) { } // Test on EV8 and clear it 
  __disable_irq(); // Disable the interrupts mechanism to prevent the I2C communication from corruption 
  I2C_SendData(CODEC_I2C, RegisterValue); // Prepare the register value to be sent 
  while(!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF)) { } //!< Wait till all data have been physically transferred on the bus 
  I2C_GenerateSTOP(CODEC_I2C, ENABLE); // End the configuration sequence 
  __enable_irq(); // Re-enable the interrupt mechanism 
  return result; // Return the verifying value: 1 (Passed) or 0 (Failed) 
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
  //keep codec OFF
  Codec_WriteRegisterForInit(CODEC_MAP_PWR_CTRL1, CODEC_POWER_DOWN);
  //begin initialization sequence
  Codec_WriteRegisterForInit(0x00, 0x99);
  Codec_WriteRegisterForInit(0x47, 0x80);
  regValue = Codec_ReadRegister(0x32);
  Codec_WriteRegisterForInit(0x32, regValue | 0x80);
  regValue = Codec_ReadRegister(0x32);
  Codec_WriteRegisterForInit(0x32, regValue & (~0x80));
  Codec_WriteRegisterForInit(0x00, 0x00);

  Codec_WriteRegisterForInit(CODEC_MAP_PWR_CTRL2, CODEC_HEADPHONE_DEVICE);
  Codec_WriteRegisterForInit(CODEC_MAP_PLAYBACK_CTRL1, 0x70);
  Codec_WriteRegisterForInit(CODEC_MAP_CLK_CTRL, AUTO_CLOCK_DETECT);
  Codec_WriteRegisterForInit(CODEC_MAP_IF_CTRL1, 0x07);
  Codec_WriteRegisterForInit(CODEC_MAP_MISC_CTRL, 0x04);
  Codec_WriteRegisterForInit(CODEC_MAP_ANALOG_SET, 0x00);
  Codec_WriteRegisterForInit(CODEC_MAP_LIMIT_CTRL1, 0x00);
  Codec_WriteRegisterForInit(CODEC_MAP_PCMA_VOL, 0x0A);
  Codec_WriteRegisterForInit(CODEC_MAP_PCMB_VOL, 0x0A);
  Codec_WriteRegisterForInit(CODEC_MAP_TONE_CTRL, 0x0F);

  Codec_WriteRegisterForInit(CODEC_MAP_PWR_CTRL1, CODEC_POWER_ON);
  Codec_SetVolume(70);
}

void Codec_Init(void)
{
  GPIO_InitTypeDef PinInitStruct;
  GPIO_StructInit(&PinInitStruct);
  I2S_InitTypeDef I2S_InitType;
  I2C_InitTypeDef I2C_InitType;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

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

  //enable I2S and I2C clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_SPI3, ENABLE);
  RCC_PLLI2SCmd(ENABLE);

  // I2S pins
  PinInitStruct.GPIO_OType = GPIO_OType_PP;
  PinInitStruct.GPIO_Pin = I2S3_SCLK_PIN | I2S3_SD_PIN | I2S3_MCLK_PIN;
  GPIO_Init(GPIOC, &PinInitStruct);
  PinInitStruct.GPIO_Pin = I2S3_WS_PIN;
  GPIO_Init(GPIOA, &PinInitStruct);

  //prepare output ports for alternate function
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  //keep Codec off for now
  GPIO_ResetBits(GPIOD, CODEC_RESET_PIN);

  // configure I2S port
  SPI_I2S_DeInit(CODEC_I2S);
  I2S_InitType.I2S_AudioFreq = I2S_AudioFreq_44k;
  I2S_InitType.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitType.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitType.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitType.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitType.I2S_CPOL = I2S_CPOL_Low;
  I2S_Init(CODEC_I2S, &I2S_InitType);

  // configure I2C port
  I2C_DeInit(CODEC_I2C);
  I2C_InitType.I2C_ClockSpeed = 100000;
  I2C_InitType.I2C_Mode = I2C_Mode_I2C;
  I2C_InitType.I2C_OwnAddress1 = CORE_I2C_ADDRESS;
  I2C_InitType.I2C_Ack = I2C_Ack_Enable;
  I2C_InitType.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitType.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_Cmd(CODEC_I2C, ENABLE);
  I2C_Init(CODEC_I2C, &I2C_InitType);
}

void Init_DMA(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_BufferSize = 2048;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&sample_buffer;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI3->DR));
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream5, ENABLE);
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPI3,ENABLE);
}

void PeriphInit(void)
{
  Codec_Init();
  Codec_CS42L22Init();
  I2S_Cmd(CODEC_I2S, ENABLE);
  Init_DMA();
}
