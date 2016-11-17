#include <stdio.h>
#include <string.h>  
#include "MotorHeadDriver.h" 
#include "TaskTimeManager.h"
#include "tim.h"
static void Motor_HProtectCheck(MOTOR_HEAD_PARM* motor);
static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
/****************************************************
������:MotorHeadInit
����: ͷ�����ʵ���ʼ��
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
void MotorHeadInit(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN){
	memset(motor, 0,sizeof(MOTOR_HEAD_PARM)); 
	motor->TIMx = TIMx;
	motor->GPIOx = GPIOx;
	motor->GPIO_PIN = GPIO_PIN;
	//motor->Angle_Enc_Code = HEAD_ENC_MAX * 1.0 / HEAD_MAX_ANGLE;
	//motor->Encoder_Max = HEAD_ENC_MAX; 
	motor->Encoder_Min = MID_OFFSET; 
	motor->SelfState = SELF_H_ERROR;  
	
	Motor_H.RunState = MOTOR_H_STOP;							//��ֵͷ����ʼ״̬
	motor->ExecSpeedPWM = ExecSpeedPWM;						//��ֵPWM���ú���
	motor->ExecuteHeadMotor = ExecuteHeadMotor;		//��ִֵ�к���
	motor->SetHeadMotorParam = SetHeadMotorParam;	//��ֵ�������ú���
	motor->StopHeadMotor = StopHeadMotor;					//��ֵֹͣ����
	motor->MotorProtectCheck = Motor_HProtectCheck; //��ֵ�����������
	
	PID_Init(&motor->PID_H_Speed, 40, 1, 0, 1, 2400);
}

void ErrorDefault(void){
	if(Motor_H.BoardVersion == 41){
			Motor_H.Head_Enc_Max = HEAD_41ENC_MAX; 
			Motor_H.PID_Proportion = PID_41PROPORTION;
		}else if(Motor_H.BoardVersion == 45){
			Motor_H.Head_Enc_Max = HEAD_45ENC_MAX;
			Motor_H.PID_Proportion = PID_45PROPORTION;
		} 
    Motor_H.Angle_Enc_Code = Motor_H.Head_Enc_Max * 1.0 / HEAD_MAX_ANGLE;
    Motor_H.Encoder_Max = Motor_H.Head_Enc_Max; 
}

/****************************************************
������:ExecSpeedPWM
����: �������PWM
����:	��Ң 2015��10��14��22:26:22
****************************************************/	 
void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM){
	if(PWM > HEAD_MAX_PWM) PWM = HEAD_MAX_PWM;
	if(PWM < 0) PWM = 0;
	//TIM_SetCompare1(TIMx, PWM);
  TIMx->CCR3 = PWM;
}

/****************************************************
������:ExecuteHeadMotor
����: ִ�е������
����:	��Ң 2015��10��14��22:26:22
****************************************************/	 
void ExecuteHeadMotor(MOTOR_HEAD_PARM* motor){
//	if(motor->Diff_Angle_Enc < (motor->Angle_Enc_Code * 5)){ 
//		motor->StopHeadMotor(motor);			//���ֹͣ
//		return;
//	}
	motor->RunState = MOTOR_H_RUN;
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_PIN, (GPIO_PinState)motor->MotorDirCMD);
	motor->Speed = motor->SpeedCMD;
	motor->PWM_Out = 0;
	motor->Last_Encoder = -1;
	motor->PID_H_Speed.PID_Reset(&motor->PID_H_Speed);
}

/****************************************************
������:SetHeadMotorParam
����: ���õ������
����:	��Ң 2015��10��14��22:26:22
****************************************************/
 void SetHeadMotorParam(MOTOR_HEAD_PARM* motor, uint16_t speed,uint16_t angle){
	if(motor->SelfState == SELF_H_DONE){//�Լ�ģʽ�����нǶ�У�飬uint���Ͳ���У��С��0
		angle = angle > HEAD_MAX_ANGLE?HEAD_MAX_ANGLE:angle;
	} 
	
	int16_t Diff_Angle = (int16_t)(motor->Angle_Cur - angle);
	if(Diff_Angle < 0) Diff_Angle = -Diff_Angle;
	//�ٶ�����
	if(Diff_Angle < 15 && speed > 40){
		speed = 40;
	}
	
	motor->SpeedCMD = (int16_t)(speed / motor->PID_Proportion);//speed / PID_PROPORTION;
	
	if(speed > 0 && speed < motor->PID_Proportion)//��0��С�ٶ�
		motor->SpeedCMD = 1;
	motor->AngleCMD = angle;
	if(angle == 120 &&  Motor_H.BoardVersion == 45){//�е㲻ͨ������ֱ�Ӷ�ֵ
		if(motor->location == LOCATION_RIGHT)
			motor->Angle_Enc_CMD = Motor_H.Head_Mid_Enc_R;
		else if(motor->location == LOCATION_LEFT)
			motor->Angle_Enc_CMD = Motor_H.Head_Mid_Enc_L;
	} else{ 
		motor->Angle_Enc_CMD = (int16_t)(angle * motor->Angle_Enc_Code);
	}
	motor->Diff_Angle_Enc = motor->Angle_Enc_CMD - motor->Angle_Enc_Cur;
 
	if(motor->Diff_Angle_Enc < 0)
		motor->Diff_Angle_Enc = -motor->Diff_Angle_Enc;
	if(motor->Angle_Cur > motor->AngleCMD){
		Motor_H.MotorDirCMD = DIR_MOTOR_HEAD_L;
	}else if(motor->Angle_Cur < motor->AngleCMD){
		Motor_H.MotorDirCMD = DIR_MOTOR_HEAD_R;
	}
}

