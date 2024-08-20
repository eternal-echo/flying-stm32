#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_OUTPUT Serial

#include <memory>

#include <Arduino.h>
#include <STM32FreeRTOS.h>

#include "ros.hpp"

#include <EasyLogger.h>
#include "ServoEasing.hpp"
#include "imu.h"

HardwareSerial ros_serial(PD8, PD9);

osThreadId_t tid_servo;
osThreadId_t tid_imu;
osThreadId_t tid_ros;
const osThreadAttr_t thread_attr_servo = {
	.name = "servo_thread",
	.stack_size = 2048,
	.priority = osPriorityNormal,
};
const osThreadAttr_t thread_attr_imu = {
	.name = "imu_thread",
	.stack_size = 2048,
	.priority = osPriorityNormal,
};
const osThreadAttr_t thread_attr_ros = {
	.name = "ros_thread",
	.stack_size = 8192,
	.priority = osPriorityHigh,
};

ServoEasing arm_servo;
IMU imu;


void imu_thread(void *argument)
{
	LOG_DEBUG("IMU", "starting imu");
	imu.setup();  // 初始化 IMU
	while(1)
	{
		imu.update();  // 更新 IMU 数据
		osDelay(1000);
	}
}

void servo_thread(void *argument)
{
	LOG_DEBUG("SERVO", "starting servo");
	arm_servo.attach(PB15, 45);
	osDelay(500); // Wait for servo to reach start position.
	while(1)
	{
		LOG_DEBUG("SERVO", "testing servo");
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


void publishString(std_msgs::String* msg, void* arg)
{
  (void)(arg);

  static int cnt = 0;
  sprintf(msg->data, "Hello ros2arduino %d", cnt++);
}

class StringPub : public ros2::Node
{
public:
  StringPub()
  : Node("ros2arduino_pub_node")
  {
    ros2::Publisher<std_msgs::String>* publisher_ = this->createPublisher<std_msgs::String>("arduino_chatter");
    this->createWallFreq(2, (ros2::CallbackFunc)publishString, nullptr, publisher_);
  }
};

void ros_thread(void *argument)
{
	// RosNode ros_node(ros_serial, 2);
	LOG_INFO("ros_thread", "Creating RosNode");
	ros_serial.begin(115200);
	while (!ros_serial) {
		osDelay(1000);
	}
	LOG_INFO("ros_thread", "RosNode's serial port connected");
	ros2::init(&ros_serial);
	LOG_INFO("ros_thread", "RosNode init done");
	static StringPub StringNode;
	while (1) {
		ros2::spin(&StringNode);
		osDelay(1000);
	}
	// std::unique_ptr<RosNode> ros_node = std::make_unique<RosNode>(ros_serial, 2);
	// LOG_INFO("ros_thread", "RosNode init");
	// ros_node->init();
	// LOG_INFO("ros_thread", "RosNode init done");
	// while(1)
	// {
	// 	ros_node->spin();
	// 	osDelay(1000);
	// }
}

/*----------------------------------------------------------------------------
 *      Main: Initialize and start the application
 *---------------------------------------------------------------------------*/
void app_main (void */*argument*/) {
	LOG_INFO("app_main", "Creating threads");

	tid_servo = osThreadNew(servo_thread, NULL, &thread_attr_servo);
	tid_imu = osThreadNew(imu_thread, NULL, &thread_attr_imu);
	tid_ros = osThreadNew(ros_thread, NULL, &thread_attr_ros);

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
	LOG_ERROR("SETUP", "osKernelGetState() != osKernelReady");

	while(1) {
		osDelay(osWaitForever);
	}
}

void loop()
{
}
