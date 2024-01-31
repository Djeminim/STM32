#include "stm32f10x.h"
//#include "system_stm32f10x.c"
#include "ENCODER.h"

void ENCODER ()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

//включение необходимых пинов и портов (лучше делать в 1 месте);
//8- настройка ножки в режим Pull_up/pull-down (кнопка)
  
  GPIOA->CRH &= 0xFFFFFF0F; //Энокдер. PА9-Кнопка
  GPIOA->CRH |= 0x00000080;
  
  GPIOC->CRH &= 0xFFF000FF;
  GPIOC->CRH |= 0x00088800; //PC10, PC11- Энокдер
  
 //Если мы настроили CRH в режим пуш/пул, то ODR отвечает за подтяжку к земле
  //И поднятию и опускании 1 на портах.
  
  GPIOA->ODR |= GPIO_ODR_ODR9;
  GPIOC->ODR |= GPIO_ODR_ODR11;
  GPIOC->ODR |= GPIO_ODR_ODR12;
  
  
//-------------------------------------------------------
//Настройка прерываний по убывающему фронту
  EXTI->FTSR |= EXTI_FTSR_TR9;
  EXTI->FTSR |= EXTI_FTSR_TR11;
 

//Разрешение прерываний
  EXTI->IMR |= EXTI_IMR_MR9;
  EXTI->IMR |= EXTI_IMR_MR11;
  

//Указатель откуда ловить прерывания
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PA; //PÀ9 EXTICR3 ÎÒÑ×ÅÒ Ñ 0 = ÏÎÒÎÌ ÐÅÀËÜÍÛÉ EXTICR
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PC; //PC11 EXTICR3

//Сброс флагов прерываний
  EXTI->PR |= EXTI_PR_PR9; 
  EXTI->PR |= EXTI_PR_PR11;
  
  
  NVIC_EnableIRQ(EXTI15_10_IRQn); //Âêëþ÷èòü ïðåðûâàíèÿ ñ 10-ïî 15 íîæêè
  NVIC_EnableIRQ(EXTI9_5_IRQn);
  
}

void Caunter ()
{
  if (Parameter_Selection == 0)
  {
    edinizi = E;
    desiatki = D;
  }
  
  else if (Parameter_Selection == 1)
  {
    edinizi_1 = E;
    desiatki_1 = D;
    E1 = edinizi_1;
    D1 = desiatki_1;
  }
  
  else if (Parameter_Selection == 2)
  {
    edinizi_2 = E;
    desiatki_2 = D;
    E2 = edinizi_2;
    D2 = desiatki_2;
  } 
}


 void EXTI9_5_IRQHandler () 
{ 
  if (EXTI->PR & EXTI_PR_PR9) //EXTI->PR & EXTI_PR_PR9
  {
    if (GPIOA->IDR & GPIO_IDR_IDR9)
    {
    Parameter_Selection++;
        
    if (Parameter_Selection > 2)
    {
      Parameter_Selection = 0;
    }
    
    EXTI->PR |= EXTI_PR_PR9;
    EXTI->PR |= EXTI_PR_PR9; 
    }
  }
}


void EXTI15_10_IRQHandler () 
{
  if (EXTI->PR & EXTI_PR_PR11)
  {
    if (Parameter_Selection == 1)
    {
      E = E1;
      D = D1;
    }
   
    else if (Parameter_Selection == 2)
    {
      E = E2;
      D = D2;
    }
    
    if (GPIOC->IDR & GPIO_IDR_IDR12)
    {
      if (E < 0x39 && D <= 0x39) 
      { 
        E++;
      }
          
      else if (E >= 0x39 && D < 0x39)
      {
        D++;
        E = 0x30;
      }
    
      else if (E == 0x39 && D == 0x39){}
    }
    
    else
    {
      if (E > 0x30 && D >= 0x30)
      { 
        E--;
      }

      else if (E == 0x30 && D > 0x30)
      {
        D--;
        E = 0x39;
      }
          
      else if (E == 0x30 && D == 0x30){}
    }
    
    Caunter();
    
    EXTI->PR |= EXTI_PR_PR11;
    EXTI->PR |= EXTI_PR_PR11;
  }
}
  
  
  
  
