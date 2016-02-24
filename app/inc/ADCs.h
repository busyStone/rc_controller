#ifndef __ADCS_H
#define __ADCS_H


#include "global.h"
#include "stm32f2xx_flash.h"


/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */

#define     ParaFlashAddr                     ADDR_FLASH_SECTOR_7        //���������ŵص�

#define     ADC_CALIB_VAL_Y1                  1000
#define     ADC_CALIB_VAL_Y2                  1500
#define     ADC_CALIB_VAL_Y3                  2000

#define		ADC_CALIB_RST_VAL				  2048

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
    uint16_t                 rx_num;
	uint16_t				 calib_valid;
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


int adc_mixer_set(MIXER_LANDING_MODE_t landingValue);
int adc_mixer_get(MIXER_LANDING_MODE_t *p_landingValue);
int adc_stick_cntr_val_set(void);
int adc_stick_most_val_set_start(void);
int adc_stick_most_val_set_end(void);
int adc_all_in_val_get(ALL_STICK_INPUT_t *stickValue);
int adc_radio_pairing_req(uint32_t x);
int adc_radio_pairing_end(void);



#endif
