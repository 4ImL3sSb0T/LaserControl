//
// Created by yuang on 25-8-10.
//

#include "opencv/track_target.h"

// 使用局部静态变量实现线程安全的单例（C++11及以后版本）
Tracker& Tracker::getInstance(const int index) {
    static Tracker instance(index);
    return instance;
}

Tracker::Tracker(const int index) {
	m_cap = cv::VideoCapture(index);
	if (Tracker::m_cap.isOpened()) {
		m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 1200);
		m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 800);
		m_cap.set(cv::CAP_PROP_FPS, 60);
		m_cap.set(cv::CAP_PROP_EXPOSURE, -3);
	}
}

Tracker::~Tracker() {
	// 析构函数实现（如果需要清理操作）
	if (Tracker::m_cap.isOpened()) {
		Tracker::m_cap.release();
	}
}

void Tracker::createTask() const {
	// 任务创建实现
}

void Tracker::m_opencv_task() const {
	if (m_cap.isOpened()) {
		m_cap.read(m_frame);
		if (m_frame.empty() == true) return;
		m_frame.copyTo(m_draw);

		
		
		cv::imshow("Draw", m_draw);
		cv::waitKey(1);
	}
}

Tracker::ObjectInfo Tracker::getObjectInfo(const ObjectType type) {
	auto info = ObjectInfo {
		.type = ObjectType::None,
		.position = cv::Point2f(0, 0),
		.radius = 0
	};
	switch (type) {
		default:
			break;		
	}
	return info;
	
}

