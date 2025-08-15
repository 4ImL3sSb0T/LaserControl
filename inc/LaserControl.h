//
// Created by yuang on 25-8-15.
//

#ifndef LASERCONTROL_H
#define LASERCONTROL_H

#include <serial/serial.h>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

#include  "opencv/track_target.h"

class LaserControl {
public:
	LaserControl& operator=(const LaserControl&) = delete;
	LaserControl(const LaserControl &) = delete;

	static LaserControl& getInstance();
	
	LaserControl();
	~LaserControl();
	
	int OpenSerialPort(const std::string &port, uint32_t baud);
	static std::string EnumeratePortToString();

	
	int CreateTask(uint32_t freq);
	
private:
	serial::Serial* m_serial = nullptr;
	int SendString(const std::string& data) const;
	int m_process(uint32_t freq) const;

	friend class LaserControlUnitClass;

	std::atomic<bool> is_running = false;
};



#endif //LASERCONTROL_H
