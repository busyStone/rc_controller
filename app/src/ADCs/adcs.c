/**
  ******************************************************************************
  * @file    adcs.c
  * @author  skyborne leon
  * @version V1.0.0
  * @date    04/11/2015
  * @brief   ң�������Ƹˡ����������źŴ���
  ******************************************************************************
  */ 

#include "cmsis_os.h"
#include "ADCs.h"
#include "usarts.h"


#include <stdlib.h>
#include <string.h>


/////////////////////ȫ�ֱ�������///////////////////////////////////////
uint16_t					g_adc_calibr_val[ADC_MODULE_NUMBER-1];				//У��ֵ����
uint16_t					g_ppm_channels[PPM_CH_NUM];
MISC_SW_VALUE				g_misc_sli_sw_value;

MIXER_LANDING_MODE_t		g_landingSetting;									//L1~L6ģʽ��ϼ�����
uint8_t						g_SetStickMaxMinFlag;								//���ø�������ֵ��־
Param_To_Store_t 			g_Param_To_Store;									//��������ֵ����
MSG_QUEUE_t		 			g_adc_msg;											//ADC��ͨ��ֵ����
uint16_t					g_gets_adcs_val[ADC_MODULE_NUMBER];					//���������ȡ��ͨ��ֵ����
MISC_SW_VALUE				g_lastSlideSwVal;									//���������ȡ������ֵ����





///////////////////��������/////////////////////////////////
Param_To_Store_t ReadFlash(uint32_t addr);
void WriteFlash(uint32_t addr, uint32_t *pdata, uint32_t len);
uint16_t calibrate_calc(uint16_t curADC, uint16_t calibHighValue, uint16_t calibMidValue, uint16_t calibLowValue);



/***************************************************************************************************
* @fn      Task_ADCs
*
* @brief   ң�������Ƹˡ����������źŴ�������
* @param   NULL
* @return  null
***************************************************************************************************/ 
void Task_ADCs(void const * argument)
{
	uint8_t			 i;
	
	argument = argument;

	//------��flash�ж�ȡУ��ֵ��ʧЧ��������-----------------
	g_Param_To_Store = (Param_To_Store_t) ReadFlash(ParaFlashAddr);
	if(g_Param_To_Store.valid != 1)			//������Ч,�ָ�Ĭ��ֵ
	{
		for(i=0; i<(ADC_MODULE_NUMBER-1); i++)
		{
			g_Param_To_Store.ADCs_Calibrate_value[i].HighValue = 4095;
			g_Param_To_Store.ADCs_Calibrate_value[i].MidValue = 2048;
			g_Param_To_Store.ADCs_Calibrate_value[i].LowValue = 0;
		}
		memset(g_Param_To_Store.channel_value, 0, sizeof(g_Param_To_Store.channel_value));
		g_Param_To_Store.failsafeMode = FAILSAFE_MODE_RTL;
	}
	
	for(;;)
	{
		xQueueReceive(xSemaphore_ForADCs, &g_adc_msg, portMAX_DELAY);

		//��������
		for(i=0; i<(ADC_MODULE_NUMBER-1); i++)
		{
			if(i <= STICK_LV)
			{
				 calibrate_calc(g_adc_msg.msg[i], &g_adc_calibr_val[i], g_Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
								g_Param_To_Store.ADCs_Calibrate_value[i].MidValue, g_Param_To_Store.ADCs_Calibrate_value[i].LowValue);
			}
			else
			{
				calibrate_calc(g_adc_msg.msg[i], &g_adc_calibr_val[i], g_Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
							  (g_Param_To_Store.ADCs_Calibrate_value[i].MidValue - g_Param_To_Store.ADCs_Calibrate_value[i].LowValue)/2,
							   g_Param_To_Store.ADCs_Calibrate_value[i].LowValue);
			}
			
		}

		//�������زɼ�
		misc_slide_value = Misc_Slide_Switch_Read();

		//����ͨ��
		

		//����Task_disp��ʾ������ֵ


		//���ͷ����


		/***********���ն��д���*************/
		//Task_disp�����Ķ漰����У�������--��HighValue��MidValue��LowValue�ֱ𱣴�,����


		//Task_disp������ʧЧ����ģʽ����-- ��ǰֵ����ģʽ�����¼��ǰ���ź�ģʽֵ����������ֱ�ӱ��棬 ����flash

		osDelay(2);		
	}
}

/***************************************************************************************************
* @fn      SKYBORNE_ADC_DMA_IRQHandlerCallbackHook
*
* @brief   ң�������Ƹ���������ص�����
* @param   adcs_value -- ADC ֵ����
*          len ����
* @return  null
***************************************************************************************************/ 
void SKYBORNE_ADC_DMA_IRQHandlerCallbackHook(uint16_t *adcs_value, uint8_t len)
{
	MSG_QUEUE_t msg;

	msg.len = len;
	memcpy(msg.msg, adcs_value, len);
	//both ISR and Task
	osMessagePut(xQueue_ToADCs, &msg, 0);
}

/***************************************************************************************************
 * @fn      ReadFlash
 *
 * @brief   ����Ӧ��ַ��FLASH����
 * @param   @arg addr -- ָ�������ݵ�ַ
 *   
 * @return  ���ض�ȡ����wordֵ
 ***************************************************************************************************/
