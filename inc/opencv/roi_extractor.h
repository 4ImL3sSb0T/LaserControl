#ifndef ROI_EXTRACTOR_H
#define ROI_EXTRACTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <optional>

struct ROIInfo {
    cv::Point2i center;
    double area;
    std::vector<cv::Point> approx;
    cv::Rect bounds;
    
    ROIInfo() : center(0, 0), area(0.0) {}
    ROIInfo(cv::Point2i c, double a, const std::vector<cv::Point>& ap, const cv::Rect& b)
        : center(c), area(a), approx(ap), bounds(b) {}
};

struct ROIResult {
    cv::UMat roi_image;
    std::optional<ROIInfo> roi_info;
    
    ROIResult() {}
    ROIResult(const cv::UMat& img, const std::optional<ROIInfo>& info)
        : roi_image(img), roi_info(info) {}
};

class ROIExtractor {
private:
    double min_area_;
    double crop_percent_;
    std::optional<cv::Point2i> prev_center_;
    
    // 缓存常用的结构元素，避免重复创建
    cv::UMat morph_kernel_;
    
    struct Candidate {
        std::vector<cv::Point> approx;
        cv::Point2i center;
        double area;
        
        Candidate(const std::vector<cv::Point>& ap, const cv::Point2i& c, double a)
            : approx(ap), center(c), area(a) {}
    };
    
    std::optional<Candidate> selectBestCandidate(const std::vector<Candidate>& candidates);

public:
    explicit ROIExtractor(double min_area = 50000.0, double crop_percent = 0.01);
    
    ROIResult extractROI(const cv::UMat& frame, cv::UMat* draw_frame = nullptr, int gray_threshold = 120);
    void reset();
};

// 工具函数
cv::UMat cropROIBorder(const cv::UMat& img, double percent = 0.1);
std::pair<bool, std::vector<cv::Point>> isApproxRect(const std::vector<cv::Point>& contour, double epsilon_factor = 0.02);
std::optional<cv::Point2i> calcCenter(const std::vector<cv::Point>& approx);
double distance(const cv::Point2i& p1, const cv::Point2i& p2);
ROIResult extractROISimple(const cv::UMat& frame, double min_area = 50000.0, double crop_percent = 0.03);
    
#endif // ROI_EXTRACTOR_H