/**
  ******************************************************************************
  * @file    cs43l22.c
  * @author  MCD Application Team
  * @brief   This file provides the CS43L22 Audio Codec driver.   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Include/cs43l22.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @addtogroup CS43L22
  * @brief     This file provides a set of functions needed to drive the 
  *            CS43L22 audio codec.
  * @{
  */

/** @defgroup CS43L22_Private_Types
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup CS43L22_Private_Defines
  * @{
  */
#define I2Cx_TIMEOUT_MAX 0x1000
#define CODEC_STANDARD 0x04

#define VOLUME_CONVERT(Volume)    (((Volume) > 100)? 255:((uint8_t)(((Volume) * 255) / 100)))  
/* Uncomment this line to enable verifying data sent to codec after each write 
   operation (for debug purpose) */
#ifndef VERIFY_WRITTENDATA
#define VERIFY_WRITTENDATA 0
#endif /* VERIFY_WRITTENDATA */
/**
  * @}
  */ 

/** @defgroup CS43L22_Private_Macros
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup CS43L22_Private_Variables
  * @{
  */

/* Audio codec driver structure initialization */  

/**
  * @}
  */ 

/** @defgroup CS43L22_Function_Prototypes
  * @{
  */
static HAL_StatusTypeDef CODEC_IO_Write(cs43l22_HandlerTypeDef *hcs43, uint8_t Reg, uint8_t Value);
/**
  * @}
  */ 

/** @defgroup CS43L22_Private_Functions
  * @{
  */ 

/**
  * @brief Initializes the audio codec and the control interface.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param OutputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
  * @param Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Init(cs43l22_HandlerTypeDef *hcs43, uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{
  uint8_t err = 0;
  
  /*Save Output device for mute ON/OFF procedure*/
  switch (OutputDevice)
  {
  case OUTPUT_DEVICE_SPEAKER:
    hcs43->outputDevice = 0xFA;
    break;
    
  case OUTPUT_DEVICE_HEADPHONE:
    hcs43->outputDevice = 0xAF;
    break;
    
  case OUTPUT_DEVICE_BOTH:
    hcs43->outputDevice = 0xAA;
    break;
    
  case OUTPUT_DEVICE_AUTO:
    hcs43->outputDevice = 0x05;
    break;    
    
  default:
    hcs43->outputDevice = 0x05;
    break;    
  }
  
  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init(hcs43);

  /* Keep Codec powered OFF */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL1, 0x01);

  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, hcs43->outputDevice);
  
  /* Clock configuration: Auto detection */  
  err += CODEC_IO_Write(hcs43, CS43L22_REG_CLOCKING_CTL, 0x81);
  
  /* Set the Slave Mode and the audio Standard */  
  err += CODEC_IO_Write(hcs43, CS43L22_REG_INTERFACE_CTL1, CODEC_STANDARD);
  
  /* Set the Master volume */
  err += cs43l22_SetVolume(hcs43, Volume);
  
  /* If the Speaker is enabled, set the Mono mode and volume attenuation level */
  if(OutputDevice != OUTPUT_DEVICE_HEADPHONE)
  {
    /* Set the Speaker Mono mode */  
    err += CODEC_IO_Write(hcs43, CS43L22_REG_PLAYBACK_CTL2, 0x06);
    
    /* Set the Speaker attenuation level */  
    err += CODEC_IO_Write(hcs43, CS43L22_REG_SPEAKER_A_VOL, 0x00);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_SPEAKER_B_VOL, 0x00);
  }
  
  /* Additional configuration for the CODEC. These configurations are done to reduce
  the time needed for the Codec to power off. If these configurations are removed, 
  then a long delay should be added between powering off the Codec and switching 
  off the I2S peripheral MCLK clock (which is the operating clock for Codec).
  If this delay is not inserted, then the codec will not shut down properly and
  it results in high noise after shut down. */
  
  /* Disable the analog soft ramp */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);
  /* Disable the digital soft ramp */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_MISC_CTL, 0x04);
  /* Disable the limiter attack level */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_LIMIT_CTL1, 0x00);
  /* Adjust Bass and Treble levels */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_TONE_CTL, 0x0F);
  /* Adjust PCM volume level */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_PCMA_VOL, 0x0A);
  err += CODEC_IO_Write(hcs43, CS43L22_REG_PCMB_VOL, 0x0A);
  
  /* Return communication control value */
  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief  Deinitializes the audio codec.
  * @param  None
  * @retval  None
  */
