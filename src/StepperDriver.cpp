#include "StepperDriver.h"

StepperDriver::StepperDriver(int pin, int channel, int change_step, const char *name)
    : MotorDriver(pin, channel, change_step, name)
{
}

StepperDriver::~StepperDriver()
{
}

void StepperDriver::smoothDutyChanger()
{
    // Debug
    Serial.print("Stepper smoothDutyChanger running in core ");
    Serial.println(xPortGetCoreID());

    while (1)
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
        int gated_input = max(min(this->current_duty_cycle, this->max_duty_cycle), this->min_duty_cycle);
        ledcWrite(this->pwm_channel, this->dutyCycleToPWM(gated_input));
        // Serial.print(current_duty_cycle);

        vTaskDelay(20 / portTICK_PERIOD_MS); // Delay the task in the proper way for a number of ticks
    }
}