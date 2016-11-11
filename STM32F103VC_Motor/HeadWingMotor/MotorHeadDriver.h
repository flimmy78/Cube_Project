#ifndef __MOTOR_HEAD_DRIVER_H__
#define __MOTOR_HEAD_DRIVER_H__  
#include "ProtocolFrame.h"
#include "PID.h"
#include "MotorDriver.h"

#define HEAD_MAX_PWM 2399			//ͷ���ٶ����PWM
//#define HEAD_BASE_PWM 200			//ͷ���ٶȻ���PWM

#define HEAD_INIT_PWM 2000

#define HEAD_MAX_ANGLE 240			//ͷ�����Ƕ�
#define HEAD_MIN_ANGLE 0				//ͷ����С�Ƕ�

#define HEAD_LIMIT_TRUE 0
#define HEAD_LIMIT_FALSE 1

#define DIR_MOTOR_HEAD_L		0		//ͷ����ת
#define DIR_MOTOR_HEAD_R		1		//ͷ����ת
#define DIR_MOTOR_HEAD_NONE	3		//ͷ���޶���

#define MID_OFFSET	0	//�����е�ƫ��

#define PID_41PROPORTION 5   //4.1�����ٶȱ���  
#define PID_45PROPORTION 10   	 //4.5�����ٶȱ���  

#define HEAD_45MID_ENC 128						//4.5�����е�����
#define HEAD_45MID_ENC_L 126					//4.5��������е�����
#define HEAD_45MID_ENC_R 128					//4.5�����Ҳ��е�����
#define HEAD_41MID_ENC 259						//4.1�����е�����
#define HEAD_41MID_ENC_L 259					//4.1��������е�����
#define HEAD_41MID_ENC_R 259					//4.1�����Ҳ��е�����

#define HEAD_41ENC_MAX 517				//4.1�����������
#define HEAD_45ENC_MAX 256				//4.5�����������
#define HEAD_DIFFENC	20				//�����������
#define HEAD_ELE_MAX 5000				//����������
#define HEAD_ELE_MIN 50					//��С�������

#define H_TIME_OUT_US (10 * 1000 * 1000) //�Լ쳬ʱʱ��

#define GET_LIMIT_HEAD_L 			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)	//��ͷ��λ��
#define GET_LIMIT_HEAD_R  			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)	//��ͷ��λ��
#define GET_ENC_HEAD_A 			HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7)	//ͷ��A�������
#define GET_ENC_HEAD_B 			HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6)	//ͷ��B�������
typedef enum	{MOTOR_H_STOP, MOTOR_H_RUN }RUN_STATE_H;		//ͷ������˶�״̬
typedef enum	{SELF_H_ERROR, SELF_H_RUNNING, SELF_H_DONE}SELF_STATE_H;
typedef enum	{LOCATION_UNKNOWN, LOCATION_MIDDLE, LOCATION_LEFT, LOCATION_RIGHT}LOCATION_H;
/****************************************************
	�ṹ����:	MOTOR_HEAD_PARM
	����: ͷ�����ʵ��
	����:	liyao 2015��9��14��12:24:31
****************************************************/
typedef struct _MOTOR_HEAD_PARM MOTOR_HEAD_PARM ;
struct _MOTOR_HEAD_PARM			//����Ĳ���������Ҫ����һ�����õ�
{
	MOTOR_ID	motorID;				//���ID 1-2
	Protocol_Info_T 	Exec_Protocol;//����ִ�е�Э��
	uint8_t   BoardVersion;		//���̰汾
	int16_t	  Speed;					//��ǰ�ٶ�
	int16_t 	SpeedCMD;				//�˴�������λ������δִ�е��ٶ�	0-100 
	uint8_t		MotorDir;				//�����ǰ����
	uint8_t		MotorDirCMD;		//�˴�������λ�������ķ���ָ�1��2
	int16_t 	PWM_Out;				//�����PWM
	uint16_t  Current;				//��ǰ����
	uint16_t  CurrentLast;		//�ϴε���
	uint16_t 	Electricity;		//AD�ɼ�������Ϣ
	uint16_t 	ElectricityLast;//���һ��AD�ɼ�������Ϣ
	uint8_t 	FuseSensor;			//����˿״̬
	
	TIM_TypeDef* 	TIMx;		//��ӦPWM��ʱ��
	GPIO_TypeDef* GPIOx;	//��ӦPWM	IO��
	uint16_t GPIO_PIN;		//��ӦPWM	IO��
	
	PID_T PID_H_Speed;
	//PID_T PID_H_Location;
	int16_t Last_Encoder;
	int16_t Diff_Encoder;
	int16_t Diff_Angle_Enc;
	
	int8_t				Self_TaskID;
	int8_t 			  LimitFlag;
	RUN_STATE_H   RunState;				//�Ƿ�������
	SELF_STATE_H	SelfState;
	float 				Angle_Enc_Code;	//1��Ƕȶ�Ӧ��λ��ֵ�������
	int16_t    		Encoder_Max;	//���Ƕ�
	int16_t    		Encoder_Min;	//��С�Ƕ�
	uint8_t				AngleCMD;				//Ŀ��Ƕ�
	int16_t				Angle_Enc_CMD;	//Ŀ��Ƕȶ�Ӧ�ı�����ֵ
	float					Angle_Cur;			//��ǰ�Ƕ�
	int16_t				Angle_Enc_Cur;	//��ǰ�Ƕȶ�Ӧ��ADֵ
	int16_t  			Protect_Time;//��������
	int16_t 			ProtectCheckEnc;
	
	LOCATION_H location;//����λ�� <120�㻹��>120�� 
	uint16_t 			Head_Enc_Max;//�������
	float					PID_Proportion;//�����ٶȱ���  
	uint16_t			Head_Mid_Enc_L;//���е�
	uint16_t			Head_Mid_Enc_R;//���е�
	
	
	void (*MotorHeadInit)(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id,TIM_TypeDef* TIMx,GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);	//��ʼ������
	void (*ExecuteHeadMotor)(MOTOR_HEAD_PARM* motor);
	void (*StopHeadMotor)(MOTOR_HEAD_PARM* motor);
	void (*MotorProtectCheck)(MOTOR_HEAD_PARM* motor);
	void (*SetHeadMotorParam)(MOTOR_HEAD_PARM* motor, uint16_t speed,uint16_t angle);
	void (*ExecSpeedPWM)(TIM_TypeDef* TIMx, int16_t PWM);
};



extern MOTOR_HEAD_PARM Motor_H;	//ͷ�����


extern void MotorHeadInit(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN);
extern void SetHeadMotorParam(MOTOR_HEAD_PARM* motor, uint16_t speed, uint16_t angle);
extern void Head_Self_Inspection(void);
extern void HeadPidRun(void);
extern void ExecuteHeadMotor(MOTOR_HEAD_PARM* motor);
extern void StopHeadMotor(MOTOR_HEAD_PARM* motor);
extern void ScanHeadMotorLimit(void);
extern int8_t Head_RunLeft(void);
#endif
 
