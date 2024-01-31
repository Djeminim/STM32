#include "stm32f10x.h"
//#include "system_stm32f10x.c"
#include "TIM2_100RB.h"

void TIM2_Init ()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  TIM2->PSC = 2400-1; //24 000 000/ 2400 = 10 000Гц 
  TIM2->ARR = 1000-1; //10 000/1000 = 10 Гц/сек (10 сигналов в секунду)
  TIM2->DIER |= TIM_DIER_UIE; //Разрешение прерываний
  TIM2->CR1 |=TIM_CR1_ARPE;

  NVIC_EnableIRQ(TIM2_IRQn);

  TIM2->CR1 |= TIM_CR1_CEN;
}
