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

void Tracker::m_opencv_task() {
	if (m_cap.isOpened()) {
		m_objects.clear();
		
		m_cap.read(m_frame);
		if (m_frame.empty() == true) return;
		m_frame.copyTo(m_draw);

		const auto roi_result = m_extractor.extractROI(m_frame, &m_draw, 80);
		
		// 添加对象信息
		if (roi_result.roi_info.has_value()) {
			const ROIInfo& roi_info = roi_result.roi_info.value();
			
			cv::circle(m_draw, roi_info.center, 5, cv::Scalar(0, 0, 255), -1);
			cv::putText(m_draw, std::to_string(roi_info.area), roi_info.bounds.tl(),
				cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
			
			m_objects.emplace_back(ObjectInfo {
			.position = roi_result.roi_info.value().center,
			.velocity = cv::Vec2f(0, 0),
			.radius = -1,
			.type = ObjectType::PaperCenter,
			});

			// 异步不要使用引用，会导致悬空引用
			const auto& roi_frame = roi_result.roi_image;
			// TODO:使用线程池来提高效率

			// 	std::future<cv::Point2f> fut = std::async(
			// 		std::launch::async,
			// 		&Tracker::getLaserPos,
			// 		this, roi_frame, &m_draw
			// 	);
			// auto laser_pos = fut.get();
			
		} else {
			spdlog::warn("No ROI Info!");
		}
		
		cv::imshow("Draw", m_draw);
		cv::waitKey(1);
	}
}

void Tracker::getObjectList(std::vector<ObjectInfo> &list) const {
	list = m_objects;
}

cv::Point2f Tracker::getLaserPos(const cv::Mat &frame, cv::Mat *draw_frame) {
	return cv::Point2f {0, 0};
}

cv::Mat Tracker::getLaserTrace(const cv::Mat &frame, cv::Mat *draw_frame) {
	return cv::Mat {};
}