Param_To_Store_t ReadFlash(uint32_t addr)
{
  Param_To_Store_t data;
  Param_To_Store_t *p = (Param_To_Store_t*)addr;
  data = *p;
  return data;	
}

/***************************************************************************************************
 * @fn      WriteFlash
 *
 * @brief   ����Ӧ��ַ��FLASHд��һ���ֵ�����
 * @param   @arg addr -- ָ����������ʼ��ַ
 *          @arg data -- Ҫд���������
 *          @arg len  -- Ҫд��������ݳ���
 * @return  NULL
 ***************************************************************************************************/
void WriteFlash(uint32_t addr, uint32_t *pdata, uint32_t len)
{
	FLASH_Status FLASHStatus;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	              FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

	FLASHStatus = FLASH_EraseSector(ADDR_FLASH_SECTOR_3, VoltageRange_3);  

	while(len--)
	{
		if(FLASHStatus == FLASH_COMPLETE)
		{
	  		FLASHStatus = FLASH_ProgramWord(addr, *pdata);
		}
		addr += 4;
		pdata++;
	}
}

/***************************************************************************************************
 * @fn      calibrate_calc
 *
 * @brief   ADC������������У��
 * @param   curADC -- ��ǰ����ֵ
 *          *pGet --  У��ֵ����
 *          calibHighValue  -- ��һУ����
 *			calibMidValue  --  �ڶ�У����
 *			calibLowValue  --  ����У����
 * @return  0 -- success
 *			-1 -- failure
 ***************************************************************************************************/

int adc_calib_calc(uint16_t curADC, uint16_t *pGet, uint16_t calibHighValue, uint16_t calibMidValue, uint16_t calibLowValue)
{
	int16_t x1,x2,x3,y1,y2,y3;

	x1 = 0;
	x2 = 2048;
	x3 = 4096;

	y1 = calibLowValue;
	y2 = calibMidValue;
	y3 = calibHighValue;


	*pGet = 
	
	return 0;
}


/***************************************************************************************************
 * @fn      adc_mixer_set
 *
 * @brief   ���÷ɿض�L1~L6��Ӧͨ���Ĳ������
 * @param   landingValue -- ָ����L1~L6ͨ������ϼ�����ֵ
 * @return  0 -- success
 ***************************************************************************************************/
int adc_mixer_set(MIXER_LANDING_MODE_t landingValue)
{
	portENTER_CRITICAL();
	g_landingSetting = landingValue;
	portEXIT_CRITICAL();

	return 0;
}

/***************************************************************************************************
 * @fn      adc_mixer_get
 *
 * @brief   ȡ�÷ɿض�L1~L6��Ӧͨ���Ĳ������
 * @param   *p_landingValue -- ��ȡָ����L1~L6ͨ������ϼ�����ֵ
 * @return  0 -- success
 *			1 -- fail
 ***************************************************************************************************/
int adc_mixer_get(MIXER_LANDING_MODE_t *p_landingValue)
{	
	if(*p_landingValue == NULL)
	{
		return -1;
	}
	
	portENTER_CRITICAL();
	*p_landingValue = g_landingSetting;
	portEXIT_CRITICAL();

	return 0;
}

/***************************************************************************************************
 * @fn      adc_stick_cntr_val_set
 *
 * @brief   ���ø��漰��ť��ֵ
 * @param   null
 * @return  0 -- success
 ***************************************************************************************************/
int adc_stick_cntr_val_set(void)
{
	uint8_t i;
	
	portENTER_CRITICAL();

	for(i=0; i<ADC_MODULE_NUMBER; i++)
	{
		g_Param_To_Store.ADCs_Calibrate_value[i].MidValue = g_adc_msg.msg[i];
	}	
	
	portEXIT_CRITICAL();

	return 0;
}

/***************************************************************************************************
 * @fn      adc_stick_most_val_set_start
 *
 * @brief   ��ʼ���ø��漰��ť�����Сֵ
 * @param   null
 * @return  0 -- success
 ***************************************************************************************************/
int adc_stick_most_val_set_start(void)
{	
	portENTER_CRITICAL();

	g_SetStickMaxMinFlag = 1;	
	
	portEXIT_CRITICAL();

	return 0;
}

/***************************************************************************************************
 * @fn      adc_stick_most_val_set_end
 *
 * @brief   ���ø��漰��ť�����Сֵ����
 * @param   null
 * @return  0 -- success
 ***************************************************************************************************/
int adc_stick_most_val_set_end(void)
{	
	portENTER_CRITICAL();

	g_SetStickMaxMinFlag = 0;	
	
	portEXIT_CRITICAL();

	return 0;
}


/***************************************************************************************************
 * @fn      get_AllInputsValue
 *
 * @brief   ��ȡ���ж桢��ť���������ص�У����ֵ
 * @param   null
 * @return  0 -- success
 *			1 -- fail
 ***************************************************************************************************/
int adc_all_in_val_get(ALL_STICK_INPUT_t *stickValue)
{
	if(stickValue == NULL)
	{
		return -1;
	}

	portENTER_CRITICAL();
	
	memcpy(*stickValue->adcs,  g_gets_adcs_val, ADC_MODULE_NUMBER);
			
	*stickValue.SLSW = g_lastSlideSwVal;
	
	portEXIT_CRITICAL();

	return 0;		
}


//////////////end of file/////////////////////////////////////////////////////
