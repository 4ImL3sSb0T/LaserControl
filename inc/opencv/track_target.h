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
#include "IntervalTimer.h"

namespace ComputerVision {
	struct HSVRange {
		cv::Scalar lower;
		cv::Scalar upper;
	};
	enum class ObjectType {
		None,
		PaperCenter,
		LaserPoint
	};

	struct ObjectInfo {
		cv::Point2f position {0, 0};
		cv::Vec2f velocity {0, 0};
		float radius = 0.0f;
		ObjectType type = ObjectType::None;
		cv::Scalar color = {0, 0 ,0};
	};
	cv::Scalar hsvToBgrAverage(const cv::Scalar& lower, const cv::Scalar& upper);

	class Tracker {
	public:
		// 删除构造函数，使用getInstance替代
		static Tracker& getInstance(const int index);
		Tracker(const Tracker&) = delete;
		Tracker& operator=(const Tracker&) = delete;
		void m_opencv_task();
		void getObjectList(std::vector<ObjectInfo>& list) const;
		// void createTask() const;
		
		HSVRange laser_center_range {{0, 0, 160}, {255, 255, 255}};
		HSVRange uv_around_range {{139, 61, 62}, {148, 255, 207}};
		HSVRange red_around_range {{0, 0, 0}, {255, 255, 255}};
	private:
		Tracker(const int index);
		~Tracker();
		Tracker() = delete;
		
		std::vector<ObjectInfo> m_objects {};
		ROIExtractor m_extractor;
		IntervalTimer m_paper_time {5};
		IntervalTimer m_frame_fps {5};

		cv::UMat m_frame;
		cv::UMat m_draw;
		cv::VideoCapture m_cap;

		static ObjectInfo getLaserPos(const cv::UMat &frame, cv::UMat *draw_frame,
		                              const HSVRange& hsv_laser_center, const HSVRange& hsv_laser_around,
		                              int min_radius = 5, int max_radius = 150);

		static cv::UMat getLaserTrace(const cv::UMat& frame, cv::UMat* draw_frame,
		                              const HSVRange& hsv_range);
	};
	
}


#endif //TRACK_TARGET_H
