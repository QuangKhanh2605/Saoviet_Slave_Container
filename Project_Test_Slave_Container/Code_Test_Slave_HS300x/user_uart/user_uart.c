
#include "user_uart.h"

/*==================var struct======================*/
uint8_t Uart485Buff[20] = {0};
sData 	sUart485 = {&Uart485Buff[0], 0};

uint8_t UartDebugBuff[20] = {0};
sData   sUartDebug = {&UartDebugBuff[0],0};

uint8_t UartDebugRxByte = 0;
uint8_t Uart485RxByte = 0;
uint32_t count_irq = 0;
/*==================Function======================*/

void Init_Uart_Debug_Rx_IT (void)
{
    HAL_UART_Receive_IT(&uart_debug, &UartDebugRxByte, 1);
}


void Init_Uart_485_Rx_IT (void)
{
    HAL_UART_Receive_IT(&huart1, &Uart485RxByte, 1);
}


void Init_Uart_Module (void)
{   
    Init_Uart_Debug_Rx_IT();
    Init_Uart_485_Rx_IT();
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{   
    if (huart->Instance == huart1.Instance)
	{ 
        *(sUart485.Data_a8 + sUart485.Length_u16++) = Uart485RxByte;

        if (sUart485.Length_u16 >= (sizeof(Uart485Buff) - 1))
            sUart485.Length_u16 = 0;
       //sUart485.Data_a8[(sUart485.Length_u16)++] = Uart485RxByte;
       HAL_UART_Receive_IT(&huart1, &Uart485RxByte, 1);
    }
    
        if (huart->Instance == huart3.Instance)
	{ 
//        *(sUart485.Data_a8 + sUart485.Length_u16++) = Uart485RxByte;
//
//        if (sUart485.Length_u16 >= (sizeof(Uart485Buff) - 1))
//            sUart485.Length_u16 = 0;

       //sUart485.Data_a8[(sUart485.Length_u16)++] = Uart485RxByte;
       HAL_UART_Receive_IT(&huart3, &UartDebugRxByte, 1);
    }
}






