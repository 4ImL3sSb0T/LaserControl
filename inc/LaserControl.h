//
// Created by yuang on 25-8-15.
//

#ifndef LASERCONTROL_H
#define LASERCONTROL_H

#include <serial/serial.h>
#include <vector>

class LaserControl {
public:
	LaserControl& operator=(const LaserControl&) = delete;
	LaserControl(const LaserControl&) = delete;

	LaserControl& getInstance();
	
	LaserControl();
	~LaserControl();
	
	int OpenSerialPort(std::string port, uint32_t baud);
	static std::string EnumeratePortToString();
	
	int CreateTask();
	
private:
	serial::Serial* m_serial = nullptr;
	int m_process();
};



#endif //LASERCONTROL_H