/****************************************************
������:StopHeadMotor
����: ���ֹͣ
����:	��Ң 2015��10��14��22:26:22
****************************************************/
void StopHeadMotor(MOTOR_HEAD_PARM* motor){
	motor->ExecSpeedPWM(motor->TIMx, 0);
	motor->PID_H_Speed.PID_Reset(&motor->PID_H_Speed);
	motor->SpeedCMD = 0;
	motor->Last_Encoder = -1;
	if(motor->MotorDirCMD == DIR_MOTOR_HEAD_L || motor->MotorDirCMD == DIR_MOTOR_HEAD_R)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_PIN, (GPIO_PinState)!motor->MotorDirCMD);
	motor->MotorDirCMD = DIR_MOTOR_HEAD_NONE;
	motor->RunState = MOTOR_H_STOP;
}

/****************************************************
������:MotorProtectCheck
����: ����������
����:	��Ң 2015��10��14��22:26:22
****************************************************/ 
void Motor_HProtectCheck(MOTOR_HEAD_PARM* motor){
	if(motor->RunState == MOTOR_H_STOP){		//�˶�״̬Ϊֹͣ
		motor->ProtectCheckEnc = motor->Protect_Time = 0;
	}else{
		if(motor->Protect_Time++ == 30){		//�������ۼӵ�100
			motor->Protect_Time = 0;					//��ʼ������������ 
			if(motor->ProtectCheckEnc == motor->Angle_Enc_Cur){
				motor->StopHeadMotor(motor);			//���ֹͣ
				#ifdef PRINT_ERR 
				printf("����ͷ���������\r\n");
				#endif
			}
			motor->ProtectCheckEnc = motor->Angle_Enc_Cur;
		}
	}
}
/****************************************************
������:HeadPidRun
����: ͨ��pid�㷨����PWM
����:	��Ң 2015��10��14��22:26:22
****************************************************/
void HeadPidRun(void){
	if(Motor_H.RunState == MOTOR_H_RUN && Motor_H.SpeedCMD == 0) 
		Motor_H.StopHeadMotor(&Motor_H);
	if(Motor_H.RunState == MOTOR_H_STOP)
		return; 
	if(Motor_H.Last_Encoder == -1){
		Motor_H.Last_Encoder = Motor_H.Angle_Enc_Cur;
		return;
	}
	//��λʱ���ڱ���������
	Motor_H.Diff_Encoder = Motor_H.Angle_Enc_Cur - Motor_H.Last_Encoder;
	if(Motor_H.Diff_Encoder < 0)
		Motor_H.Diff_Encoder = -Motor_H.Diff_Encoder;
	Motor_H.Last_Encoder = Motor_H.Angle_Enc_Cur;
	
	//�ǶȲ�ֵ����
	int16_t Diff_Angle_Enc =  Motor_H.Angle_Enc_CMD - Motor_H.Angle_Enc_Cur;
//	
//	//����Ƿ�ﵽĿ��λ��
//	if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L && Diff_Angle_Enc >=0){
//		Motor_H.StopHeadMotor(&Motor_H);			//���ֹͣ
//		return;
//	}else if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R && Diff_Angle_Enc <=MID_OFFSET){
//		Motor_H.StopHeadMotor(&Motor_H);			//���ֹͣ
//		return;
//	}
	#ifdef PRINT_ERR 
		//printf("Ŀ��ENC:%d,��ǰENC:%d\r\n", Motor_H.Angle_Enc_CMD, Motor_H.Angle_Enc_Cur);
	#endif
	//û�дﵽĿ��λ��
	if(Diff_Angle_Enc < 0)
		Diff_Angle_Enc = -Diff_Angle_Enc; 
	
//�ٶ�Ϊ100ʱ ��ǰ70���뿪ʼ���Լ��ٱȽϺ���  �ʴ��ٶ�*0.7���뿪ʼ����
	if(Diff_Angle_Enc < (Motor_H.SpeedCMD*3) )
			Motor_H.Speed = (int16_t)(Motor_H.SpeedCMD * (Diff_Angle_Enc/(Motor_H.SpeedCMD*3.0)));
	
	if(Motor_H.Speed < 1){ 
		Motor_H.Speed = 1; 
	}
		
	//pid����ִ��
	PID_T* pid_s = &Motor_H.PID_H_Speed; 
	pid_s->PID_Calculate(pid_s,Motor_H.Diff_Encoder,Motor_H.Speed);

	Motor_H.PWM_Out += pid_s->Res;
	if(Motor_H.PWM_Out > HEAD_MAX_PWM)
		Motor_H.PWM_Out = HEAD_MAX_PWM;
	ExecSpeedPWM(Motor_H.TIMx, Motor_H.PWM_Out);
}
/****************************************************
������:ScanHeadMotorLimit
����: ɨ��ͷ����λ���
����:	��Ң 2015��10��14��22:26:22
****************************************************/
void ScanHeadMotorLimit(void){
	if(System_Mode == Normal){
		Motor_H.MotorProtectCheck(&Motor_H);
	}  
	int16_t Diff_Angle_Enc =  Motor_H.Angle_Enc_CMD - Motor_H.Angle_Enc_Cur; 
	if(System_Mode == Normal && GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE && GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE){
		if(Motor_H.BoardVersion == 45){
			Motor_H.Angle_Enc_Cur = HEAD_45MID_ENC;                 //liyao20160906 
			;
		}else if(Motor_H.BoardVersion == 41){
			Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Max/2;                  //liyao20160906
		} 
	}
	//����Ƿ�ﵽĿ��λ��
	if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L && Diff_Angle_Enc >=0){
		Motor_H.StopHeadMotor(&Motor_H);			//���ֹͣ
		#ifdef PRINT_ERR 
			printf("cur:%d,%f.2,B%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_B,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
		#endif
		return;
	}else if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R && Diff_Angle_Enc <=MID_OFFSET){
		Motor_H.StopHeadMotor(&Motor_H);			//���ֹͣ
		#ifdef PRINT_ERR 
			printf("cur:%d,%f.2,B%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_B,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
		#endif
		return;
	}  
	if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE && Motor_H.Angle_Enc_Cur < Motor_H.Encoder_Max*0.2){ //������λ��Ϊ0
		Motor_H.LimitFlag |= (1<<1);//��λ��1
		Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Min;
		if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L){//������������
		Motor_H.StopHeadMotor(&Motor_H);//���ֹͣ
		#ifdef PRINT_ERR 
			printf("ͷ������λ\r\n");
		#endif
		}
	}else{
		Motor_H.LimitFlag &= ~(1<<1);//��λ��0
	}
	
	if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE && Motor_H.Angle_Enc_Cur > Motor_H.Encoder_Max*0.8){	//������λ��Ϊ0
		Motor_H.LimitFlag |= (1<<0);//��λ��1
		Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Max;
		if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R ){//������������
		Motor_H.StopHeadMotor(&Motor_H);
		#ifdef PRINT_ERR 
			printf("ͷ������λ\r\n");
		#endif
		}
	}else{
		Motor_H.LimitFlag &= ~(1<<0);//��λ��0
	}
  
  
  
