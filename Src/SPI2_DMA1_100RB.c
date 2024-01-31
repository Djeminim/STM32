

void SPI2_100RB (void)
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN ;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  GPIOA->CRH &= 0xFFF0FF00;
  GPIOA->CRH |= 0x00080088;

  GPIOB->CRH &= 0x0000FFFF;
  GPIOB->CRH |= 0xB4B30000;

    // Конфигурируем выводы SPI2 (смотреть в как настроены пины в Prod-t Speci-n 7.1 SPI). ->
  // PB12, SPI2_NSS: Если мы управляем тогда General purpose output push-pull
  // PB13, SPI2_SCK:(Master) Alternate function push-pull
  // PB14, SPI2_MISO: input, pull up/down
  // PB15, SPI2_MOSI: (Mast	er) Alternate function push-pull
  // PA8 , RESET
  
  GPIOC->CRL &= 0xF0F000FF;
  GPIOC->CRL &= 0x03088800; //PC10, PC11- Инокдер. PC12-Кнопка

  SPI2->CR1 &= ~SPI_CR1_DFF; //передача по 8бит
  SPI2->CR1 |= SPI_CR1_LSBFIRST; //Формат кадра (LSB первым передается)
  SPI2->CR1 |= SPI_CR1_SSM; //Сигнал SS может быть получен либо с вывода NSS, либо бита SSI регистра CR1.
  SPI2->CR1 |= SPI_CR1_SSI;
  SPI2->CR1 &= ~SPI_CR1_BR; //скорость (предделитель 2)
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

  DMA1_Channel5->CMAR |= (uint32_t) & Result; //прописать адрес откуда будем брать информацию для отправки
  DMA1_Channel5->CPAR |= (uint32_t) & SPI2->DR; //адрес переферии

  //------------------------------------------------------------
  //Прием данных по SPI2 (выгрузка информации) (канал 4)

  DMA1_Channel4->CCR |= DMA_CCR4_CIRC; //Беспрерывная передачи информации
  DMA1_Channel4->CCR &= ~DMA_CCR4_DIR; //чтение с переферии
  DMA1_Channel4->CCR |= DMA_CCR4_TCIE; //Прерывание по завершению отправки

  DMA1_Channel4->CMAR |= (uint32_t) & SPI2->DR;
  DMA1_Channel4->CPAR |= (uint32_t) & Vigryzka;

  DMA1_Channel4->CNDTR |= 5; //Число циклов DMA в рамках 1 прерывания таймера
  DMA1_Channel5->CNDTR |= 5;//Равно размеру массива в котором данные для отправки

  DMA1_Channel4->CCR |= DMA_CCR4_EN;//включение каналов
  DMA1_Channel5->CCR |= DMA_CCR5_EN;
        
 
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);//Включение функций прерываний DMA
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}
