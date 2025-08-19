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
	if (m_serial != nullptr) m_serial->close();
 	m_serial = std::make_shared<serial::Serial>(port, baud, serial::Timeout::simpleTimeout(1000));
 	spdlog::info("Open serial port {}, serial state is {}", port, m_serial->isOpen());
	// TODO: 开启接收线程
	// auto receive = std::thread([this]() { m_serial->readline(rx_buffer);
	// });
	// receive.detach();
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
 	const std::string command = fmt::format("M,{},{},{}\n", static_cast<int>(motor), speed, pulse);
 	const size_t bytes_wrote = SendString(command);
	if (bytes_wrote <= 1) spdlog::warn("Serial sending warning, {} bytes were snet !", bytes_wrote);
 	return bytes_wrote;
 }

[[noreturn]] int LaserControl::m_process(const uint32_t intervalTime) {
 	using clock = std::chrono::steady_clock;
 	const auto interval = std::chrono::milliseconds(intervalTime);
 	spdlog::info("OpenCV process started");
 	while (true) {
 		std::unique_lock<std::mutex> lock(m_mutex);
 		m_cv.wait(lock, [this]{ return is_running == true;});
 		lock.unlock();
 		
 		const auto start_time = clock::now();
 		// TODO: Opencv 处理图形
 		// OpenCV 只负责更新数据, 对数据的处理放到switch里处理
 		// 这里处理的频率和OpenCV处理的频率同步, 串口发送也是同步的
 		
 		// 处理图形
 		Tracker::getInstance(0).m_opencv_task();
 		// 拿到处理完的数据
 		Tracker::getInstance(0).getObjectList(object_list);
 		switch (m_mode) {
 			case MachineMode::Manual: {
 				mode_manual(intervalTime);
 				break;
 			}
 			case MachineMode::Draw: {
 				mode_draw(intervalTime);
 				break;
 			}
 			case MachineMode::Track: {
 				mode_track(intervalTime);
 				break;
 			}
 			default: {
 				spdlog::error("Unknow Mode {}", static_cast<int>(m_mode));
 			}
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
 	spdlog::info("Control thread stopped");
 	return 1;
 }

int LaserControl::CreateTask(uint32_t interval) {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		is_running = true;
	}
	if (m_thread.joinable()) m_thread.join();
 	m_thread = std::thread(&LaserControl::m_process, this, interval);
 	spdlog::info("Control thread started");
 	m_thread.detach();
 	return 1;
 }

int LaserControl::StopTask() {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		is_running = false;
	}
	return 1;
 }


int LaserControl::mode_draw(uint32_t interval) {
 	if (m_serial == nullptr) return -1;
	 return 1;
 }

int LaserControl::mode_track(uint32_t interval) {
	if (m_serial == nullptr) return -1;
	cv::Point2f laser_pos {0, 0};
	cv::Point2f center_pos {0, 0};
	int laser_cnt = 0, center_cnt = 0;
	for (auto& object: object_list) {
		switch (object.type) {
			case Tracker::ObjectType::LaserPoint: {
				laser_pos = object.position;
				laser_cnt++;
				break;
			}
			case Tracker::ObjectType::PaperCenter: {
				center_pos = object.position;
				center_cnt++;
				break;
			}
			default: {
				spdlog::error("Unknow Type {}", static_cast<int>(object.type));
			}
		}
	}
	if (laser_cnt > 1 || center_cnt > 1) {
		spdlog::warn("Multiple objects detected ! {} laser points, {} Center point", laser_cnt, center_cnt);
		return -1;
	}
	if (laser_cnt == 0 || center_cnt == 0) {
        spdlog::warn("No laser point or center point detected !");
        return -1;
	}
	const auto step_out_x = pid_x.Compute(center_pos.x, laser_pos.x);
	const auto step_out_y = pid_y.Compute(center_pos.y, laser_pos.y);
	SetMotorPulse(MotorIndex::StepMotorX, 100, static_cast<int32_t>(step_out_x));
	SetMotorPulse(MotorIndex::StepMotorY, 100, static_cast<int32_t>(step_out_y));
	return 1;
 }

int LaserControl::mode_manual(uint32_t interval) {
 	if (m_serial == nullptr) return -1;
 	 return 1;
 }


