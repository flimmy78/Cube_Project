#ifndef _ULTRASONICDRIVER_H_
#define _ULTRASONICDRIVER_H_
#include "Log.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include "Combuff.h"


//-----����������
#define WAVE_SINGLE_BUFF 100
#define WAVE_CHANNEL_COUNT 3
#define ADC_IN1_CHANNEL  0
#define ADC_IN2_CHANNEL  1
#define ADC_TEMP_CHANNEL 2

#define SAMPLING_INTERVAL 5 	//������� ��λus
#define ULT_WAVE_COUNT 16
#define ULT_ADC	hadc
#define ULT_WAVE_TIM htim16
#define ULT_CAL_TIM  htim17
#define ULT_CYCLE_TIM htim1

#define NEAR_DISTANCE 245
#define FAR_DISTANCE  120



typedef enum { ULT_NONE, ULT_SEND, ULT_SEND_FINISH, ULT_RECEIVE, ULT_RECEIVE_FINISH, ULT_DMA_FINISH }ULT_STATE;
typedef enum { ULT_OK, ULT_BUSY, ULT_TIMEOUT,ULT_ERROR }ULT_RESULT;
typedef struct _Ultrasonic_T Ultrasonic_T;
struct _Ultrasonic_T{
	uint8_t Wave;
	volatile ULT_STATE Ult_State;
	uint8_t Wave_Adc_Buff[WAVE_SINGLE_BUFF][WAVE_CHANNEL_COUNT];
	int8_t TaskID;
	uint16_t Sn_Code;
	uint8_t  ID;
	
	volatile uint8_t It_Cnt;
	volatile uint8_t In1MaxValue;
	volatile int16_t In1MaxTiming;
	volatile uint8_t In2MaxValue;
	volatile int16_t In2MaxTiming;
	uint8_t DistanceSection;
	uint8_t NearCnt;
	uint8_t FarCnt;
	ULT_RESULT (*Send)(Ultrasonic_T*);
	ULT_RESULT (*DataClear)(Ultrasonic_T* a);
};


extern void ULT_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
extern ULT_RESULT Ultrasonic_Init(Ultrasonic_T* Ultrasonic);  
extern int8_t getTempSensor(void);

static Ultrasonic_T* _Ultrasonic;





//-----��ݷ���
#define ULT_CAL_TIM_START() __HAL_TIM_CLEAR_IT(&ULT_CAL_TIM, TIM_IT_UPDATE);__HAL_TIM_SET_COUNTER(&ULT_CAL_TIM, 0);HAL_TIM_Base_Start_IT(&ULT_CAL_TIM) //��ʼ��ʱ
#define ULT_CAL_TIM_STOP()  HAL_TIM_Base_Stop_IT(&ULT_CAL_TIM) 	  //ֹͣ��ʱ
#define ULT_CAL_TIM_CNT()   __HAL_TIM_GET_COUNTER(&ULT_CAL_TIM)	//��ȡ��ʱ��

//-----��ݷ���
#define ULT_WAVE_TIM_START() __HAL_TIM_CLEAR_IT(&ULT_WAVE_TIM, TIM_IT_UPDATE);__HAL_TIM_SET_COUNTER(&ULT_WAVE_TIM, 0);HAL_TIM_Base_Start_IT(&ULT_WAVE_TIM) //��ʼ��ʱ
#define ULT_WAVE_TIM_STOP()  HAL_TIM_Base_Stop_IT(&ULT_WAVE_TIM) 	  //ֹͣ��ʱ

////-----��ݷ���
//#define TIM14_START() __HAL_TIM_SET_COUNTER(&htim14, 0);HAL_TIM_Base_Start(&htim14) //��ʼ��ʱ
//#define TIM14_STOP()  HAL_TIM_Base_Stop_IT(&htim14) 	  //ֹͣ��ʱ
//#define TIM14_CNT()   __HAL_TIM_GET_COUNTER(&htim14)	//��ȡ��ʱ��

//���DMA������
//��ADC��ʱ��������
//����ADC�ɼ�
#define ULT_DMA_START(hadc)			memset(_Ultrasonic->Wave_Adc_Buff, 0, WAVE_SINGLE_BUFF * WAVE_CHANNEL_COUNT);\
																HAL_TIM_Base_Start(&ULT_CYCLE_TIM);\
																HAL_ADC_Start_DMA(hadc, (uint32_t*)_Ultrasonic->Wave_Adc_Buff, WAVE_SINGLE_BUFF * WAVE_CHANNEL_COUNT)	
//ֹͣADC�ɼ�
//ֹͣADC��ʱ������
#define ULT_DMA_STOP(hadc)			HAL_ADC_Stop_DMA(hadc); \
																HAL_TIM_Base_Stop(&ULT_CYCLE_TIM)	
																

//-----�㷨����
//#define DATA_CONVERT(var,step) abs(*(var + step) - DataTmplate[step])
#define DATA_CONVERT(var,step) abs(*var)
#define HIGH_THRESHOLD 120
#define LOW_THRESHOLD  10


//uint8_t SendUltrasonic(uint8_t WaveCount){
//	
//}


#endif


