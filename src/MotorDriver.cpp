#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
    // Setup motor PWM
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
    ledcAttachPin(pwm_pin, pwm_channel);

    // Create task to smoothly change motor speed
    xTaskCreate(
        dispatcher,               // Task function.
        "smoothSpeedChanger",     // String with name of task.
        1000,                     // Stack size in bytes.
        this,                     // Parameter passed as input of the task
        1,                        // Priority of the task.
        &this->increaser_handle); // Task handle.
}

MotorDriver::~MotorDriver()
{
    ledcDetachPin(pwm_pin);
    vTaskDelete(this->increaser_handle);
}

void MotorDriver::dispatcher(void *pvParameters)
{
    MotorDriver *md = (MotorDriver *)pvParameters;
    md->smoothSpeedChanger();
    // In case of stack jump (?) delete current task
    vTaskDelete(NULL);
}

void MotorDriver::setGoalDutyCycle(int goal)
{
    this->goal_duty_cycle = goal;
    if (this->armed && !this->failsafe_active && goal <= 1000)
    {
        this->soft_started = true;
    }
}

void MotorDriver::setArmed(bool is_armed)
{
    this->armed = is_armed;
    if (!is_armed)
    {
        this->soft_started = false;
    }
}

void MotorDriver::setFailsafeActive(bool failsafe_is_active)
{
    this->failsafe_active = failsafe_is_active;
    if (failsafe_is_active)
    {
        this->soft_started = false;
    }
}

void MotorDriver::smoothSpeedChanger()
{
    // Debug
    Serial.print("smoothSpeedChanger running in core ");
    Serial.println(xPortGetCoreID());

    while (1)
    {
        // Check if the motor should shut down
        if (this->failsafe_active || !this->armed || !this->soft_started)
        {
            int pwm_value = this->dutyCycleToPWM(990); // Off-state duty cycle 990, currently harcoded
            ledcWrite(0, pwm_value);
        }
        else
        {
            // Serial.print("Goal: ");
            // Serial.println(goal_duty_cycle);
            if (this->current_duty_cycle < this->goal_duty_cycle)
            {
                this->current_duty_cycle += min(this->goal_duty_cycle - this->current_duty_cycle, this->duty_cycle_change_step);
            }
            else if (current_duty_cycle > goal_duty_cycle)
            {
                this->current_duty_cycle += max(this->goal_duty_cycle - this->current_duty_cycle, -this->duty_cycle_change_step);
            }
            int gated_input = min(this->current_duty_cycle, 1990);
            ledcWrite(0, this->dutyCycleToPWM(gated_input));
            // Serial.print(current_duty_cycle);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS); // Delay the task in the proper way for a number of ticks
    }
}

/*
Calculate the correct PWM value for a specified duty cycle given specific PWM parameters.
*/
int MotorDriver::dutyCycleToPWM(int duty_cycle)
{
    return (int)(duty_cycle / (1000.0 * (1000 / (float)this->pwm_frequency)) * (pow(2, this->pwm_resolution)));
}