HAL_StatusTypeDef cs43l22_DeInit(cs43l22_HandlerTypeDef *hcs43)
{
  /* Deinitialize Audio Codec interface */
  return AUDIO_IO_DeInit(hcs43);
}

/**
  * @brief  Get the CS43L22 ID.
  * @param DeviceAddr: Device address on communication Bus.   
  * @retval The CS43L22 ID 
  */
uint8_t cs43l22_ReadID(cs43l22_HandlerTypeDef *hcs43)
{
  uint8_t Value;
  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init(hcs43); 
  
  Value = AUDIO_IO_Read(hcs43, CS43L22_CHIPID_ADDR);
  Value = (Value & CS43L22_ID_MASK);
  
  return((uint32_t) Value);
}

HAL_StatusTypeDef cs43l22_SendSound(cs43l22_HandlerTypeDef *hcs43, uint16_t* pBuffer, uint16_t Size)
{
  uint8_t counter = 0;
  counter += HAL_I2S_Transmit_DMA(hcs43->hi2s, pBuffer, Size);
  return (counter == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @param DeviceAddr: Device address on communication Bus.   
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Play(cs43l22_HandlerTypeDef *hcs43)
{
  uint8_t err = 0;

  if(!(hcs43->isPlaying))
  {
    /* Enable the digital soft ramp */
    err += CODEC_IO_Write(hcs43, CS43L22_REG_MISC_CTL, 0x06);
  
    /* Enable Output device */  
    err += cs43l22_SetMute(hcs43, AUDIO_MUTE_OFF);
    
    /* Power on the Codec */
    err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL1, 0x9E);
    hcs43->isPlaying = 1;
  }

  /* Return communication control value */
  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Pauses playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Pause(cs43l22_HandlerTypeDef *hcs43)
{  
  uint8_t err = 0;
 
  /* Pause the audio file playing */
  /* Mute the output first */
  err += cs43l22_SetMute(hcs43, AUDIO_MUTE_ON);
  
  /* Put the Codec in Power save mode */    
  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL1, 0x01);

  if (!err) err += HAL_I2S_DMAPause(hcs43->hi2s);

  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Resumes playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Resume(cs43l22_HandlerTypeDef *hcs43)
{
  uint8_t err = 0;
  volatile uint32_t index = 0x00;
  /* Resumes the audio file playing */  
  /* Unmute the output first */
  err += cs43l22_SetMute(hcs43, AUDIO_MUTE_OFF);

  for(index = 0x00; index < 0xFF; index++);
  
  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, hcs43->outputDevice);

  /* Exit the Power save mode */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL1, 0x9E);

  if (!err) err += HAL_I2S_DMAResume(hcs43->hi2s);

  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @param CodecPdwnMode: selects the  power down mode.
  *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration 
  *                           (user should re-Initialize the codec in order to 
  *                            play again the audio stream).
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Stop(cs43l22_HandlerTypeDef *hcs43, uint32_t CodecPdwnMode)
{
  uint8_t err = 0;
  
  err += HAL_I2S_DMAStop(hcs43->hi2s);

  /* Mute the output first */
  err += cs43l22_SetMute(hcs43, AUDIO_MUTE_ON);

  /* Disable the digital soft ramp */
  err += CODEC_IO_Write(hcs43, CS43L22_REG_MISC_CTL, 0x04);
  
  /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
  err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL1, 0x9F);
  
  hcs43->isPlaying = 0;
  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Sets higher or lower the codec volume level.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param Volume: a byte value from 0 to 255 (refer to codec registers 
  *                description for more details).
  *         
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_SetVolume(cs43l22_HandlerTypeDef *hcs43, uint8_t Volume)
{
  uint8_t err = 0;
  uint8_t convertedvol = VOLUME_CONVERT(Volume);

  if(convertedvol > 0xE6)
  {
    /* Set the Master volume */
    err += CODEC_IO_Write(hcs43, CS43L22_REG_MASTER_A_VOL, convertedvol - 0xE7);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_MASTER_B_VOL, convertedvol - 0xE7);
  }
  else
  {
    /* Set the Master volume */
    err += CODEC_IO_Write(hcs43, CS43L22_REG_MASTER_A_VOL, convertedvol + 0x19);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_MASTER_B_VOL, convertedvol + 0x19);
  }

  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Sets new frequency.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param AudioFreq: Audio frequency used to play the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_SetFrequency(cs43l22_HandlerTypeDef *hcs43, uint32_t AudioFreq)
{
  if (hcs43->isPlaying) cs43l22_Stop(hcs43, CODEC_STANDARD);

  // TODO: Set Frequency

  return HAL_OK;
}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_SetMute(cs43l22_HandlerTypeDef *hcs43, uint8_t Cmd)
{
  uint8_t err = 0;
  
  /* Set the Mute mode */
  if(Cmd == AUDIO_MUTE_ON)
  {
    err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0xFF);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_HEADPHONE_A_VOL, 0x01);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_HEADPHONE_B_VOL, 0x01);
  }
  else /* AUDIO_MUTE_OFF Disable the Mute */
  {
    err += CODEC_IO_Write(hcs43, CS43L22_REG_HEADPHONE_A_VOL, 0x00);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_HEADPHONE_B_VOL, 0x00);
    err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, hcs43->outputDevice);
  }
  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Switch dynamically (while audio file is played) the output target 
  *         (speaker or headphone).
  * @note This function modifies a global variable of the audio codec driver: OutputDev.
  * @param DeviceAddr: Device address on communication Bus.
  * @param Output: specifies the audio output target: OUTPUT_DEVICE_SPEAKER,
  *         OUTPUT_DEVICE_HEADPHONE, OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO 
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_SetOutputMode(cs43l22_HandlerTypeDef *hcs43, uint8_t Output)
{
  uint8_t err = 0;
  
  switch (Output) 
  {
    case OUTPUT_DEVICE_SPEAKER:
      err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0xFA); /* SPK always ON & HP always OFF */
      hcs43->outputDevice = 0xFA;
      break;
      
    case OUTPUT_DEVICE_HEADPHONE:
      err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0xAF); /* SPK always OFF & HP always ON */
      hcs43->outputDevice = 0xAF;
      break;
      
    case OUTPUT_DEVICE_BOTH:
      err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0xAA); /* SPK always ON & HP always ON */
      hcs43->outputDevice = 0xAA;
      break;
      
    case OUTPUT_DEVICE_AUTO:
      err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0x05); /* Detect the HP or the SPK automatically */
      hcs43->outputDevice = 0x05;
      break;    
      
    default:
      err += CODEC_IO_Write(hcs43, CS43L22_REG_POWER_CTL2, 0x05); /* Detect the HP or the SPK automatically */
      hcs43->outputDevice = 0x05;
      break;
  }  
  return (err == 0)? HAL_OK : HAL_ERROR;
}

