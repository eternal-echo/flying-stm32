#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_OUTPUT Serial

#include <Arduino.h>

#include <EasyLogger.h>
#include "ServoEasing.hpp"

ServoEasing arm_servo;

void setup()
{
	arm_servo.attach(PB15, 45);
	delay(500); // Wait for servo to reach start position.
	
	Serial.begin(115200);

	LOG_INFO("SETUP", "flying squirrel starting up");
}

void loop()
{
	LOG_DEBUG("LOOP", "testing servo");
	for(int i = 0; i < 180; i++)
	{
		arm_servo.easeTo(i, 40);
	}
	for(int i = 180; i > 0; i--)
	{
		arm_servo.easeTo(i, 40);
	}
}
