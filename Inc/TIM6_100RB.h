#include "stm32f10x.h"
#include "TIM6_100RB.h"

void TIM6_Init ()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM6->PSC = 24000-1; //24 000 000/ 24 000 = 1000Гц
	TIM6->ARR = 1000-1; //1000/1000 = 1cек
	TIM6->DIER |= TIM_DIER_UIE; //Разрешение прерываний
	TIM6->CR1 |=TIM_CR1_ARPE;

	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	TIM6->RC1 |= TIM_CR1_CEN;

}
