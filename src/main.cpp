#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_OUTPUT Serial

#include <Arduino.h>
#include <STM32FreeRTOS.h>

#include <EasyLogger.h>
#include "ServoEasing.hpp"
#include "imu.h"

osThreadId_t tid_servo;
osThreadId_t tid_imu;

ServoEasing arm_servo;
IMU imu;

void imu_thread(void *argument)
{
	imu.setup();  // 初始化 IMU
	while(1)
	{
		imu.update();  // 更新 IMU 数据
		osDelay(1000);
	}
}

void servo_thread(void *argument)
{
	arm_servo.attach(PB15, 45);
	osDelay(500); // Wait for servo to reach start position.
	while(1)
	{
		LOG_DEBUG("SERVOss", "testing servo");
		for(int i = 0; i < 180; i++)
		{
			arm_servo.easeTo(i, 40);
		}
		for(int i = 180; i > 0; i--)
		{
			arm_servo.easeTo(i, 40);
		}
		osDelay(1000);
	}
}

/*----------------------------------------------------------------------------
 *      Main: Initialize and start the application
 *---------------------------------------------------------------------------*/
void app_main (void */*argument*/) {
	tid_servo = osThreadNew(servo_thread, NULL, NULL);
	tid_imu = osThreadNew(imu_thread, NULL, NULL);	
	while(1) {
		osDelay(osWaitForever);
	}
}

void setup()
{
	Serial.begin(115200);

	LOG_INFO("SETUP", "flying squirrel starting up");

	osKernelInitialize();
	osThreadNew(app_main, NULL, NULL);    // Create application main thread
	if (osKernelGetState() == osKernelReady) {
		osKernelStart();                    // Start thread execution
	}

	while(1);
}

void loop()
{
}
