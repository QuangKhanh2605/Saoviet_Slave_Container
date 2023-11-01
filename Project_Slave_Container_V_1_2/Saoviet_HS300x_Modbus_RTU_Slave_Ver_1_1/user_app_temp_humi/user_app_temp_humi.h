#ifndef USER_APP_TEMP_HUMI__
#define USER_APP_TEMP_HUMI__

#include "user_modbus_rtu.h"
#include "user_uart.h"

#define FLASH_ID_BAURATE        0x08000000 + 1024*30
#define FLASH_CALIB_SENSOR      0x08000000 + 1024*31

#define DEFAUL_READ_FLASH       0xA5A5A5A5

#define TIME_WDG_RESET_MCU      60*60000

#define ASCII_NUMBER_VALUE      48

#define BAUDRATE_DEFAULT        9600
#define ID_DEFAULT              26
/*====================== Function Handle ======================*/

uint8_t     ModbusRTU_Slave(UART_BUFFER *sUart2, uint8_t *addr_stm32l0xx, uint32_t *baud_rate, int16_t tem, int16_t humi);
void        Change_Baudrate_AddrSlave_Calib_ATCommand(UART_BUFFER *sUart2, uint8_t *addr_stm32l0xx, uint32_t *baud_rate, int16_t *drop_tem, int16_t *drop_humi);

void        FLASH_WritePage(uint32_t address_flash, uint32_t data1, uint32_t data2);
uint32_t    FLASH_ReadData32(uint32_t addr);

void        Uart2_Init(UART_BUFFER *sUart2, uint32_t baud_rate);

void        Get_Length_Variable(uint8_t *length, uint32_t variable);
int8_t      Terminal_Receive(UART_BUFFER *sUart2, uint8_t addr_stm32l0xx);

void        Packing_Frame(uint8_t data_frame[], uint8_t addr_stm32l0xx, uint16_t addr_register, uint16_t length, uint32_t baud_rate, int16_t Tem, int16_t Humi);

void        AT_Command_IF(UART_BUFFER *sUart2, uint8_t addr_stm32l0xx, uint32_t baud_rate, int16_t drop_tem, int16_t drop_humi);
void        Send_Data_Terminal(UART_BUFFER *sUart2, void *data);
uint32_t    Get_Uint_In_String(uint8_t aData[], uint16_t posStart, uint16_t posEnd);
#endif

