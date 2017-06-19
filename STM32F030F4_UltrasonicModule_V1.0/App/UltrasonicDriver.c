#include "UltrasonicDriver.h"
#include "gpio.h"
#include "adc.h"
static Ultrasonic_T* _Ultrasonic;
uint8_t Ult_Cnt;
uint16_t jishi1 = 0, jishi2 = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &ULT_WAVE_TIM){
		Ult_Cnt--;
		HAL_GPIO_WritePin(C_A_GPIO_Port, C_A_Pin,(GPIO_PinState)(Ult_Cnt%2));
		HAL_GPIO_WritePin(C_Y_GPIO_Port, C_Y_Pin,(GPIO_PinState)((Ult_Cnt+1)%2));
		if(Ult_Cnt == 0){
			GPIO_Float_Init();//ת��ΪIO��������
			HAL_TIM_Base_Stop_IT(&ULT_WAVE_TIM);
			_Ultrasonic->Ult_State = ULT_SEND_FINISH;
		}
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	jishi1 = ULT_CAL_TIM_CNT();	
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	_Ultrasonic->Ult_State = ULT_DMA_FINISH;
	jishi2 = ULT_CAL_TIM_CNT();
	ULT_DMA_STOP(ULT_ADC);		
//	printf("dmaʱ�䣺%d,%d\r\n", jishi1,jishi2);
	jishi1 = jishi2 = 0;
}

ULT_RESULT Ultrasonic_Init(Ultrasonic_T* Ult){
	Ult->Wave = ULT_WAVE_COUNT;
	Ult->Ult_State = ULT_NONE;
	Ult->Wave_Adc_Buff = MALLOC(WAVE_ADC_BUFF);
	MALLOC_CHECK(Ult->Wave_Adc_Buff, NULL);
	_Ultrasonic = Ult;
	return ULT_OK;
}

void printData(Ultrasonic_T* Ult){
	for(uint16_t i = 0; i < WAVE_ADC_BUFF; i+=2){
		printf("%d\t%d\r\n", Ult->Wave_Adc_Buff[i], Ult->Wave_Adc_Buff[i+1]);
		//if(i%2==0) continue;
//		printf("%d",Ult->Wave_Adc_Buff[i]);
//		if( i > 0 && i % 2 == 1)
//			printf("\r\n");
	}
}

ULT_RESULT Ultrasonic_Send(Ultrasonic_T* Ult){	

	uint8_t in = 0,out = 0;
	Ult->Ult_State = ULT_SEND;
	Ult_Cnt = Ult->Wave;
	MX_GPIO_Init();//ת��ΪIO���
	__HAL_TIM_SET_COUNTER(&ULT_WAVE_TIM, 0);//��0 timer
	HAL_TIM_Base_Start_IT(&ULT_WAVE_TIM);		//������ʱ��
	ULT_CAL_TIM_START();									  //��ʱ����ʼ��ʱ
	//printf("%X,%X\r\n",Ult->Ult_State, ULT_SEND_FINISH);
	while(Ult->Ult_State != ULT_SEND_FINISH);
//	
//	while(ULT_CAL_TIM_CNT() < 1400);//�㿪ä��
	ULT_DMA_START(&ULT_ADC);								//ADC��ʼ�ɼ�
	Ult->Ult_State = ULT_RECEIVE;		//������״̬ת��
	while(Ult->Ult_State != ULT_DMA_FINISH);
	printData(Ult); 
	return ULT_TIMEOUT;
	while(ULT_CAL_TIM_CNT() < 15*1000){//��������ڣ�15ms �� 5M��ѭ�����Ҳ���
		in = 0,out = 0;
		for(uint16_t i = 0; i < WAVE_ADC_BUFF; i++){//WAVE_ADC_BUFF
			if(((Ult->Wave_Adc_Buff[i] > 130 && Ult->Wave_Adc_Buff[i] < 140) ||
				  (Ult->Wave_Adc_Buff[i] > 110 && Ult->Wave_Adc_Buff[i] < 120)) && 
				   Ult->Wave_Adc_Buff[i] > 0){
				in = 1;
			}	 
			if(Ult->Wave_Adc_Buff[i] > 140 || Ult->Wave_Adc_Buff[i] < 110){
				out = 1;
			}
		}
		if(in == 1 )//&& out == 0)
			{
				uint16_t tl = ULT_CAL_TIM_CNT();
				
				Ult->Ult_State = ULT_RECEIVE_FINISH;
				ULT_DMA_STOP(&ULT_ADC);	
				ULT_CAL_TIM_STOP();//��ʱ���رռ�ʱ
				
				printf("��ʱ:%d,����:%d\r\n", tl, (int)(0.34f*tl)/2);
				return ULT_OK;
			}
		
	}
	printData(Ult);
	return ULT_TIMEOUT;
}

int8_t getTempSensor(void){
	
	HAL_ADC_DeInit(&hadc);
	HAL_ADC_Temp_Init();
	HAL_ADC_Start(&hadc); 
	HAL_ADC_PollForConversion(&hadc,0xffff);
	int8_t temp = (1430 - (HAL_ADC_GetValue(&hadc)*3300/255 ))/4 + 25;
	printf("�ضȣ�%d\r\n", temp);
	HAL_ADC_Stop(&hadc); 
	HAL_ADC_DeInit(&hadc);
	MX_ADC_Init();
	return 0;
}


