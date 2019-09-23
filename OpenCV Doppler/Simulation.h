#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include "SimpleInterface.h"

constexpr int windowWidth = 1920; //screenwidth
constexpr int windowHeight = 1020; //screenheight - 60 (windows taskbar)
constexpr int controlPanelWidth = 400; //400 is standard
//constexpr int particlesPerWave = 60;
constexpr float pi = static_cast<float>(3.14159265358979323846); // pi
constexpr int amtOfThreads = 8;

class Wave;

//for positions:
//x-positive = right
//y-positive = down

class Hitbox {
public:
	Hitbox(std::pair<float, float> pos, std::pair<float, float> boxSize) : position(pos), size(boxSize) {
	}
	void Draw(cv::Mat img);
	std::pair<float, float> position, size;
	cv::Scalar color = cv::Scalar(30, 100, 255);
};

class WaveParticle {
	friend Wave;
public:
	WaveParticle(cv::Point cameraSpeed, std::pair<float, float> position, std::pair<float, float> velocity, cv::Scalar color = cv::Scalar(255, 200, 100));
private:
	~WaveParticle() { }
	void UpdatePosition();
	void Draw(cv::Mat img);
	void Collide(Hitbox *hitbox);
	bool CollidingWith(Hitbox *hitbox);
	void MultiplyVelocity(int multiplier) { _velocity = _velocity * multiplier; }
	void SetPosition(std::pair<float, float> pos) { _position = pos; }
	void SetSize(int size) { waveSize = size; }
	void SetColor(cv::Scalar color) { _color = color; }
	void SetCameraSpeed(cv::Point cameraSpeed) { _cameraSpeed = cameraSpeed; }

	cv::Scalar _color;
	cv::Point _cameraSpeed;
	std::pair<float, float> _velocity, _position;
	int waveSize = 3;
};


class Wave {
public:
	Wave(cv::Point cameraSpeed, cv::Point position, int sizeIncrease, int lifetime, int *particlesPerWave, int size = 3, std::vector<cv::Scalar>* lightColor = nullptr); //wave constructor
	Wave(cv::Point cameraSpeed, std::pair<float, float> position, int sizeIncrease, int lifetime, int *particlesPerWave, std::vector<WaveParticle*> *particles, int size = 3, std::vector<cv::Scalar>* lightColor = nullptr); //particle wave constructor
	
	~Wave();
	void Frame(std::vector<Hitbox*>* hitboxes);
	void Draw(cv::Mat img);

	void SetSizeIncrease(int sizeIncrease) { _sizeIncrease = sizeIncrease; }
	void SetSpeed(cv::Point speed) { _cameraSpeed = speed; }
	void LifetimeDecrease(int FrameDelay) { _lifetime -= FrameDelay; }
	bool IsDead() { return _lifetime <= 0; }
	

private:
	
	const cv::Scalar *WaveColor = new cv::Scalar(255, 200, 100);
	cv::Point _position, _cameraSpeed;
	int _size = 1, _sizeIncrease, waveSize;
	int _lifetime;
	int *_particlesPerWave;
	bool _particles = false, light = false;
	float angleStep;
	std::vector<WaveParticle*>* waveParticles;

	void IncreaseSize() { _size += _sizeIncrease; }
	void UpdatePosition();

	std::vector<cv::Scalar>* _lightColor;
};

class WaveSource {
friend class WaveSimulation;
public:
	WaveSource(cv::Point position, cv::Point screenSize, cv::Point sourceSpeed = cv::Point(0, 0), cv::Point cameraSpeed = cv::Point(0, 0), int wavedelay = 1000, int waveLifetime = 3000);

	inline void SetSourceSpeed(cv::Point sourceSpeed) { _sourceSpeed = sourceSpeed; }
	inline void LockCamera(bool lockCamera) { _cameraSpeed = lockCamera ? _sourceSpeed : cv::Point(0, 0); }
	void SetWaveSizeIncrease(int newIncrease);
	void Frame();
	void Draw(cv::Mat img);
private:

	void UpdatePositions();
	void UpdateLifetime();
	void SpawnWave();
	void UpdateWaveSpeed() { _waveSpeed = _waveSpeed > 0 ? _waveSpeed : 1; }
	void AddHitbox(cv::Point position, cv::Point size);

	const int FrameDelay = 30;
	const cv::Scalar WaveSourceColor = cv::Scalar(200, 29, 0);

	cv::Point _position, _sourceSpeed, _cameraSpeed;
	cv::Point _screenSize;
	int _waveFrequency, currentWaveDelay = 0, _waveLifetime, _waveSpeed = 5;
	int particlesPerWave = 200;

	bool _particleWave = false, _lightWave = true;

	//int waveBounces = 0; //deprecated and not used in this program anymore
	std::vector<Wave*> waves;
	std::vector<WaveParticle*>* waveParticles;
	std::vector<std::pair<float, float>> particleVelocities;
	std::vector<Hitbox*> hitboxes;
	std::map<float, cv::Scalar> *wavelengthMap;
};

class WaveSimulation {
public:
	WaveSimulation();
	
	void RunSimulation();

private:
	bool lockCamera = false, pause = false;
	cv::Mat _img;
	ControlPanel* ctrlP;
	WaveSource* ws;
	cv::Point _cameraSpeed;
};
