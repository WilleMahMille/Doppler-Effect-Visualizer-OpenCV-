#include "SimpleInterface.h"
#include <Windows.h>


void OnMouseClick(int evnt, int x, int y, int flags, void* userData) {
	if (flags == cv::EVENT_FLAG_LBUTTON || evnt == cv::EVENT_LBUTTONUP ) {

		ControlPanel* panel = reinterpret_cast<ControlPanel*>(userData);
		panel->Click(cv::Point(x, y) - panel->GetPosition(), evnt, flags);
	}
}

CheckBox::CheckBox(cv::Point position, cv::Point size, bool* value) : _position(position), _size(size), _value(value) {
	_checkedImg = cv::Mat::zeros(size, CV_8UC3);
	cv::resize(Resources::LoadImg("checked.png"), _checkedImg, size);
	//cv::Mat insetImg(img, cv::Rect(_position.x, _position.y, _position.x + _size.x, _position.y + _size.y));
	
	_uncheckedImg = cv::Mat::zeros(size, CV_8UC3);
	cv::resize(Resources::LoadImg("unchecked.png"), _uncheckedImg, size);
}
bool CheckBox::IsClicked(cv::Point click, int type) {
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void CheckBox::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y || img.cols < _position.x + _size.x) {
		std::cout << "Error, button out of bounds\n";
		return;
	}
	if (*_value) {
		_checkedImg.copyTo(img(cv::Rect(_position.x, _position.y, _size.x, _size.y)));
	}
	else {
		_uncheckedImg.copyTo(img(cv::Rect(_position.x, _position.y, _size.x, _size.y)));
	}
}
void CheckBox::Click(cv::Point position, int type, int flag) {
	*_value = type == cv::EVENT_LBUTTONUP ? !*_value : *_value;
}


TwoDTrackbar::TwoDTrackbar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int* valOne, int* valTwo) : _position(position), _size(size), _bottomNumber(bottomNumber), _topNumber(topNumber), _valOne(valOne), _valTwo(valTwo) {
	_scale = std::pair<double, double>(static_cast<double>(size.x) / (topNumber.x - bottomNumber.x), static_cast<double>(size.y) / (topNumber.y - bottomNumber.y));
	trackbarImg = cv::Mat::zeros(size, CV_8UC3);
	
	cv::Point gradLinePos = cv::Point(bottomNumber.x >= 0 ? 10 : (topNumber.x < 0 ? size.x - 10 : static_cast<int>(size.x - _scale.first * topNumber.x)), bottomNumber.y >= 0 ? size.y - 10 : (topNumber.y < 0 ? 10 : static_cast<int>(_scale.second * topNumber.y)));

	//draw trackbarImg
	cv::line(trackbarImg, cv::Point(0, gradLinePos.y), cv::Point(size.x, gradLinePos.y), cv::Scalar(255, 255, 255));
	cv::line(trackbarImg, cv::Point(gradLinePos.x, 0), cv::Point(gradLinePos.x, size.y), cv::Scalar(255, 255, 255));
	for (int i = 0; i <= topNumber.x - bottomNumber.x; i++) {
		int lineSize = 2 + (i % 5 == 0 ? 2 : 0) + (i % 10 == 0 ? 2 : 0) + (i % 100 == 0 ? 2 : 0);
		cv::line(trackbarImg, cv::Point(static_cast<int>(i * _scale.first), gradLinePos.y + lineSize), cv::Point(static_cast<int>(i * _scale.first), gradLinePos.y - lineSize), cv::Scalar(255, 255, 255));
	}
	for (int i = 0; i <= topNumber.y - bottomNumber.y; i++) {
		int lineSize = 2 + (i % 10 == 0 ? 2 : 0) + (i % 5 == 0 ? 2 : 0);
		cv::line(trackbarImg, cv::Point(gradLinePos.x + lineSize, static_cast<int>(i * _scale.second)), cv::Point(gradLinePos.x - lineSize, static_cast<int>(i * _scale.second)), cv::Scalar(255, 255, 255));
	}

}
bool TwoDTrackbar::IsClicked(cv::Point click, int type) {
	if (type == cv::EVENT_LBUTTONUP && toCenter) {
		*_valOne = (_topNumber.x + _bottomNumber.x) / 2;
		*_valTwo = (_topNumber.y + _bottomNumber.y) / 2;
	}
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void TwoDTrackbar::Click(cv::Point position, int type, int flag) {
	if (flag == cv::EVENT_FLAG_LBUTTON) {
		*_valOne = static_cast<int>(!lockX ? _bottomNumber.x	+ (position.x - _position.x) / _scale.first : *_valOne);
		*_valTwo = static_cast<int>(!lockY ? _bottomNumber.y + (_size.y - position.y + _position.y) / _scale.second : *_valTwo);
	}
}
void TwoDTrackbar::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y || img.cols < _position.x + _size.x) {
		std::cout << "Error, button out of bounds\n";
		return;
	}
	trackbarImg.copyTo(img(cv::Rect(_position.x, _position.y, _size.x, _size.y)));
	cv::Point selectorPos = cv::Point(static_cast<int>(_position.x + (*_valOne - _bottomNumber.x) * _scale.first), static_cast<int>(_position.y + _size.y - (*_valTwo - _bottomNumber.y) * _scale.second));
	cv::circle(img, selectorPos, 6, cv::Scalar(50, 255, 50), 2);
}


