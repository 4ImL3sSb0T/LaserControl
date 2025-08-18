//
// Created by yuang on 25-8-10.
//

#ifndef TRACK_TARGET_H
#define TRACK_TARGET_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include <future>

#include "opencv/roi_extractor.h"

class Tracker {
public:
	enum class ObjectType {
		LaserPoint,
		PaperCenter,
		None
	};

	struct ObjectInfo {
		cv::Point2f position;
		cv::Vec2f velocity;
		float radius;
		ObjectType type;
	};

	// 删除构造函数，使用getInstance替代
	static Tracker& getInstance(const int index);
	Tracker(const Tracker&) = delete;
	Tracker& operator=(const Tracker&) = delete;
	void m_opencv_task();
	void getObjectList(std::vector<ObjectInfo>& list) const;
	// void createTask() const;
	
private:
	Tracker(const int index);
	~Tracker();
	Tracker() = delete;



	std::vector<ObjectInfo> m_objects {};

	ROIExtractor m_extractor;

	cv::Mat m_frame;
	cv::Mat m_draw;
	cv::VideoCapture m_cap;

	cv::Point2f getLaserPos(const cv::Mat& frame, cv::Mat* draw_frame);
	cv::Mat getLaserTrace(const cv::Mat& frame, cv::Mat* draw_frame);
};

#endif //TRACK_TARGET_H
