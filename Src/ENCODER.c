#include "stm32f10x.h"
//#include "system_stm32f10x.c"
#include "ENCODER.h"

void ENCODER ()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

//��������� ��� ����������� ����� � ������ (����� ������ � 1 �����;
//8- ��������� ����� � ����� Pull_up/pull-down (������)
  
  GPIOA->CRH &= 0xFFFFFF0F; //�������. P�9-������)
  GPIOA->CRH |= 0x00000080;
  
  GPIOC->CRH &= 0xFFF000FF;
  GPIOC->CRH |= 0x00088800; //PC10, PC11- �������)
  
  //���� �� ��������� CRH � ����� ���/���, �� ODR �������� �� �������� � �����
  //� �������� � ��������� 1 �� ������.
  
  GPIOA->ODR |= GPIO_ODR_ODR9;
  GPIOC->ODR |= GPIO_ODR_ODR11;
  GPIOC->ODR |= GPIO_ODR_ODR12;
  
  
//-------------------------------------------------------
//��������� ���������� �� ���������� ������
  EXTI->FTSR |= EXTI_FTSR_TR9;
  EXTI->FTSR |= EXTI_FTSR_TR11;
 

//���������� ����������
  EXTI->IMR |= EXTI_IMR_MR9;
  EXTI->IMR |= EXTI_IMR_MR11;
  

//��������� ������ ������ ����������
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PA; //P�9 EXTICR3 ������ � 0 = ����� �������� EXTICR
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PC; //PC11 EXTICR3

//����� ������ ����������
  EXTI->PR |= EXTI_PR_PR9; 
  EXTI->PR |= EXTI_PR_PR11;
  
  
  NVIC_EnableIRQ(EXTI15_10_IRQn); //�������� ���������� � 10-�� 15 �����
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
  
  
  
  