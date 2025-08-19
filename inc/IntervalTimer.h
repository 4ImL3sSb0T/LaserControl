//
// Created by yuang on 25-8-19.
//

#ifndef INTERVALTIMER_H
#define INTERVALTIMER_H

#include <chrono>
#include <queue>
#include <ring_buffer.h>

class IntervalTimer {
public:
    IntervalTimer(uint32_t maxTimePoints);
    ~IntervalTimer();
    void TimerTrigger();
    IntervalTimer() = delete;
    std::chrono::duration<double> getAverageTimeInterval();
    std::chrono::duration<double> getLastTimeInterval();
private:
    ring_buffer<std::chrono::time_point<std::chrono::high_resolution_clock>> m_timePoints;
};



#endif //INTERVALTIMER_H
