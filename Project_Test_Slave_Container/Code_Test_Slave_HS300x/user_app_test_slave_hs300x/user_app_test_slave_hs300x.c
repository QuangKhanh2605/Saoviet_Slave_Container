#include "user_app_test_slave_hs300x.h"

#include "user_modbus_rtu.h"
#include "stdio.h"
#include "string.h"

static uint8_t fevent_test_slave_entry(uint8_t event);
static uint8_t fevent_test_slave_display(uint8_t event);
static uint8_t fevent_test_slave_transmit(uint8_t event);
static uint8_t fevent_test_slave_handle_receive(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppTestSlave [] =
{
  { _EVENT_TEST_SLAVE_ENTRY,             1, 0, 0,          fevent_test_slave_entry},
  
  { _EVENT_TEST_SLAVE_DISPLAY,           0, 0, 10000,      fevent_test_slave_display},    
  
  { _EVENT_TEST_SLAVE_TRANSMIT,          0, 0, 0,          fevent_test_slave_transmit},
  
  { _EVENT_TEST_SLAVE_HANDLE_RECEIVE,    0, 0, 400, 	   fevent_test_slave_handle_receive},
    
};   

/*================= Variable =================*/

int16_t temRx=0;
int16_t humiRx=0;

int16_t T[20]={0};
int16_t H[20]={0};

uint8_t address_slave_get = 1;

char Tem_Humi[50];

uint8_t aError[100]={0};
sData sError=
{
    .Data_a8 = aError,
};

/*================= Function Run ====================*/
static uint8_t fevent_test_slave_entry(uint8_t event)
{
    fevent_active(sEventAppTestSlave, _EVENT_TEST_SLAVE_TRANSMIT);
    fevent_active(sEventAppTestSlave, _EVENT_TEST_SLAVE_DISPLAY);
    sEventAppTestSlave[_EVENT_TEST_SLAVE_DISPLAY].e_systick = HAL_GetTick();
    return 1;
}

static uint8_t fevent_test_slave_display(uint8_t event)
{
    display_uart();
    fevent_active(sEventAppTestSlave, _EVENT_TEST_SLAVE_TRANSMIT);
    fevent_enable(sEventAppTestSlave, event);
    return 1;
}

static uint8_t fevent_test_slave_transmit(uint8_t event)
{
    uint8_t Frame[8];
    sData sFrame = {&Frame[0], 0};
    
    if(address_slave_get > NUMBER_SLAVE) 
    {
      address_slave_get = 1;
    }
    else
    {
        ModRTU_Master_Read_Frame(&sFrame, address_slave_get, 0x03, 0x02, 4);
        HAL_GPIO_WritePin(NET485IO_GPIO_PORT, NET485IO_PIN, NET485IO_PIN_SET);
        HAL_Delay(10);
        HAL_UART_Transmit(&huart1, sFrame.Data_a8, sFrame.Length_u16, 1000);
        HAL_Delay(5);
        HAL_GPIO_WritePin(NET485IO_GPIO_PORT, NET485IO_PIN, NET485IO_PIN_RESET);
        
        fevent_active(sEventAppTestSlave, _EVENT_TEST_SLAVE_HANDLE_RECEIVE);
        sEventAppTestSlave[_EVENT_TEST_SLAVE_HANDLE_RECEIVE].e_systick = HAL_GetTick();
    }
    
    return 1;
}

static uint8_t fevent_test_slave_handle_receive(uint8_t event)
{
    uint16_t CRC_rx = sUart485.Data_a8[sUart485.Length_u16-1] << 8 | sUart485.Data_a8[sUart485.Length_u16-2];
    uint16_t CRC_check = ModRTU_CRC(&sUart485.Data_a8[0], sUart485.Length_u16-2);
    
    temRx  = -100;
    humiRx = -100;
    if(sUart485.Length_u16 > 0)
    {
        if(CRC_check == CRC_rx)
        {
            if(sUart485.Data_a8[1] == 0x03)
            {
                if(address_slave_get == sUart485.Data_a8[0])
                {
                    temRx = sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
                    humiRx= sUart485.Data_a8[9]<<8 | sUart485.Data_a8[10];
                }
            }  
        }
    }
    else
    {
        temRx  = 0;
        humiRx = 0;
    }
    
    
    T[address_slave_get - 1]=temRx; 
    H[address_slave_get - 1]=humiRx;
    
    if(temRx == -100 && humiRx == -100)
    {
        sError.Length_u16 = 0 ;
        if(sUart485.Length_u16 < 50)
        {
          Convert_Hex_To_String_Hex(&sError, &sUart485);
        }
    }
    
    Delete_Buffer(&sUart485);
    
    address_slave_get++;
    fevent_active(sEventAppTestSlave, _EVENT_TEST_SLAVE_TRANSMIT);
    return 1;
}

//void AppTestSlaveHs300x(void)
//{
//    if(HAL_GetTick()- GetTick_Ms > TIME_SAMPLING) 
//    {
//        display_uart();
//        GetTick_Ms = HAL_GetTick();
//    }
//    
//    if(HAL_GetTick()- getTick_transmit_uart > 200) 
//    {
//        static uint8_t address_Tx = 1;
//        uint8_t Frame[8];
//        sData sFrame = {&Frame[0], 0};
//        
//        ModRTU_Master_Read_Frame(&sFrame, address_Tx, 0x03, 0x02, 4);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
//        HAL_UART_Transmit(&huart1, sFrame.Data_a8, sFrame.Length_u16, 1000);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
//        address_Tx++;
//        if(address_Tx > NUMBER_SLAVE) address_Tx=1;
//        getTick_transmit_uart = HAL_GetTick();
//    }
//    
//    uint8_t check_address_slave = Modbus_RTU_Master();
//    if(check_address_slave != 0)
//    {
//        T[check_address_slave - 1]=temRx; 
//        H[check_address_slave - 1]=humiRx;
//    }    
//}


/*================= Function Handle ==================*/

void Init_AppTest(void)
{
    HAL_GPIO_WritePin(ON_OFF_IC_485_GPIO_PORT, ON_OFF_IC_485_PIN, ON_IC_485_PIN);
}

void display_uart(void)
{
    static uint32_t count = 0;
	count++;
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",T[0],T[1],T[2],T[3],T[4]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r",(uint16_t)strlen("\r"),1000);
	
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",T[5],T[6],T[7],T[8],T[9]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
    
    sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",T[10],T[11],T[12],T[13],T[14]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
    
    sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",T[15],T[16],T[17],T[18],T[19]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
	
    
    
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
    
    
	
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",H[0],H[1],H[2],H[3],H[4]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
				
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",H[5],H[6],H[7],H[8],H[9]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
	
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",H[10],H[11],H[12],H[13],H[14]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
    
	sprintf(Tem_Humi,"| %d | %d | %d | %d | %d |",H[15],H[16],H[17],H[18],H[19]);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
    
    
    
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
	
    if(sError.Length_u16 > 0)
    {
        HAL_UART_Transmit(&huart3,(uint8_t *)"Error_Recv: ",(uint16_t)strlen("Error_Recv: "),1000);
        HAL_UART_Transmit(&huart3,sError.Data_a8,sError.Length_u16,1000);
        HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
        sError.Length_u16 = 0;
    }
    
	
	sprintf(Tem_Humi,"-----------------------%d------------------------",count);
	HAL_UART_Transmit(&huart3, (uint8_t *)Tem_Humi, (uint16_t)strlen(Tem_Humi), 1000);
	HAL_UART_Transmit(&huart3,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
}



//uint8_t Modbus_RTU_Master(void)
//{
//	if(Check_CountBuffer_Complete_Uart_485(sUart485)==1)
//	{
//		uint16_t addressRx = sUart485.Data_a8[0];
//		uint16_t funCode   = sUart485.Data_a8[1];
//        if(funCode == 0x03)
//        {
//            uint16_t CRC_rx = sUart485.Data_a8[sUart485.Length_u16-1] << 8 | sUart485.Data_a8[sUart485.Length_u16-2];
//            uint16_t CRC_check = ModRTU_CRC(&sUart485.Data_a8[0], sUart485.Length_u16-2);
//            if(CRC_check == CRC_rx)
//            {
//                temRx= sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
//                humiRx= sUart485.Data_a8[9]<<8 | sUart485.Data_a8[10];
//                Delete_Buffer(&sUart485);
//                return addressRx;
//            }  
//		}
//		Delete_Buffer(&sUart485);
//	}
//	return 0;
//}

/*============= Function Handle ==============*/
uint8_t Check_CountBuffer_Complete_Uart_485(sData sUart)
{
  static uint16_t countBuffer_uart = 0;
  static uint32_t Get_systick = 0;
  static uint16_t State_systick = 0;
  uint8_t answer=0;
  
  if(sUart.Length_u16 > 0)
  {
      if(State_systick == 0 )
      {
          Get_systick = HAL_GetTick();
          countBuffer_uart = sUart.Length_u16;
          State_systick = 1;
      }
      else
      {
          if(HAL_GetTick() - Get_systick > 5)	
          {
              if(countBuffer_uart == sUart.Length_u16)
              {
                  answer = 1;
              }
              else
              {
                  Get_systick = HAL_GetTick();
                  countBuffer_uart = sUart.Length_u16;
              }
          }
          else
          {
              if(countBuffer_uart != sUart.Length_u16)
              {
                  Get_systick = HAL_GetTick();
                  countBuffer_uart = sUart.Length_u16;
              }
          }
      }
  }
  else
  {
      State_systick = 0;
  }
  return answer;
}

void Delete_Buffer(sData *sUart)
{
    for(uint8_t i = 0; i< sUart->Length_u16; i++)
    {
        sUart->Data_a8[i] = 0x00;
    }
    sUart->Length_u16 = 0;
}

uint8_t AppTestSlaveHs300x_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = false;

	for (i = 0; i < _EVENT_TEST_SLAVE_END; i++)
	{
		if (sEventAppTestSlave[i].e_status == 1)
		{
            Result = true;

			if ((sEventAppTestSlave[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppTestSlave[i].e_systick)  >=  sEventAppTestSlave[i].e_period))
			{
                sEventAppTestSlave[i].e_status = 0;  //Disable event
				sEventAppTestSlave[i].e_systick = HAL_GetTick();
				sEventAppTestSlave[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}


