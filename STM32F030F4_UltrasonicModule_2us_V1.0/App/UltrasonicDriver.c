#include "UltrasonicDriver.h"
#include "gpio.h"
#include "adc.h"
#include "TaskTimeManager.h"  
static Ultrasonic_T* _Ultrasonic;
uint8_t Ult_Cnt;
uint16_t jishi1 = 0, jishi2 = 0;
uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &ULT_WAVE_TIM){
		Ult_Cnt--;
		HAL_GPIO_WritePin(C_A_GPIO_Port, C_A_Pin,(GPIO_PinState)(Ult_Cnt%2));
		HAL_GPIO_WritePin(C_Y_GPIO_Port, C_Y_Pin,(GPIO_PinState)((Ult_Cnt+1)%2));
		if(Ult_Cnt == 0){
//			DelayUS(15);
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
	printf("dmaʱ�䣺%d,%d\r\n", jishi1,jishi2);
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

//uint8_t DataTmplate[] = {123,123,128,128,123,123,128,86,124,128,121,124,33,128,123,123,128,128,124,124,128,128,123,123,128,96,124,128,96,124,64,128,57,124,84,128,55,124,56,128,33,124,28,36,9,117,8,124,4,128,0,124,9,128,2,103,10,77,0,33,8,15,2,6,6,15,4,18,5,22,5,15,4,18,5,8,4,10,5,3,4,2,4,3,5,2,5,1,4,1,5,2,5,3,4,0,5,2,4,2,4,1,4,2,4,1,5,0,5,0,5,0,5,2,4,0,4,3,4,1,4,1,4,3,5,2,4,2,5,3,4,1,4,0,5,0,4,1,4,1,5,2,5,4,5,2,5,1,4,1,4,3,5,1,4,1,5,0,5,4,5,2,5,1,5,1,4,1,4,1,4,1,4,3,4,1,4,3,4,0,5,2,4,2,4,3,4,3,4,4,4,3,3,4,5,2,5,2,4,4,5,2,4,0,4,1,4,2,4,2,4,1,5,0,4,3,4,0,4,1,5,5,4,2,5,4,5,2,4,1,5,1,4,3,4,0,5,2,5,3,4,0,4,1,4,0,5,3,4,0,4,3,5,1,4,3,5,4,5,1,4,0,4,2,4,2,5,3,4,0,4,0,5,6,5,0,5,3,5,2,4,3,4,1,5,1,4,4,4,3,4,0,4,1,4,4,4,2,4,0,4,4,4,2,5,2,4,2,4,0,5,1,5,2,4,3,4,1,4,1,4,3,4,1,4,3,4,2,4,3,4,1,4,0,4,2,4,2,4,1,4,3,4,2,4,0,4,3,4,3,4,0,4,2,4,3,4,1,4,3,4,2,4,4,4,1,4,1,4,3,4,0,5,2,4,0,4,5,4,2,5,2,4,5,4,5,4,0,4,5,4,2,4,1,5,3,5,4,4,0,4,3,5,0,5,1,4,3,4,4,5,1,5,4,4,2,4,1,4,4,4,1,4,2,4,2,4,3,4,1,4,2,4,4,4,3,4,1,4,2,4,5,4,1,5,1,4,2,4,2,4,0,4,4,4,2,4,3,5,2,4,3,4,2,4,6,4,3,4,3,4,3,4,4,5,2,4,1,4,1,5,6,5,1,5,1,4,4,4,1,4,1,4,0,4,2,4,4,4,1,4,1,4,3,4,1,4,4,4,1,5,2,4,3,4,1,4,0,4,2,4,3,5,3,5,0,5,1,4,10,5,4,4,3,4,3,4,4,4,0,5,2,5,0,4,7,4,0,5,2,4,5,4,2,4,1,4,0,4,4,4,1,5,5,4,1,4,3,5,2,5,0,4,3,5,3,4,2,4,1,4,2,4,4,4,0,4,3,5,3,4,3,4,2,4,4,4,2,4,6};
uint8_t DataTmplate[] = {78,251,90,252,252,0,251,0,252,0,252,0,251,0,252,0,127,0,0,251,0,252,0,252,0,252,0,251,0,251,250,98,252,0,251,0,251,0,251,0,250,0,0,105,0,252,0,252,0,252,0,251,0,252,126,248,252,0,252,0,250,0,252,0,252,0,130,0,0,250,0,252,0,252,0,251,0,253,0,252,251,86,251,0,251,0,252,0,252,0,250,0,0,105,0,252,0,252,0,252,0,251,0,252,133,247,252,0,251,0,251,0,252,0,252,0,131,0,0,250,0,252,0,252,0,252,0,251,0,252,251,80,252,0,251,0,251,0,251,0,251,0,0,102,0,252,0,252,0,252,0,252,0,250,135,247,251,0,252,0,252,0,252,0,251,0,131,0,0,250,0,251,0,252,0,252,0,252,0,252,251,78,251,0,251,0,252,0,251,0,251,0,0,104,0,251,0,252,0,252,0,252,0,252,154,243,251,0,251,0,3,53,0,251,0,252,0,252,0,251,0,252,0,252,250,100,251,0,252,0,252,0,250,0,245,0,0,149,0,251,0,252,0,252,0,251,0,252,0,252,250,124,251,0,252,0,252,0,246,0,0,145,0,252,0,250,0,252,0,252,0,251,0,251,247,154,251,0,251,0,252,0,252,0,91,0,0,251,0,251,0,252,0,252,0,252,0,251,174,237,251,0,251,0,252,0,252,0,252,0,0,82,0,251,0,251,0,252,0,252,0,251,147,245,251,0,251,0,251,0,252,0,250,0,0,111,0,252,0,251,0,252,0,252,0,251,103,250,251,0,251,0,251,0,252,0,252,0,22,26,0,252,0,252,0,251,0,252,0,252,50,252,252,2,250,0,252,0,251,0,252,0,60,3,0,252,0,252,0,251,0,251,0,252,1,252,251,70,252,0,251,0,252,0,251,0,245,0,0,152,0,252,0,252,0,251,0,252,0,252,249,146,252,0,251,0,251,0,251,0,241,0,0,168,0,252,0,252,0,252,0,251,0,252,251,89,252,0,252,0,251,0,251,0,249,0,0,113,0,252,0,252,0,252,0,251,0,252,78,252,252,0,251,0,251,0,251,0,251,0,193,0,0,226,0,252,0,252,0,251,0,251,35,252,252,10,252,0,251,0,251,0,251,0,171,0,0,242,0,252,0,252,0,252,0,251,11,252,251,37,252,0,252,0,251,0,252,0,248,0,0,113,0,251,0,252,0,252,0,251,0,251,194,218,252,0,252,0,251,0,252,0,251,0,77,1,0,251,0,252,0,252,0,251,0,251,119,248,252,0,252,0,252,0,251,0,251,0,147,0,0,249,0,251,0,252,0,252,0,251,57,253,251,1,252,0,252,0,251,0,251,0,201,0,0,210,0,251,0,252,0,252,0,251,5,252,251,67,252,0,251,0,252,0,251,0,249,0,8,75,0,252,0,252,0,251,0,252,0,252,181,222,252,0,251,0,252,0,251,0,252,0,67,4,0,252,0,252,0,251,0,251,0,252,185,212,252,0,252,0,251,0,251,0,251,0,65,14,0,251,0,252,0,252,0,250,0,252,130,247,252,0,252,0,251,0,251,0,251,0,179,0,0,215,0,251,0,252,0,252,0,251,29,252,249,53,252,0,252,0,250,0,251,0,201,0,0,188,0,251,0,252,0,252,0,251,65,251,248,15,251,0,252,0,252,0,251,0,188,0,8,204,0,252,0,251,0,252,0,252,34,251,211,83,251,0,252,0,252,0,251,0,242,0,59,80,0,251,0,251,0,252,0,252,1,251,171,185,251,0,252,0,252,0,251,0,246,0,79,38,0,251,0,252,0,252,0,252,1,251,161,196,248,0,252,0,251,0,251,0,242,0,87,38,3,251,0,251,0,252,0,252,26,251,156,182,230,0,249,0,250,0,248,0,178,0,100,61,38,252,6,252,1,251,18,252,72,252,135,231,192,0,232,0,243,0,231,0,184,0,123,0,62,252,19,252,6,251,17,252,63,252,123,250,180,0,222,0,235,0,221,0,181,0,126,0,75,251,37,252,25,252,39,251,72,252,117,252,160,0,193,0,207,0,201,0,177,0,141,0,102,156,71,252,50,252,49,252,66,251,96,252,135,197,169,0,194,0,200,0,187,0,157,0,118,7,83,252,56,252,49,252,63,251,91,252,129,237,164,0,187,0,192,0,180,0,153,0,119,6,89,252,69,252,64,251,75,251,96,252,123,250,148,0,168,0,174,0,170,0,154,0,131,0,108,208,87,252,77,252};
void printData(Ultrasonic_T* Ult){
	printf("\v");
	for(uint16_t i = 0; i < WAVE_ADC_BUFF; i+=2){
		//printf("%d\t%d\r\n", *(Ult->Wave_Adc_Buff+i), *(Ult->Wave_Adc_Buff+i+1));
		printf("%d\t%d\r\n", *(Ult->Wave_Adc_Buff+i) - DataTmplate[i], *(Ult->Wave_Adc_Buff+i+1) - DataTmplate[i+1]);
		//if(i%2==0) continue;
//		printf("%d",Ult->Wave_Adc_Buff[i]);
//		if( i > 0 && i % 2 == 1)
//			printf("\r\n");
	}
}

ULT_RESULT Ultrasonic_Send(Ultrasonic_T* Ult){	
	if(Ult->Ult_State != ULT_NONE)
		return ULT_BUSY;
	uint16_t Distance = 0;
	Ult->Ult_State = ULT_SEND;
	Ult_Cnt = Ult->Wave;
	MX_GPIO_Init();//ת��ΪIO���
	__HAL_TIM_SET_COUNTER(&ULT_WAVE_TIM, 0);//��0 timer
	ULT_DMA_START(&ULT_ADC);								//ADC��ʼ�ɼ�
	HAL_TIM_Base_Start_IT(&ULT_WAVE_TIM);		//������ʱ��
	ULT_CAL_TIM_START();									  //��ʱ����ʼ��ʱ
	//printf("%X,%X\r\n",Ult->Ult_State, ULT_SEND_FINISH);
	//while(Ult->Ult_State != ULT_SEND_FINISH);//�ȴ��������������
	//while(ULT_CAL_TIM_CNT() < 1200);//�㿪ä��
	Ult->Ult_State = ULT_RECEIVE;		//������״̬ת��
	while(Ult->Ult_State != ULT_DMA_FINISH); 
	for(uint8_t i = 1; i > 0 ;i--){
		Distance = DistanceAnalysis(Ult->Wave_Adc_Buff, WAVE_ADC_BUFF);
//		printf("����:%d\r\n", Distance);
		printData(Ult); 
		if(Distance > 0)
			break;
	}
//	printf("����:%d\r\n", Distance);
	Ult->Ult_State = ULT_NONE;
	if(Distance == 0)
		return ULT_TIMEOUT;
	return ULT_OK;
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


//0 ����  1����
uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size){
	int16_t DataMax = 0, DataMaxIndex = 0;
	for(uint16_t i = 0; i < Size; i += 2){
		if(DATA_CONVERT(Data, i)  > DataMax){
			DataMax = DATA_CONVERT(Data, i);
			DataMaxIndex = i/2;
		}
	}
	
//	for(uint16_t i = 61; i < Size; i += 2){
//		if(DATA_CONVERT(Data, i) > 120){
//			DataMax = DATA_CONVERT(Data, i);
//			DataMaxIndex = i/2;
//		}
//	}
//	printf("�����:%d,��ߵ�:%d\r\n", DataMax, DataMaxIndex);
//	if(DataMaxIndex == 0)
//		return 0;
	return (DataMaxIndex - 4) * 38 * 34 / 2 / 10;
}
/*uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size){
	uint8_t HighCnt = 0, LowCnt = 0, HighFlag = 1, LowFlag = 0, LowIndex = 255, DeadZone = 0;
	uint8_t *Cursor = NULL;
//	printf("------------------------\r\n");
	//
	for(int i = 1; i < Size; i+=2){
		uint8_t tmpData = abs(*(Data+i) - 128);
		Cursor = (Data + i);
		if(DATA_CONVERT(Cursor,0) > HIGH_THRESHOLD && DATA_CONVERT( Cursor, 2) > HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) > HIGH_THRESHOLD){
			HighCnt++;
			HighFlag = 1;
			LowFlag = 0;
		}else if(DATA_CONVERT(Cursor, 0) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 2) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) < HIGH_THRESHOLD){
			LowCnt++;
			HighFlag = 0;
			LowFlag = 1;
		}else{
			if(HighFlag)
				HighCnt++;
			else if(LowFlag)
				LowCnt++;
		}
		
		if(i == 81){
			//printf("high:%d,low:%d\r\n",HighCnt,LowCnt);
			if(HighCnt > 35){
				DeadZone = 1;
				printf("hight > 35\r\n");
			}
		}
	}
	
	HighCnt = 0, LowCnt = 0, HighFlag = 1, LowFlag = 0, LowIndex = 255;
	for(int i = 0; i < Size; i+=2){
		uint8_t tmpData = DATA_CONVERT(Data, i);//abs(*(Data+i) - 128);
		Cursor = (Data + i);
		if(DATA_CONVERT(Cursor,0) > HIGH_THRESHOLD && DATA_CONVERT( Cursor, 2) > HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) > HIGH_THRESHOLD){
			HighCnt++;
			HighFlag = 1;
			LowFlag = 0;
		}else if(DATA_CONVERT(Cursor, 0) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 2) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) < HIGH_THRESHOLD){
			LowCnt++;
			HighFlag = 0;
			LowFlag = 1;
			if(LowIndex == 255)
				LowIndex = i;
		}else{
			if(HighFlag)
				HighCnt++;
			else if(LowFlag)
				LowCnt++;
		}
		
	}
	if(DeadZone)
		printf("ä���ڲ�ࣺ%d, %d\r\n", LowIndex, (int)(LowIndex / 2.0 * 38.0 * 0.034/2));
	else
		printf("�ϰ�����ä����\r\n");
	return 0;
}*/

