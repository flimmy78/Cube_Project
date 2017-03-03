#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
#include "stm32f0xx_hal.h"
#include "main.h"

#define STEPMOTOR_STEP 200	//�������360�㲽��
#define MICROSTEP 32       	//ϸ�ֿ�����ϸ�ֲ���
#define PERIMETER 39.94  			//ͬ����ת��һȦƤ���˶����� ��λmm
#define MAX_SIZE 850

#define DIR_RIGHT 1
#define DIR_LEFT 0
#define SET_RIGHT_DIR() HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port, GPIO_DIR_Pin, GPIO_PIN_SET)
#define SET_LEFT_DIR() HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port, GPIO_DIR_Pin, GPIO_PIN_RESET)

typedef enum{NONE_M, RUNNING_M, FINISH_M}MOTOR_STATE;
typedef struct _StepMotor StepMotor;
struct _StepMotor{
	uint8_t MicroStep;
	//float Location_Cur_Offset;//����ָ�ǰ�Ѿ��˶�����
	float Location_Cmd_Offset;//ָ���˶�����
	uint32_t Location_Cur_Step;//����ָ�ǰ�Ѿ��˶�����
	uint32_t Location_Cmd_Step;//ָ���˶�����
	uint32_t Max_Step;				//��̨������о���
	uint32_t Absolute_Step;	  //ȫ�־��Բ���
	
	uint8_t Dir_Cmd;		//�˶�����
	uint8_t Location_Cmd_Speed;//ָ���˶��ٶ�
	float SingleStep_D;//�������� mm
	float SingleStep_A;//�����Ƕ� mm
	uint16_t SingleMillimeter_Setp;//�˶�1��������Ĳ���
	MOTOR_STATE Motor_State;//�˶�״̬
	
	void (*Run_Offset)(uint8_t dir, float Offset, uint8_t Speed);
	void (*Run_Angle)(uint8_t dir,float Angle, uint8_t Speed);
	void (*Stop)(void);
};


extern StepMotor stepMotor;

extern void StepMotor_Init(void);
extern void Scan_Limit(void);
#endif

