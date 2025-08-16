//
// Created by yuang on 25-8-15.
//

#include "LaserControl.h"
#include <fmt/core.h>

 LaserControl::LaserControl() {
	
}

LaserControl::~LaserControl() {
 	if (m_serial != nullptr) {
 		m_serial->close();
 		delete m_serial;
 	}
 }

int LaserControl::OpenSerialPort(const std::string& port, const uint32_t baud) {
 	m_serial = new serial::Serial(port, baud, serial::Timeout::simpleTimeout(1000));
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

int LaserControl::SendString(const std::string &data) const {
 	if (m_serial == nullptr) return -1;
	const size_t bytes_wrote = m_serial->write(data);
 	return bytes_wrote;
 }

int LaserControl::m_process(const uint32_t intervalTime) {
 	using clock = std::chrono::steady_clock;
 	auto next = clock::now();
 	const auto interval = std::chrono::milliseconds(intervalTime);
 	
 	while (is_running) {
 		switch (m_mode) {
 			case MachineMode::Manual: {
 				mode_draw(intervalTime);
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
 		next += interval;
 		std::this_thread::sleep_until(next);
 	}
 	
 	return 1;
 }

int LaserControl::CreateTask(uint32_t interval) {
 	m_thread = std::thread(&LaserControl::m_process, this, interval);
 	m_thread.detach();
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


