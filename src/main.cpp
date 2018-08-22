
#include "main.h"

#include <boost/range/iterator_range.hpp>

#include "src/visual_odemetry/visual_odemetry.h"
#include "src/kitti/kitti.h"

using namespace boost::filesystem;

int main(int argc, char *argv[]) {

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;


    std::string data_dir = FLAGS_data_dir + "/" + FLAGS_seq + "/";

    //Iterate through directory
    path p(data_dir + FLAGS_image_dir);

    if (!is_directory(p)) {
        LOG(INFO) << data_dir + FLAGS_image_dir + " is not a directory";
        return 0;
    }

    std::vector<std::string> file_names;
    for (auto &entry : boost::make_iterator_range(directory_iterator(p), {})) {
        file_names.push_back(entry.path().string());
    }
    std::sort(file_names.begin(), file_names.end());

    cv::Mat output;

    cv::Mat intrinsics = loadKittiCalibration(data_dir + FLAGS_calib_file, FLAGS_calib_line_number);
    LOG(INFO) << "Camera matrix: " << intrinsics;

    double focal = intrinsics.at<double>(0, 0);
    cv::Point2d pp(intrinsics.at<double>(0, 2), intrinsics.at<double>(1, 2));

    LOG(INFO) << "Focal length " << focal << ", principal point: " << pp;


    //Load ground truth
    std::vector<Matrix> result_poses;

    cv::Mat map(1500, 1500, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat_<double> pose = cv::Mat::eye(4, 3, CV_64FC1);

    bool done = false;

    VisualOdemetry vo(focal, pp);

    for (const auto &file_name : file_names) {

        cv::Mat image = cv::imread(file_name);

        vo.addImage(image, &pose);

        result_poses.emplace_back(kittiResultMat(pose));

        cv::Point2d draw_pos = cv::Point2d(pose.at<double>(0,3) + map.cols / 2, -pose.at<double>(2,3) + map.rows / 2);
        cv::circle(map, draw_pos, 2, cv::Scalar(255, 0, 0), 2);

        cv::imshow("Map", map);
        cv::imshow("Features", vo.drawMatches(image));

        char key = static_cast<char>(cv::waitKey(1));
        if (key == 27) {
            done = true;
            break;
        }
    }

    std::string res_dir = FLAGS_res_dir + "/" + FLAGS_seq;
    std::string gt_poses_path = FLAGS_poses + "/" + FLAGS_seq + ".txt";

    saveResults(gt_poses_path, res_dir, FLAGS_seq, result_poses);

    while (!done) {
        char key = static_cast<char>(cv::waitKey(33));
        if (key == 27) {
            done = true;
        }
    }


}