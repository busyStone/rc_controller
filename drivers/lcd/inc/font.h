#ifndef __FONT_H
#define __FONT_H 	   

#include "Stm32f2xx.h"


typedef struct
{								// �ַ���ṹ
    uint8_t Index[1];
    uint8_t Mask[14];
}LittleCharStruct;


extern LittleCharStruct LittleCharLib[];

#endif
