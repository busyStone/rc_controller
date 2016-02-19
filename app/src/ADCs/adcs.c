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
#include "communicate.h"
#include "power.h"


#include <stdlib.h>
#include <string.h>


/////////////////////ȫ�ֱ�������///////////////////////////////////////
int16_t 					g_adc_calibr_val[ADC_MODULE_NUMBER-1];				//У��ֵ����
uint16_t					g_ppm_channels[PPM_CH_NUM];
MISC_SW_VALUE				g_misc_sli_sw_value;

uint8_t             		g_landingSettingFlag;								//L1~L6ģʽ��ϼ�����
uint8_t						g_SetStickMaxMinFlag;								//���ø�������ֵ��־
uint8_t                     g_radioPairingReqFlag;                              //ң������ջ��������flag
uint8_t                     g_saveParaFlag;                                     //���������־

Param_To_Store_t 			g_Param_To_Store;									//��������ֵ����
MSG_QUEUE_t		 			g_adc_msg;											//ADC��ͨ��ֵ����
uint16_t					g_gets_adcs_val[ADC_MODULE_NUMBER];					//���������ȡ��ͨ��ֵ����





///////////////////��������/////////////////////////////////
Param_To_Store_t ReadFlash(uint32_t addr);
void WriteFlash(uint32_t addr, uint32_t *pdata, uint32_t len);
int adc_calib_calc(uint16_t curADC, uint16_t *pGet, uint16_t calibHighValue, uint16_t calibMidValue, uint16_t calibLowValue, uint16_t enable);
float adc_voltage_calc(uint16_t adc_val);
void adc_calib_ram_reset(ADC_Value_Calibration_t *calib_data, uint8_t len);


