#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class ControlPanel;

class Button {
friend ControlPanel;
private:
	Button(cv::Point position, cv::Point size, int layer);
	void SetImage(cv::Mat img) { img.copyTo(_img); }
	bool IsClicked(cv::Point click);
	void Draw(cv::Mat img);
	cv::Point _position, _size;
	cv::Mat _img;
	int _layer; //lower layer means lower down
};



class ControlPanel {
public:
	ControlPanel(const char* windowName);
	void AddTrackBar(const char* name, int& value);
	void AddButton(cv::Point position, cv::Point size, int layer);
	const char* GetWindowName() { return _windowName; }
private:
	const char* _windowName;
	std::vector<Button> buttons;
};
