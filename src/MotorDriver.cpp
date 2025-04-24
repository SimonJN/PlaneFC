#include "MotorDriver.h"

MotorDriver::MotorDriver(int pin, int channel, int change_step, const char *name)
{
    this->duty_cycle_change_step = change_step;
    this->pwm_channel = channel;
    this->pwm_pin = pin;

    // Setup motor PWM
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
    ledcAttachPin(pwm_pin, pwm_channel);

    // Create task to smoothly change motor speed
    xTaskCreate(
        dispatcher,                  // Task function.
        name,                        // String with name of task.
        1000,                        // Stack size in bytes.
        this,                        // Parameter passed as input of the task
        1,                           // Priority of the task.
        &this->duty_changer_handle); // Task handle.
}

MotorDriver::~MotorDriver()
{
    ledcDetachPin(pwm_pin);
    vTaskDelete(this->duty_changer_handle);
}

void MotorDriver::dispatcher(void *pvParameters)
{
    MotorDriver *md = (MotorDriver *)pvParameters;
    md->smoothDutyChanger();
    // In case of stack jump (?) delete current task
    vTaskDelete(NULL);
}

void MotorDriver::smoothDutyChanger()
{
    Serial.println("No custom smooth duty changer defined! This function will exit without effect.");
}

void MotorDriver::setDutyMinMax(int min_duty, int max_duty)
{
    this->min_duty_cycle = min_duty;
    this->max_duty_cycle = max_duty;
}

void MotorDriver::setGoalDutyCycle(int goal)
{
    this->goal_duty_cycle = goal;
}

/*
Calculate the correct PWM value for a specified duty cycle given specific PWM parameters.
*/
int MotorDriver::dutyCycleToPWM(int duty_cycle)
{
    return (int)(duty_cycle / (1000.0 * (1000 / (float)this->pwm_frequency)) * (pow(2, this->pwm_resolution)));
}