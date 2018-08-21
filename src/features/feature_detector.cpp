#include "feature_detector.h"


FeatureDetector::FeatureDetector(){
    gpu_detector_ = cv::cuda::FastFeatureDetector::create(30, true, cv::FastFeatureDetector::TYPE_9_16, kMaxFeatures);
}

std::vector<cv::Point2f> FeatureDetector::detect(const cv::cuda::GpuMat &image_gpu) {
    std::vector<cv::KeyPoint> keypoints;

    gpu_detector_->detect(image_gpu, keypoints);

    std::vector<cv::Point2f> results;

    for(const auto &kp : keypoints){
        results.push_back(kp.pt);
    }
    return results;
}
