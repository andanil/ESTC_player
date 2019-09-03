#include "wave_file.h"

extern WAVE_FormatTypeDef wave_format;

ErrorCode WaveParsing(uint8_t *file_len, uint16_t *buffer)
{
  uint32_t temp = 0x00;
  uint32_t extraformatbytes = 0;
  temp = ReadUnit((uint8_t*)buffer, 0, 4, BigEndian);
  if (temp != CHUNK_ID)
  {
    return(Unvalid_RIFF_ID);
  }
  wave_format.RIFFchunksize = ReadUnit((uint8_t*)buffer, 4, 4, LittleEndian);
  temp = ReadUnit((uint8_t*)buffer, 8, 4, BigEndian);
  if (temp != FILE_FORMAT)
  {
    return(Unvalid_WAVE_Format);
  }
  temp = ReadUnit((uint8_t*)buffer, 12, 4, BigEndian);
  if (temp != FORMAT_ID)
  {
    return(Unvalid_FormatChunk_ID);
  }
  temp = ReadUnit((uint8_t*)buffer, 16, 4, LittleEndian);
  if (temp != 0x10)
  {
    extraformatbytes = 1;
  }
  wave_format.FormatTag = ReadUnit((uint8_t*)buffer, 20, 2, LittleEndian);
  if (wave_format.FormatTag != WAVE_FORMAT_PCM)
  {
    return(Unsupporetd_FormatTag);
  }
  wave_format.NumChannels = ReadUnit((uint8_t*)buffer, 22, 2, LittleEndian);
  wave_format.SampleRate = ReadUnit((uint8_t*)buffer, 24, 4, LittleEndian);
  wave_format.ByteRate = ReadUnit((uint8_t*)buffer, 28, 4, LittleEndian);
  wave_format.BlockAlign = ReadUnit((uint8_t*)buffer, 32, 2, LittleEndian);
  wave_format.BitsPerSample = ReadUnit((uint8_t*)buffer, 34, 2, LittleEndian);
  if (wave_format.BitsPerSample != BITS_PER_SAMPLE_16) 
  {
    return(Unsupporetd_Bits_Per_Sample);
  }
  *file_len = 36;
  if (extraformatbytes == 1)
  {
    temp = ReadUnit((uint8_t*)buffer, 36, 2, LittleEndian);
    if (temp != 0x00)
    {
      return(Unsupporetd_ExtraFormatBytes);
    }
    temp = ReadUnit((uint8_t*)buffer, 38, 4, BigEndian);
    if (temp != FACT_ID)
    {
      return(Unvalid_FactChunk_ID);
    }
    temp = ReadUnit((uint8_t*)buffer, 42, 4, LittleEndian);  
    *file_len += 10 + temp;
  }
  temp = ReadUnit((uint8_t*)buffer, *file_len, 4, BigEndian);
  *file_len += 4;
  /*if (temp != DATA_ID)
  {
    return(Unvalid_DataChunk_ID);
  }*/
  wave_format.DataSize = ReadUnit((uint8_t*)buffer, *file_len, 4, LittleEndian);
  *file_len += 4;
  return(Valid_WAVE_File);
}

uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t num_of_bytes, Endianness bytes_format)
{
  uint32_t index = 0;
  uint32_t temp = 0;
  
  for (index = 0; index < num_of_bytes; index++)
  {
    temp |= buffer[idx + index] << (index * 8);
  }
  
  if (bytes_format == BigEndian)
  {
    temp = __REV(temp);
  }
  return temp;
}