#ifndef IMU_H
#define IMU_H

#include <STM32FreeRTOS.h>
#include <EasyLogger.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

class IMU {
public:
    IMU();
    bool setup();
    void update();

private:
    MPU6050 mpu;
    uint8_t interruptPin;
    uint8_t mpuIntStatus;
    uint8_t devStatus;
    uint16_t packetSize;
    uint16_t fifoCount;
    uint8_t fifoBuffer[64];

    Quaternion q;
    VectorInt16 aa;
    VectorInt16 aaReal;
    VectorInt16 aaWorld;
    VectorFloat gravity;
    float euler[3];
    float ypr[3];

    osEventFlagsId_t imuEventFlags;

    // event flag bits
    static const uint32_t IMU_EVENT_FLAG_DMP_READY = 1 << 0;

    // functions
    bool initializeMPU();
    void dmpDataReady();
};

#endif
