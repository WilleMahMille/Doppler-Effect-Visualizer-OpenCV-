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
	virtual bool IsClicked(cv::Point click) = 0;
	virtual void Click(cv::Point position, int type, int flag) = 0;
};

class CheckBox : UserInput {
	friend ControlPanel;
private:
	CheckBox(cv::Point position, cv::Point size, bool* value);
	bool IsClicked(cv::Point click);
	void Draw(cv::Mat img);
	void Click(cv::Point position, int type, int flag);
	cv::Point _position, _size;
	cv::Mat _uncheckedImg, _checkedImg;
	bool* _value;
};

class TwoDTrackbar : UserInput { 
	friend ControlPanel;
private:
	TwoDTrackbar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, double* valOne, double* valTwo);
	//valOne is x, valTwo is y
	bool IsClicked(cv::Point click);
	void Click(cv::Point position, int type, int flag);
	void Draw(cv::Mat img);
	bool* GetLockX() { return &lockX; }
	bool* GetLockY() { return &lockY; }

	double *_valOne, *_valTwo;
	bool lockX = false, lockY = false, toCenter = false;
	cv::Mat trackbarImg, selector;
	cv::Point _position, _size, _bottomNumber, _topNumber;
	std::pair<double, double> _scale; //scale is numbers per pixel

};


class ControlPanel {
public:
	ControlPanel(const char* windowName, cv::Size size);
	void AddTwoDTrackBar(cv::Point position, cv::Point size, cv::Point bottomNumber, cv::Point topNumber, int layer, double* valueOne, double* valueTwo);
	void AddCheckBox(cv::Point position, int layer, bool* value, cv::Point size = cv::Point(50, 50));
	//lower layer means lower down, older buttons are pushed up if a newer one have a lower or the same layer
	void Click(cv::Point position, int type, int flag);
	void Draw();
	const char* GetWindowName() { return _windowName; }
	int GetTopLayer() { return static_cast<int>(inputs.size()); }
	cv::Size GetSize() { return _size; }
	cv::Mat GetDesign() { return _design; }

private:
	const char* _windowName;
	std::vector<UserInput*> inputs;
	cv::Size _size;
	cv::Mat _design;
};