/**
  * @brief Resets cs43l22 registers.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
HAL_StatusTypeDef cs43l22_Reset(cs43l22_HandlerTypeDef *hcs43)
{
  return HAL_OK;
}


HAL_StatusTypeDef AUDIO_IO_Init(cs43l22_HandlerTypeDef *hcs43)
{
  return HAL_OK;
}


HAL_StatusTypeDef AUDIO_IO_DeInit(cs43l22_HandlerTypeDef *hcs43)
{
  return HAL_OK;
}


HAL_StatusTypeDef AUDIO_IO_Check(cs43l22_HandlerTypeDef *hcs43)
{
  return HAL_I2C_IsDeviceReady(hcs43->hi2c, hcs43->deviceAddr, 10, 1000);
}


HAL_StatusTypeDef AUDIO_IO_Write(cs43l22_HandlerTypeDef *hcs43, uint8_t Reg, uint8_t Value)
{
  return HAL_I2C_Mem_Write(hcs43->hi2c, hcs43->deviceAddr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2Cx_TIMEOUT_MAX);
}


uint8_t AUDIO_IO_Read(cs43l22_HandlerTypeDef *hcs43, uint8_t Reg)
{
  uint8_t value = 0;

  HAL_I2C_Mem_Read(hcs43->hi2c, hcs43->deviceAddr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2Cx_TIMEOUT_MAX);
  return value;
}


/**
  * @brief  Writes/Read a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @param  Value: Data to be written
  * @retval None
  */
static HAL_StatusTypeDef CODEC_IO_Write(cs43l22_HandlerTypeDef *hcs43, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = 0;
  status = AUDIO_IO_Write(hcs43, Reg, Value);
  if(status) return status;
  
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */  
  status = (AUDIO_IO_Read(hcs43, Reg) == Value)? HAL_OK:HAL_ERROR;
#endif /* VERIFY_WRITTENDATA */
  
  return status;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
