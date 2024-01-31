/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include "stm32f10x.h"
#include "system_stm32f10x.c"
unsigned char edinizi=0x30;
unsigned char desiatki=0x30;

unsigned char edinizi_1=0x30;
unsigned char desiatki_1=0x30;

unsigned char edinizi_2=0x30;
unsigned char desiatki_2=0x30;

unsigned char Result [21];
unsigned char Parameter_Selection = 0;

unsigned char Vigryzka;

unsigned char E = 0x30;
unsigned char D = 0x30;

unsigned char E1 = 0x30;
unsigned char D1 = 0x30;

unsigned char E2 = 0x30;
unsigned char D2 = 0x30;


#include "TIM2_100RB.h"
#include "Display_mini.h"
#include "ENCODER.h"

void main(void)
{  
  Result[0] = (0x5F);
  Result[1] = ((desiatki) & 0x0F);
  Result[2] = (((desiatki & 0xF0)>>4) & 0x0F);
  Result[3] = ((edinizi) & 0x0F);
  Result[4] = (((edinizi & 0xF0)>>4) & 0x0F);
  
  Result[5] = ((' ') & 0x0F);
  Result[6] = ((((' ') & 0xF0)>>4) & 0x0F);
  Result[7] = ((' ') & 0x0F);
  Result[8] = ((((' ') & 0xF0)>>4) & 0x0F);
  
  Result[9] = ((desiatki_1) & 0x0F);
  Result[10] = ((((desiatki_1) & 0xF0)>>4) & 0x0F);
  Result[11] = ((edinizi_1) & 0x0F);
  Result[12] = ((((edinizi_1) & 0xF0)>>4) & 0x0F);
  
  Result[13] = ((' ') & 0x0F);
  Result[14] = ((((' ') & 0xF0)>>4) & 0x0F);
  Result[15] = ((' ') & 0x0F);
  Result[16] = ((((' ') & 0xF0)>>4) & 0x0F);


  Result[17] = ((desiatki_2) & 0x0F);
  Result[18] = (((desiatki_2 & 0xF0)>>4) & 0x0F);
  Result[19] = ((edinizi_2) & 0x0F);
  Result[20] = (((edinizi_2 & 0xF0)>>4) & 0x0F);

  
  SPI2_100RB();
  Display();
  ENCODER();
  DMA1C4C5_Init();
  TIM2_Init();
 
  
  while (1)
  {
  }
}