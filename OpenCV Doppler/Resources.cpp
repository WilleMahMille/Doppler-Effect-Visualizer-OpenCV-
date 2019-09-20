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
		img = cv::imread(workingDir + "..\\x64\\Debug\\" + imgName, CV_8UC4);
		if (img.empty()) {
			std::cout << "Image does not exist\n";
			img = cv::Mat::zeros(cv::Size(1, 1), CV_8UC4);
		}
	}
	return img;
}

std::map<float, cv::Scalar>* Resources::GetWavelengthToRgbMap(float accuracy) {
	std::map<float, cv::Scalar> *map = new std::map<float, cv::Scalar>();

	const int base = 380; //380 nM
	for (int i = 0; i < 400 * accuracy; i++) {
		map->insert(std::make_pair<float, cv::Scalar>(base + i / accuracy, GetRgbFromWavelength(base + i / accuracy)));
	}
	return map;
}

cv::Scalar Resources::GetRgbFromWavelength(float wavelength) {
	float gamma = 0.8;
	float red = 0;
	float green = 0;
	float blue = 0;
	if (wavelength >= 380 && wavelength <= 440) {
		float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
		red = pow((-(wavelength - 440) / (440 - 380)) * attenuation, gamma);
		//green = 0
		blue = pow(attenuation, gamma);
	}
	else if (wavelength >= 440 && wavelength <= 490) {
		//red = 0
		green = pow((wavelength - 440) / (490 - 440), gamma);
		blue = 1;
	}
	else if (wavelength >= 490 && wavelength <= 510) {
		//red = 0
		green = 1;
		blue = pow(-(wavelength - 510) / (510 - 490), gamma);
		
	}
	else if (wavelength >= 510 && wavelength <= 580) {
		red = pow((wavelength - 510) / (580 - 510), gamma);
		green = 1;
		//blue = 0
	}
	else if (wavelength >= 580 && wavelength <= 645) {
		red = 1;
		green = pow(-(wavelength - 645) / (645 - 580), gamma);
		//blue = 0
	}
	else if (wavelength >= 645 && wavelength <= 750) {
		float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
		red = pow(attenuation, gamma);
		//green = 0
		//blue = 0
	}
	
	cv::Scalar bgr = cv::Scalar(blue, green, red); //bgr is opencv's standard (and windows')
	bgr *= 255;
	return bgr;
}

std::pair<float, float> Resources::PointToPair(cv::Point point) {
	return std::pair<float, float>(point.x, point.y);
}
cv::Point Resources::PairToPoint(std::pair<float, float> pair) {
	return cv::Point(static_cast<int>(pair.first), static_cast<int>(pair.second));
}

float Resources::GetWavelengthFromVelocity(std::pair<float, float> deltaPosition, cv::Point velocity) {
	//needs a different implementation
	//still incorrect,  but probably less incorrect
	
	float vFinalX = deltaPosition.first * velocity.x;
	float vFinalY = -deltaPosition.second * velocity.y;

	float relativeVelocity = vFinalX + vFinalY;
	
	relativeVelocity *= multiplier;
	float wavelength = standardWavelength * sqrt((1 - relativeVelocity / c) / (1 + relativeVelocity / c));
	//I believe the problem is the wavelength equation

	return wavelength;
}