//  if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE){
//			printf("ͷ������λ\r\n"); 
//  }
//  if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE){
//			printf("ͷ������λ\r\n"); 
//  }
}
/****************************************************
������:Head_RunLeft
����: ͷ����ת�����λ
****************************************************/	
int8_t Head_RunLeft(){
	MOTOR_HEAD_PARM* motor_H = &Motor_H;
	uint16_t timeout = 0;
//������λ
	HAL_GPIO_WritePin(motor_H->GPIOx, motor_H->GPIO_PIN, (GPIO_PinState)DIR_MOTOR_HEAD_L);
	ExecSpeedPWM(motor_H->TIMx, HEAD_INIT_PWM);
	motor_H->Last_Encoder = motor_H->Angle_Enc_Cur = 1000;
	DelayMS(500);
	while(GET_LIMIT_HEAD_L == HEAD_LIMIT_FALSE ){
		DelayMS(10);
		if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE)
			DelayMS(1000);
		if(timeout++ == 100){
			timeout = 0;
			motor_H->Diff_Encoder = motor_H->Angle_Enc_Cur - motor_H->Last_Encoder;
			motor_H->Last_Encoder = motor_H->Angle_Enc_Cur;
			if(GET_LIMIT_HEAD_L == HEAD_LIMIT_FALSE &&  motor_H->Diff_Encoder == 0){//�����Լ�
				motor_H->StopHeadMotor(motor_H);
				#ifdef PRINT_ERR  
				printf("��ʱ\r\n");
				#endif
				return -1;
			}
		}
	} //�ȴ���������λ
	motor_H->Angle_Enc_Cur = 0;
	ExecSpeedPWM(motor_H->TIMx, 0);
	return 0;
}
/****************************************************
������:Head_Self_Inspection
����: ִ��ͷ�������ʼ��
****************************************************/	 
void Head_Self_Inspection(){
	MOTOR_HEAD_PARM* motor_H = &Motor_H;
	uint16_t timeout = 0;
	#ifdef PRINT_ERR  
		printf("ͷ����ʼ����ʼ\r\n");
	#endif
	if(Head_RunLeft() == -1)
		return; 
	DelayMS(500);
	#ifdef PRINT_ERR
		printf("ͷ�����λ���\r\n");
	#endif
	//����ҹ�λ����
	HAL_GPIO_WritePin(motor_H->GPIOx, motor_H->GPIO_PIN, (GPIO_PinState)DIR_MOTOR_HEAD_R);
	ExecSpeedPWM(motor_H->TIMx, HEAD_INIT_PWM);
	motor_H->Last_Encoder = motor_H->Angle_Enc_Cur = 0;
	DelayMS(500);
	while(GET_LIMIT_HEAD_R == HEAD_LIMIT_FALSE ){
		DelayMS(10);
		if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE)
			DelayMS(1000);
		if(timeout++ == 100){
			timeout = 0;
			motor_H->Diff_Encoder = motor_H->Angle_Enc_Cur - motor_H->Last_Encoder;
			motor_H->Last_Encoder = motor_H->Angle_Enc_Cur;
			if(GET_LIMIT_HEAD_R == HEAD_LIMIT_FALSE && motor_H->Diff_Encoder == 0){//�����Լ�
				motor_H->StopHeadMotor(motor_H);
				#ifdef PRINT_ERR  
				printf("�ҳ�ʱ\r\n");
				#endif
				return;
			}
		}
		//��ȡ��¼ͷ������˶�����
		ReadCurrent();
		motor_H->ElectricityLast = motor_H->Electricity;
	}//�ȴ���������λ0
	#ifdef PRINT_ERR 
		printf("ͷ���ҹ�λ���\r\n");
	#endif
	motor_H->Encoder_Min = MID_OFFSET;
	motor_H->Encoder_Max = motor_H->Angle_Enc_Cur;//motor_H->Angle_Enc_Cur;	//��¼�������
	if(motor_H->Encoder_Max > 300){
		motor_H->BoardVersion = 41;
		motor_H->Head_Enc_Max = HEAD_41ENC_MAX;
		motor_H->Head_Mid_Enc_L = HEAD_41MID_ENC_L;
		motor_H->Head_Mid_Enc_R = HEAD_41MID_ENC_R;
		motor_H->PID_Proportion = PID_41PROPORTION;
	}else{
		motor_H->BoardVersion = 45;
		motor_H->Head_Enc_Max = HEAD_45ENC_MAX;
		motor_H->Head_Mid_Enc_L = HEAD_45MID_ENC_L;
		motor_H->Head_Mid_Enc_R = HEAD_45MID_ENC_R;
		motor_H->PID_Proportion = PID_45PROPORTION;
	}
	//����1���Ӧ�ı�����ֵ
	motor_H->Angle_Enc_Code = motor_H->Encoder_Max * 1.0 / HEAD_MAX_ANGLE;
	motor_H->StopHeadMotor(motor_H);
	#ifdef PRINT_ERR 
		printf("��ʼ����ϣEEnc_Max:%d-----Enc_Min:%d-----Angle_Code:%f\r\n",
		motor_H->Encoder_Max,motor_H->Encoder_Min,motor_H->Angle_Enc_Code);
	#endif
		if(
		 motor_H->Angle_Enc_Cur == 0  || 
		 motor_H->Angle_Enc_Code == 0 ||
		 motor_H->Encoder_Max == 0 
	){
		#ifdef PRINT_ERR 
		printf("�Լ�ʧ��return -1 %f %d %f %d \r\n",motor_H->Angle_Cur,motor_H->Angle_Enc_Cur,
		motor_H->Angle_Enc_Code,motor_H->Encoder_Max);
		#endif
		ErrorDefault();
		return;
	}
	ExecSpeedPWM(motor_H->TIMx, 0);
	DelayMS(500);
	motor_H->SelfState = SELF_H_DONE; 
	motor_H->Angle_Cur = Motor_H.Angle_Enc_Cur / Motor_H.Angle_Enc_Code;
	motor_H->SetHeadMotorParam(motor_H, 50, 120);
	motor_H->ExecuteHeadMotor(motor_H);
}



















