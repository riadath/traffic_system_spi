#include "SPI.h"

void SPI1_Config(bool ifMaster){
	/*
	mode == true => Master
	mode == false => Slave
	*/
	
	/**********GPIO CONFIG***********/

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	/*PA5,PA6,PA7 in AF MODE*/
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | 
					GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5 | 
					GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
	/*SET AF5 FOR PA5,6,7*/
	GPIOA->AFR[0] |= (5 << 20) | (5 << 24) | (5 << 28);    
	
	/****************SPI Configuration*****************/
	SPI1->CR1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA); /*SELECT POLARITY AND PHASE [set 11]*/
	SPI1->CR1 &= ~SPI_CR1_RXONLY; /*SELECT FULL DUPLEX MODE*/
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST; /*MSB FIRST*/
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; /*SOFTWARE SLAVE MANAGEMENT*/
	SPI1->CR1 &= ~SPI_CR1_DFF; /*8 BIT DATA STREAM*/
	
	if(ifMaster){
		SPI1->CR1 |= SPI_CR1_MSTR; /*SELECT MASTER MODE*/
		/*
		Baud Rate = Fpclk / 16 [011]
		Baud Rate = 90mhz / 16 = 5.625
		*/
		SPI1->CR1 |= 3 << SPI_CR1_BR_Pos; /*SET BAUDE RATE*/
	}
	else{
		SPI1->CR1 &= ~SPI_CR1_MSTR; /*SELECT SLAVE MODE*/
	}
	
	/*Not using RXNEIE/TXEIE for now.
	pore korbo*/
	
	SPI1->CR2 = 0U;
}

void SPI1_Enable(void){
	SPI1->CR1 |= SPI_CR1_SPE;
}
void SPI1_Disable(void){
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void SPI1_Send(char *data){
	/*SEND ALL DATA BITS*/
	for(int i = 0;i < (int)strlen(data);i++){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = data[i];
	}
	/*WAIT FOR LAST BIT TO TRANSFER*/
	while(!(SPI1->SR & SPI_SR_TXE));
	/*WAIT UNITL SPI NOT BUSY*/
	while(SPI1->SR & SPI_SR_BSY);
	
}

char* SPI1_Receive(void){
	char ret[100],ch = 0;
	int idx = 0;
	while(ch != '@'){
		while(SPI1->SR & SPI_SR_BSY);
		SPI1->DR = 0; /*SEND DUMMY DATA*/
		ch = (uint8_t)SPI1->DR;
		if(ch != '@')ret[idx++] = ch;
	}
	ret[idx] = '\0';
	return ret;
}
























