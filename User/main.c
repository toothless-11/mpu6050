#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "MPU6050.h"
#include "Timer.h"
#include "stm32f10x_tim.h"


int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
//int32_t i = 0;
int main(void)
{
	/*模块初始化*/
	OLED_Init();						//OLED初始化

	MPU6050_Init();		//MPU6050初始化
	Timer_Init();		//Timer
	Serial_Init();						//串口初始化
	
	/*串口基本函数*/
//	Serial_SendByte(0x41);				//串口发送一个字节数据0x41
//	
//	uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};	//定义数组
//	Serial_SendArray(MyArray, 4);		//串口发送一个数组
//	
//	Serial_SendString("\r\nNum1=");		//串口发送字符串
//	
//	Serial_SendNumber(111, 3);			//串口发送数字
	
	
	

	while(1)
	{
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//获取MPU6050的数据
		OLED_ShowSignedNum(2, 1, AX, 5);		//OLED显示5位数据，高位空缺补0
		OLED_ShowSignedNum(3, 1, AY, 5);
		OLED_ShowSignedNum(4, 1, AZ, 5);
		OLED_ShowSignedNum(2, 8, GX, 5);
		OLED_ShowSignedNum(3, 8, GY, 5);
		OLED_ShowSignedNum(4, 8, GZ, 5);
	}
	
}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		//判断是否是TIM2的更新事件触发的中断
	{
		Serial_SendByte('B');
		Serial_SendSignedNum(AX);
		Serial_SendSignedNum(AY);
		Serial_SendSignedNum(AZ);
		Serial_SendSignedNum(GX);
		Serial_SendSignedNum(GY);
		Serial_SendSignedNum(GZ);
		Serial_SendByte('A');
		//验证发送速度
//		Serial_SendByte('A');
//		Serial_SendSignedNum(i++);
//		Serial_SendByte('B');
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除TIM2更新事件的中断标志位
															//中断标志位必须清除
															//否则中断将连续不断地触发，导致主程序卡死
	}
}