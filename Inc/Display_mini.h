
#ifndef Display_mini //если имя не определено
#define Display_mini //определить имя

extern unsigned char Result[21];
extern unsigned char Vigryzka;

extern unsigned char edinizi;
extern unsigned char desiatki;

extern unsigned char edinizi_1;
extern unsigned char desiatki_1;

extern unsigned char edinizi_2;
extern unsigned char desiatki_2;

void SPI2_100RB (void);
void DMA1C4C5_Init ();
unsigned char Send (unsigned char bait);
void Delay_mc (unsigned char Time);
void Data (unsigned char D);
void Command (unsigned char C);
void Display();

#endif //если имя определилось, то повторно не повторять