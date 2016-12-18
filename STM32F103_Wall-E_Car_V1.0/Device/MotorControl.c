#include "MotorControl.h"
#include "tim.h"
Motor_T Motor_L,Motor_R;
Motor_T Motor_L_Bak,Motor_R_Bak;
Motor_T *motor_L = &Motor_L,*motor_R = &Motor_R;

uint16_t _Motor_Get_Encoder(Motor_T* motor);

void _Motor_Run(Motor_T* motor, MOTOR_DIR dir, uint16_t speed);
void _Motor_Custom(Motor_T* motor, MOTOR_DIR dir, uint16_t speed, int16_t Encoder);
uint16_t _Motor_Set_Pwm(Motor_T* motor, uint16_t Pwm);
static void _Motor_PID_Control(Motor_T* motor); 
/********************************************************************************************************
*  Function Name  : Motor_Init					                                                           
*  Object					: ���������ʼ��
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��11��11:06:17                     
********************************************************************************************************/
void Motor_Init(void){
	memset(motor_L, 0, sizeof(Motor_T));
	memset(motor_R, 0, sizeof(Motor_T));
	motor_L->Id 						= MOTOR_L;
	motor_L->ENC_TIMx 					= &htim3;
	motor_L->PWM_TIMx 					= &htim2;
	motor_L->TIM_Channel 		= TIM_CHANNEL_1;
	motor_L->GPIOx 			= GPIOB;
	motor_L->INA_GPIO_Pinx 	= GPIO_PIN_0;
	//motor_L->INB_GPIO_Pinx 	= GPIO_PIN_1;
	motor_L->IsOpposite_ENC = 1;	//�����������෴
	//PID_Init(&motor_L->PID_Speed, 10, 6, 2, 1, 1000);
	//PID_Init(&motor_L->PID_Speed, 25, 10, 0, 1, 1000);//p:35-40 
	PID_Init(&motor_L->PID_Speed, 10, 6, 2, 0, 1000);
	PID_Init(&motor_L->PID_Location, 0.5, 0, 0, 1, 100);
	
	motor_R->Id 						= MOTOR_R;
	motor_R->ENC_TIMx 					= &htim4;
	motor_R->PWM_TIMx 					= &htim2;
	motor_R->TIM_Channel 		= TIM_CHANNEL_2;
	motor_R->GPIOx 			= GPIOB;
	motor_R->INA_GPIO_Pinx 	= GPIO_PIN_1;
	//motor_R->INB_GPIO_Pinx 	= GPIO_PIN_3;
	motor_R->IsOpposite_DIR			= 1;						//ת���෴
	PID_Init(&motor_R->PID_Speed, 10, 6, 2, 0, 1000);
	//PID_Init(&motor_R->PID_Speed, 25, 10, 0, 1, 1000);
	PID_Init(&motor_R->PID_Location, 0.5, 0, 0, 1, 100);
	

	motor_L->Encoder_Cur = motor_R->Encoder_Cur = INIT_ENC;
	motor_L->Encoder_Last = motor_R->Encoder_Last = INIT_ENC;
	motor_L->Speed_Enc = motor_R->Speed_Enc = MAX_ENC / 100;
	motor_L->Motor_Run = motor_R->Motor_Run = _Motor_Run;
	motor_L->Motor_Get_Encoder = motor_R->Motor_Get_Encoder = _Motor_Get_Encoder;
	motor_L->Motor_Custom = motor_R->Motor_Custom = _Motor_Custom;
	motor_L->Motor_Set_Pwm = motor_R->Motor_Set_Pwm = _Motor_Set_Pwm;
	motor_L->Enc_Angle_Code = motor_R->Enc_Angle_Code = RING_ENC/360.0;				//������Ӧ����ֵ
	
	__HAL_TIM_SET_COUNTER(motor_L->ENC_TIMx, INIT_ENC);//���ñ�����ΪĬ��ֵ
	__HAL_TIM_SET_COUNTER(motor_R->ENC_TIMx, INIT_ENC);
	
	__HAL_TIM_CLEAR_IT(motor_L->ENC_TIMx, TIM_IT_UPDATE);//�����ʱ������ж�
	__HAL_TIM_CLEAR_IT(motor_R->ENC_TIMx, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(motor_L->ENC_TIMx);		//������ʱ������ж�
	HAL_TIM_Base_Start_IT(motor_R->ENC_TIMx);
	HAL_TIM_Encoder_Start(motor_L->ENC_TIMx, TIM_CHANNEL_ALL);//������ʱ������ɼ�
	HAL_TIM_Encoder_Start(motor_R->ENC_TIMx, TIM_CHANNEL_ALL); 
	HAL_TIM_PWM_Start(motor_L->PWM_TIMx, motor_L->TIM_Channel); //������ʱ��PWM���
	HAL_TIM_PWM_Start(motor_R->PWM_TIMx, motor_R->TIM_Channel);
}

/********************************************************************************************************
*  Function Name  : _Motor_PID_Clear					                                                           
*  Object					: ���PID����ֵ
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��11��14:10:47                      
********************************************************************************************************/
void _Motor_PID_Clear(Motor_T* motor){
	motor->PID_Location.PID_Reset(&motor->PID_Location);
	motor->PID_Speed.PID_Reset(&motor->PID_Speed);
	motor->Encoder_Cur = INIT_ENC;
	motor->Encoder_Last = INIT_ENC;
	__HAL_TIM_SET_COUNTER(motor->ENC_TIMx, INIT_ENC);
	//TIM_SetCounter(motor_R->ENC_TIMx,INIT_ENC);
	/*motor->Encoder_Cur = INIT_ENC;
	motor->Encoder_Last = INIT_ENC;*/
}


/********************************************************************************************************
*  Function Name  : _Motor_Set_Dir					                                                           
*  Object					: ���õ���˶�����
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��11��14:10:47                      
********************************************************************************************************/
void _Motor_Set_Dir(Motor_T* motor, MOTOR_DIR dir){
	uint8_t direnc,direnc_N = 0;
	if(MOTOR_UP == dir){
		direnc = 0x00;
		direnc_N = 0x01;
	}else if(MOTOR_DOWN == dir){
		direnc = 0x01;
		direnc_N = 0x00;
	}
//	else if(MOTOR_STOP == dir){
//		direnc = ~motor->DirCMD; 
//		direnc_N = motor->DirCMD;
//	}
	motor->DirCMD = dir;
	
	//�������
	if(motor->IsOpposite_DIR){//����Ƿ���
		HAL_GPIO_WritePin(motor->GPIOx, motor->INA_GPIO_Pinx, (GPIO_PinState)direnc_N);
		//GPIO_WriteBit(motor->GPIOx, motor->INB_GPIO_Pinx, (BitAction)(dir & 0x02));
	}else{//����
		HAL_GPIO_WritePin(motor->GPIOx, motor->INA_GPIO_Pinx, (GPIO_PinState)direnc);
		//GPIO_WriteBit(motor->GPIOx, motor->INB_GPIO_Pinx, (BitAction)(dir & 0x01));
	}
	if(motor->DirCMD == MOTOR_STOP){ 
		__HAL_TIM_SET_COMPARE(motor->PWM_TIMx, motor->TIM_Channel, 0);  
	}
		
}

void Motor_Turn_Dir(Motor_T* motor){
	if(motor->Dir == MOTOR_UP)
			_Motor_Set_Dir(motor, MOTOR_DOWN);
	else if(motor->Dir == MOTOR_DOWN)
			_Motor_Set_Dir(motor, MOTOR_UP);
		
}
/********************************************************************************************************
*  Function Name  : _Motor_Set_Pwm				                                                           
*  Object					: ���õ���ٶ�
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��5�� 12:43:21                   
********************************************************************************************************/
uint16_t _Motor_Set_Pwm(Motor_T* motor, uint16_t Pwm){
	if(Pwm > MAX_PWM)
		Pwm = MAX_PWM;
	motor->TIM_PWM = Pwm;
	__HAL_TIM_SET_COMPARE(motor->PWM_TIMx, motor->TIM_Channel, Pwm);
	return Pwm;
}

/********************************************************************************************************
*  Function Name  : _Motor_Run					                                                           
*  Object					: ���ֱ���˶�
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��11��7��12:55:01                       
********************************************************************************************************/
void _Motor_Run(Motor_T* motor, MOTOR_DIR dir, uint16_t speed){
	motor->Cmd_Type = STRAIGHT;
	speed /= 2;
	
	//�л�����ʱ���pid
	if(dir != motor->DirCMD)
		motor->PID_Speed.PID_Reset(&motor->PID_Speed);
	//���������б�
	if(dir == MOTOR_UP)
		motor->SpeedCMD = speed;
	else if(dir == MOTOR_DOWN){
		motor->SpeedCMD = -speed;
	}else if(dir == MOTOR_STOP){
		motor->SpeedCMD = 0;
	}
	motor->DirCMD = dir; 
}

/********************************************************************************************************
*  Function Name  : _Motor_Custom				                                                           
*  Object					: ���ָ���˶�
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��11��11:06:17                     
********************************************************************************************************/
void _Motor_Custom(Motor_T* motor, MOTOR_DIR dir, uint16_t speed, int16_t Encoder){
	motor->Cmd_Type = CUSTOM;
	_Motor_Set_Dir(motor, dir);
	motor->SpeedCMD = speed;
	_Motor_PID_Clear(motor);
	//motor->Encoder_CMD_Diff = Encoder;
	if(dir == MOTOR_UP)
		motor->Encoder_CMD = INIT_ENC + Encoder;
	else if(dir == MOTOR_DOWN)
		motor->Encoder_CMD = INIT_ENC - Encoder;
}


/********************************************************************************************************
*  Function Name  : _Motor_Get_Encoder					                                                           
*  Object					: ��ȡ���������ֵ
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��5�� 12:43:21                   
********************************************************************************************************/
uint16_t _Motor_Get_Encoder(Motor_T* motor){
	motor->Encoder_Cur = __HAL_TIM_GET_COUNTER(motor->ENC_TIMx);
	if(motor->IsOpposite_ENC) 
		return INIT_ENC + (INIT_ENC - motor->Encoder_Cur);
	else
		return motor->Encoder_Cur;
}

/********************************************************************************************************
*  Function Name  : Motor_Get_DiffEncoder					                                                           
*  Object					: ��ȡ�����������ֵ
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��5�� 12:43:21                   
********************************************************************************************************/
int32_t Motor_Get_DiffEncoder(Motor_T* motor){
	motor->Encoder_Cur = motor->Motor_Get_Encoder(motor); //���±�����ֵ
	//------------------------��������ֵ���㣨�ٶȷ�����-------------------------
	if(motor->Overflow_Flag){		//�ж϶�ʱ���������
		motor->Overflow_Flag = 0;	
		if(motor->Encoder_Last > INIT_ENC){		//������� �ϴε�ֵ��32767~65535֮��
			motor->Encoder_Diff = 0xFFFF - motor->Encoder_Last + motor->Encoder_Cur;
		}else if(motor->Encoder_Last < INIT_ENC){//������� �ϴε�ֵ��0~32767֮��
			motor->Encoder_Diff = -(0xFFFF - motor->Encoder_Cur + motor->Encoder_Last);
		}else{															
			motor->Encoder_Diff = 0;
		}
	}else{
		motor->Encoder_Diff = (int32_t)motor->Encoder_Cur - (int32_t)motor->Encoder_Last;
	}
	motor->Encoder_Last = motor->Encoder_Cur;
	return motor->Encoder_Diff;
}


/********************************************************************************************************
*  Function Name  : Motor_Location_Calculate					                                                           
*  Object					: ���λ�û�
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2016��12��17��15:30:34
********************************************************************************************************/
void Motor_Location_Calculate(float SetValue){
	PID_T  *pid_l;
	//------------------------λ��ƫ��PID����  ����׷------------------------- 
	
	motor_L->Location_Diff = motor_L->Encoder_Cur - motor_R->Encoder_Cur;
	pid_l = &motor_L->PID_Location;
	pid_l->PID_Calculate(pid_l, motor_L->Location_Diff , SetValue);
	motor_L->PID_location_Res = (int16_t)pid_l->Res;
	
	motor_R->Location_Diff = motor_R->Encoder_Cur - motor_L->Encoder_Cur; 
	pid_l = &motor_R->PID_Location;
	pid_l->PID_Calculate(pid_l, motor_R->Location_Diff , SetValue);
	motor_R->PID_location_Res = (int16_t)pid_l->Res;
	
	
	
//	if(motor_L->Encoder_Cur > motor_R->Encoder_Cur ){
//		pid_l = &motor_R->PID_Location;
//		motor_R->Location_Diff = motor_L->Encoder_Cur - motor_R->Encoder_Cur;
//		pid_l->PID_Calculate(pid_l, -motor_R->Location_Diff , SetValue);
//		motor_R->PID_location_Res = (int16_t)pid_l->Res;
//	}else if(motor_R->Encoder_Cur > motor_L->Encoder_Cur){
//		pid_l = &motor_L->PID_Location;
//		motor_L->Location_Diff = motor_R->Encoder_Cur - motor_L->Encoder_Cur ;
//		pid_l->PID_Calculate(pid_l, -motor_L->Location_Diff , SetValue);
//		motor_L->PID_location_Res = (int16_t)pid_l->Res;
//	}  
}

/********************************************************************************************************
*  Function Name  : Motor_Speed_Calculate					                                                           
*  Object					: ����ٶȻ�
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��1�� 22:03:50 25ms���������150
********************************************************************************************************/
float Motor_Speed_Calculate(Motor_T* motor,float Speed){
	PID_T  *pid_s = &motor->PID_Speed;
	pid_s->PID_Calculate(pid_s, motor->Encoder_Diff,Speed);
	if(Speed == 0){  
		return 0;
	}
	return pid_s->Res + PWM_BASE;
}

/********************************************************************************************************
*  Function Name  : Motor_PID					                                                           
*  Object					: ����������PID����
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��1�� 22:03:50                      
********************************************************************************************************/
uint8_t isPause = 0;
void Motor_PID(void){
	//printf("Motor_L��\r\n");
	motor_L->Encoder_Diff = Motor_Get_DiffEncoder(motor_L);
	motor_R->Encoder_Diff = Motor_Get_DiffEncoder(motor_R);
	
	//printf("32\t%d\t%d\r\n",motor_L->Encoder_Diff,motor_R->Encoder_Diff);
	//printf("%d",motor->Encoder_Diff);//, motor->ENC_TIMx->Instance->CNT); //motor->ENC_TIMx->Instance->CNT);//
	if(motor_L->DirCMD == motor_R->DirCMD && motor_L->SpeedCMD == motor_R->SpeedCMD){
		Motor_Location_Calculate(0);
	}
//	else{
//		motor_L->PID_Location.PID_Reset(&motor_L->PID_Location);
//		motor_R->PID_Location.PID_Reset(&motor_R->PID_Location);
//	}
	
	_Motor_PID_Control(&Motor_L);
	_Motor_PID_Control(&Motor_R);  
}

 
/*------------------Callback--------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == motor_L->ENC_TIMx){
		motor_L->Overflow_Flag = 1;
		//printf("LENC\r\n");
	}else if(htim == motor_R->ENC_TIMx){
		motor_R->Overflow_Flag = 1;
		//printf("RENC\r\n");
	}
}
 
	
/********************************************************************************************************
*  Function Name  : Motor_PID_Control					                                                           
*  Object					: ���PID����
*  ����						�� ��
*  ���						�� ��								                         	                                     
*  ��ע						�� ��Ң 2015��12��1�� 22:03:50 25ms���������150
********************************************************************************************************/
static void _Motor_PID_Control(Motor_T* motor){
	int16_t PWM_Res = 0;
	//------------------------����˶�������-------------------------	
	if(motor->Encoder_Diff > 0){ 			
		motor->Dir = MOTOR_UP;
		//printf("up\r\n");
	}else if(motor->Encoder_Diff < 0){	
		motor->Dir = MOTOR_DOWN;  
		//printf("down\r\n");
	}else{ 
		motor->Dir = MOTOR_STOP; 
		//printf("stop\r\n");
	}
	//------------------------PID����-------------------------	
	if(motor->Cmd_Type == STRAIGHT){	//ֱ���˶�����
		PWM_Res = Motor_Speed_Calculate(motor, motor->SpeedCMD + motor->PID_location_Res);
	}else if(motor->Cmd_Type == CUSTOM){//ԭ��ת�����
		if(abs(motor->Encoder_CMD - motor->Encoder_Cur) < 500){//���ٲ���
			motor->Speed = abs(motor->Encoder_CMD - motor->Encoder_Cur)/500.0 * motor->SpeedCMD;
			if(motor->Speed < 10) motor->Speed = 10;
		}else{
			motor->Speed = motor->SpeedCMD;
		}
		if(motor->Encoder_CMD < INIT_ENC){//��ת
			if(motor->Encoder_CMD < motor->Encoder_Cur)//��תδ��λ
				PWM_Res = Motor_Speed_Calculate(motor, (motor->Speed * motor->Speed_Enc));
			else		//��״��λ
			{motor->Motor_Run(motor, MOTOR_STOP, 0); motor->Cmd_Type = NONE;}//NONE�����ж�ָ��ִ�����
		}else{														//ǰת
			if(motor->Encoder_CMD > motor->Encoder_Cur)//ǰתδ��λ
				PWM_Res = Motor_Speed_Calculate(motor, (motor->Speed * motor->Speed_Enc));
			else														//ǰת��λ
			{motor->Motor_Run(motor, MOTOR_STOP, 0); motor->Cmd_Type = NONE;}//NONE�����ж�ָ��ִ�����
		}
	}	
	
	
	if(PWM_Res > 0 && motor->DirCMD == MOTOR_UP){
		_Motor_Set_Dir(motor, MOTOR_UP);
	  _Motor_Set_Pwm(motor, PWM_Res);
	}else if(PWM_Res < 0 && motor->DirCMD == MOTOR_DOWN){
		_Motor_Set_Dir(motor, MOTOR_DOWN);
		_Motor_Set_Pwm(motor, -PWM_Res);
	}else if(motor->DirCMD == MOTOR_BRAKE){
		Motor_Turn_Dir(motor);
		_Motor_Set_Pwm(motor, 0); 
		motor->DirCMD = MOTOR_STOP; 
		printf("��תת��\r\n"); 
	}else if(motor->DirCMD == MOTOR_STOP){
		_Motor_Set_Pwm(motor, 0); 
		_Motor_PID_Clear(motor);
	}else{ 
		_Motor_Set_Pwm(motor, 0);
		_Motor_PID_Clear(motor);
	}
	motor->PID_Speed_Res = PWM_Res;
//	printf("locationRes:%d speed target:%d��PWM_Res��%d\r\n", motor->PID_location_Res, motor->SpeedCMD + motor->PID_location_Res,PWM_Res); 
//	printf("L_ENC:%d, R_ENC:%d\r\n", motor_L->Encoder_Diff, motor_R->Encoder_Diff);
	//�ٶȿ��� 
	
}



