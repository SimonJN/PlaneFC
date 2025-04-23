#include <Arduino.h>
#include "CRSFforArduino.hpp"


#define RX2 16
#define TX2 17

CRSFforArduino *crsf = nullptr;

void increaser(void *pvParameter);

int goal = 1000;
int current = 0;
int change_step = 33;

const int pwmChannel = 0;     // Use PWM channel 0
const int pwmFrequency = 500;  // 500 Hz frequency for the ESC
const int pwmResolution = 16; // 16-bit resolution
const int pwmPin = 5;         // Pin connected to the ESC signal wire

void setup() {
  Serial.begin(115200);
  Serial.println("Fungerar");

  xTaskCreate(
    increaser,          /* Task function. */
    "increaser",        /* String with name of task. */
    10000,              /* Stack size in bytes. */
    NULL,               /* Parameter passed as input of the task */
    1,                  /* Priority of the task. */
    NULL);              /* Task handle. */
  
  // Setup motor PWM
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(pwmPin, pwmChannel);
}


void loop() {
  Serial.print ("loop() running in core ");
  Serial.println (xPortGetCoreID());
  delay(1000);
}

void increaser(void *pvParameters) {
  Serial.print ("increaser running in core ");
  Serial.println (xPortGetCoreID());
  while (1) {
    if (current < goal) {
      current += min(goal - current, change_step);
    } else if (current > goal)
    {
      current += max(goal - current, -change_step);
    }

    // ledcWrite(0, current);
    Serial.println(current);
    delay(500);
  }
}