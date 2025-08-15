//
// Created by yuang on 25-8-15.
//

#include "LaserControl.h"

 LaserControl::LaserControl() {
	
}

LaserControl::~LaserControl() {
 	if (m_serial != nullptr) {
 		m_serial->close();
 		delete m_serial;
 	}
 }

int LaserControl::OpenSerialPort(std::string port, uint32_t baud) {
 	m_serial = new serial::Serial(port, baud, serial::Timeout::simpleTimeout(1000));
 	if (!m_serial->isOpen()) return -1; else return 1;
 }

std::string LaserControl::EnumeratePortToString() {
 	std::string ret;
 	std::vector<serial::PortInfo> ports = serial::list_ports();
 	for (auto &port : ports) {
 		ret += port.port + "\000";
 	}
 	return ret;
 }
