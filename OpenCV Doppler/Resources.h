#pragma once
#include <Windows.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

struct Resources {
public:
	static std::string GetWorkingDir();
	static cv::Mat LoadImg(const char* imgName);


};
