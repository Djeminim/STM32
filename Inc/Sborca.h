#ifndef TIM2_100RB //если имя не определено
#define TIM2_100RB //определить имя

void TIM2_Init ();

#endif //если имя определилось, то повторно не повторять


#include "stm32f10x.h"
#include "system_stm32f10x.c"
#include "ENCODER.с"
#include "Display_mini.с"
#include "SPI2_DMA1_100RB.с"
#include "TIM2_100RB.с"
#include "TIM2_100RB.h"
