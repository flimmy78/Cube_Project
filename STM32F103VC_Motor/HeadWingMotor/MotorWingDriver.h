#ifndef __MOTOR_WING_DRIVER_H__
#define __MOTOR_WING_DRIVER_H__
#include "MotorDriver.h" 
#include "ProtocolFrame.h"
#include "tim.h"
#define WING_MAX_PWM 1500			//��򳣹桢����˶��ٶ�
//#define WING_SLOWLY_PWM 120		//����½��������PWM
#define WING_BASE_PWM 500			//������ϻ���PWM

#define WING_INIT_PWM 1000			//����ʼ���ٶ�
#define WING_KEEP_PWM 150			//��򱣳��ٶ�
#define WING_DOWN_PWM 800			//����½�ʱ��˲ʱPWM

#define WING_MAX_ANGLE 110			//������Ƕ�
#define WING_MIN_ANGLE 0				//�����С�Ƕ�

#define WING_LIMIT_TRUE 0				
#define WING_LIMIT_FALSE 1 			

#define WING_ENC_MAX 50 				//�������
#define WING_DIFFENC 5					//�����������
#define WING_ELE_MAX 5000				//����������
#define WING_ELE_MIN 50					//��С�������

#define W_TIME_OUT_US (5 * 1000 * 1000) //�Լ쳬ʱʱ�� 
  
typedef enum {DIR_WING_U, DIR_WING_D, DIR_WING_CUSTOM} DIR_WING;//����˶�������
typedef enum { MOTOR_W_STOP, MOTOR_W_KEEP, MOTOR_W_RUN}RUN_STATE;		//������˶�״̬
typedef enum	{SELF_W_ERROR, SELF_W_DONE}SELF_STATE_W;
//liyao 2015��9��6��13:39:57
#define GET_ENCODER_WING_L 		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) //��������
#define GET_ENCODER_WING_R  	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)	//�ҳ������
#define GET_LIMIT_WING_L 		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)	//�����λ��
#define GET_LIMIT_WING_R  	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)	//�ҳ���λ��
/****************************************************
	�ṹ����:	MOTOR_WING_PARM
	����: �����ʵ��
	����:	liyao 2015��9��14��12:24:31
****************************************************/
typedef struct _MOTOR_WING_PARM MOTOR_WING_PARM ;
struct _MOTOR_WING_PARM						//����Ĳ���������Ҫ����һ�����õ�
{
	MOTOR_ID	MotorID;			//���ID 1-2
	Protocol_Info_T 	Exec_Protocol;//����ִ�е�Э��
	
	int16_t	  Speed;				//��ǰ�ٶ�
	int16_t 	SpeedCMD;		//�˴�������λ������δִ�е��ٶ�	0-100		��λΪ0xff�����������ǰ��Ҫ�ȶ�DIRһ��ʱ��
	DIR_WING		MotorDir;			//����ִ�еķ���
	DIR_WING		MotorDirCMD;		//�����
	uint32_t	Encoder;			//��ʼ����0x7fffffff���ٴ˻��������Ӽ�����ÿ���ϴ���ָ���ʼֵ
	uint32_t	EncoderLast;			//��ʼ����0x7fffffff���ٴ˻��������Ӽ�����ÿ���ϴ���ָ���ʼֵ
	int32_t  EncoderSUM;	//��λ����ۼ�ֵ;
	
	int16_t EncoderMAX;	//���������ֵ
	int16_t EncoderCMD;	//Ŀ�������ֵ
	int16_t Diff_Encoder;
	
	uint16_t  Current;				//��ǰ����
	uint16_t Electricity;//AD�ɼ�������Ϣ
	uint16_t 	ElectricityLast;		//���һ��AD�ɼ�������Ϣ
	uint8_t  Down_Flag;//�����˶���־λ
	
	TIM_TypeDef* TIMx;		//��ӦPWM��ʱ��
	GPIO_TypeDef* GPIOx;	//��ӦPWM	IO��
	uint16_t GPIO_Pin;		//��ӦPWM	IO��
	
	SELF_STATE_W	SelfState;
	uint8_t LimitState;	//��λ���
	RUN_STATE RunState;	//�Ƿ�������
	float Angle_Code;	//1��Ƕȶ�Ӧ������ֵ�������
	uint8_t Speed_Code;//1�ٶȶ�Ӧ��PWM
	uint16_t Angle_Cur;	//��ǰ�Ƕ�
	uint16_t Angle_CMD;	//��ǰ�Ƕ�
	uint16_t  Protect_Time;//��������
	
	uint16_t  PWM_Base;//��С�ܶ�pwm
	float Gain_PWM;//PWM����
	
	void (*MotorWingInit)(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
	void (*ExecuteWingMotor)(MOTOR_WING_PARM* motor);
	void (*StopWingMotor)(MOTOR_WING_PARM* motor);
	void (*KeepWingMotor)(MOTOR_WING_PARM* motor);
	void (*LimitTrigger)(MOTOR_WING_PARM* motor);
	void (*MotorProtectCheck)(MOTOR_WING_PARM* motor);
	void (*SetWingMotorParam)(MOTOR_WING_PARM* motor, uint16_t speed,int16_t angle);
	void (*ExecSpeedPWM)(TIM_TypeDef* TIMx, int16_t PWM);
	
};  



extern MOTOR_WING_PARM Motor_WL;	//���
extern MOTOR_WING_PARM Motor_WR;	//�ҳ�


extern void MotorWingInit(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
extern void SetWingMotorParam(MOTOR_WING_PARM* motor, uint16_t speed,int16_t angle);
extern void Wing_Self_Inspection(void);
//static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
extern void	ExecuteWingMotor(MOTOR_WING_PARM* motor);
extern void StopWingMotor(MOTOR_WING_PARM* motor);
extern void ScanWingMotorLimit(void);
extern void KeepWingMotor(MOTOR_WING_PARM* motor);
extern void BacklashWingMotor(MOTOR_WING_PARM* motor);
#endif

