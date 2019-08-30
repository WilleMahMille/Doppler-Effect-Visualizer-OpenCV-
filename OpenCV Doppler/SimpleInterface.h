#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Resources.h"

class ControlPanel;

class UserInput {
public:
	virtual void Draw(cv::Mat img) = 0;
	virtual bool IsClicked(cv::Point click, int type) = 0;
	virtual void Click(cv::Point position, int type, int flag) = 0;
};

class CheckBox : UserInput {
	friend ControlPanel;
private:
	CheckBox(cv::Point position, cv::Point size, bool* value);
	bool IsClicked(cv::Point click, int type);
	void Draw(cv::Mat img);
	void Click(cv::Point position, int type, int flag);
	cv::Point _position, _size;
	cv::Mat _uncheckedImg, _checkedImg;
	bool* _value;
};

class TwoDTrackbar : UserInput {
	friend ControlPanel;
public:
	bool* GetLockX() { return &lockX; }
	bool* GetLockY() { return &lockY; }
	bool* GetToCenter() { return &toCenter; }
	int* GetValOne() { return _valOne; }
	int* GetValTwo() { return _valTwo; }
private:
	TwoDTrackbar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int *valOne, int *valTwo);
	//valOne is x, valTwo is y
	bool IsClicked(cv::Point click, int type);
	void Click(cv::Point position, int type, int flag);
	void Draw(cv::Mat img);

	int *_valOne, *_valTwo;
	bool lockX = false, lockY = false, toCenter = false;
	cv::Mat trackbarImg;
	cv::Point _position, _size, _bottomNumber, _topNumber;
	std::pair<double, double> _scale; //scale is numbers per pixel
};

class Trackbar : UserInput {
	friend ControlPanel;
public:
	bool* GetToCenter() { return &toCenter; }
	int* GetValue() { return _value; }
private:
	Trackbar(cv::Point position, cv::Point size, int botNumber, int topNumber, int *value);
	//good y-value for size is 50
	bool IsClicked(cv::Point click, int type);
	void Click(cv::Point click, int type, int flag);
	void Draw(cv::Mat img);

	cv::Mat trackbarImg;
	cv::Point _position, _size;
	int _botNumber, _topNumber;
	double _scale;
	int* _value;
	bool toCenter = false;
};

struct DynamicTextBase {
	virtual	 ~DynamicTextBase() { }
	virtual void Draw(cv::Mat img){ }
};
template<typename T>
class DynamicText : public DynamicTextBase {
	friend ControlPanel;
public:
	DynamicText<T>(T* textVar, cv::Point position, double textSize = 1, cv::Scalar color = cv::Scalar(210, 210, 210)) : _textVar(textVar), _position(position), _textSize(textSize), _color(color) {
		
		try {
			std::to_string(*textVar);
		}
		catch (int exception) {
			std::cout << "Error, type can't be cast to string\n";
			throw(std::invalid_argument("Error, type can't be cast to string"));
		}
	}
private:
	void Draw(cv::Mat img) {
		std::string text = std::to_string(*_textVar);
		cv::putText(img, text, _position, cv::FONT_HERSHEY_PLAIN, _textSize, _color, 1);
	}
	T* _textVar;
	cv::Point _position;
	double _textSize;
	cv::Scalar _color;
};


class ControlPanel {
public:
	ControlPanel(const char* windowName, cv::Size size, cv::Point position, cv::Mat showImg);
	TwoDTrackbar* AddTwoDTrackBar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int layer, int *valueOne, int *valueTwo);
	CheckBox* AddCheckBox(cv::Point position, int layer, bool* value, cv::Point size = cv::Point(50, 50));
	Trackbar* AddTrackbar(cv::Point position, cv::Point size, int botNumber, int topNumber, int layer, int *value);
	void AddDynamicText(DynamicTextBase* dynamicText);
	//lower layer means lower down, older buttons are pushed up if a newer one have a lower or the same layer
	void Click(cv::Point position, int type, int flag);
	void Draw();
	const char* GetWindowName() { return _windowName; }
	int GetTopLayer() { return static_cast<int>(inputs.size()); }
	cv::Point GetPosition() { return _position; }
	cv::Size GetSize() { return _size; }
	cv::Mat GetDesign() { return _design; }

private:
	cv::Point _position;
	const char* _windowName;
	std::vector<UserInput*> inputs;
	std::vector<DynamicTextBase*> dynamicTexts;
	cv::Size _size;
	cv::Mat _design, _img, _showImg;
};



