

#include "feature_detector.h"

FeatureDetector::FeatureDetector(){
    gpu_detector_ = cv::cuda::FastFeatureDetector::create(20, true, cv::FastFeatureDetector::TYPE_9_16, kMaxFeatures);
    descriptor_ = cv::cuda::ORB::create(2000);

    akaze_ = cv::AKAZE::create();
    brisk_ = cv::BRISK::create();

}

void FeatureDetector::detectFAST(VOFrame *frame) {
    std::vector<cv::KeyPoint> keypoints;

    gpu_detector_->detect(frame->gpu_image, keypoints);

    for(const auto &kp : keypoints){
        frame->points.push_back(kp.pt);
    }
}


void FeatureDetector::detectFAST(const VOFrame &frame, std::vector<cv::KeyPoint> *keypoints) {
    gpu_detector_->detect( frame.gpu_image, *keypoints);
}



void FeatureDetector::detectComputeORB(const VOFrame &frame, std::vector<cv::KeyPoint> *keypoints, cv::Mat *descriptors){
    cv::cuda::GpuMat descriptors_gpu;
    descriptor_->detectAndCompute(frame.gpu_image, cv::noArray(), *keypoints, descriptors_gpu);
    descriptors_gpu.download(*descriptors);
}


void FeatureDetector::computeORB(const VOFrame &frame, std::vector<cv::KeyPoint> *keypoints, cv::Mat *descriptors){
    cv::cuda::GpuMat descriptors_gpu;

    keypoints->clear();
    for (const auto &p: frame.points){
        cv::KeyPoint kp;
        kp.pt = p;
        keypoints->push_back(kp);
    }

    descriptor_->compute(frame.gpu_image, *keypoints, descriptors_gpu);
    descriptors_gpu.download(*descriptors);
}

void FeatureDetector::detectComputeAKAZE(const VOFrame &frame, std::vector<cv::KeyPoint> *keypoints, cv::Mat *descriptors){
    akaze_->detectAndCompute(frame.image, cv::noArray(), *keypoints, *descriptors);
}


void FeatureDetector::detectComputeBRISK(const VOFrame &frame, std::vector<cv::KeyPoint> *keypoints, cv::Mat *descriptors){
    brisk_->detectAndCompute(frame.image, cv::noArray(), *keypoints, *descriptors);
}

