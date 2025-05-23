#pragma once
#include <Arduino.h>

class BatteryManager
{
private:
    TaskHandle_t battery_updater_handle;

    void batteryUpdater();
    static void dispatcher(void *pvParameters);

    int measure_pin;

    float voltage = 0.0f;

    float adcToVoltage(float adc);

public:
    BatteryManager(int pin, const char *name);
    ~BatteryManager();

    float getVoltage();
};