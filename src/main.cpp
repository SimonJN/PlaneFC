#include <Arduino.h>
#include "CRSFforArduino.hpp"

#define RX2 16
#define TX2 17

CRSFforArduino *crsf = nullptr;

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels);
void increaser(void *pvParameter);

HardwareSerial s(2);

int goal = 1000;
int current = 1000;
int change_step = 100;

bool failsafe_active = false;
bool armed = false;
bool soft_started = false;

const int pwmChannel = 0;     // Use PWM channel 0
const int pwmFrequency = 500;  // 500 Hz frequency for the ESC
const int pwmResolution = 16; // 16-bit resolution
const int pwmPin = 5;         // Pin connected to the ESC signal wire

void setup() {
  Serial.begin(115200);
  Serial.println("Fungerar");

  crsf = new CRSFforArduino(&s, TX2, RX2); // Pin names correspond to pin name on receiver

  if (!crsf->begin())
  {
      Serial.println("CRSF for Arduino failed to initialise.");
      delete crsf;
      crsf = nullptr;
      while (1)
      {
          delay(10);
      }
  }

  crsf->setRcChannelsCallback(onReceiveRcChannels);

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

  Serial.print ("loop() running in core ");
  Serial.println (xPortGetCoreID());
}


void loop() {
  crsf->update();
}

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels) {
  static unsigned long lastPrint = millis();
  if (millis() - lastPrint >= 20) {
    lastPrint = millis();

    failsafe_active = rcChannels->failsafe;
    armed = crsf->rcToUs(rcChannels->value[4]) > 1500;
    if (failsafe_active || !armed) {
      soft_started = false;
    }

    static bool initialised = false;
    static bool lastFailSafe = false;
    if (rcChannels->failsafe != lastFailSafe || !initialised)
    {
        initialised = true;
        lastFailSafe = rcChannels->failsafe;
        Serial.print("FailSafe: ");
        Serial.println(lastFailSafe ? "Active" : "Inactive");
    }

    if (rcChannels->failsafe == false)
    {
        // Serial.print("RC Channels <A: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[0]));
        // Serial.print(", E: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[1]));
        // Serial.print(", T: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[2]));
        // Serial.print(", R: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[3]));
        // Serial.print(", Aux1: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[4]));
        // Serial.print(", Aux2: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[5]));
        // Serial.print(", Aux3: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[6]));
        // Serial.print(", Aux4: ");
        // Serial.print(crsf->rcToUs(rcChannels->value[7]));
        // Serial.println(">");
        
        goal = crsf->rcToUs(rcChannels->value[2]);
        if (!soft_started && goal <= 1000)
        {
          soft_started = true;
        }
        
    }
  }
}

void increaser(void *pvParameters) {
  Serial.print ("increaser running in core ");
  Serial.println (xPortGetCoreID());
  while (1) {
    if (failsafe_active || !armed || !soft_started)
    {
      int pwm_value = (int)(990/(1000.0*(1000/(float) pwmFrequency))*(pow(2,pwmResolution)));
      ledcWrite(0, pwm_value);
    } else {
      // Serial.print("Goal: ");
      // Serial.println(goal);
      if (current < goal) {
        current += min(goal - current, change_step);
      } else if (current > goal)
      {
        current += max(goal - current, -change_step);
      }
      int gated_input = min(current, 1990);
      int pwm_value = (int)(gated_input/(1000.0*(1000/(float) pwmFrequency))*(pow(2,pwmResolution)));
      ledcWrite(0, pwm_value);
      // Serial.print(current);
      // Serial.print(" which is ");
      // Serial.println(pwm_value);
    }
    delay(20);
  }
}