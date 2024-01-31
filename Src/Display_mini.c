#include "stm32f10x.h"
//#include "system_stm32f10x.c"
#include "Display_mini.h"


unsigned char Send (unsigned char bait)
{
	unsigned short Spi2TimeOut = 0;
	while ((!(SPI2->SR & SPI_SR_TXE)) && (Spi2TimeOut < 1000)) {Spi2TimeOut++;}
	Spi2TimeOut = 0;
	SPI2->DR = bait; //Çàãðóçêà â äð
	while ((!(SPI2->SR & SPI_SR_RXNE)) && (Spi2TimeOut < 1000)) {Spi2TimeOut++;}
	return SPI2->DR; //Выгрузка из др (Обязательно нужно делать, для перезарядки)
}

void Delay_mc (unsigned char Time)
{
	unsigned int Taktov = Time * 2178; //2178 - (ñòîëüêî òàêòîâ â 1mc)
	for (Taktov=0; Taktov<Time; Taktov++);
}

void Data (unsigned char D) //äàííûå äëÿ êîíòðîëëåðà
{
	GPIOB->ODR &= ~GPIO_ODR_ODR12;

	for (int k=0; k<20;k++); //Порой нужно делать задержку
                                //Чтобы контроллер диплея успел
                                //обработать данные

	Send (0x5F); //код который определяет data это или comand
	Send ((D)&0x0F);
	Send (((D&0xF0)>>4)&0x0F);

	for (int k=0; k<20;k++);
	GPIOB->ODR |= GPIO_ODR_ODR12;
	for (int k=0; k<20;k++);
}

void Command (unsigned char C) //Команда для контроллера
{
	GPIOB->ODR &= ~GPIO_ODR_ODR12;
	for (int k=0; k<20;k++);

	Send (0x1F);
	Send ((C)&0x0F);
	Send (((C & 0xF0)>>4)&0x0F);

	for (int k=0; k<20;k++);
	GPIOB->ODR |= GPIO_ODR_ODR12;
	for (int k=0; k<20;k++);
}


void SPI2_100RB (void)
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN ;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  GPIOA->CRH &= 0xFFF0FF00;
  GPIOA->CRH |= 0x00080088;

  GPIOB->CRH &= 0x0000FFFF;
  GPIOB->CRH |= 0xB4B30000;
  
  GPIOC->CRL &= 0xF0FFFFFF;
  GPIOC->CRL |= 0x03000000; 
  
  GPIOB->ODR |= GPIO_ODR_ODR12;//Ñðàçó âûêëþ÷àåì CS Ó SPI2

   // Конфигурируем выводы SPI2 (смотреть в как настроены пины в Prod-t Speci-n 7.1 SPI). ->
  // PB12, SPI2_NSS: Если мы управляем тогда General purpose output push-pull
  // PB13, SPI2_SCK:(Master) Alternate function push-pull
  // PB14, SPI2_MISO: input, pull up/down
  // PB15, SPI2_MOSI: (Mast	er) Alternate function push-pull
  // PA8 , RESET
  
  SPI2->CR1 &= ~SPI_CR1_DFF; //передача по 8бит
  SPI2->CR1 |= SPI_CR1_LSBFIRST; //Формат кадра (LSB первым передается)
  SPI2->CR1 |= SPI_CR1_SSM; //Сигнал SS может быть получен либо с вывода NSS, либо бита SSI регистра CR1.
  SPI2->CR1 |= SPI_CR1_SSI;
  SPI2->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 ; // делитель скорости передачи 256
  SPI2->CR1 |=SPI_CR1_MSTR; //Ведущий режим
  SPI2->CR1 |=SPI_CR1_MSTR; //Ведущий режим
  SPI2->CR1 &= ~SPI_CR1_CPOL & ~SPI_CR1_CPHA;

  SPI2->CR2 |= SPI_CR2_TXDMAEN; //Проверка передачи данных по TX(передача)
  SPI2->CR2 |= SPI_CR2_RXDMAEN; //Проверка передачи данных по RX(прием)


  SPI2->CR1 |= SPI_CR1_SPE; //Вкл. SPI
  
}

