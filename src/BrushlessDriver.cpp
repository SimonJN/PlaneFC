#include "BrushlessDriver.h"

BrushlessDriver::BrushlessDriver(int pin, int channel, int change_step, const char *name)
    : MotorDriver(pin, channel, change_step, name)
{
}

BrushlessDriver::~BrushlessDriver()
{
}

void BrushlessDriver::setArmed(bool is_armed)
{
    this->armed = is_armed;
    if (!is_armed)
    {
        this->soft_started = false;
    }
}

void BrushlessDriver::setFailsafeActive(bool failsafe_is_active)
{
    this->failsafe_active = failsafe_is_active;
    if (failsafe_is_active)
    {
        this->soft_started = false;
    }
}

void BrushlessDriver::setGoalDutyCycle(int goal)
{
    this->goal_duty_cycle = goal;
    if (this->armed && !this->failsafe_active && goal <= 1000)
    {
        this->soft_started = true;
    }
}

void BrushlessDriver::smoothDutyChanger()
{
    // Debug
    Serial.print("Brushless smoothDutyChanger running in core ");
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
            ledcWrite(this->pwm_channel, this->dutyCycleToPWM(gated_input));
            // Serial.print(current_duty_cycle);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS); // Delay the task in the proper way for a number of ticks
    }
}