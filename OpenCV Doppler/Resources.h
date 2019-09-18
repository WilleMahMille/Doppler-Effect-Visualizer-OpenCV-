#pragma once

#include <Windows.h>
#include <iostream>
#include <iterator>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
constexpr float standardWavelength = 550;
constexpr float c = 299792458;
constexpr float multiplier = c / 50;

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
	static std::map<float, cv::Scalar>* GetWavelengthToRgbMap(float accuracy);
	//accuracy, how many colors per nm in wavelength, accuracy of 2 means 2 colors per nm, 1 per 0.5nm
	static cv::Scalar GetRgbFromWavelength(float wavelength);
	static float GetWavelengthFromVelocity(std::pair<float, float> deltaPosition, cv::Point velocity); //position required

};
