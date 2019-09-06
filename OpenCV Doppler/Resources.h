#pragma once

#include <Windows.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

inline std::pair<float, float> operator *(std::pair<float, float> p, int i) {
	std::pair<float, float> pair = std::make_pair<float, float>(p.first * i, p.second * i);
	return std::make_pair<float, float>(p.first * i, p.second * i);
}

inline void operator +=(std::pair<float, float> &p1, std::pair<float, float> p2) {
	//overloaded incorrectly
	p1 = std::pair<float, float>(p1.first + p2.first, p1.second + p2.second);
}

struct Resources {
public:
	static std::string GetWorkingDir();
	static cv::Mat LoadImg(const char* imgName);
	static std::pair<float, float> PointToPair(cv::Point point);
	static cv::Point PairToPoint(std::pair<float, float> pair);
};
