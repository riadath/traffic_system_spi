#include "CLOCK.h"
#include "GPIO.h"
#include "SYS_INIT.h"
#include "USART.h"
#include "TRAFFIC_SYSTEM.h"
#include "SPI.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx.h"

static char input_buff[100];
static char *rcv_str;
static uint32_t global_time = 0;

/*FUNCTION PROTOTYPES*/
void TIM5Config(void);
void TIM2Config(void);
void TIM3Config(void);
void tim5_delay(uint16_t ms);


void USART2_IRQHandler(void);
void SPI1_IRQHandler(void);
void getString(void);
void init(void);
void mainLoop(void);


/**************************
     TIMER CONFIG
*************************/
void TIM5Config(void){
	RCC->APB1ENR |= (1<<3);
	
	TIM5->PSC = 45000 - 1; /* fck = 45 mhz, CK_CNT = fck / (psc[15:0] + 1)*/
	TIM5->ARR = 0xFFFF; /*maximum clock count*/
	
	TIM5->CR1 |= (1<<0);
	
	while(!(TIM5->SR & (1<<0)));
	
}
void TIM2Config(void){
	RCC->APB1ENR |= (1<<0);
	
	TIM2->PSC = 45000 - 1; /* fck = 90 mhz, CK_CNT = fck / (psc[15:0] + 1)*/
	TIM2->ARR = 0xFFFF; /*maximum clock count*/
	
	TIM2->CR1 |= (1<<0);
	
	while(!(TIM2->SR & (1<<0)));
	
}


void TIM3Config(void){
	RCC->APB1ENR |= (1<<1);
	
	TIM3->PSC = 45000 - 1; /* fck = 90 mhz, CK_CNT = fck / (psc[15:0] + 1)*/
	TIM3->ARR = 0xFFFF; /*maximum clock count*/
	
	TIM3->CR1 |= (1<<0);
	
	while(!(TIM3->SR & (1<<0)));
}

void tim5_delay(uint16_t ms){
	ms = (uint16_t)2 * ms;
	TIM5->CNT = 0;
	while(TIM5->CNT < ms){
		if(TIM2->CNT > report_interval*2){
			global_time += report_interval/1000;
			show_traffic_info(global_time);
			TIM2->CNT = 0;
		}
	}
}



void getString(void){
    uint8_t ch,idx = 0;
    ch = UART_GetChar(USART2);
    while(ch != '!'){
        input_buff[idx++] = ch;
        ch = UART_GetChar(USART2);
        if(ch == '!')break;
    }      
    input_buff[idx] = '\0';
    
}
void USART2_IRQHandler(void){
	
    USART2->CR1 &= ~(USART_CR1_RXNEIE);
    getString();
    USART2->CR1 |= (USART_CR1_RXNEIE);
}





void SPI1_IRQHandler(void){
	
	SPI1->CR2 &= ~SPI_CR2_RXNEIE;

	rcv_str = SPI1_Receive();
	
	
	if(strlen(rcv_str) != 0){
		sendString("RCV ::: ");
		sendString(rcv_str);
		sendString(" :::\n");
		strcpy(rcv_str,"");
	}
	
	SPI1->CR2 |= SPI_CR2_RXNEIE;
}

void init(void){
    /*	Configuration */
    GPIO_InitTypeDef gpio_config;
	initClock();
	sysInit();
    TIM5Config();
    TIM2Config();
    TIM3Config();
	UART2_Config();
    
	/*
    
    //config for output 
	gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_config.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_Init(GPIOA, &gpio_config);
    gpio_config.Pin = GPIO_PIN_1|GPIO_PIN_2;
	GPIO_Init(GPIOB, &gpio_config);
	
	//config for input 
	gpio_config.Mode = GPIO_MODE_INPUT;
	gpio_config.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_3;
    GPIO_Init(GPIOB, &gpio_config);
	*/
	

	//timer start
//	TIM2->CNT = 0;
}
void mainLoop(void){
    while(true){
        I2C1->CR1 |= I2C_CR1_ACK; 

        runningTime = 0;
        clearLEDs();
        trafficNS = (uint16_t) rand()%2;
        trafficEW = (uint16_t) rand()%2;

        balanced = trafficEW & trafficNS;
        if(balanced==0) runningTime += extraTime;	
        if (trafficNS)GPIO_WritePin(GPIOB, TRAFFIC_NS, GPIO_PIN_SET);
        if (trafficEW)GPIO_WritePin(GPIOB, TRAFFIC_EW, GPIO_PIN_SET);
        if (runningNS) {
        GPIO_WritePin(GPIOA, GREEN_NS, GPIO_PIN_SET);
        GPIO_WritePin(GPIOA, RED_EW, GPIO_PIN_SET);
        runningTime += g_delayNS;
        }
        else {
        GPIO_WritePin(GPIOA, GREEN_EW, GPIO_PIN_SET);
        GPIO_WritePin(GPIOA, RED_NS, GPIO_PIN_SET);
        runningTime += g_delayEW;
        }
        tim5_delay(runningTime*1000);

        if(runningNS){
        GPIO_WritePin(GPIOA, GREEN_NS, GPIO_PIN_RESET);
        tim5_delay(y_delayNS*1000);
        }else {
        GPIO_WritePin(GPIOA, GREEN_EW, GPIO_PIN_RESET);
        tim5_delay(y_delayEW*1000);
        }

        runningNS = (runningNS==1)? 0:1;
		
    }   
}


















int main(void)
{   
	uint8_t temp = 0;
	
	init();
	SPI1_Config(temp);

    
    strcpy(input_buff,"");
    sendString("HELLO I'M IN\n");
	
    GPIO_InitTypeDef gpio_config;
	gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_config.Pin = GPIO_PIN_5;
    GPIO_Init(GPIOB, &gpio_config);
	
	if(!temp){
		
		sendString("Inside Read Loop\n");
		while(1){
			
			GPIO_WritePin(GPIOB,5,GPIO_PIN_SET);
			ms_delay(500);
			GPIO_WritePin(GPIOB,5,GPIO_PIN_RESET);
			ms_delay(500);
		}
	}
	
	else{

		sendString("Inside Write Loop\n");

		while(1){
			if(strlen(input_buff) != 0){
				sendString("TO SEND ::: ");
				sendString(input_buff);
				sendString(" :::\n");
				
				SPI1_Send("#");
				SPI1_Send(input_buff);
				
				strcpy(input_buff,"");
			}
		}
	}

}






