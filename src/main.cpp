#include <Wire.h>
#include <ICM20948_WE.h>
#include <ArduinoLog.h>  // 引入 ArduinoLog 库

#define ICM20948_ADDR 0x68

ICM20948_WE myIMU = ICM20948_WE(ICM20948_ADDR);

void setup() {
  // 初始化 Wire 接口，使用 PB11 和 PB10 作为 I2C 的 SDA 和 SCL
  Wire.setSDA(PB11);
  Wire.setSCL(PB10);
  Wire.begin();
  Wire.setClock(400000);  // 设置 I2C 通信速率为 400kHz

  // 初始化串口，并设置 ArduinoLog 库
  Serial.begin(115200);
  while (!Serial) {}
  
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);  // 初始化 ArduinoLog
  
  if (!myIMU.init()) {
    Log.error("ICM20948 does not respond");
	while (1) {
	  delay(10);
	}
  } else {
    Log.notice("ICM20948 is connected");
  }

  if (!myIMU.initMagnetometer()) {
    Log.error("Magnetometer does not respond");
	while (1) {
		delay(10);
	}
  } else {
    Log.notice("Magnetometer is connected");
  }

  // 设置磁力计工作模式为 20Hz 连续测量模式
  myIMU.setMagOpMode(AK09916_CONT_MODE_20HZ);
}

void loop() {
  // 读取传感器数据
  myIMU.readSensor();
  
  // 获取磁力计数据，单位是 µT
  xyzFloat magValue = myIMU.getMagValues(); 

  // 使用 ArduinoLog 打印磁力计数据
  Log.verbose("Magnetometer Data in µTesla: ");
  Log.verbose("%F   %F   %F", magValue.x, magValue.y, magValue.z);
  
  // 每隔 1 秒更新一次
  delay(1000);
}
