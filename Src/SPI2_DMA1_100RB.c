

void SPI2_100RB (void)
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN ;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  GPIOA->CRH &= 0xFFF0FF00;
  GPIOA->CRH |= 0x00080088;

  GPIOB->CRH &= 0x0000FFFF;
  GPIOB->CRH |= 0xB4B30000;

    // ������������� ������ SPI2 (�������� � ��� ��������� ���� � Prod-t Speci-n 7.1 SPI). ->
  // PB12, SPI2_NSS: ���� �� ��������� ����� General purpose output push-pull
  // PB13, SPI2_SCK:(Master) Alternate function push-pull
  // PB14, SPI2_MISO: input, pull up/down
  // PB15, SPI2_MOSI: (Mast	er) Alternate function push-pull
  // PA8 , RESET
  
  GPIOC->CRL &= 0xF0F000FF;
  GPIOC->CRL &= 0x03088800; //PC10, PC11- �������. PC12-������

  SPI2->CR1 &= ~SPI_CR1_DFF; //�������� �� 8���
  SPI2->CR1 |= SPI_CR1_LSBFIRST; //������ ����� (LSB ������ ����������)
  SPI2->CR1 |= SPI_CR1_SSM; //������ SS ����� ���� ������� ���� � ������ NSS, ���� ���� SSI �������� CR1.
  SPI2->CR1 |= SPI_CR1_SSI;
  SPI2->CR1 &= ~SPI_CR1_BR; //�������� (������������ 2)
  SPI2->CR1 |=SPI_CR1_MSTR; //������� �����
  SPI2->CR1 &= ~SPI_CR1_CPOL & ~SPI_CR1_CPHA;

  SPI2->CR2 |= SPI_CR2_TXDMAEN; //�������� �������� ������ �� TX(��������)
  SPI2->CR2 |= SPI_CR2_RXDMAEN; //�������� �������� ������ �� RX(�����)

  SPI2->CR1 |= SPI_CR1_SPE; //���. SPI
}

void DMA1C4C5_Init ()
{
  //�������� ������ �� SPI2 �� ����������� � ��������� (����� 5)

  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  DMA1_Channel5->CCR |= DMA_CCR5_DIR; //������ � ���������� (������)
  DMA1_Channel5->CCR |= DMA_CCR5_TCIE; //������ ��� ���������� ��������

  DMA1_Channel5->CMAR |= (uint32_t) & Result; //��������� ����� ������ ����� ����� ���������� ��� ��������
  DMA1_Channel5->CPAR |= (uint32_t) & SPI2->DR; //����� ���������

  //------------------------------------------------------------
  //����� ������ �� SPI2 (�������� ����������) (����� 4)

  DMA1_Channel4->CCR |= DMA_CCR4_CIRC; //������������ �������� ����������
  DMA1_Channel4->CCR &= ~DMA_CCR4_DIR; //������ � ���������
  DMA1_Channel4->CCR |= DMA_CCR4_TCIE; //���������� �� ���������� ��������

  DMA1_Channel4->CMAR |= (uint32_t) & SPI2->DR;
  DMA1_Channel4->CPAR |= (uint32_t) & Vigryzka;

  DMA1_Channel4->CNDTR |= 5; //����� ������ DMA � ������ 1 ���������� �������
  DMA1_Channel5->CNDTR |= 5;//����� ������� ������� � ������� ������ ��� ��������

  DMA1_Channel4->CCR |= DMA_CCR4_EN;//��������� �������
  DMA1_Channel5->CCR |= DMA_CCR5_EN;
        
 
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);//��������� ������� ���������� DMA
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}
