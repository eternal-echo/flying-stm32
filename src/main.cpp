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

	Serial.println("Position your ICM20948 flat and don't move it - calibrating...");
	delay(1000);
	myIMU.autoOffsets();
	Serial.println("Done!"); 

	/* enables or disables the acceleration sensor, default: enabled */
	// myIMU.enableAcc(true);

	/*  ICM20948_ACC_RANGE_2G      2 g   (default)
	*  ICM20948_ACC_RANGE_4G      4 g
	*  ICM20948_ACC_RANGE_8G      8 g   
	*  ICM20948_ACC_RANGE_16G    16 g
	*/
	myIMU.setAccRange(ICM20948_ACC_RANGE_2G);

  /*  Choose a level for the Digital Low Pass Filter or switch it off.  
   *  ICM20948_DLPF_0, ICM20948_DLPF_2, ...... ICM20948_DLPF_7, ICM20948_DLPF_OFF 
   *  
   *  IMPORTANT: This needs to be ICM20948_DLPF_7 if DLPF is used in cycle mode!
   *  
   *  DLPF       3dB Bandwidth [Hz]      Output Rate [Hz]
   *    0              246.0               1125/(1+ASRD) (default)
   *    1              246.0               1125/(1+ASRD)
   *    2              111.4               1125/(1+ASRD)
   *    3               50.4               1125/(1+ASRD)
   *    4               23.9               1125/(1+ASRD)
   *    5               11.5               1125/(1+ASRD)
   *    6                5.7               1125/(1+ASRD) 
   *    7              473.0               1125/(1+ASRD)
   *    OFF           1209.0               4500
   *    
   *    ASRD = Accelerometer Sample Rate Divider (0...4095)
   *    You achieve lowest noise using level 6  
   */
  myIMU.setAccDLPF(ICM20948_DLPF_6);    

  /*  Acceleration sample rate divider divides the output rate of the accelerometer.
   *  Sample rate = Basic sample rate / (1 + divider) 
   *  It can only be applied if the corresponding DLPF is not off!
   *  Divider is a number 0...4095 (different range compared to gyroscope)
   *  If sample rates are set for the accelerometer and the gyroscope, the gyroscope
   *  sample rate has priority.
   */
  myIMU.setAccSampleRateDivider(10);
}

void loop() {
  // 读取传感器数据
  myIMU.readSensor();
  
  xyzFloat accRaw = myIMU.getAccRawValues();
  xyzFloat corrAccRaw = myIMU.getCorrectedAccRawValues();
  xyzFloat gVal = myIMU.getGValues();
  float resultantG = myIMU.getResultantG(gVal);

  // 使用 ArduinoLog 打印数据
  Log.verbose("Raw acceleration values (x,y,z): %d.%d, %d.%d, %d.%d\n", (int)accRaw.x, (int)(accRaw.x * 1000) % 1000, (int)accRaw.y, (int)(accRaw.y * 1000) % 1000, (int)accRaw.z, (int)(accRaw.z * 1000) % 1000);
  Log.verbose("Corrected acceleration values (x,y,z): %d.%d, %d.%d, %d.%d\n", (int)corrAccRaw.x, (int)(corrAccRaw.x * 1000) % 1000, (int)corrAccRaw.y, (int)(corrAccRaw.y * 1000) % 1000, (int)corrAccRaw.z, (int)(corrAccRaw.z * 1000) % 1000);
  Log.verbose("Gyro values (x,y,z): %d.%d, %d.%d, %d.%d\n", (int)gVal.x, (int)(gVal.x * 1000) % 1000, (int)gVal.y, (int)(gVal.y * 1000) % 1000, (int)gVal.z, (int)(gVal.z * 1000) % 1000);
  Log.verbose("Resultant G: %d.%d\n", (int)resultantG, (int)(resultantG * 1000) % 1000);
  
  // 每隔 5 秒更新一次
  delay(5000);
}
