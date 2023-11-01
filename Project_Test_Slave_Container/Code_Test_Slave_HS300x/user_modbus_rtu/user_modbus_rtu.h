#ifndef _USER_MODBUS_RTU_
#define _USER_MODBUS_RTU_	_USER_MODBUS_RTU_H


#include "stm32l1xx_hal.h"
#include "user_util.h"

/*======================== Define ======================*/

#define FUN_READ_BYTE       0x03
#define FUN_READ_REGIS      0x04
#define FUN_WRITE_BYTE      0x06


/*======================== Funcion ======================*/
uint16_t    ModRTU_CRC(uint8_t* buf, int len);
void        ModRTU_Convert_Special_Byte (uint8_t* Buff_Source, uint16_t* length);

void        ModRTU_Master_Read_Frame (sData *pFrame, uint8_t Address, uint8_t FunCode, uint16_t Add_Data, uint16_t LengthData);
void        ModRTU_Master_Write_Frame (sData *pFrame, uint8_t Address, uint8_t FunCode, uint16_t Add_Data, uint16_t LengthData, uint8_t* aData);
void        ModRTU_Slave_ACK_Read_Frame (sData *pFrame, uint8_t Address, uint8_t FunCode, uint16_t Add_Data, uint16_t LengthData, uint8_t* aData);
void        ModRTU_Slave_ACK_Write_Frame (sData *pFrame, uint8_t Address, uint8_t FunCode, uint16_t Add_Data, uint16_t LengthData);

void        Response_Error_CRC(sData *pFrame, uint8_t Address, uint8_t FunCode, uint8_t Error_Code);
#endif