Trackbar::Trackbar(cv::Point position, cv::Point size, int botNumber, int topNumber, int* value) : _position(position), _size(size), _botNumber(botNumber), _topNumber(topNumber), _value(value){
	_scale = size.x / static_cast<double>(topNumber - botNumber);
	trackbarImg = cv::Mat::zeros(size, CV_8UC3);

	int gradLinePos = size.y / 2;
	cv::line(trackbarImg, cv::Point(0, gradLinePos), cv::Point(size.x, gradLinePos), cv::Scalar(255, 255, 255));
	double scaleY = size.y / 25;
	for (int i = 0; i <= topNumber - botNumber; i++) {
		int lineSize = static_cast<int>(_scale < 0.1 ? (i % 100 == 0 ? scaleY: 0) + (i % 500 == 0 ? scaleY : 0) + (i % 1000 == 0 ? scaleY : 0) : (scaleY + (i % 5 == 0 ? scaleY : 0) + (i % 10 == 0 ? scaleY : 0) + (i % 100 == 0 ? scaleY * 2 : 0)));
		cv::line(trackbarImg, cv::Point(static_cast<int>(i * _scale), gradLinePos + lineSize), cv::Point(static_cast<int>(i * _scale), gradLinePos - lineSize), cv::Scalar(255, 255, 255));
	}
	
}
bool Trackbar::IsClicked(cv::Point click, int type) {
	if (type == cv::EVENT_LBUTTONUP && toCenter) {
		*_value = (_topNumber + _botNumber) / 2;
	}
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void Trackbar::Click(cv::Point click, int type, int flag) {
	if (flag == cv::EVENT_FLAG_LBUTTON) {
		*_value = static_cast<int>(_botNumber + (click.x - _position.x) / _scale);
	}
}
void Trackbar::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y || img.cols < _position.x + _size.x) {
		std::cout << "Error, button out of bounds\n";
		return;
	}
	trackbarImg.copyTo(img(cv::Rect(_position.x, _position.y, _size.x, _size.y)));
	cv::Point selectorPos = cv::Point(static_cast<int>(_position.x + (*_value - _botNumber) * _scale), static_cast<int>(_position.y + _size.y / 2));
	cv::circle(img, selectorPos, 6, cv::Scalar(50, 255, 50), 2);

}


ControlPanel::ControlPanel(const char* windowName, cv::Size size, cv::Point position, cv::Mat showImg) : _windowName(windowName), _size(size), _position(position), _showImg(showImg) {
	
	_design = cv::Mat::zeros(size, CV_8UC3);
	_img = cv::Mat::zeros(size, CV_8UC3);
	cv::namedWindow(_windowName);
	cv::setMouseCallback(_windowName, OnMouseClick, reinterpret_cast<void*>(this));
}
void ControlPanel::Click(cv::Point position, int type, int flag) {
	for (int i = static_cast<int>(inputs.size()) - 1; i >= 0; i--) {
		if (inputs[i]->IsClicked(position, type)) {
			inputs[i]->Click(position, type, flag);
		}
	}
	Draw();
}
void ControlPanel::Draw() {
	_design.copyTo(_img(cv::Rect(0, 0, _design.cols, _design.rows)));
	for (int i = 0; i < inputs.size(); i++) {
		inputs[i]->Draw(_img);
	}
	for (int i = 0; i < dynamicTexts.size(); i++) {
		dynamicTexts[i]->Draw(_img);
	}
	cv::line(_img, cv::Point(0, 0), cv::Point(0, _size.height), cv::Scalar(150, 150, 150), 2);
	_img.copyTo(_showImg(cv::Rect(_position.x, _position.y, _size.width, _size.height)));
}
CheckBox* ControlPanel::AddCheckBox(cv::Point position, int layer, bool* value, cv::Point size) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return nullptr;
	}
	CheckBox* cb = new CheckBox(position, size, value);
	inputs.insert(inputs.begin() + layer, cb);
	return cb;
}
TwoDTrackbar* ControlPanel::AddTwoDTrackBar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int layer, int *valueOne, int *valueTwo) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return nullptr;
	}
	TwoDTrackbar *tb = new TwoDTrackbar(position, size, bottomNumber, topNumber, valueOne, valueTwo);
	inputs.insert(inputs.begin() + layer, tb);
	return tb;
}
Trackbar* ControlPanel::AddTrackbar(cv::Point position, cv::Point size, int botNumber, int topNumber, int layer, int *value) {
	if (layer > inputs.size()) {
		std::cout << "Error, layer too high (over top layer)\n";
		return nullptr;
	}
	Trackbar* tb = new Trackbar(position, size, botNumber, topNumber, value);
	inputs.insert(inputs.begin() + layer, tb);
	return tb;
}
void ControlPanel::AddDynamicText(DynamicTextBase* dynamicText) {
	dynamicTexts.push_back(dynamicText);
}





