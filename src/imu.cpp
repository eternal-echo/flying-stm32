#include "imu.h"
#include <Wire.h>
#include <Arduino.h>


void IMU::dmpDataReady() {
    osEventFlagsSet(imuEventFlags, IMU_EVENT_FLAG_DMP_READY);
}

IMU::IMU() : interruptPin(PB12), mpuIntStatus(0), devStatus(0), packetSize(42), fifoCount(0) {}

bool IMU::setup() {
    LOG_INFO("IMU_SETUP", "Initializing IMU...");

    // Create a os event for IMU
    imuEventFlags = osEventFlagsNew(NULL);
    if (imuEventFlags == NULL) {
        LOG_ERROR("IMU_SETUP", "Failed to create IMU event flags");
        return false;
    }

    // Join I2C bus
    Wire.setSDA(PB11);
    Wire.setSCL(PB10);
    Wire.begin();
    Wire.setClock(400000);  // 400kHz I2C clock

    // Initialize MPU6050
    if (initializeMPU() == false) {
        return false;
    }
    pinMode(interruptPin, INPUT);
    auto dmp_int_handler = std::bind(&IMU::dmpDataReady, this);
    attachInterrupt(digitalPinToInterrupt(interruptPin), dmp_int_handler, RISING);
    LOG_INFO("IMU_SETUP", "IMU initialized");
    return true;
}

bool IMU::initializeMPU() {
    LOG_INFO("IMU_INIT", "Initializing I2C devices...");
    mpu.initialize();

    // Verify connection
    if (mpu.testConnection()) {
        LOG_INFO("IMU_INIT", "MPU6050 connection successful");
    } else {
        LOG_ERROR("IMU_INIT", "MPU6050 connection failed");
        return false;
    }

    // Load and configure the DMP
    LOG_INFO("IMU_INIT", "Initializing DMP...");
    devStatus = mpu.dmpInitialize();

    // Set gyro and accel offsets
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);

    if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        
        LOG_INFO("IMU_INIT", "Enabling DMP...");
        mpu.setDMPEnabled(true);

        packetSize = mpu.dmpGetFIFOPacketSize();
        return true;
    } else {
        LOG_ERROR("IMU_INIT", "DMP Initialization failed (code" << devStatus << ")");
    }
    return false;
}

void IMU::update() {
    // Wait for DMP data ready event
    osEventFlagsWait(imuEventFlags, IMU_EVENT_FLAG_DMP_READY, osFlagsWaitAny, osWaitForever);

    // Read data from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        // Get quaternion, gravity, and yaw/pitch/roll
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        LOG_INFO("IMU_UPDATE", "Yaw: "<< (ypr[0] * 180 / M_PI) << 
            ", Pitch: "<< (ypr[1] * 180 / M_PI) <<", Roll: " << (ypr[2] * 180 / M_PI));
    } else {
        LOG_WARNING("IMU_UPDATE", "Failed to read FIFO packet");
    }
}