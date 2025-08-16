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
#include <opencv2/opencv.hpp>

#include "opencv/track_target.h"

class LaserControl {
	enum class MachineState {
		Idle,
		Ready,
		Running,
		Error,
		Calibrating,
	};

	enum class MachineMode {
		Manual,
		Track,
		Draw,
	};
	
public:
	LaserControl& operator=(const LaserControl&) = delete;
	LaserControl(const LaserControl &) = delete;

	static LaserControl& getInstance();
	
	LaserControl();
	~LaserControl();
	
	int OpenSerialPort(const std::string &port, uint32_t baud);
	static std::string EnumeratePortToString();
	
	int CreateTask(uint32_t interval);
	
	MachineState m_state = MachineState::Idle;
	MachineMode m_mode = MachineMode::Manual;
private:
	serial::Serial* m_serial = nullptr;
	int SendString(const std::string& data) const;
	int m_process(uint32_t interval);

	friend class LaserControlUnitClass;

	std::atomic<bool> is_running = false;

	cv::Vec2f laser_target_pos {0.0f, 0.0f};
	cv::Vec2f laser_current_pos {0.0f, 0.0f};
	cv::Vec2f center_pos {0.0f, 0.0f};

	std::thread m_thread;

	int mode_manual(uint32_t interval);
	int mode_track(uint32_t interval);
	int mode_draw(uint32_t interval);
};



#endif //LASERCONTROL_H
