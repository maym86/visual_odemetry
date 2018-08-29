

#include "feature_detector.h"

FeatureDetector::FeatureDetector(){
    gpu_detector_ = cv::cuda::FastFeatureDetector::create(20, true, cv::FastFeatureDetector::TYPE_9_16, kMaxFeatures);
}

void FeatureDetector::detect(VOFrame *frame) {
    std::vector<cv::KeyPoint> keypoints;

    gpu_detector_->detect(frame->gpu_image, keypoints);

    frame->points.clear();
    for(const auto &kp : keypoints){
        frame->points.push_back(kp.pt);
    }
}


void FeatureDetector::compute(VOFrame *frame) {

    std::vector<cv::KeyPoint> keypoints;
    for (const auto &p : frame->points ){

        cv::KeyPoint kp;
        kp.pt = p;
        keypoints.push_back(kp);
    }

    gpu_detector_->compute(frame->image, keypoints, frame->descriptors);
}