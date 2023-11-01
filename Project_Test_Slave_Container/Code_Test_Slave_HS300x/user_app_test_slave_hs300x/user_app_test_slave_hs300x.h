#ifndef USER_APP_TEST_SLAVE_HS300X__
#define USER_APP_TEST_SLAVE_HS300X__

#include "stm32l1xx_hal.h"
#include "user_uart.h"
#include "event_driven.h"

/*=============== Define ================*/
#define TIME_SAMPLING           5000
#define NUMBER_SLAVE            20

#define NET485IO_GPIO_PORT      GPIOA
#define NET485IO_PIN            GPIO_PIN_8
#define NET485IO_PIN_RESET      GPIO_PIN_RESET
#define NET485IO_PIN_SET        GPIO_PIN_SET

#define ON_OFF_IC_485_GPIO_PORT GPIOC
#define ON_OFF_IC_485_PIN       GPIO_PIN_9
#define ON_IC_485_PIN           GPIO_PIN_RESET

/*======================================*/

typedef enum
{
    _EVENT_TEST_SLAVE_ENTRY,
    _EVENT_TEST_SLAVE_DISPLAY,
    _EVENT_TEST_SLAVE_TRANSMIT,
    _EVENT_TEST_SLAVE_HANDLE_RECEIVE,
    
    _EVENT_TEST_SLAVE_END,
}eKindEventTestSlave;


/*=============== Function ===============*/

void        Init_AppTest(void);
void        display_uart(void);
uint8_t     Modbus_RTU_Master(void);
uint8_t     Check_CountBuffer_Complete_Uart_485(sData sUart);
void        Delete_Buffer(sData *sUart);

void        AppTestSlaveHs300x(void);
uint8_t     AppTestSlaveHs300x_Task(void);


#endif
