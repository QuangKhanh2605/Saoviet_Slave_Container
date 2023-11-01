
#ifndef USER_UART_H_
#define USER_UART_H_

#include "usart.h"
#include "user_util.h"
/*======Define Uart ==============*/
#define uart_debug	    huart3
#define uart_485	    huart1

/*===========Var struct=============*/
   
extern uint8_t UartDebugBuff[20];
extern sData sUart485;

/*==================Function==================*/
void Init_Uart_Module (void);

void Init_Uart_Debug_Rx_IT (void);
void Init_Uart_485_Rx_IT (void);

#endif /* USER_UART_H_ */
