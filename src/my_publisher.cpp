#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <camera_info_manager/camera_info_manager.h>
#include <sensor_msgs/CameraInfo.h>
#include <camera_calibration_parsers/parse.h>
// using namespace  camera_info_manager;
using namespace camera_calibration_parsers;
int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it_l(nh);
  image_transport::ImageTransport it_r(nh);
  camera_info_manager::CameraInfoManager cinfo_(nh);
  
  image_transport::CameraPublisher pub_l = it_l.advertiseCamera("/stereo/left/image_raw", 1);
  image_transport::CameraPublisher pub_r = it_r.advertiseCamera("/stereo/right/image_raw", 1);

  sensor_msgs::CameraInfoPtr ci(new sensor_msgs::CameraInfo(cinfo_.getCameraInfo()));
//  ci.reset(new sensor_msgs::CameraInfo());
  
  cv::Mat image_l = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  cv::Mat image_r = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);
  sensor_msgs::ImagePtr msg_l = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image_l).toImageMsg();
  sensor_msgs::ImagePtr msg_r = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image_r).toImageMsg();

  ci->height = msg_l->height;
  ci->width = msg_l->width;

  ros::Rate loop_rate(3);
  while (nh.ok()) {
    pub_l.publish(msg_l, ci);
    pub_r.publish(msg_r, ci);
    ros::spinOnce();
    loop_rate.sleep();
  }
}

