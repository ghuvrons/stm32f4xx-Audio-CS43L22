/**
  ******************************************************************************
  * @file    cs43l22.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the cs43l22.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS43L22_H
#define __CS43L22_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */ 
  
/** @addtogroup CS43L22
  * @{
  */

/** @defgroup CS43L22_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup CS43L22_Exported_Constants
  * @{
  */ 

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K            ((uint32_t)8000)  

/** CS43l22 Registers  ***/
#define   CS43L22_REG_ID                  0x01
#define   CS43L22_REG_POWER_CTL1          0x02
#define   CS43L22_REG_POWER_CTL2          0x04
#define   CS43L22_REG_CLOCKING_CTL        0x05
#define   CS43L22_REG_INTERFACE_CTL1      0x06
#define   CS43L22_REG_INTERFACE_CTL2      0x07
#define   CS43L22_REG_PASSTHR_A_SELECT    0x08
#define   CS43L22_REG_PASSTHR_B_SELECT    0x09
#define   CS43L22_REG_ANALOG_ZC_SR_SETT   0x0A
#define   CS43L22_REG_PASSTHR_GANG_CTL    0x0C
#define   CS43L22_REG_PLAYBACK_CTL1       0x0D
#define   CS43L22_REG_MISC_CTL            0x0E
#define   CS43L22_REG_PLAYBACK_CTL2       0x0F
#define   CS43L22_REG_PASSTHR_A_VOL       0x14
#define   CS43L22_REG_PASSTHR_B_VOL       0x15
#define   CS43L22_REG_PCMA_VOL            0x1A
#define   CS43L22_REG_PCMB_VOL            0x1B
#define   CS43L22_REG_BEEP_FREQ_ON_TIME   0x1C
#define   CS43L22_REG_BEEP_VOL_OFF_TIME   0x1D
#define   CS43L22_REG_BEEP_TONE_CFG       0x1E
#define   CS43L22_REG_TONE_CTL            0x1F
#define   CS43L22_REG_MASTER_A_VOL        0x20
#define   CS43L22_REG_MASTER_B_VOL        0x21
#define   CS43L22_REG_HEADPHONE_A_VOL     0x22
#define   CS43L22_REG_HEADPHONE_B_VOL     0x23
#define   CS43L22_REG_SPEAKER_A_VOL       0x24
#define   CS43L22_REG_SPEAKER_B_VOL       0x25
#define   CS43L22_REG_CH_MIXER_SWAP       0x26
#define   CS43L22_REG_LIMIT_CTL1          0x27
#define   CS43L22_REG_LIMIT_CTL2          0x28
#define   CS43L22_REG_LIMIT_ATTACK_RATE   0x29
#define   CS43L22_REG_OVF_CLK_STATUS      0x2E
#define   CS43L22_REG_BATT_COMPENSATION   0x2F
#define   CS43L22_REG_VP_BATTERY_LEVEL    0x30
#define   CS43L22_REG_SPEAKER_STATUS      0x31
#define   CS43L22_REG_TEMPMONITOR_CTL     0x32
#define   CS43L22_REG_THERMAL_FOLDBACK    0x33
#define   CS43L22_REG_CHARGE_PUMP_FREQ    0x34

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/** 
  * @brief  CS43L22 ID  
  */  
#define  CS43L22_ID            0xE0
#define  CS43L22_ID_MASK       0xF8
/**
  * @brief Chip ID Register: Chip I.D. and Revision Register
  *  Read only register
  *  Default value: 0x01
  *  [7:3] CHIPID[4:0]: I.D. code for the CS43L22.
  *        Default value: 11100b
  *  [2:0] REVID[2:0]: CS43L22 revision level.
  *        Default value: 
  *        000 - Rev A0
  *        001 - Rev A1
  *        010 - Rev B0
  *        011 - Rev B1
  */
#define CS43L22_CHIPID_ADDR    0x01

/**
  * @}
  */ 

/** @defgroup CS43L22_Exported_Macros
  * @{
  */

/**
  * @}
  */ 

/** @defgroup CS43L22_Exported_Functions
  * @{
  */

/*------------------------------------------------------------------------------
                               Audio Handler
------------------------------------------------------------------------------*/

typedef struct {
  uint16_t deviceAddr;
  I2C_HandleTypeDef *hi2c;
  I2S_HandleTypeDef *hi2s;
  uint8_t isPlaying;
  uint8_t volume;
  uint8_t outputDevice;
  uint32_t audioFrequency;
} cs43l22_HandlerTypeDef;

/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* High Layer codec functions */
HAL_StatusTypeDef cs43l22_Init(cs43l22_HandlerTypeDef*, uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
HAL_StatusTypeDef cs43l22_DeInit(cs43l22_HandlerTypeDef*);
uint8_t           cs43l22_ReadID(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef cs43l22_StreamSound(cs43l22_HandlerTypeDef*, uint16_t* pBuffer, uint16_t Size);
HAL_StatusTypeDef cs43l22_Play(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef cs43l22_Pause(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef cs43l22_Resume(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef cs43l22_Stop(cs43l22_HandlerTypeDef*, uint32_t Cmd);
HAL_StatusTypeDef cs43l22_SetVolume(cs43l22_HandlerTypeDef*, uint8_t Volume);
HAL_StatusTypeDef cs43l22_SetFrequency(cs43l22_HandlerTypeDef*, uint32_t AudioFreq);
HAL_StatusTypeDef cs43l22_SetMute(cs43l22_HandlerTypeDef*, uint8_t Cmd);
HAL_StatusTypeDef cs43l22_SetOutputMode(cs43l22_HandlerTypeDef*, uint8_t Output);
HAL_StatusTypeDef cs43l22_Reset(cs43l22_HandlerTypeDef*);

/* AUDIO IO functions */
HAL_StatusTypeDef AUDIO_IO_Init(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef AUDIO_IO_DeInit(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef AUDIO_IO_Check(cs43l22_HandlerTypeDef*);
HAL_StatusTypeDef AUDIO_IO_Write(cs43l22_HandlerTypeDef*, uint8_t Reg, uint8_t Value);
uint8_t           AUDIO_IO_Read(cs43l22_HandlerTypeDef*, uint8_t Reg);
HAL_StatusTypeDef AUDIO_IO_SetFrequency(cs43l22_HandlerTypeDef*, uint32_t AudioFreq);

/* Audio driver structure */

#endif /* __CS43L22_H */

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
