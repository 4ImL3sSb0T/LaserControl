//
// Created by yuang on 25-8-10.
//

#ifndef TRACK_TARGET_H
#define TRACK_TARGET_H

#include <opencv2/opencv.hpp>
#include <vector>

class Tracker {
public:
	enum class ObjectType {
		LaserPoint,
		PaperCenter,
		None
	};

	struct ObjectInfo {
		cv::Vec2f position;
		cv::Vec2f velocity;
		float radius;
		ObjectType type;
	};

	// 删除构造函数，使用getInstance替代
	static Tracker& getInstance(int index);
	Tracker(const Tracker&) = delete;
	Tracker& operator=(const Tracker&) = delete;
	
	void createTask() const;
	ObjectInfo getObjectInfo(ObjectType type);
	
private:
	Tracker(int index);
	~Tracker();
	Tracker() = delete;

	void m_opencv_task() const;

	std::vector<ObjectInfo> m_objects;

	static cv::Mat m_frame;
	static cv::Mat m_draw;
	static cv::VideoCapture m_cap;
};

#endif //TRACK_TARGET_H
