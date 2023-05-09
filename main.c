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

/*************************
	TRAFFIC SYSTEM
*************************/
static char status[3][200];
static uint16_t runningNS = 1;
static uint16_t GREEN_NS = 6;
static uint16_t GREEN_EW = 5;
static uint16_t RED_EW = 4;
static uint16_t RED_NS = 7;
static uint16_t TRAFFIC_NS = 2;
static uint16_t TRAFFIC_EW = 1;
static uint16_t report_interval = 5000;


static uint16_t runningTime = 2000;
static uint16_t extraTime = 0;
static uint16_t g_delayNS = 5;
static uint16_t r_delayNS = 2;
static uint16_t y_delayNS = 1;

static uint16_t trafficNS;
static uint16_t trafficEW;
static uint16_t balanced;

static uint16_t g_delayEW = 5;
static uint16_t r_delayEW = 2;
static uint16_t y_delayEW = 1;



void parseCommand(char command[]);
void show_traffic_info(void);
void showTrafficConfig(uint32_t light_no);
void showReportIntervalConfig(void);
void setDelayTraffic(char ch,uint32_t del,uint32_t light_no);
void clearLEDs(void);

void setDelayTraffic(char ch,uint32_t del,uint32_t light_no){
	
	if(light_no == 1){
		if (ch == 'G'){
			g_delayNS = (uint16_t)del;
		}else  if (ch == 'Y'){
			y_delayNS = (uint16_t)del;
		}else if(ch == 'R'){
			r_delayNS = (uint16_t)del;
		}
	}else if(light_no == 2){
		if (ch == 'G'){
			g_delayEW = (uint16_t)del;
		}else  if (ch == 'Y'){
			y_delayEW = (uint16_t)del;
		}else if(ch == 'R'){
			r_delayEW = (uint16_t)del;
		}
	}
}

void showTrafficConfig(uint32_t light_no){
	char str[50];
	if (light_no == 1)
		sprintf(str,"\ntraffic light 1 G Y R %d %d %d %d\n",(uint32_t)g_delayNS,
		(uint32_t)y_delayNS,(uint32_t)r_delayNS,(uint32_t)extraTime);
	if (light_no == 2)
		sprintf(str,"\ntraffic light 2 G Y R %d %d %d %d\n",(uint32_t)g_delayEW,
		(uint32_t)y_delayEW,(uint32_t)r_delayEW,(uint32_t)extraTime);
	
	UART_SendString(USART2,str);

}

void showReportIntervalConfig(void){
	char str[50];
	sprintf(str,"\ntraffic monitor %d\n",(uint32_t)report_interval/1000);
	
	UART_SendString(USART2,str);
}
void show_traffic_info(void){
    char temp[600];
	char *G_NS_state = (GPIO_PIN_6 & GPIOB->ODR)?"ON":"OFF";
	char *R_NS_state = (GPIO_PIN_7 & GPIOB->ODR)?"ON":"OFF";
	char *G_EW_state = (GPIO_PIN_5 & GPIOB->ODR)?"ON":"OFF";
	char *R_EW_state = (GPIO_PIN_4 & GPIOB->ODR)?"ON":"OFF";
	char *Y_NS_state = (RED_NS == 0 && GREEN_NS == 0)? "ON" : "OFF";
	char *Y_EW_state = (RED_EW == 0 && GREEN_EW == 0)? "ON" : "OFF";
	
	char *NS_congestion = (GPIO_PIN_1 & GPIOB->ODR)?"heavy traffic":"light traffic";
	char *EW_congestion = (GPIO_PIN_2 & GPIOB->ODR)?"heavy traffic":"light traffic";
	
	char str0[50],str1[50],str2[50],str3[50];
	
	sprintf(str0, "\n%d traffic light 1 %s %s %s\n", (uint32_t) global_time, G_NS_state, Y_NS_state, R_NS_state);
	sprintf(str1, "%d traffic light 2 %s %s %s\n", (uint32_t) global_time, G_EW_state, Y_EW_state, R_EW_state);
	sprintf(str2, "%d road north south %s \n", (uint32_t) global_time, NS_congestion);
	sprintf(str3, "%d road east west %s \n", (uint32_t) global_time, EW_congestion);

    

    strcpy(temp, status[1]);
    strcpy(status[1],status[2]);
    strcpy(status[0], temp);
    
    sprintf(temp,"%s%s%s%s",str0,str1,str2,str3);

    strcpy(status[2],temp);

    UART_SendString(USART2, "\n-------------------------------");

//    UART_SendString(USART2,status[0]);
//    UART_SendString(USART2,status[1]);
    UART_SendString(USART2,status[2]);
    UART_SendString(USART2, "-------------------------------\n");
}


