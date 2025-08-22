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
#include <spdlog/spdlog.h>
#include <condition_variable>
#include <mutex>

#include "opencv/track_target.h"
#include "PIDControllor.hpp"

class LaserControl {
public:
	enum class MotorIndex {
		StepMotorX ,
		StepMotorY,
		StepMotorZ,
		ServoMotorX,
		ServoMotorY,
		ServoMotorZ,
	};
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
	
	LaserControl& operator=(const LaserControl&) = delete;
	LaserControl(const LaserControl &) = delete;

	static LaserControl& getInstance();
	
	LaserControl();
	~LaserControl();
	
	int OpenSerialPort(const std::string &port, uint32_t baud);
	static std::string EnumeratePortToString();
	
	int CreateTask(uint32_t interval);
	int StopTask();

	size_t SetMotorPulse(MotorIndex motor, uint32_t speed, int32_t pulse) const;
	
	MachineState m_state = MachineState::Idle;
	MachineMode m_mode = MachineMode::Track;
private:
	// serial::Serial* m_serial = nullptr;
	std::shared_ptr<serial::Serial> m_serial = nullptr;
	size_t SendString(const std::string& data) const;
	int m_process(uint32_t interval);

	friend class LaserControlUnitClass;

	std::atomic<bool> is_running = false;
	
	std::vector<ComputerVision::ObjectInfo> object_list {};

	std::thread m_thread;

	int mode_manual(uint32_t interval);
	int mode_track(uint32_t interval);
	int mode_draw(uint32_t interval);

	std::string rx_buffer {};

	PIDControllor pid_x, pid_y;

	std::mutex m_mutex;
	std::condition_variable m_cv;

	cv::UMat target_trace;

};



#endif //LASERCONTROL_H
