//#include "TRAFFIC_SYSTEM.h"

//void setDelayTraffic(char ch,uint32_t del,uint32_t light_no){
//	
//	if(light_no == 1){
//		if (ch == 'G'){
//			g_delayNS = (uint16_t)del;
//		}else  if (ch == 'Y'){
//			y_delayNS = (uint16_t)del;
//		}else if(ch == 'R'){
//			r_delayNS = (uint16_t)del;
//		}
//	}else if(light_no == 2){
//		if (ch == 'G'){
//			g_delayEW = (uint16_t)del;
//		}else  if (ch == 'Y'){
//			y_delayEW = (uint16_t)del;
//		}else if(ch == 'R'){
//			r_delayEW = (uint16_t)del;
//		}
//	}
//}

//void showTrafficConfig(uint32_t light_no){
//	char str[50];
//	if (light_no == 1)
//		sprintf(str,"\ntraffic light 1 G Y R %d %d %d %d\n",(uint32_t)g_delayNS,
//		(uint32_t)y_delayNS,(uint32_t)r_delayNS,(uint32_t)extraTime);
//	if (light_no == 2)
//		sprintf(str,"\ntraffic light 2 G Y R %d %d %d %d\n",(uint32_t)g_delayEW,
//		(uint32_t)y_delayEW,(uint32_t)r_delayEW,(uint32_t)extraTime);
//	
//	UART_SendString(USART2,str);

//}

//void showReportIntervalConfig(void){
//	char str[50];
//	sprintf(str,"\ntraffic monitor %d\n",(uint32_t)report_interval/1000);
//	
//	UART_SendString(USART2,str);
//}
//void show_traffic_info(uint32_t global_time){
//    char temp[600];
//	char *G_NS_state = (GPIO_PIN_6 & GPIOB->ODR)?"ON":"OFF";
//	char *R_NS_state = (GPIO_PIN_7 & GPIOB->ODR)?"ON":"OFF";
//	char *G_EW_state = (GPIO_PIN_5 & GPIOB->ODR)?"ON":"OFF";
//	char *R_EW_state = (GPIO_PIN_4 & GPIOB->ODR)?"ON":"OFF";
//	char *Y_NS_state = (RED_NS == 0 && GREEN_NS == 0)? "ON" : "OFF";
//	char *Y_EW_state = (RED_EW == 0 && GREEN_EW == 0)? "ON" : "OFF";
//	
//	char *NS_congestion = (GPIO_PIN_1 & GPIOB->ODR)?"heavy traffic":"light traffic";
//	char *EW_congestion = (GPIO_PIN_2 & GPIOB->ODR)?"heavy traffic":"light traffic";
//	
//	char str0[50],str1[50],str2[50],str3[50];
//	
//	sprintf(str0, "\n%d traffic light 1 %s %s %s\n", (uint32_t) global_time, G_NS_state, Y_NS_state, R_NS_state);
//	sprintf(str1, "%d traffic light 2 %s %s %s\n", (uint32_t) global_time, G_EW_state, Y_EW_state, R_EW_state);
//	sprintf(str2, "%d road north south %s \n", (uint32_t) global_time, NS_congestion);
//	sprintf(str3, "%d road east west %s \n", (uint32_t) global_time, EW_congestion);

//    

//    strcpy(temp, status[1]);
//    strcpy(status[1],status[2]);
//    strcpy(status[0], temp);
//    
//    sprintf(temp,"%s%s%s%s",str0,str1,str2,str3);

//    strcpy(status[2],temp);

//    UART_SendString(USART2, "\n-------------------------------");

////    UART_SendString(USART2,status[0]);
////    UART_SendString(USART2,status[1]);
//    UART_SendString(USART2,status[2]);
//    UART_SendString(USART2, "-------------------------------\n");
//}


//void parseCommand(char command[]){
//	char c1,c2,c3;
//	uint32_t light_no,del1,del2,del3,ext,rep_int;
//	if (command[0] == 'c'){
//		if(command[15] == 'l'){
//			sscanf(command,"config traffic light %d %c %c %c %d %d %d %d",
//			&light_no,&c1,&c2,&c3,&del1,&del2,&del3,&ext);
//			
//			setDelayTraffic(c1,del1,light_no);
//			setDelayTraffic(c3,del3,light_no);
//			setDelayTraffic(c2,del2,light_no);
//			
//			extraTime = (uint16_t)ext;
//		}
//		else if(command[15] == 'm'){
//			sscanf(command,"config traffic monitor %d",&rep_int);
////            char tstr[40];
////            sprintf(tstr,"interval RCV :: %d\n",rep_int);
////            sendString(tstr);
//			report_interval = (uint16_t)(rep_int * 1000);
//		}
//	}
//	else if(command[0] == 'r'){
//		if (strlen(command) == 4){
//			showTrafficConfig(1);
//			showTrafficConfig(2);
//			showReportIntervalConfig();
//		}
//		else if (command[13] == 'l'){
//			sscanf(command,"read traffic light %d",&light_no);
//			showTrafficConfig(light_no);
//		}
//		else if(command[13] == 'm'){
//			showReportIntervalConfig();
//		}
//	}
//}




//void clearLEDs(void){
//    for(uint16_t i = 0; i < 8; i++){
//        GPIO_WritePin(GPIOB, i, GPIO_PIN_RESET);
//    }
//}





