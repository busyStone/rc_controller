#ifndef __ANA_INPUTS_H
#define __ANA_INPUTS_H


#include "Stm32f2xx.h"
#include "Stm32f2xx_gpio.h"
#include "Stm32f2xx_adc.h"
#include "Misc.h"
#include "Stm32f2xx_rcc.h"
#include "Stm32f2xx_dma.h"
#include "cmsis_os.h"

#include <stdint.h>


//-----------------ҡ�ˡ����ֶ˿ڶ���---------------- 
//�Ҷ�����
#define		STICK_RV_PIN			GPIO_Pin_0
#define		STICK_RV_PORT			GPIOA
#define		STICK_RV_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define		STICK_RV_PIN_SOURCE		GPIO_PinSource0
#define		STICK_RV_ADC			ADC1
#define		STICK_RV_ADC_CLK		RCC_APB2Periph_ADC1
#define		STICK_RV_ADC_CHANNEL	ADC_Channel_0

//�Ҷ�����
#define		STICK_RH_PIN			GPIO_Pin_1
#define		STICK_RH_PORT			GPIOA
#define		STICK_RH_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define		STICK_RH_PIN_SOURCE		GPIO_PinSource1
#define		STICK_RH_ADC			ADC1
#define		STICK_RH_ADC_CLK		RCC_APB2Periph_ADC1
#define		STICK_RH_ADC_CHANNEL	ADC_Channel_1

//�������
#define		STICK_LH_PIN			GPIO_Pin_2
#define		STICK_LH_PORT			GPIOA
#define		STICK_LH_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define		STICK_LH_PIN_SOURCE		GPIO_PinSource2
#define		STICK_LH_ADC			ADC1
#define		STICK_LH_ADC_CLK		RCC_APB2Periph_ADC1
#define		STICK_LH_ADC_CHANNEL	ADC_Channel_2

//�������
#define		STICK_LV_PIN			GPIO_Pin_3
#define		STICK_LV_PORT			GPIOA
#define		STICK_LV_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define		STICK_LV_PIN_SOURCE		GPIO_PinSource3
#define		STICK_LV_ADC			ADC1
#define		STICK_LV_ADC_CLK		RCC_APB2Periph_ADC1
#define		STICK_LV_ADC_CHANNEL	ADC_Channel_3

//���ϲ���
#define		ROTATE_LU_PIN			GPIO_Pin_6
#define		ROTATE_LU_PORT			GPIOA
#define		ROTATE_LU_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define		ROTATE_LU_PIN_SOURCE	GPIO_PinSource6
#define		ROTATE_LU_ADC			ADC1
#define		ROTATE_LU_ADC_CLK		RCC_APB2Periph_ADC1
#define		ROTATE_LU_ADC_CHANNEL	ADC_Channel_6

//���²���
#define		ROTATE_LD_PIN			GPIO_Pin_4
#define		ROTATE_LD_PORT			GPIOC
#define		ROTATE_LD_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define		ROTATE_LD_PIN_SOURCE	GPIO_PinSource4
#define		ROTATE_LD_ADC			ADC1
#define		ROTATE_LD_ADC_CLK		RCC_APB2Periph_ADC1
#define		ROTATE_LD_ADC_CHANNEL	ADC_Channel_14

//���ϲ���
#define		ROTATE_RU_PIN			GPIO_Pin_0
#define		ROTATE_RU_PORT			GPIOB
#define		ROTATE_RU_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define		ROTATE_RU_PIN_SOURCE	GPIO_PinSource0
#define		ROTATE_RU_ADC			ADC1
#define		ROTATE_RU_ADC_CLK		RCC_APB2Periph_ADC1
#define		ROTATE_RU_ADC_CHANNEL	ADC_Channel_8

//���²���
#define		ROTATE_RD_PIN			GPIO_Pin_5
#define		ROTATE_RD_PORT			GPIOC
#define		ROTATE_RD_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define		ROTATE_RD_PIN_SOURCE	GPIO_PinSource5
#define		ROTATE_RD_ADC			ADC1
#define		ROTATE_RD_ADC_CLK		RCC_APB2Periph_ADC1
#define		ROTATE_RD_ADC_CHANNEL	ADC_Channel_15

//-----------------��ѹ���˿ڶ���----------------
#define		MONITOR_VOL_PIN			GPIO_Pin_0
#define		MONITOR_VOL_PORT		GPIOC
#define		MONITOR_VOL_GPIO_CLK	RCC_AHB1Periph_GPIOC
#define		MONITOR_VOL_PIN_SOURCE	GPIO_PinSource0
#define		MONITOR_VOL_ADC			ADC1
#define		MONITOR_VOL_ADC_CLK		RCC_APB2Periph_ADC1
#define		MONITOR_VOL_ADC_CHANNEL	ADC_Channel_10

#define		SKYBORNE_ADC				ADC1
#define		SKYBORNE_ADC_CLK			RCC_APB2Periph_ADC1
#define		SKYBORNE_ADC_DMA_STREAM		DMA2_Stream0
#define		SKYBORNE_ADC_DMA_CHANNEL 	DMA_Channel_0
#define		SKYBORNE_ADC_DMA_IRQn		DMA2_Stream0_IRQn
#define		SKYBORNE_ADC_DMA_IRQHandler	DMA2_Stream0_IRQHandler
#define		SKYBORNE_ADC_DMA_TC_FLAG	DMA_IT_TCIF0
#define		SKYBORNE_ADC_DMA_CLK		RCC_AHB1Periph_DMA2
#define		SKYBORNE_ADC_NVIC_PRIORITY	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1

//-------------�ṹ����---------------------------

typedef enum
{
	STICK_RV,
	STICK_RH,
	STICK_LH,
	STICK_LV,
	ROTATE_LU,
	ROTATE_LD,
	ROTATE_RU,
	ROTATE_RD,
	MONITOR_VOL,
	ADC_MODULE_NUMBER
}SKYBORNE_ADC_MODULES;


//-------------��������---------------------------

void ana_inputs_init(void);
void ana_inputs_sample_start(void);
void ana_inputs_adc_dma_irq_handler_callback(void);

#endif
