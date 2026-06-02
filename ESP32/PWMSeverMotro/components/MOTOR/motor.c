#include "motor.h"
#include "pwm.h"

void set_angle(uint8_t angle)
{
    if(angle>180) angle=180;
    uint16_t duty=(angle/1000+0.025)*1024;
    duty_set(duty);
}