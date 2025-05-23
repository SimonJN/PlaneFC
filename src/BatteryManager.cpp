#include "BatteryManager.h"

BatteryManager::BatteryManager(int pin, const char *name)
{
    this->measure_pin = pin;

    // Create task to smoothly change motor speed
    xTaskCreate(
        dispatcher,                     // Task function.
        name,                           // String with name of task.
        10000,                          // Stack size in bytes.
        this,                           // Parameter passed as input of the task
        1,                              // Priority of the task.
        &this->battery_updater_handle); // Task handle.
}

BatteryManager::~BatteryManager()
{
    vTaskDelete(this->battery_updater_handle);
}

void BatteryManager::dispatcher(void *pvParameters)
{
    BatteryManager *bm = (BatteryManager *)pvParameters;
    bm->batteryUpdater();
    // In case of stack jump (?) delete current task
    vTaskDelete(NULL);
}

void BatteryManager::batteryUpdater()
{
    // Debug
    Serial.print("Battery Manager running in core ");
    Serial.println(xPortGetCoreID());

    while (1)
    {
        int samples = 64;
        int total = 0;
        for (int i = 0; i < samples; i++)
        {
            total = total + analogRead(this->measure_pin);
        }

        this->voltage = this->adcToVoltage(total / float(samples));
        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay the task in the proper way for a number of ticks
    }
}

float BatteryManager::adcToVoltage(float adc)
{
    return 0.0045868557 * adc + 1.8379070085; // Experimental data approximation
}

float BatteryManager::getVoltage()
{
    return this->voltage;
}