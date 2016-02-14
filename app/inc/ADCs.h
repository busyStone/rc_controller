#ifndef __ADCS_H
#define __ADCS_H


#include "global.h"
#include "stm32f2xx_flash.h"

#define     ParaFlashAddr                     0x0800C000        //���������ŵص�
#define 	ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */

#define     ADC_CALIB_VAL_Y1                  1000
#define     ADC_CALIB_VAL_Y2                  1500
#define     ADC_CALIB_VAL_Y3                  2000

#define     LOW_PASS_FILTER_PARA              0.5

#define     MAX(a,b)                            (a>b ? a : b)
#define     MIN(a,b)                            (a<b ? a : b)

typedef struct
{
	uint16_t HighValue;
	uint16_t MidValue;
	uint16_t LowValue;
}ADC_Value_Calibration_t;

typedef struct
{
	ADC_Value_Calibration_t  ADCs_Calibrate_value[ADC_MODULE_NUMBER-1];
    MIXER_LANDING_MODE_t     mixer_channel_config;
	uint16_t 				 failsafeMode;
	uint16_t				 valid;
    uint32_t                 rx_num;
}Param_To_Store_t;


typedef enum
{
	CH1_PITCH,			
	CH2_ROLL,
	CH3_THROTTLE,
	CH4_YAW,
	CH5_MIX,					//����ģʽ
	CH6_CR_PITCH,				//��̨pitch
	CH7_CR_ROLL,				//��̨ROLL
	CH8_CR_YAW,					//��̨YAW
	CH9_CR_BACKUP,				//��̨backup
	CH10_PHOTO,					//���/¼����ͣ(����)
	CH11_CAM_VIDEO,				//cam/vedio�л�
	CH12_FAILSAFE_MODE,			//ʧЧģʽ  RTL  ��ͣ  landing  Ԥ��λ��(ȥ��)
	PPM_CH_NUM
}PPM_CH_FORMAT_t;


typedef struct
{
	uint16_t					adcs[ADC_MODULE_NUMBER];		//������ѹֵ
	MISC_SW_VALUE		        SW;						        //����������ֵ	
}ALL_STICK_INPUT_t;

void Task_ADCs(void const * argument);



uint8_t set_mixer(MIXER_LANDING_MODE_t landingValue);

MIXER_LANDING_MODE_t get_mixer(void);

uint8_t set_stickCenterValue(void);

uint8_t set_stickMaxMinStart(void);

uint8_t set_stickMaxMinEnd(void);

ALL_STICK_INPUT_t get_AllInputsValue(void);


#endif
