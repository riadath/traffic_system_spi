#ifndef __SPI_H
#define __SPI_H
#ifdef __cplusplus
extern "C" {
#endif

#include "CLOCK.h"
#include "GPIO.h"
#include "SYS_INIT.h"
#include "USART.h"
#include "TRAFFIC_SYSTEM.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx.h"

void SPI1_Config(bool ifMaster);
void SPI1_Send(char *data);
char* SPI1_Receive(void);
#ifdef __cplusplus
}
#endif
#endif
