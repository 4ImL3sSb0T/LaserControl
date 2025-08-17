#pragma once
#include <stdint.h>

class PIDControllor {
public:
    struct PIDConfig {
        float kp = 0.0f;
        float ki = 0.0f;
        float kd = 0.0f;
        uint16_t sampleTimeMs = 5;
        float maxOutput = 0.0f;
        float maxIntegral = 0.0f;
    };

public:
    // 修复：默认参数后面的所有参数都必须有默认值，或者移除默认参数
    PIDControllor(float kp, float ki, float kd, uint16_t sampleTimeMs, float maxOutput, float maxIntegral);
    PIDControllor(const PIDConfig& config)
        : kp(config.kp), ki(config.ki), kd(config.kd),
          sampleTimeMs(config.sampleTimeMs), maxOutput(config.maxOutput), maxIntegral(config.maxIntegral) {}
    PIDControllor() = default;

    void SetConfig(const PIDConfig& config) {
        kp = config.kp;
        ki = config.ki;
        kd = config.kd;
        sampleTimeMs = config.sampleTimeMs;
        maxOutput = config.maxOutput;
        maxIntegral = config.maxIntegral;
    }

    float Compute(float setpoint, float measured);

    void Reset();

    float kp = 0;
    float ki = 0;
    float kd = 0;
    // 修复：调整成员变量声明顺序，按照初始化列表的顺序
    uint16_t sampleTimeMs = 5;
    float maxOutput = 0;
    float maxIntegral = 0;
    float integral = 0;
    float previousError = 0;
    float previousMeasured = 0;

private:

};
