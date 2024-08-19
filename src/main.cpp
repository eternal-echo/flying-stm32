#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_ICM20948.h>
#include <ArduinoLog.h>

#define SERIAL_PORT Serial
#define WIRE_PORT Wire

Adafruit_ICM20948 icm; // Create an ICM20948 object
Adafruit_Sensor *accel; // Create a sensor object for the accelerometer

void setup(void) {
  // Initialize Serial port for communication
  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT) { delay(10); } // Wait for Serial to be ready

  // Initialize logging
  Log.begin(LOG_LEVEL_VERBOSE, &SERIAL_PORT);

  // Initialize I2C communication
  WIRE_PORT.setSDA(PB11);
  WIRE_PORT.setSCL(PB10);
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000); // Set I2C clock to 400kHz

  // Initialize the ICM20948 sensor with I2C
  if (!icm.begin_I2C(0x68)) {
    Log.error(F("Failed to find ICM20948 chip"));
    while (1) {
      delay(10);
    }
  }

  Log.info(F("ICM20948 found and initialized!"));

  // Enable accelerometer DLPF and set the cutoff frequency
  icm.enableAccelDLPF(true, ICM20X_ACCEL_FREQ_5_7_HZ);
  Log.info(F("Accelerometer DLPF enabled with cutoff frequency 5.7 Hz"));

  // Get an Adafruit_Sensor compatible object for the accelerometer
  accel = icm.getAccelerometerSensor();
}

void loop() {
  // Get a new sensor event
  sensors_event_t event;

  // Retrieve the latest accelerometer event data
  accel->getEvent(&event);

  // Log the accelerometer data
  Log.verbose(F("Accel X: %f, Y: %f, Z: %f"),
              event.acceleration.x, event.acceleration.y, event.acceleration.z);

  // Delay before the next data retrieval
  delay(10);
}