void parseCommand(char command[]){
	char c1,c2,c3;
	uint32_t light_no,del1,del2,del3,ext,rep_int;
	if (command[0] == 'c'){
		if(command[15] == 'l'){
			sscanf(command,"config traffic light %d %c %c %c %d %d %d %d",
			&light_no,&c1,&c2,&c3,&del1,&del2,&del3,&ext);
			
			setDelayTraffic(c1,del1,light_no);
			setDelayTraffic(c3,del3,light_no);
			setDelayTraffic(c2,del2,light_no);
			
			extraTime = (uint16_t)ext;
		}
		else if(command[15] == 'm'){
			sscanf(command,"config traffic monitor %d",&rep_int);
			report_interval = (uint16_t)(rep_int * 1000);
		}
	}
	else if(command[0] == 'r'){
		if (strlen(command) == 4){
			showTrafficConfig(1);
			showTrafficConfig(2);
			showReportIntervalConfig();
		}
		else if (command[13] == 'l'){
			sscanf(command,"read traffic light %d",&light_no);
			showTrafficConfig(light_no);
		}
		else if(command[13] == 'm'){
			showReportIntervalConfig();
		}
	}
}




void clearLEDs(void){
    for(uint16_t i = 0; i < 8; i++){
        GPIO_WritePin(GPIOB, i, GPIO_PIN_RESET);
    }
}


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
			show_traffic_info();
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
	
	if(SPI1->SR & SPI_SR_RXNE)
		rcv_str = SPI1_Receive();
	
	
	if(strlen(rcv_str) != 0){
		sendString("RCV ::: ");
		sendString(rcv_str);
		sendString(" :::\n");
		strcpy(input_buff,rcv_str);
		parseCommand(input_buff);
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
	
    //config for output 
	gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_config.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Init(GPIOB, &gpio_config);
    gpio_config.Pin = GPIO_PIN_1|GPIO_PIN_2;
	GPIO_Init(GPIOB, &gpio_config);

	//timer start
	TIM2->CNT = 0;
}

void mainLoop(void){
    while(true){
        runningTime = 0;
        clearLEDs();
        trafficNS = (uint16_t) rand()%2;
        trafficEW = (uint16_t) rand()%2;

        balanced = trafficEW & trafficNS;
        if(balanced==0) runningTime += extraTime;	
        if (trafficNS)GPIO_WritePin(GPIOB, TRAFFIC_NS, GPIO_PIN_SET);
        if (trafficEW)GPIO_WritePin(GPIOB, TRAFFIC_EW, GPIO_PIN_SET);
        if (runningNS) {
			GPIO_WritePin(GPIOB, GREEN_NS, GPIO_PIN_SET);
			GPIO_WritePin(GPIOB, RED_EW, GPIO_PIN_SET);
			runningTime += g_delayNS;
        }
        else {
			GPIO_WritePin(GPIOB, GREEN_EW, GPIO_PIN_SET);
			GPIO_WritePin(GPIOB, RED_NS, GPIO_PIN_SET);
			runningTime += g_delayEW;
        }
        tim5_delay(runningTime*1000);

        if(runningNS){
			GPIO_WritePin(GPIOB, GREEN_NS, GPIO_PIN_RESET);
			tim5_delay(y_delayNS*1000);
        }else {
			GPIO_WritePin(GPIOB, GREEN_EW, GPIO_PIN_RESET);
			tim5_delay(y_delayEW*1000);
        }

        runningNS = (runningNS==1)? 0:1;
		
    }   
}



int main(void)
{   
	uint8_t temp = 1;
	
	init();
	
	SPI1_Config(temp);

    
    strcpy(input_buff,"");
    sendString("HELLO I'M IN\n");

	
	if(!temp){
		sendString("Inside Read Loop\n");
		mainLoop();
	}
	
	else{

		sendString("Inside Write Loop\n");

		while(1){
			if(strlen(input_buff) != 0){
				sendString("TO SEND ::: ");
				sendString(input_buff);
				sendString(" :::\n");
				
//				SPI1_Send("#");
				SPI1_Send(input_buff);
				
				strcpy(input_buff,"");
			}
		}
	}

}






