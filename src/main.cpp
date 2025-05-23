#include <Arduino.h>
#include "CRSFforArduino.hpp"

#include "BrushlessDriver.h"
#include "StepperDriver.h"
#include "BatteryManager.h"

#define RX2 17
#define TX2 16

CRSFforArduino *crsf = nullptr;

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels);

bool failsafe_active = false;
bool armed = false;
bool soft_started = false;

HardwareSerial s(2);

BrushlessDriver *edf = nullptr;

StepperDriver *left_stepper = nullptr;
StepperDriver *right_stepper = nullptr;

BatteryManager *bm = nullptr;

void setup()
{
  Serial.begin(115200);

  edf = new BrushlessDriver(13, 0, 100, "edf");
  left_stepper = new StepperDriver(14, 1, 100, "left_stepper");
  right_stepper = new StepperDriver(4, 1, 100, "right_stepper");
  bm = new BatteryManager(32, "battery_manager");

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

  Serial.print("Main loop() running in core ");
  Serial.println(xPortGetCoreID());
}

void loop()
{
  crsf->update();
}

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels)
{
  static int counter = 0;
  static unsigned long lastPrint = millis();
  if (millis() - lastPrint >= 20)
  {
    lastPrint = millis();

    crsf->telemetryWriteBattery(bm->getVoltage() * 1000, 0, 0, 0);

    failsafe_active = rcChannels->failsafe;
    armed = crsf->rcToUs(rcChannels->value[4]) > 1500;
    edf->setFailsafeActive(failsafe_active);
    edf->setArmed(armed);

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

      int goal = crsf->rcToUs(rcChannels->value[2]);
      edf->setGoalDutyCycle(goal);

      int stepper_goal = crsf->rcToUs(rcChannels->value[1]);
      left_stepper->setGoalDutyCycle(stepper_goal);
      right_stepper->setGoalDutyCycle(stepper_goal);
    }
  }
}