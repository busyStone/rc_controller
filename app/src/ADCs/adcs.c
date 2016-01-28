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
uint16_t					gets_adcs_value[MODULE_NUMBER];						//��ͨ��ֵ����
uint16_t					adcs_value_buf[MODULE_NUMBER];						//��ͨ��ֵ�ڶ�����
uint16_t					adcs_calibrate_value[MODULE_NUMBER-1];				//У��ֵ����
uint16_t					ppm_channels[PPM_CH_NUM];
MISC_SLIDE_SWITCH_VALUE		misc_slide_value;


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
	Param_To_Store_t Param_To_Store;
	uint8_t			 i;
	
	argument = argument;

	//------��flash�ж�ȡУ��ֵ��ʧЧ��������-----------------
	Param_To_Store = (Param_To_Store_t) ReadFlash(ParaFlashAddr);
	if(Param_To_Store.valid != 1)			//������Ч,�ָ�Ĭ��ֵ
	{
		for(i=0; i<(MODULE_NUMBER-1); i++)
		{
			Param_To_Store.ADCs_Calibrate_value[i].HighValue = 4095;
			Param_To_Store.ADCs_Calibrate_value[i].MidValue = 2048;
			Param_To_Store.ADCs_Calibrate_value[i].LowValue = 0;
		}
		memset(Param_To_Store.channel_value, 0, sizeof(Param_To_Store.channel_value));
		Param_To_Store.failsafeMode = FAILSAFE_MODE_RTL;
	}
	
	for(;;)
	{
		osSemaphoreWait(xSemaphore_ForADCs, osWaitForever);

		memcpy(adcs_value_buf, gets_adcs_value, MODULE_NUMBER);

		//��������
		for(i=0; i<(MODULE_NUMBER-1); i++)
		{
			if(i <= STICK_LV)
			{
				adcs_calibrate_value[i] = calibrate_calc(adcs_value_buf[i], Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
													 Param_To_Store.ADCs_Calibrate_value[i].MidValue, Param_To_Store.ADCs_Calibrate_value[i].LowValue);
			}
			else
			{
				adcs_calibrate_value[i] = calibrate_calc(adcs_value_buf[i], Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
													 (Param_To_Store.ADCs_Calibrate_value[i].MidValue - Param_To_Store.ADCs_Calibrate_value[i].LowValue)/2,
													 Param_To_Store.ADCs_Calibrate_value[i].LowValue);
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
	memcpy(gets_adcs_value, adcs_value, len);
	//both ISR and Task
	osSemaphoreRelease(xSemaphore_ForADCs);
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
 * @brief   ����Ӧ��ַ��FLASHд��һ���ֵ�����
 * @param   @arg addr -- ָ����������ʼ��ַ
 *          @arg data -- Ҫд���������
 *          @arg len  -- Ҫд��������ݳ���
 * @return  NULL
 ***************************************************************************************************/

uint16_t calibrate_calc(uint16_t curADC, uint16_t calibHighValue, uint16_t calibMidValue, uint16_t calibLowValue)
{
	return 0;
}
	

//////////////end of file/////////////////////////////////////////////////////