void DMA1C4C5_Init ()
{
   //Передача данных по SPI2 от контроллера в переферию (Канал 5)


  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  DMA1_Channel5->CCR |= DMA_CCR5_DIR; //чтение с котроллера (памяти)
  DMA1_Channel5->CCR |= DMA_CCR5_TCIE; //Прерыв при завершении отправки
  DMA1_Channel5->CCR |= DMA_CCR5_MINC; // Инкремент адреса памяти
  
  DMA1_Channel5->CMAR |= (uint32_t) & Result[0]; //Т.К DIR=1, то берем инфу из CMAR;
  DMA1_Channel5->CPAR |= (uint32_t) & SPI2->DR; //адрес переферии



 //------------------------------------------------------------
  //Прием данных по SPI2 (выгрузка информации) (канал 4)

  //DMA1_Channel4->CCR |= DMA_CCR4_CIRC; //Беспрерывная передачи информации
  DMA1_Channel4->CCR &= ~DMA_CCR4_DIR; //чтение с 0переферии
  DMA1_Channel4->CCR |= DMA_CCR4_TCIE; //Прерывание по завершению отправки

  DMA1_Channel4->CPAR |= (uint32_t) & SPI2->DR; //Т.К DIR=0,то берем инфу из СPAR 
  DMA1_Channel4->CMAR |= (uint32_t) & Vigryzka;

/*
Настройка количества циклов DMA за 1 прерывание таймера прописывается в ПРЕРЫВАНИИ ТАЙМЕРА!!!
И ТАМ ЖЕ ВКЛЮЧАЕТСЯ DMA!!!

  DMA1_Channel4->CNDTR |= 5; //Число циклов DMA в рамках 1 прерывания таймера
  DMA1_Channel5->CNDTR |= 5;//Равно размеру массива в котором данные для отправки

  DMA1_Channel4->CCR |= DMA_CCR4_EN;//включение каналов
  DMA1_Channel5->CCR |= DMA_CCR5_EN;
 */       
 
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);//Включение функций прерываний DMA
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void Display()
{
  GPIOC->ODR |= GPIO_ODR_ODR6; //PС6=1 (установка RESET в режим ожидания)

  Delay_mc(1);
  Command (0x2A);
  Command (0x71);
  Data (0x00);
  Command (0x28);
  Command (0x08);
  Command (0x2A);
  Command (0x79);
  Command (0xD5);
  Command (0x70);
  Command (0x78);
  Command (0x08);
  Command (0x06);
  Command (0x72);
  Data (0x00);
  Command (0x2A);
  Command (0x79);
  Command (0xDA);
  Command (0x00);
  Command (0xDC);
  Command (0x00);
  Command (0x81);
  Command (0x7F);
  Command (0xD9);
  Command (0xF1);
  Command (0xDB);
  Command (0x40);
  Command (0x78);
  Command (0x28);
  Command (0x01);
  Command (0x80);
  Command (0x0C);
  Delay_mc(100); //100mc (2 400 000 тактов)
}

void TIM2_IRQHandler (void)
{
  Command(0x80); //Перевод коретки дисплея в 0:
  Command(0x80);
  
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

  
  
  for (int k=0; k<100;k++);
  
  GPIOB->ODR &= ~GPIO_ODR_ODR12; //Вкл CS, запуск передачи
  
  //(указать расмер массива для переда и принятия данных)
  DMA1_Channel5->CNDTR |= 21; //Число циклов DMA в рамках 1 прерывания таймера (указать расмер массива)
  DMA1_Channel4->CNDTR |= 21;//Равно размеру массива в котором данные для отправки

  DMA1_Channel5->CCR |= DMA_CCR5_EN;//включение каналов (без ЦИРЦ оно будет выключаться по завершению прерывания
  DMA1_Channel4->CCR |= DMA_CCR4_EN;
  
  TIM2->SR &= ~TIM_SR_UIF; //Перезапуск таймера
}

void DMA1_Channel5_IRQHandler (void) //Проверка готовности SPI принять данные
{
  DMA1_Channel5->CCR &= ~DMA_CCR5_EN;

  while (!(SPI2->SR & SPI_SR_TXE)); //Проверка флага TXE.
  while (SPI2->SR & SPI_SR_BSY); 
  
  GPIOB->ODR |= GPIO_ODR_ODR12; //выключение CR у SPI по завершению передачи DMA
  DMA1->IFCR |=DMA_IFCR_CGIF5; //Перезарядка для прерываний DMA(отчистка модуля прерываний TEIE)
}

void DMA1_Channel4_IRQHandler (void)
{
  DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
  DMA1->IFCR |=DMA_IFCR_CGIF4;
}

