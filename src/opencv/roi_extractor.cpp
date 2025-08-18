#include "opencv/roi_extractor.h"
#include <algorithm>
#include <cmath>

cv::Mat cropROIBorder(const cv::Mat& img, double percent) {
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

double distance(const cv::Point2i& p1, const cv::Point2i& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

ROIExtractor::ROIExtractor(double min_area, double crop_percent)
    : min_area_(min_area), crop_percent_(crop_percent) {
    reset();
}

ROIResult ROIExtractor::extractROI(const cv::Mat& frame, cv::Mat* draw_frame, int gray_threshold) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    
    cv::Mat binary;
    cv::threshold(gray, binary, gray_threshold, 255, cv::THRESH_BINARY_INV);
    
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat closed;
    cv::morphologyEx(binary, closed, cv::MORPH_CLOSE, kernel);
    cv::bitwise_not(closed, closed);
    
    // Debug显示
    cv::imshow("closed", binary);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(closed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // 绘制所有轮廓用于调试
    if (draw_frame) {
        cv::drawContours(*draw_frame, contours, -1, cv::Scalar(0, 255, 0), 2);
    }
    
    std::vector<Candidate> candidates;
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
        return ROIResult(cv::Mat(), std::nullopt);
    }
    
    auto selected = selectBestCandidate(candidates);
    
    if (selected) {
        // 计算边界框
        cv::Rect bounding_rect = cv::boundingRect(selected->approx);
        
        // 应用裁切百分比
        int crop_w = static_cast<int>(bounding_rect.width * crop_percent_);
        int crop_h = static_cast<int>(bounding_rect.height * crop_percent_);
        
        cv::Rect roi_rect(
            std::max(0, bounding_rect.x - crop_w),
            std::max(0, bounding_rect.y - crop_h),
            std::min(frame.cols - std::max(0, bounding_rect.x - crop_w), bounding_rect.width + 2 * crop_w),
            std::min(frame.rows - std::max(0, bounding_rect.y - crop_h), bounding_rect.height + 2 * crop_h)
        );
        
        cv::Mat roi = frame(roi_rect);
        
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
        return ROIResult(cv::Mat(), std::nullopt);
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
        // 有历史中心，选择距离最近的
        auto min_dist_it = std::min_element(candidates.begin(), candidates.end(),
            [this](const Candidate& a, const Candidate& b) {
                double dist_a = distance(a.center, *prev_center_);
                double dist_b = distance(b.center, *prev_center_);
                return dist_a < dist_b;
            });
        return *min_dist_it;
    }
}

void ROIExtractor::reset() {
    prev_center_.reset();
}

ROIResult extractROISimple(const cv::Mat& frame, double min_area, double crop_percent) {
    ROIExtractor extractor(min_area, crop_percent);
    return extractor.extractROI(frame);
}
