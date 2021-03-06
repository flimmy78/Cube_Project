/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "TaskTimeManager.h"
#include "ProtocolFrame.h"
#include "ComBuff.h" 
#include "MFRC522.h"

#define LED_ON() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define LED_OFF() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void LED_TEST(void){
  static uint8_t seq = 0; 
	//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (GPIO_PinState)(seq%2));
  printf("LED_TEST:%d\r\n", seq++); 
//	State_P_T state = {0};
//	state.para1 = 32;
//	Protocol_Send(REPORT_STATE, &state, sizeof(State_P_T));
}

uint8_t BuffPWD[6] = {0};
uint8_t TmpBuff[6] = {0};
uint16_t TmpPWD = 0;
uint8_t* getTestPWD(){
	memcpy(TmpBuff, BuffPWD, 6);
	if(BuffPWD[0] == 255){
		BuffPWD[1] ++;}
	if(BuffPWD[1] == 255){
		BuffPWD[1] = 0;
		BuffPWD[2] ++;}
	if(BuffPWD[2] == 255){
		BuffPWD[2] = 0;
		BuffPWD[3] ++;}
	if(BuffPWD[3] == 255){
		BuffPWD[3] = 0;
		BuffPWD[4] ++;}
	if(BuffPWD[4] == 255){
		BuffPWD[4] = 0;
		BuffPWD[5] ++;}
	
	BuffPWD[0]++; 
	if(BuffPWD[5] == 255){
		return NULL;
	}
	return TmpBuff;
}


unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char g_ucTempbuf[20];
unsigned char card_seq[20];
void MFRC522_Run(void){ 
	
		printf("%d,%d,%d\r\n", TmpBuff[0], TmpBuff[1], TmpBuff[2]);
//		printf("ModeReg_3D:%X\r\n",ReadRawRC(ModeReg));
//		printf("TReloadRegL_30:%X\r\n",ReadRawRC(TReloadRegL));
//		printf("TReloadRegH_0:%X\r\n",ReadRawRC(TReloadRegH));
//		printf("TModeReg_8D:%X\r\n",ReadRawRC(TModeReg));
//		printf("TPrescalerReg_3E:%X\r\n",ReadRawRC(TPrescalerReg));
//	int8_t res = ReadRawRC(CWGsCfgReg); 
	//
	int8_t status = PcdRequest(PICC_REQALL, card_seq);//寻天线区内未进入休眠状态
	if (status != MI_OK)//未寻到卡返回继续寻卡
	{
		LED_OFF();
		return;
  }
	LED_ON();
	status = PcdAnticoll(card_seq); //防冲突处理，输出卡片序列号，4字节
	if (status != MI_OK)
	{ 
		printf("PcdAnticoll != MI_OK\r\n");
		return;   
	}else{
		printf("card sequence number:%02x %02x %02x %02x\r\n",card_seq[0],card_seq[1],card_seq[2],card_seq[3]);
		
		status = PcdSelect(card_seq); //选择卡片，输入卡片序列号，4字节
		if (status != MI_OK)
		{    
			printf("PcdSelect != MI_OK\r\n");
			return;    
		}
		UnlockBlock0();
		return;
		
//		//读取卡片内所有数据 
//		uint8_t j = 0;
//		for(uint8_t i = 0; i < 16; i++){
//			status = PcdAuthState(PICC_AUTHENT1A, i*4, DefaultKey, card_seq);		//在读写卡之前需要先进行认证
//			if (status != MI_OK)
//			{    
//				printf("PcdAuthState != MI_OK\r\n");
//				return; 
//			}
//			printf("第%d扇区\r\n", i);
//			for(j = 0; j < 4; j++){
//				printf("\t第%d块\r\n",i*4+j);
//				status = PcdRead(i*4+j, g_ucTempbuf);//读取卡片数据 地址，数据回填
//				if (status != MI_OK)
//				{    
//					printf("\tPcdRead != MI_OK\r\n");
//					break;
//				} 
//				printf("\t");
//				for(uint16_t z = 0; z < 16; z++){  
//					printf(" 0x%02X", g_ucTempbuf[z]);
//				}
//				printf("\r\n");
//			}
//			if(j != 4)
//				break; 
//		}

		uint8_t private_key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		uint8_t data[] = {0xDE, 0x4B, 0xA2, 0x69, 0x5E, 0x08, 0x04, 0x00, 0x01, 0x0D, 0x63, 0xE3, 0xC1, 0xBB, 0x77, 0x1D};
			status = PcdAuthState(PICC_AUTHENT1A, 0, private_key, card_seq);		//在读写卡之前需要先进行认证
			if (status != MI_OK)
			{    
				printf("PcdAuthState != MI_OK\r\n");
				return; 
			}
		status = PcdWrite(0, data);//读取卡片数据 地址，数据回填
		//printf("卡数据：%02X,%02X\r\n", g_ucTempbuf[0], g_ucTempbuf[1]);
		if (status != MI_OK)
		{    
			printf("PcdWrite != MI_OK\r\n");
			return; 
		}else{
			printf("数据写卡成功\r\n");
		}
		
		 
		status = PcdHalt();   //命令卡片进入休眠状态
		if (status != MI_OK)
		{    
			printf("PcdHalt != MI_OK\r\n"); 
			return;
		}else{
			printf("卡休眠\r\n");
			return;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_2){
		printf("中断\r\n");
	}
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  TaskTime_Init();
  ProtocolFrame_Init();
  Log_Init();
  ComBuff_Init(); 
	
	LED_OFF(); 
	PcdReset();
	PcdAntennaOff();
	M500PcdConfigISOType( 'A' ); 

	
	/*-----------测试调试任务-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,500), LED_TEST, Real_Mode);
	/*-----------持续传输任务-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,30), SenderKeepTransmit, Count_Mode);
	/*-----------协议解析任务-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,30), PaddingProtocol, Real_Mode);
	/*-----------协议执行任务-----------------*/
	TaskTime_Add(TaskID++, TimeCycle(0,30), FetchProtocols, Real_Mode);
	/*-----------执行读卡器任务-----------------*/
	TaskTime_Add(TaskID++, TimeCycle(0,500), MFRC522_Run, Real_Mode);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    TaskTime_Run(); 
		getTestPWD();
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
    Log.error("Error_Handler");
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
