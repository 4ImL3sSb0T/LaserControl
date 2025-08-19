//
// Created by yuang on 25-8-10.
//

#include "opencv/track_target.h"
#include <chrono>

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
		m_cap.set(cv::CAP_PROP_EXPOSURE, -5);
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

Tracker::ObjectInfo getLaserPos(const cv::UMat &frame, cv::UMat *draw_frame,
							const HSVRange& hsv_r, const HSVRange& hsv_laser,
							int min_radius = 5, int max_radius = 15) {
	Tracker::ObjectInfo laser_info_obj {};
	
	if (frame.empty()) {
		spdlog::warn("Frame is empty, cannot get laser position.");
		return laser_info_obj;
	}
	cv::Point2f laser_pos {};
	cv::UMat hsv, mask, opening, dilated;
	cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
	cv::inRange(hsv, hsv_r.lower, hsv_r.upper, mask);
	cv::morphologyEx(mask, opening, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	cv::dilate(opening, dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

	static std::vector<cv::Vec3f> circles {};
	circles.clear();
	cv::HoughCircles(dilated, circles, cv::HOUGH_GRADIENT, 1, 50, 10, 5, min_radius, max_radius);

	cv::Point2f best_point {};
	if (!circles.empty()) {
		float best_radius = 0;
		int area_max = 0;
		for (const auto& circle : circles) {
			static cv::UMat mask_color;
			cv::inRange(hsv, hsv_laser.lower, hsv_laser.upper, mask_color);
			static cv::UMat mask_circle = cv::UMat::zeros(mask.rows, mask.cols, mask.type());
			mask_circle.setTo(cv::Scalar(0, 0, 0));
			cv::circle(mask_circle, cv::Point(static_cast<int>(circle[0]), static_cast<int>(circle[1])),
				static_cast<int>(circle[2] * 1.5), cv::Scalar(255, 255, 255), -1);
			// 这个mask_combined是可能的激光点周围经过hsv range后的区域,目的是筛选出可能的激光点周围颜色最符合要求的点
			cv::UMat mask_combined;
			cv::bitwise_or(mask_color, mask_circle, mask_combined);
			auto area = cv::countNonZero(mask_combined);
			if (area > area_max) {
				area_max = area;
				best_point = cv::Point2f(circle[0], circle[1]);
				best_radius = static_cast<float>(circle[2]);
			}
		}
		if (draw_frame != nullptr) {
			cv::circle(*draw_frame, best_point, static_cast<int>(best_radius), cv::Scalar(0, 0, 255), 2);
			cv::putText(*draw_frame, "Laser", best_point, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
		}
		laser_info_obj = Tracker::ObjectInfo {
			.position = best_point,
			.velocity = cv::Vec2f(0, 0),
			.radius = best_radius,
			.type = Tracker::ObjectType::LaserPoint,
		};
	} else {
		spdlog::warn("No laser detected.");
	}
	
	return laser_info_obj;
}

cv::UMat getLaserTrace(const cv::UMat& frame, cv::UMat* draw_frame,
		const HSVRange& hsv_range) {
	constexpr auto color = cv::Scalar(0, 0, 255);
	static cv::UMat last_trace;
	cv::UMat hsv;
	cv::UMat laser_trace = cv::UMat::zeros(frame.rows, frame.cols, CV_8UC1);
	cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
	cv::inRange(hsv, hsv_range.lower, hsv_range.upper, laser_trace);
	cv::bitwise_or(laser_trace, laser_trace, laser_trace);
	last_trace = laser_trace;
	
	if (draw_frame != nullptr) {
		static cv::UMat laser_trace_color = cv::UMat::zeros(draw_frame->rows, draw_frame->cols, CV_8UC3);
		laser_trace_color.setTo(color, laser_trace);
		cv::addWeighted(*draw_frame, 1, laser_trace_color, 0.5, 0.0, *draw_frame);
	}
	return laser_trace;
};



