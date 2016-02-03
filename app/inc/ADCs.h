#ifndef __ADCS_H
#define __ADCS_H


#include "global.h"
#include "stm32f2xx_flash.h"

#define     ParaFlashAddr                     0x0800C000        //���������ŵص�
#define 	ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */



typedef struct
{
	uint16_t HighValue;
	uint16_t MidValue;
	uint16_t LowValue;
}ADC_Value_Calibration_t;

typedef struct
{
	ADC_Value_Calibration_t  ADCs_Calibrate_value[ADC_MODULE_NUMBER-1];
	uint16_t 				 failsafeMode;
	uint16_t				 channel_value[5];
	uint16_t				 valid;
	uint16_t				 align;
}Param_To_Store_t;


typedef enum
{
	CH1_PITCH,			
	CH2_ROLL,
	CH3_THROTTLE,
	CH4_YAW,
	CH5_MIX,					//����ģʽ
	CH6_LANDING_MODE,			//����/RTLģʽ
	CH7_CR_PITCH,				//��̨pitch
	CH8_CR_ROLL,				//��̨ROLL
	CH9_CR_YAW,					//��̨YAW
	CH10_CR_BACKUP,				//��̨backup
	CH11_PHOTO,					//���/¼����ͣ
	CH12_CAM_VIDEO,				//cam/vedio�л�
	CH13_TRAING,				//ѵ��--����ռͨ��
	CH14_FAILSAFE_MODE,			//ʧЧģʽ  RTL  ��ͣ  landing  Ԥ��λ��
	CH15_FAILSAFE_THROTTLE,		//Ԥ��λ��--����
	CH16_FAILSAFE_MIX,			//������ģʽ��Ӧ��5ͨ��ͨ��ֵ  -- ��mavlink, ������
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
