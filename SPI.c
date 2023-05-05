#include "SPI.h"

void SPI1_Config(bool ifMaster){
	/*
	mode == true => Master
	mode == false => Slave
	*/
	
	/**********GPIO CONFIG***********/

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	/*PA4,5,PA6,PA7 in AF MODE*/
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | 
					GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5 | 
					GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
	/*SET AF5 FOR PA4,5,6,7*/
	GPIOA->AFR[0] |= (5 << 20) | (5 << 24) | (5 << 28);    
	
	/****************SPI Configuration*****************/
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	SPI1->CR1 |= SPI_CR1_SPE; /*SPI PERIPHERAL ENABLE*/
	/*SELECT POLARITY AND PHASE*/
	
	SPI1->CR1 |= SPI_CR1_CPHA; 
	SPI1->CR1 &= ~SPI_CR1_CPOL;
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST; /*MSB FIRST*/
	SPI1->CR1 &= ~SPI_CR1_DFF; /*8 BIT DATA STREAM*/
	

	
	if(ifMaster){
		SPI1->CR1 |= 2 << SPI_CR1_BR_Pos; /*SET BAUDE RATE*/
		SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; /*SOFTWARE SLAVE MANAGEMENT*/
		

		SPI1->CR1 &= ~SPI_CR1_RXONLY;
		
		SPI1->CR1 |= SPI_CR1_MSTR; /*SELECT MASTER MODE*/
		/*
		Baud Rate = Fpclk / 16 [011]
		Baud Rate = 90mhz / 16 = 5.625
		*/
		
	}
	else{

		SPI1->CR1 &= ~SPI_CR1_RXONLY;
		SPI1->CR1 &= ~SPI_CR1_MSTR; /*SELECT SLAVE MODE*/
		
		SPI1->CR2 |= SPI_CR2_RXNEIE;
		NVIC_EnableIRQ(SPI1_IRQn);
		
	}	
}


void SPI1_Send(char *data){
	
	uint8_t temp;
	/*SEND ALL DATA BITS*/
	sendString("inside send\n");
	
	
	for(int i = 0;i < (int)strlen(data);i++){
		
		while(!(SPI1->SR & SPI_SR_TXE));
		while(SPI1->SR & SPI_SR_BSY);
		SPI1->DR = data[i];
	}
	
	
	/*WAIT FOR LAST BIT TO TRANSFER*/
	while(!(SPI1->SR & SPI_SR_TXE));
	/*WAIT UNITL SPI NOT BUSY*/
	while(SPI1->SR & SPI_SR_BSY);
	
	
	temp = (uint8_t)SPI1->DR;
	temp = (uint8_t)SPI1->SR;

}

char* SPI1_Receive(void){
	char ret[100],ch = 0;
	int idx = 0,flag = 0;

	while(ch != '@'){
		while(!(SPI1->SR & SPI_SR_RXNE));
		ch = (uint8_t)SPI1->DR;
		if(ch != '@'){
			if(flag)
				ret[idx++] = ch;
			flag = 1;
		}
		else break;
	}
	
	ret[idx] = '\0';
	
	return ret;
}
























