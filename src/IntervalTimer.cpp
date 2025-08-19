//
// Created by yuang on 25-8-19.
//

#include "../inc/IntervalTimer.h"
#include <numeric>

IntervalTimer::IntervalTimer(uint32_t maxTimePoints) {
    if (maxTimePoints < 2) maxTimePoints = 2;
    m_timePoints.resize(maxTimePoints);
}

IntervalTimer::~IntervalTimer() {
    m_timePoints.clear();
}

void IntervalTimer::TimerTrigger() {
    m_timePoints.push_back(std::chrono::high_resolution_clock::now());
}

std::chrono::duration<double> IntervalTimer::getAverageTimeInterval() {
    if (m_timePoints.size() < 2) return std::chrono::duration<double>(0.0);
    const auto total_interval = std::chrono::duration<double>(
        m_timePoints.back() - m_timePoints.front());
    return total_interval / (m_timePoints.size() - 1);
}

std::chrono::duration<double> IntervalTimer::getLastTimeInterval() {
    if (m_timePoints.size() < 2) return std::chrono::duration<double>(0.0);
    const auto interval = std::chrono::duration_cast<std::chrono::duration<double>>(m_timePoints.back() - m_timePoints[m_timePoints.size() - 2]);
    return interval;
}

