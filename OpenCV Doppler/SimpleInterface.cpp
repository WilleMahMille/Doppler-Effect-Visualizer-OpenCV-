#include "SimpleInterface.h"
#include <Windows.h>


void OnMouseClick(int evnt, int x, int y, int flags, void* userData) {
	if (flags == cv::EVENT_FLAG_LBUTTON || evnt == cv::EVENT_LBUTTONUP ) {

		ControlPanel* panel = reinterpret_cast<ControlPanel*>(userData);
		panel->Click(cv::Point(x, y), evnt);
	}
}

CheckBox::CheckBox(cv::Point position, cv::Point size, bool* value) : _position(position), _size(size), _value(value) {
	_checkedImg = cv::Mat::zeros(cv::Size(50, 50), CV_8UC3);
	cv::resize(Resources::LoadImg("checked.png"), _checkedImg, cv::Size(50, 50));
	//cv::Mat insetImg(img, cv::Rect(_position.x, _position.y, _position.x + _size.x, _position.y + _size.y));
	
	_uncheckedImg = cv::Mat::zeros(cv::Size(50, 50), CV_8UC3);
	cv::resize(Resources::LoadImg("unchecked.png"), _uncheckedImg, cv::Size(50, 50));
}
bool CheckBox::IsClicked(cv::Point click) {
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void CheckBox::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y || img.cols < _position.x + _size.x) {
		std::cout << "Error, button out of bounds\n";
		return;
	}
	if (*_value) {
		_checkedImg.copyTo(img(cv::Rect(_position.x, _position.y, 50, 50)));
	}
	else {
		_uncheckedImg.copyTo(img(cv::Rect(_position.x, _position.y, 50, 50)));
	}
}
void CheckBox::Click(cv::Point position, int type) {
	*_value = type == cv::EVENT_LBUTTONUP ? !*_value : *_value;
}

TwoDTrackbar::TwoDTrackbar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, cv::Mat selector, double* valOne, double* valTwo) : _position(position), _size(size), _bottomNumber(bottomNumber), _valOne(valOne), _valTwo(valTwo) {
	_scale = cv::Point((topNumber.x - bottomNumber.x) / size.x, (topNumber.y - bottomNumber.y) / size.y);
	trackbarImg = cv::Mat::zeros(size, CV_8UC3);
	cv::Point center = cv::Point((topNumber.x - bottomNumber.x) / 2, (topNumber.y - bottomNumber.y) / 2); //incorrect

	cv::line(trackbarImg, cv::Point(0, center.y), cv::Point(size.x, center.y), cv::Scalar(255, 255, 255));
	cv::line(trackbarImg, cv::Point(center.x, 0), cv::Point(center.x, size.y), cv::Scalar(255, 255, 255));
	for (int i = bottomNumber.x; i <= topNumber.x; i++) {
		int lineSize = 2 + (i % 10 == 0 ? 2 : 0) + (i % 5 == 0 ? 2 : 0);
		cv::line(trackbarImg, cv::Point(i * _scale.x, center.y + lineSize), cv::Point(i * _scale.x, center.y - lineSize), cv::Scalar(255, 255, 255));
	}
	for (int i = bottomNumber.y; i <= topNumber.y; i++) {
		int lineSize = 2 + (i % 10 == 0 ? 2 : 0) + (i % 5 == 0 ? 2 : 0);
		cv::line(trackbarImg, cv::Point(center.x + lineSize, i * _scale.y), cv::Point(center.x - lineSize, i * _scale.y), cv::Scalar(255, 255, 255));
	}
	cv::imshow("trackbar", trackbarImg);

}

void TwoDTrackbar::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y) {
		std::cout << "Error, button out of bounds\n";
		return;
	}

}


ControlPanel::ControlPanel(const char* windowName, cv::Size size) : _windowName(windowName), _size(size) {
	cv::namedWindow(_windowName);
	cv::resizeWindow(_windowName, _size);
	cv::setMouseCallback(_windowName, OnMouseClick, reinterpret_cast<void*>(this));

}
void ControlPanel::Click(cv::Point position, int type) {
	for (int i = inputs.size() - 1; i >= 0; i--) {
		if (inputs[i]->IsClicked(position)) {
			inputs[i]->Click(position, type);
		}
	}
	Draw();
}
void ControlPanel::Draw() {
	cv::Mat img = cv::Mat::zeros(_size, CV_8UC3);
	for (int i = 0; i < inputs.size(); i++) {
		inputs[i]->Draw(img);
	}
	cv::imshow(_windowName, img);
}
void ControlPanel::AddCheckBox(cv::Point position, int layer, bool* value, cv::Point size) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return;
	}
	inputs.insert(inputs.begin() + layer, new CheckBox(position, size, value));
}
void ControlPanel::AddTwoDTrackBar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int layer, double* valueOne, double* valueTwo) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return;
	}
	inputs.insert(inputs.begin() + layer, new TwoDTrackbar(position, size, bottomNumber, topNumber, Resources::LoadImg("unchecked.png"), valueOne, valueTwo));
}


