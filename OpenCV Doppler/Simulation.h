#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SimpleInterface.h"


class Wave {
public:
	Wave(cv::Point speed, cv::Point position, int sizeIncrease, int lifetime);
	void Frame();

	void SetSizeIncrease(int sizeIncrease) { _sizeIncrease = sizeIncrease; }
	void SetSpeed(cv::Point speed) { _speed = speed; }
	void LifetimeDecrease(int FrameDelay) { _lifetime -= FrameDelay; }
	bool IsDead() { return _lifetime <= 0; }
	void Draw(cv::Mat img, cv::Scalar WaveColor) { cv::circle(img, _position, _size / 2, WaveColor);	}

private:

	cv::Point _position, _speed;
	int _size = 10, _sizeIncrease;
	int _lifetime;

	void IncreaseSize() { _size += _sizeIncrease; }
	void ApplySpeed() { _position += _speed; }
};

class WaveSource {
friend class WaveSimulation;
public:
	WaveSource(cv::Point position, cv::Point screenSize, cv::Point sourceSpeed = cv::Point(0, 0), cv::Point cameraSpeed = cv::Point(0, 0), int wavedelay = 1000, int waveLifetime = 3000) : _position(position), _sourceSpeed(sourceSpeed), _cameraSpeed(cameraSpeed), _waveFrequency(wavedelay), _waveLifetime(waveLifetime), _screenSize(screenSize) {
	}

	inline void SetSourceSpeed(cv::Point sourceSpeed) { _sourceSpeed = sourceSpeed; }
	inline void SetCameraSpeed(cv::Point cameraSpeed) { _cameraSpeed = cameraSpeed; }
	void SetWaveSizeIncrease(int newIncrease);
	void Frame(cv::Mat img);

private:
	const int FrameDelay = 30;
	const cv::Scalar WaveSourceColor = cv::Scalar(200, 29, 0);
	const cv::Scalar WaveColor = cv::Scalar(255, 200, 100);

	cv::Point _position, _sourceSpeed, _cameraSpeed;
	cv::Point _screenSize;
	int reverseX = 0, reverseY = 0;
	int _waveFrequency, currentWaveDelay = 0, _waveLifetime, _waveSpeed = 5;
	//int waveBounces = 0; //deprecated and not used in this program anymore
	std::vector<Wave> waves;

	void UpdatePositions();
	void UpdateLifetime();
	void SpawnWave();
	void Draw(cv::Mat img);
	void UpdateWaveSpeed() { _waveSpeed = _waveSpeed > 0 ? _waveSpeed : 1; }
};

class WaveSimulation {
public:
	WaveSimulation();
	
	void SetCameraSpeed(cv::Point speed);
	void RunSimulation();

private:
	ControlPanel* ctrlP;
	WaveSource* ws;
	cv::Point _cameraSpeed;
};
