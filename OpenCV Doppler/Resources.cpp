#include "Resources.h"


std::string Resources::GetWorkingDir() {
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf);
}

cv::Mat Resources::LoadImg(const char* imgName) {
	cv::String workingDir = GetWorkingDir() + "\\";
	cv::Mat img = cv::imread(workingDir + imgName);
	if (img.empty()) {
		img = cv::imread(workingDir + "..\\x64\\Debug\\" + imgName);
		if (img.empty()) {
			std::cout << "Image does not exist\n";
			img = cv::Mat::zeros(cv::Size(1, 1), CV_8UC3);
		}
	}
	return img;
}