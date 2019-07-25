#ifndef PERIPH_H
#define PERIPH_H

#include "ff.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"
#include "actions_indication.h"

// pins to codec
#define I2S3_WS_PIN 	GPIO_Pin_4  //port A

#define I2S3_MCLK_PIN 	GPIO_Pin_7  //port C
#define I2S3_SCLK_PIN 	GPIO_Pin_10 //port C
#define I2S3_SD_PIN 	GPIO_Pin_12 //port C

#define CODEC_RESET_PIN GPIO_Pin_4  //port D

#define I2C_SCL_PIN  GPIO_Pin_6  //port B
#define I2C_SDA_PIN  GPIO_Pin_9  //port B

#define CODEC_I2C I2C1
#define CODEC_I2S SPI3

#define CORE_I2C_ADDRESS  0x33
#define CODEC_I2C_ADDRESS 0x94

#define CODEC_I2S_ADDRESS 0x40003C0C

#define DMA_MAX_SZE                    0xFFFF
#define DMA_MAX(x)           (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)

#define VOLUME_CONVERT(x)    ((x > 100)? 100:((uint8_t)((x * 255) / 100)))

#define AUDIO_I2S_DMA_STREAM  DMA1_Stream7

// register map bytes for CS42L22
#define CODEC_MAP_CHIP_ID   0x01
#define CODEC_MAP_PWR_CTRL1 0x02
#define CODEC_MAP_PWR_CTRL2 0x04
#define CODEC_MAP_CLK_CTRL  0x05
#define CODEC_MAP_IF_CTRL1  0x06
#define CODEC_MAP_IF_CTRL2  0x07
#define CODEC_MAP_PASSTHROUGH_A_SELECT 0x08
#define CODEC_MAP_PASSTHROUGH_B_SELECT 0x09
#define CODEC_MAP_ANALOG_SET 0x0A
#define CODEC_MAP_PASSTHROUGH_GANG_CTRL 0x0C
#define CODEC_MAP_PLAYBACK_CTRL1 0x0D
#define CODEC_MAP_MISC_CTRL 0x0E
#define CODEC_MAP_PLAYBACK_CTRL2 0x0F
#define CODEC_MAP_PASSTHROUGH_A_VOL 0x14
#define CODEC_MAP_PASSTHROUGH_B_VOL 0x15
#define CODEC_MAP_PCMA_VOL 0x1A
#define CODEC_MAP_PCMB_VOL 0x1B
#define CODEC_MAP_BEEP_FREQ_ONTIME 0x1C
#define CODEC_MAP_BEEP_VOL_OFFTIME 0x1D
#define CODEC_MAP_BEEP_TONE_CFG 0x1E
#define CODEC_MAP_TONE_CTRL 0x1F
#define CODEC_MAP_MASTER_A_VOL 0x20
#define CODEC_MAP_MASTER_B_VOL 0x21
#define CODEC_MAP_HP_A_VOL 0x22
#define CODEC_MAP_HP_B_VOL 0x23
#define CODEC_MAP_SPEAK_A_VOL 0x24
#define CODEC_MAP_SPEAK_B_VOL 0x25
#define CODEC_MAP_CH_MIX_SWAP 0x26
#define CODEC_MAP_LIMIT_CTRL1 0x27
#define CODEC_MAP_LIMIT_CTRL2 0x28
#define CODEC_MAP_LIMIT_ATTACK 0x29
#define CODEC_MAP_OVFL_CLK_STATUS 0x2E
#define CODEC_MAP_BATT_COMP 0x2F
#define CODEC_MAP_VP_BATT_LEVEL 0x30
#define CODEC_MAP_SPEAK_STATUS 0x31
#define CODEC_MAP_CHARGE_PUMP_FREQ 0x34

//commands for CS42L22
#define CODEC_MUTE_ON 0xFF
#define CODEC_HEADPHONE_DEVICE 0xAF 
#define CODEC_POWER_DOWN 0x01
#define CODEC_POWER_ON 0x9E
#define AUTO_CLOCK_DETECT 0x81

void PeriphInit(uint32_t AudioFreq);
uint8_t Codec_WriteRegister(uint32_t RegisterAddr, uint32_t RegisterValue);
uint8_t Codec_ReadRegister(uint32_t RegisterAddr);
uint8_t DMA_Read_Send(FRESULT fresult, int position, volatile ITStatus it_status, UINT read_bytes, uint32_t DMA_FLAG, uint8_t change_song);
void Codec_SetVolume(uint8_t Volume);
void Init_DMA_ForByteArray(uint16_t *begin_pos, uint32_t size);
void Init_DMA(void);
void DMA1_Stream7_IRQHandler(void);

#endif /* PERIPH_H */