/***************************************************************************************************
* @fn      Task_ADCs
*
* @brief   ң�������Ƹˡ����������źŴ�������
* @param   NULL
* @return  null
***************************************************************************************************/ 
void Task_ADCs(void const * argument)
{
	uint8_t			        i;
    uint8_t                 LastSetStickMaxMinFlag=0;
    uint8_t                 LastRadioPairingReqFlag;
    uint8_t                 LastPairingStatusFlag;
    uint8_t                 buf[32];
    float                   fVoltage;
    ADC_Value_Calibration_t calib_para[ADC_MODULE_NUMBER-1];
	
	argument = argument;

	//------��flash�ж�ȡУ��ֵ��ʧЧ��������-----------------
	g_Param_To_Store = (Param_To_Store_t) ReadFlash(ParaFlashAddr);
	if(g_Param_To_Store.valid != 0x55)			//������Ч,�ָ�Ĭ��ֵ
	{
		for(i=0; i<(ADC_MODULE_NUMBER-1); i++)
		{
			g_Param_To_Store.ADCs_Calibrate_value[i].HighValue = 4096;
			g_Param_To_Store.ADCs_Calibrate_value[i].MidValue = 2048;
			g_Param_To_Store.ADCs_Calibrate_value[i].LowValue = 0;            
		}

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L1.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L1.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L1.value.sws_value = (uint16_t)((SLIDE_DOWN << SW_SD_L_SHIFT) | (SLIDE_UP << SW_SE_L_SHIFT));

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L2.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L2.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L2.value.sws_value = (uint16_t)((SLIDE_DOWN << SW_SD_L_SHIFT) | (SLIDE_MID << SW_SE_L_SHIFT));

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L3.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L3.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L3.value.sws_value = (uint16_t)((SLIDE_DOWN << SW_SD_L_SHIFT) | (SLIDE_DOWN << SW_SE_L_SHIFT));

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L4.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L4.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L4.value.sws_value = (uint16_t)((SLIDE_UP << SW_SD_L_SHIFT) | (SLIDE_UP << SW_SE_L_SHIFT));

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L5.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L5.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L5.value.sws_value = (uint16_t)((SLIDE_UP << SW_SD_L_SHIFT) | (SLIDE_MID << SW_SE_L_SHIFT));

        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L6.switchs = SW_SD_MASK | SW_SE_MASK;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L6.logic = AND;
        g_Param_To_Store.mixer_channel_config.Mixer_Mode_L6.value.sws_value = (uint16_t)((SLIDE_UP << SW_SD_L_SHIFT) | (SLIDE_DOWN << SW_SE_L_SHIFT));
        
		g_Param_To_Store.failsafeMode = FAILSAFE_MODE_RTL;
        g_Param_To_Store.rx_num = 0;
		g_Param_To_Store.calib_valid = 0;
	}

    //init caibrat data
    adc_calib_ram_reset(calib_para, ADC_MODULE_NUMBER-1);
	g_SetStickMaxMinFlag = 0;

    printf("adc task read flash over. at %d\r\n", __LINE__);
	for(i=0; i<ADC_MODULE_NUMBER-1; i++)
    {
		printf("calib_para[%d].HighValue = %d, calib_para[%d].LowValue=%d @ %s, %s, %d\r\n", i, calib_para[i].HighValue, i, calib_para[i].LowValue, __FILE__, __func__, __LINE__);
    }
    
	for(;;)
	{
		ana_inputs_sample_start();
		xQueueReceive(xQueue_ToADCs, &g_adc_msg, portMAX_DELAY);

		//printf("adc task receive adc value @ %s, %s, %d\r\n", __FILE__, __func__, __LINE__);
		//for(i=0; i<g_adc_msg.len; i++)
		for(i=0; i<9; i++)
		{
			printf("ch[%d]=%d ", i, g_adc_msg.msg[i]);
		}
		printf("\r\n");
        
        //ң��У׼������
        if(g_SetStickMaxMinFlag)    
        {
            for(i=0; i<ADC_MODULE_NUMBER-1; i++)
            {
            	//printf("g_adc_msg.msg[%d] = %d, calib_para[%d].HighValue = %d, calib_para[%d].LowValue=%d @ %s, %s, %d\r\n", i, g_adc_msg.msg[i], i, calib_para[i].HighValue, i, calib_para[i].LowValue, __FILE__, __func__, __LINE__);
                calib_para[i].HighValue = MAX(calib_para[i].HighValue, g_adc_msg.msg[i]);
                calib_para[i].LowValue = MIN(calib_para[i].LowValue, g_adc_msg.msg[i]);
				printf("calib_para[%d].HighValue = %d, calib_para[%d].LowValue=%d @ %s, %s, %d\r\n", i, calib_para[i].HighValue, i, calib_para[i].LowValue, __FILE__, __func__, __LINE__);
            }
            LastSetStickMaxMinFlag = 1;

            osDelay(2);	
            continue;
        }
        
        if(LastSetStickMaxMinFlag) //У׼����
        {
            for(i=0; i<ADC_MODULE_NUMBER-1; i++)
            {
                g_Param_To_Store.ADCs_Calibrate_value[i].HighValue = calib_para[i].HighValue;
                g_Param_To_Store.ADCs_Calibrate_value[i].LowValue = calib_para[i].LowValue;
            }
            
            //init caibrat data
            adc_calib_ram_reset(calib_para, ADC_MODULE_NUMBER-1);
            LastSetStickMaxMinFlag = 0;
            g_Param_To_Store.calib_valid = 1;
			
            //��������flash
            g_saveParaFlag = 1;
        }
		
		//��������
		for(i=0; i<(ADC_MODULE_NUMBER-1); i++)
		{		
			if(i <= STICK_LV)
			{
				adc_calib_calc(g_adc_msg.msg[i], &g_adc_calibr_val[i], g_Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
							    g_Param_To_Store.ADCs_Calibrate_value[i].MidValue, g_Param_To_Store.ADCs_Calibrate_value[i].LowValue,g_Param_To_Store.calib_valid);

                //����ǰ4ͨ��
                g_ppm_channels[i] = g_adc_calibr_val[i];
				printf("g_adc_calibr_val[%d] = %d, g_Param_To_Store.calib_valid=%d @ %s, %s, %d\r\n", i,  g_adc_calibr_val[i], g_Param_To_Store.calib_valid, __FILE__, __func__, __LINE__);
			}
			else
			{
				adc_calib_calc(g_adc_msg.msg[i], &g_adc_calibr_val[i], g_Param_To_Store.ADCs_Calibrate_value[i].HighValue, 
							  (g_Param_To_Store.ADCs_Calibrate_value[i].MidValue - g_Param_To_Store.ADCs_Calibrate_value[i].LowValue)/2,
							   g_Param_To_Store.ADCs_Calibrate_value[i].LowValue,g_Param_To_Store.calib_valid);

                g_ppm_channels[CH5_MIX + (i-STICK_LV)] = g_adc_calibr_val[i];
				printf("g_adc_calibr_val[%d] = %d, g_Param_To_Store.calib_valid=%d @ %s, %s, %d\r\n", i,  g_adc_calibr_val[i], g_Param_To_Store.calib_valid, __FILE__, __func__, __LINE__);
			}                
            //printf("i=%d, val=%d, calib=%d. at %d\r\n", i, g_adc_msg.msg[i],g_adc_calibr_val[i], __LINE__);
		}        

        //��ѹ��������
        fVoltage = adc_voltage_calc(g_adc_msg.msg[ADC_MODULE_NUMBER-1]);    

        //������ֵ��ui��ȡ
        memcpy(g_gets_adcs_val, g_adc_calibr_val, ADC_MODULE_NUMBER-1);
        g_gets_adcs_val[ADC_MODULE_NUMBER-1] = (uint16_t)(fVoltage*100);
        
		//--------------�������زɼ�---------------------
		g_misc_sli_sw_value = switches_misc_sw_read();

        //���ͨ��
        if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L1, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L1;
        }
        else if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L2, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L2;
        }
        else if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L3, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L3;
        }
        else if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L4, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L4;
        }
        else if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L5, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L5;
        }
        else if(0 == adc_mixer_calc( g_Param_To_Store.mixer_channel_config.Mixer_Mode_L6, g_misc_sli_sw_value))
        {
            g_ppm_channels[CH5_MIX] = FLIGHT_MODE_L6;
        }
        else
        {
            printf("mix channel get wrong config!\r\n");
        }

        //----������------------------
        //���
        if(g_radioPairingReqFlag)
        {
            buf[0] = 0;
            buf[1] = 1;
            comm_data_send(1, buf, 2);
            LastPairingStatusFlag = 1;
        }
        else
        {
            if(LastPairingStatusFlag)
            {
                buf[0] = 0;
                buf[1] = 0;
                comm_data_send(1, buf, 2);
                LastPairingStatusFlag = 0;
            }
        }

        //ͨ������
        //comm_data_send(3, g_ppm_channels, sizeof(g_ppm_channels));
