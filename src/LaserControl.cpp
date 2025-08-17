//
// Created by yuang on 25-8-15.
//

#include "LaserControl.h"
#include <fmt/core.h>

 LaserControl::LaserControl() = default;

LaserControl::~LaserControl() {
 	is_running = false;
 }

int LaserControl::OpenSerialPort(const std::string& port, const uint32_t baud) {
 	m_serial = std::make_shared<serial::Serial>(serial::Serial(port, baud, serial::Timeout::simpleTimeout(1000)));
 	spdlog::info("Open serial port {}, serial state is {}", port, m_serial->isOpen());
 	if (!m_serial->isOpen()) return -1; else return 1;
 }

std::string LaserControl::EnumeratePortToString() {
 	std::string ret;
	for (const std::vector<serial::PortInfo> ports = serial::list_ports(); auto &port : ports) {
 		ret += port.port + '\0';
 	}
 	return ret;
 }

LaserControl& LaserControl::getInstance() {
 	static LaserControl instance;
 	return instance;
 }

size_t LaserControl::SendString(const std::string &data) const {
 	if (m_serial == nullptr) return -1;
 	size_t bytes_wrote = -1;
 	if (m_serial->available())
 		bytes_wrote = m_serial->write(data);
 	return bytes_wrote;
 }

size_t LaserControl::SetMotorPulse(MotorIndex motor, uint32_t speed, int32_t pulse) const {
 	if (m_serial == nullptr) return -1;
 	const std::string command = fmt::format("M{},{},{}", motor, speed, pulse);
 	const size_t bytes_wrote = SendString(command);
 	return bytes_wrote;
 }

int LaserControl::m_process(const uint32_t intervalTime) {
 	using clock = std::chrono::steady_clock;
 	const auto interval = std::chrono::milliseconds(intervalTime);
 	spdlog::info("OpenCV process started");
 	while (is_running) {
 		const auto start_time = clock::now();
 		// TODO: Opencv 处理图形
 		// OpenCV 只负责更新数据, 对数据的处理放到switch里处理
 		// 这里处理的频率和OpenCV处理的频率同步, 串口发送也是同步的
 		switch (m_mode) {
 			case MachineMode::Manual: {
 				
 			}
 			break;
 			case MachineMode::Draw: {
 				mode_draw(intervalTime);
 			}
 			break;
 			case MachineMode::Track: {
 				mode_track(intervalTime);
 			}
 			break;
 		}
 		const auto end_time = clock::now();
		// intervalTime 小于0，以OpenCV的频率同步。只有intervalTimed大于OpenCV处理的时间才会有sheep
 		if (const auto duration_time =
				std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time)
					.count();
			duration_time < intervalTime && intervalTime > 0) {
 			auto sleep_time = interval - (end_time - start_time);
 			std::this_thread::sleep_for(sleep_time);
 		}
 	}
 	spdlog::info("OpenCV process stopped");
 	return 1;
 }

int LaserControl::CreateTask(uint32_t interval) {
 	m_thread = std::thread(&LaserControl::m_process, this, interval);
 	spdlog::info("OpenCV thread started");
 	m_thread.detach();
 	return 1;
 }

int LaserControl::StopTask() {
	 is_running = false;
	return 1;
 }


int LaserControl::mode_draw(uint32_t interval) {
 	if (m_serial == nullptr) return -1;
	 return 1;
 }

int LaserControl::mode_track(uint32_t interval) {
 	if (m_serial == nullptr) return -1;
 	 return 1;
 }

int LaserControl::mode_manual(uint32_t interval) {
 	if (m_serial == nullptr) return -1;
 	
 	 return 1;
 }


