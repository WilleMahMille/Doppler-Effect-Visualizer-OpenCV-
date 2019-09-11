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

std::map<float, cv::Scalar>* Resources::GetWavelengthToRgbMap(float accuracy) {
	std::map<float, cv::Scalar> *map = new std::map<float, cv::Scalar>();

	const int base = 380; //280 nM
	for (int i = 0; i < 400 * accuracy; i++) {
		map->insert(std::make_pair<float, cv::Scalar>(base + i / accuracy, GetRgbFromWavelength(base + i / accuracy)));
		std::cout << "test\n";
	}
	return map;
}

cv::Scalar Resources::GetRgbFromWavelength(float wavelength) {
	cv::Scalar bgr = cv::Scalar(0, 0, 0); //bgr is opencv's standard (and windows')
	float gamma = 0.8;

	if (wavelength >= 380 && wavelength <= 440) {
		float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
		bgr[2] = pow((-(wavelength - 440) / (440 - 380)) * attenuation, gamma);
		//green = 0
		bgr[0] = pow(attenuation, gamma);
	}
	else if (wavelength >= 440 && wavelength <= 490) {
		//red = 0
		bgr[1] = pow((wavelength - 440) / (490 - 440), gamma);
		bgr[2] = 1;
	}
	else if (wavelength >= 490 && wavelength <= 510) {
		//red = 0
		bgr[1] = 1;
		bgr[2] = pow((-wavelength - 510) / (510 - 490), gamma);
	}
	else if (wavelength >= 510 && wavelength <= 580) {
		bgr[2] = pow((wavelength - 510) / (580 - 510), gamma);
		bgr[1] = 1;
		//blue = 0
	}
	else if (wavelength >= 580 && wavelength <= 645) {
		bgr[2] = 1;
		bgr[1] = pow((-wavelength - 645) / (645 - 580), gamma);
		//blue = 0
	}
	else if (wavelength >= 645 && wavelength <= 750) {
		float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
		bgr[2] = pow(attenuation, gamma);
		//green = 0
		//blue = 0
	}
	bgr *= 255;
	return bgr;
}

std::pair<float, float> Resources::PointToPair(cv::Point point) {
	return std::pair<float, float>(point.x, point.y);
}
cv::Point Resources::PairToPoint(std::pair<float, float> pair) {
	return cv::Point(static_cast<int>(pair.first), static_cast<int>(pair.second));
}

float Resources::GetWavelengthFromVelocity(std::pair<float, float> position, cv::Point velocity) {


	float absVel = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	float wavelength = standardWavelength * (c - absVel) / (c);
	return wavelength;

}