#if 0
        printf("send package at %d:\r\n",  __LINE__);
        for(i=0; i<PPM_CH_NUM; i++)
        {
            printf("%#x ", g_ppm_channels[i]);
        }
        printf("\r\n");
#endif
        //---------����FLASH---------------
        if(g_saveParaFlag)
        {
            g_saveParaFlag = 0;

            //WriteFlash(ParaFlashAddr, &g_Param_To_Store, sizeof(g_Param_To_Store)/4);
        }

        

		osDelay(50);		
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
void ana_inputs_adc_dma_irq_handler_cb_hook(uint16_t *adcs_value, uint8_t len)
{
	MSG_QUEUE_t msg;
	portBASE_TYPE taskWoken = pdFALSE;

	msg.len = len;
	memcpy(msg.msg, adcs_value, len*2);
	//both ISR and Task
	xQueueSendFromISR(xQueue_ToADCs, &msg, &taskWoken);
	portEND_SWITCHING_ISR(taskWoken);
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
int adc_calib_calc(uint16_t curADC, uint16_t *pGet, uint16_t calibHighValue, uint16_t calibMidValue, uint16_t calibLowValue, uint16_t enable)
{
	float x1,x2,x3,y1,y2,y3,y;

	if(!enable)
	{
		*pGet = curADC;
		return 0;
	}
	
	y1 = ADC_CALIB_VAL_Y1;
	y2 = ADC_CALIB_VAL_Y2;
	y3 = ADC_CALIB_VAL_Y3;

	x1 = (float)calibLowValue;
	x2 = (float)calibMidValue;
	x3 = (float)calibHighValue;

	y = y1*(((curADC-x2)*(curADC-x3))/((x1-x2)*(x1-x3))) + y2*(((curADC-x1)*(curADC-x3))/((x2-x1)*(x2-x3))) + y3*(((curADC-x1)*(curADC-x2))/((x3-x1)*(x3-x2)));

    *pGet = (int16_t)y;
    
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
     g_Param_To_Store.mixer_channel_config = landingValue;
	portEXIT_CRITICAL();
    
    g_saveParaFlag = 1;
    
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
	IF_PTR_IS_NULL_RET_NULLPTR_ERR(p_landingValue);
	
	portENTER_CRITICAL();
	*p_landingValue = g_Param_To_Store.mixer_channel_config;
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

    g_saveParaFlag = 1;

	for(i=0; i<ADC_MODULE_NUMBER; i++)
	{
		printf("g_Param_To_Store.ADCs_Calibrate_value[%d].MidValue=%d @ %s, %s, %d\r\n",i, g_Param_To_Store.ADCs_Calibrate_value[i].MidValue, __FILE__, __func__, __LINE__);
	}
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
 * @fn      adc_all_in_val_get
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
	
	memcpy(stickValue->adcs,  g_gets_adcs_val, sizeof(g_gets_adcs_val));
            
	stickValue->SW.sws_value = g_misc_sli_sw_value.sws_value;
	
	portEXIT_CRITICAL();

	for(uint8_t i=0; i<9; i++)
	{
		printf("stickValue->adcs[%d]=%d @ %s, %s, L%d\r\n", i, stickValue->adcs[i], __FILE__, __func__, __LINE__);
	}
	
	return 0;		
}

/***************************************************************************************************
 * @fn      adc_radio_pairing_req
 *
 * @brief   ң�ض���������ջ����
 * @param   null
 * @return  0 -- success
 ***************************************************************************************************/
int adc_radio_pairing_req(uint32_t x)
{
	portENTER_CRITICAL();
	
	g_radioPairingReqFlag = 1;
    g_Param_To_Store.rx_num = x;
	
	portEXIT_CRITICAL();

	return 0;		
}


/***************************************************************************************************
 * @fn      adc_radio_pairing_end
 *
 * @brief   ��Խ���
 * @param   null
 * @return  0 -- success
 ***************************************************************************************************/
int adc_radio_pairing_end(void)
{
	portENTER_CRITICAL();
	
	g_radioPairingReqFlag = 0;
	
	portEXIT_CRITICAL();
    
    g_saveParaFlag = 1;

	return 0;		
}


/***************************************************************************************************
 * @fn      adc_mixer_calc
 *
 * @brief   ��Խ���
 * @param   null
 * @return  0 -- success
 *          -1 - failure
 ***************************************************************************************************/
int adc_mixer_calc(MIXER_CHANNEL_MODE_t mixer, MISC_SW_VALUE sw_val)
{
	if(mixer.switchs != 0)
    {
        if((sw_val.sws_value & mixer.switchs) == mixer.value.sws_value)        //mixer.logic == AND һ�������� OR �� NONE Ҳһ������
        {
            return 0;
        }
        else    //mixer.logic == AND һ���������� OR �� NONE ���ܳ���
        {
            if((mixer.switchs & SW_SA_MASK) != 0)           //�ж���ϼ��еĵ�����ֵ
            {
                if((sw_val.sws_value & SW_SA_MASK) == (mixer.value.sws_value & SW_SA_MASK))
                {
                    return 0;
                }
            }

            if((mixer.switchs & SW_SB_MASK) != 0)           //�ж���ϼ��еĵ�����ֵ
            {
                if((sw_val.sws_value & SW_SB_MASK) == (mixer.value.sws_value & SW_SB_MASK))
                {
                    return 0;
                }
            }

            if((mixer.switchs & SW_SC_MASK) != 0)           //�ж���ϼ��еĵ�����ֵ
            {
                if((sw_val.sws_value & SW_SC_MASK) == (mixer.value.sws_value & SW_SC_MASK))
                {
                    return 0;
                }
            }

            if((mixer.switchs & SW_SD_MASK) != 0)           //�ж���ϼ��еĵ�����ֵ
            {
                if((sw_val.sws_value & SW_SD_MASK) == (mixer.value.sws_value & SW_SD_MASK))
                {
                    return 0;
                }
            }

            if((mixer.switchs & SW_SE_MASK) != 0)           //�ж���ϼ��еĵ�����ֵ
            {
                if((sw_val.sws_value & SW_SE_MASK) == (mixer.value.sws_value & SW_SE_MASK))
                {
                    return 0;
                }
            }

            return -1;      //��ƥ�䣬���ش���
        }
    }
    else    //��ƥ��ֵ
    {
        return -1;
    }
}


/***************************************************************************************************
 * @fn      adc_voltage_calc
 *
 * @brief   �����ѹ���ֵ
 * @param   null
 * @return  ��ѹֵ
 ***************************************************************************************************/
float adc_voltage_calc(uint16_t adc_val)
{
    int val;
    //int cap;  --�������
    float t;
    static float lastVolt = 0;

    t = (float)adc_val;
    
    t = t / 4096 * 3.3 / 0.175;      // 3.3-REF   0.175 -- resistive divider ratio

    //filter
    t = t*LOW_PASS_FILTER_PARA + (1-LOW_PASS_FILTER_PARA)*lastVolt;

    lastVolt = t;

	if(t > 3.6)
    {
        //���ֿ���
        pwr_on_off(PWR_MODULE_MAIN, PWR_ON);
    }
    else if(t <= 3.4)
    {
        //�ػ�
        pwr_on_off(PWR_MODULE_MAIN, PWR_OFF);
    }

    return (float)t;
}


void adc_calib_ram_reset(ADC_Value_Calibration_t *calib_data, uint8_t len)
{
	uint8_t i;	

	for(i=0; i<len; i++)
	{
		IF_PTR_IS_NULL_RET_NULLPTR_ERR(calib_data);
		calib_data->HighValue = ADC_CALIB_RST_VAL;
		calib_data->MidValue = ADC_CALIB_RST_VAL;
		calib_data->LowValue = ADC_CALIB_RST_VAL;
		calib_data++;
	}
}

//////////////end of file/////////////////////////////////////////////////////
