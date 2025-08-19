#include "opencv/roi_extractor.h"
#include <algorithm>
#include <cmath>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

// 在应用程序开始时初始化OpenCL
static bool initOpenCL() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        if (!cv::ocl::haveOpenCL()) {
            spdlog::warn("OpenCL not available");
            return false;
        }
        cv::ocl::setUseOpenCL(true);
        spdlog::info("OpenCL available : {} ", cv::ocl::useOpenCL());
        // 显示OpenCL设备信息
        const cv::ocl::Device& device = cv::ocl::Device::getDefault();
        if (!device.empty()) {
            spdlog::info("Using GPU device : {}", device.name());
        }
    }
    return cv::ocl::useOpenCL();
}

cv::UMat cropROIBorder(const cv::UMat& img, double percent) {
    int h = img.rows;
    int w = img.cols;
    int top = static_cast<int>(h * percent);
    int bottom = static_cast<int>(h * (1 - percent));
    int left = static_cast<int>(w * percent);
    int right = static_cast<int>(w * (1 - percent));
    
    return img(cv::Range(top, bottom), cv::Range(left, right));
}

std::pair<bool, std::vector<cv::Point>> isApproxRect(const std::vector<cv::Point>& contour, double epsilon_factor) {
    double peri = cv::arcLength(contour, true);
    std::vector<cv::Point> approx;
    approx.reserve(8); // 预分配内存，矩形轮廓通常不超过8个点
    cv::approxPolyDP(contour, approx, epsilon_factor * peri, true);

    bool is_rect = (approx.size() >= 4 && approx.size() <= 5) && cv::isContourConvex(approx);
    return std::make_pair(is_rect, approx);
}

std::optional<cv::Point2i> calcCenter(const std::vector<cv::Point>& approx) {
    cv::Moments M = cv::moments(approx);
    if (M.m00 == 0) {
        return std::nullopt;
    }
    int x = static_cast<int>(M.m10 / M.m00);
    int y = static_cast<int>(M.m01 / M.m00);
    return cv::Point2i(x, y);
}

// 使用更快的距离计算（避免开方运算）
double distanceSquared(const cv::Point2i& p1, const cv::Point2i& p2) {
    return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2);
}

double distance(const cv::Point2i& p1, const cv::Point2i& p2) {
    return std::sqrt(distanceSquared(p1, p2));
}

ROIExtractor::ROIExtractor(double min_area, double crop_percent)
    : min_area_(min_area), crop_percent_(crop_percent) {
    reset();
    
    // 初始化OpenCL
    initOpenCL();
    
    // 预创建形态学操作的结构元素，避免重复创建
    cv::Mat kernel_cpu = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    morph_kernel_ = kernel_cpu.getUMat(cv::ACCESS_READ);
}

ROIResult ROIExtractor::extractROI(const cv::UMat& frame, cv::UMat* draw_frame, int gray_threshold) {
    static cv::UMat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    
    static cv::UMat binary;
    cv::threshold(gray, binary, gray_threshold, 255, cv::THRESH_BINARY_INV);
    
    // 使用缓存的结构元素
    static cv::UMat closed;
    cv::morphologyEx(binary, closed, cv::MORPH_CLOSE, morph_kernel_);
    cv::bitwise_not(closed, closed);
    
    // 调试显示 - 注意：这需要从GPU内存下载数据
    if (cv::ocl::useOpenCL()) {
        cv::Mat closed_mat;
        closed.copyTo(closed_mat);
        cv::imshow("closed", closed_mat);
    } else {
        cv::imshow("closed", closed.getMat(cv::ACCESS_READ));
    }
    
    std::vector<std::vector<cv::Point>> contours;
    // 轮廓查找需要在CPU上进行
    cv::Mat closed_cpu;
    closed.copyTo(closed_cpu);
    cv::findContours(closed_cpu, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // 绘制所有轮廓用于调试
    if (draw_frame) {
        cv::drawContours(*draw_frame, contours, -1, cv::Scalar(0, 255, 0), 2);
    }
    
    std::vector<Candidate> candidates;
    candidates.reserve(contours.size()); // 预分配内存提高性能
    
    // 过滤和处理轮廓
    for (const auto& cnt : contours) {
        double area = cv::contourArea(cnt);
        if (area < min_area_) continue;
        
        auto [is_rect, approx] = isApproxRect(cnt, 0.02);
        if (!is_rect) continue;
        
        auto center_opt = calcCenter(approx);
        if (!center_opt) continue;
        
        candidates.emplace_back(approx, *center_opt, area);
    }
    
    if (candidates.empty()) {
        return ROIResult(cv::UMat(), std::nullopt);
    }
    
    auto selected = selectBestCandidate(candidates);
    
    if (selected) {
        // 计算边界框
        cv::Rect bounding_rect = cv::boundingRect(selected->approx);
        
        // 应用裁切百分比
        int crop_w = static_cast<int>(bounding_rect.width * crop_percent_);
        int crop_h = static_cast<int>(bounding_rect.height * crop_percent_);
        
        // 优化边界检查
        int roi_x = std::max(0, bounding_rect.x - crop_w);
        int roi_y = std::max(0, bounding_rect.y - crop_h);
        int roi_width = std::min(frame.cols - roi_x, bounding_rect.width + 2 * crop_w);
        int roi_height = std::min(frame.rows - roi_y, bounding_rect.height + 2 * crop_h);
        
        cv::Rect roi_rect(roi_x, roi_y, roi_width, roi_height);
        
        cv::UMat roi = frame(roi_rect);
        
        // 绘制检测结果
        if (draw_frame) {
            std::vector<std::vector<cv::Point>> approx_contours = {selected->approx};
            cv::drawContours(*draw_frame, approx_contours, -1, cv::Scalar(0, 255, 255), 3);
            cv::circle(*draw_frame, selected->center, 10, cv::Scalar(255, 0, 0), -1);
            cv::rectangle(*draw_frame, roi_rect, cv::Scalar(255, 255, 0), 2);
        }
        
        ROIInfo roi_info(selected->center, selected->area, selected->approx, roi_rect);
        prev_center_ = selected->center;
        
        return ROIResult(roi, roi_info);
    } else {
        return ROIResult(cv::UMat(), std::nullopt);
    }
}

std::optional<ROIExtractor::Candidate> ROIExtractor::selectBestCandidate(const std::vector<Candidate>& candidates) {
    if (candidates.empty()) {
        return std::nullopt;
    }
    
    if (!prev_center_) {
        // 没有历史中心，选择面积最大的
        auto max_area_it = std::max_element(candidates.begin(), candidates.end(),
            [](const Candidate& a, const Candidate& b) {
                return a.area < b.area;
            });
        return *max_area_it;
    } else {
        // 有历史中心，选择距离最近的（使用平方距离避免开方运算）
        auto min_dist_it = std::min_element(candidates.begin(), candidates.end(),
            [this](const Candidate& a, const Candidate& b) {
                double dist_a_sq = distanceSquared(a.center, *prev_center_);
                double dist_b_sq = distanceSquared(b.center, *prev_center_);
                return dist_a_sq < dist_b_sq;
            });
        return *min_dist_it;
    }
}

void ROIExtractor::reset() {
    prev_center_.reset();
}

ROIResult extractROISimple(const cv::UMat& frame, double min_area, double crop_percent) {
    ROIExtractor extractor(min_area, crop_percent);
    return extractor.extractROI(frame);
}