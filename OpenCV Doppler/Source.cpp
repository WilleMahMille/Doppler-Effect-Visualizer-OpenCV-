#include "Simulation.h"

int main() {

	cv::Mat img1, img2;
	img1 = cv::Mat::zeros(cv::Size(500, 500), CV_8UC3);
	img2 = cv::Mat::ones(cv::Size(50, 50), CV_8UC3);
	cv::multiply(img2, cv::Scalar::all(255), img2, 10);
	cv::Mat insetImage(img1, cv::Rect(40, 40, 50, 50));
	img2.copyTo(insetImage);
	cv::imshow("test", img1);
	cv::waitKey(0);
	cv::waitKey(5000);

	WaveSimulation waves = WaveSimulation();
	waves.RunSimulation();

	
}

