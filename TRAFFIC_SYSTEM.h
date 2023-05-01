#ifndef __TRAFFIC_SYSTEM_H
#define __TRAFFIC_SYSTEM_H
#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "CLOCK.h"
#include "GPIO.h"
#include "SYS_INIT.h"
#include "USART.h"
#include "stm32f4xx.h"

/*GLOBAL VARIABLES FOR TRAFFIC SYSTEM*/
static char status[3][200];
static uint16_t runningNS = 1;
static uint16_t GREEN_NS = 8;
static uint16_t GREEN_EW = 6;
static uint16_t RED_EW = 5;
static uint16_t RED_NS = 9;
static uint16_t TRAFFIC_NS = 2;
static uint16_t TRAFFIC_EW = 1;
static uint16_t report_interval = 5000;


static uint16_t runningTime = 2000;
static uint16_t extraTime = 3;
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
void show_traffic_info(uint32_t global_time);
void showTrafficConfig(uint32_t light_no);
void showReportIntervalConfig(void);
void setDelayTraffic(char ch,uint32_t del,uint32_t light_no);
void clearLEDs(void);


#ifdef __cplusplus
}
#endif
#endif
