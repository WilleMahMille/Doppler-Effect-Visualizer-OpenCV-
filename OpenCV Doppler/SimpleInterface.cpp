#include "SimpleInterface.h"
#include <Windows.h>

std::string GetWorkingDir() {
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf);
}
cv::Mat LoadImg(const char* imgName) {
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
void OnMouseClick(int evnt, int x, int y, int flags, void* userData) {
	if (flags == cv::EVENT_FLAG_LBUTTON || evnt == cv::EVENT_LBUTTONUP ) {

		ControlPanel* panel = reinterpret_cast<ControlPanel*>(userData);
		panel->Click(cv::Point(x, y), evnt);
	}
}

CheckBox::CheckBox(cv::Point position, bool* value) : _position(position), _value(value) {
	_size = cv::Point(50, 50);
	_checkedImg = cv::Mat::zeros(cv::Size(50, 50), CV_8UC3);
	cv::resize(LoadImg("checked.png"), _checkedImg, cv::Size(50, 50));
	//cv::Mat insetImg(img, cv::Rect(_position.x, _position.y, _position.x + _size.x, _position.y + _size.y));
	
	_uncheckedImg = cv::Mat::zeros(cv::Size(50, 50), CV_8UC3);
	cv::resize(LoadImg("unchecked.png"), _uncheckedImg, cv::Size(50, 50));
}
bool CheckBox::IsClicked(cv::Point click) {
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void CheckBox::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y) {
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
void TwoDTrackbar::Draw(cv::Mat img) {

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
void ControlPanel::AddCheckBox(cv::Point position, int layer, bool* value) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return;
	}
	inputs.insert(inputs.begin() + layer, new CheckBox(position, value));
}


void ControlPanel::Test(){

}

