#ifndef ROS_HPP
#define ROS_HPP

#include <STM32FreeRTOS.h>
#include "user_config.h" // It must be located above ros2arduino.h.
#include <ros2arduino.h>

class RosNode
{
public:
    // 构造函数允许设置串口和发布频率
    RosNode(HardwareSerial& serialPort = Serial, int publishFrequency = 2)
    : node_("ros2arduino_pub_node"),
      serialPort_(serialPort),
      publishFrequency_(publishFrequency)
    {
        // 初始化发布器
        publisher_ = node_.createPublisher<std_msgs::String>("arduino_chatter");
        // 设置定时器，定期发布消息
        node_.createWallFreq(publishFrequency_, (ros2::CallbackFunc)publishString, nullptr, publisher_);
    }

    // 初始化串口并设置 ros2arduino
    void init()
    {
        serialPort_.begin(115200);
        while (!serialPort_) {
            ;  // 等待串口连接
        }

        ros2::init(&serialPort_);
    }

    // 启动节点的循环处理
    void spin()
    {
        ros2::spin(&node_);
    }

    // 创建线程方法，适合在 FreeRTOS 中使用
    void ros_thread()
    {
        init();
        while (1)
        {
            spin();
            osDelay(1000);  // FreeRTOS延迟函数
        }
    }

private:
    ros2::Node node_;
    ros2::Publisher<std_msgs::String>* publisher_;

    HardwareSerial& serialPort_;   // 串口对象，默认使用 Serial3
    int publishFrequency_;         // 发布频率，默认 2 Hz

    // 发布字符串消息的回调函数
    static void publishString(std_msgs::String* msg, void* arg)
    {
        (void)(arg);

        static int cnt = 0;
        sprintf(msg->data, "Hello ros2arduino %d", cnt++);
    }
};

#endif // ROS_HPP
