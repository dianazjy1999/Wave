
#include<SPI.h>                       //调用SPI总线库
#include <Wire.h>
#include "paj7620.h"                  //需要调用的传感器库文件

#define GES_REACTION_TIME		500				// 定义映射实际动作的反应
#define GES_ENTRY_TIME			800				// 定义动作读入时间，最长为800ms，即800ms内一定会作出反应
#define GES_QUIT_TIME			1000        //给出一定的动作响应出血线，即时间容差，用于细致地区分不同的动作，减少不规范的动作造成的误识别

void setup()
{

  SPI.begin();                            //开始调用SPI总线
  SPI.setClockDivider(SPI_CLOCK_DIV8);    //S设置时钟分频为 8 (16/8=2Mhz)
  digitalWrite(SS,HIGH);                  // 设置与从机通讯引脚SS为高电平，即未开始循环时终止通讯，防止数据堆栈，占用内存
  
  pinMode(7,OUTPUT);       //定义输出引脚
	uint8_t error = 0;       

	Serial.begin(9600);
	Serial.println("\nPAJ7620U2 TEST DEMO: Recognize 9 gestures.");

	error = paj7620Init();			// 初始化Paj7620u2注册机
	if (error) 
	{
		Serial.print("INIT ERROR,CODE:");
		Serial.println(error);
	}
	else
	{
		Serial.println("INIT OK");
	}
	Serial.println("Please input your gestures:\n");
}                                                   //以上整段代码的作用是初始化注册机，会调用Arduino向传感器发送一些数据，并用传感器进行反馈，检测读取是否正常，如果错误会发送错误报告

void loop()
{
	byte Mastersend,Mastereceive;                   //定义主机发送与收入数据为单字节

  digitalWrite(SS, LOW);                  //调整SS引脚为低电平，开启与从机的通讯
  
	uint8_t data = 0, data1 = 0, error;             //传感器标准状态初始化
	
	error = paj7620ReadReg(0x43, 1, &data);				// 读取传感器的识别数据
	if (!error) 
	{
		switch (data) 									//不同的动作会引发传感器的不同数据反应，数据会被以paj7620ReadReg(0x43, 1, &data)的形式记录为data
		{
			case GES_RIGHT_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					Serial.println("Forward");
					delay(GES_QUIT_TIME);

          Mastersend = 1;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					Serial.println("Backward");
					delay(GES_QUIT_TIME);

          Mastersend = 2;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else
				{
					Serial.println("Right");

          Mastersend = 4;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}          
				break;
			case GES_LEFT_FLAG: 
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					Serial.println("Forward");
					delay(GES_QUIT_TIME);                 //进一步细化FORWARD,BACKWARD,LEFT的区别，给出一定QUIT_TIME，即容差时间，防止手势混淆

          Mastersend = 1;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					Serial.println("Backward");
					delay(GES_QUIT_TIME);

          Mastersend = 2;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else
				{
					Serial.println("Left");

          Mastersend = 3;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}          
				break;
			case GES_UP_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					Serial.println("Forward");
					delay(GES_QUIT_TIME);

          Mastersend = 1;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					Serial.println("Backward");
					delay(GES_QUIT_TIME);

          Mastersend = 2;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else
				{
					Serial.println("Up");              //区分FORWARD,BACKWARD,UP

          Mastersend = 5;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}          
				break;
			case GES_DOWN_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					Serial.println("Forward");      
					delay(GES_QUIT_TIME);

          Mastersend = 1;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					Serial.println("Backward");
					delay(GES_QUIT_TIME);

          Mastersend = 2;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}
				else
				{
					Serial.println("Down");

          Mastersend = 6;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				}          
				break;
			case GES_FORWARD_FLAG:
				Serial.println("Forward");
				delay(GES_QUIT_TIME);

        Mastersend = 1;                            
        Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				break;
			case GES_BACKWARD_FLAG:		  
				Serial.println("Backward");
				delay(GES_QUIT_TIME);

        Mastersend = 2;                            
        Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				break;
			case GES_CLOCKWISE_FLAG:
				Serial.println("Clockwise");

        Mastersend = 7;                            
        Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
				break;
			case GES_COUNT_CLOCKWISE_FLAG:
				Serial.println("anti-clockwise");
				break;  
			default:
				paj7620ReadReg(0x44, 1, &data1);
				if (data1 == GES_WAVE_FLAG) 
				{
					Serial.println("wave");         //通过重复计算和编译并调用图像库的方式解决WAVE手势的识别问题！！！***同时用default排除了所有其他手势的影响

          Mastersend = 7;                            
          Mastereceive=SPI.transfer(Mastersend); //发送数据到从机并和从机交换数据
         /*digitalWrite(7,HIGH);
				}
       else{
        digitalWrite(7,LOW);
        }*/
				break;
		}
	}
	delay(100);
}
