#include "PIDControllor.hpp"
#include <math.h>

PIDControllor::PIDControllor(float kp, float ki, float kd, uint16_t sampleTimeMs, float maxOutput, float maxIntegral) : 
    kp(kp), ki(ki), kd(kd), sampleTimeMs(sampleTimeMs), maxOutput(maxOutput), maxIntegral(maxIntegral),
    integral(0.0f), previousError(0.0f), previousMeasured(0.0f) {}

float PIDControllor::Compute(float setpoint, float measured) {
    float error = setpoint - measured;
    
    // 将时间单位从毫秒转换为秒
    float dt = sampleTimeMs / 1000.0f;
    
    // 积分项计算
    integral += error * dt;
    
    // 直接限制积分值本身，防止积分饱和
    if(ki != 0.0f) {
        float maxIntegralValue = maxIntegral / ki;
        integral = fmaxf(fminf(integral, maxIntegralValue), -maxIntegralValue);
    }
    
    float integralOut = ki * integral;
    
    // 微分项计算 
    float derivativeOutput = -kd * (measured - previousMeasured) / dt;
    
    float output = kp * error + integralOut + derivativeOutput;
    output = fmaxf(fminf(output, maxOutput), -maxOutput);
    
    previousError = error;
    previousMeasured = measured;
    
    return output;
}

void PIDControllor::Reset() {
    integral = 0.0f;
    previousError = 0.0f;
    previousMeasured = 0.0f;
}
