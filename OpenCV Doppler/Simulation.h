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
	Hitbox(std::pair<float, float> pos, std::pair<float, float> vel, std::pair<float, float> boxSize, int lifetime) : 
		position(pos), _velocity(vel), size(boxSize), _lifeTime(lifetime) {
	}
	void Draw(cv::Mat img);
	void SetVelocity(std::pair<float, float> velocity) { _velocity = velocity; }
	void UpdatePosition() { position += _velocity * -1; }
	void SetPosition(std::pair<float, float> position) { this->position = position; }
	void UpdateLifetime(int framedelay);
	bool IsDead() { return _lifeTime != -1 && _currentLifetime > _lifeTime; }

	std::pair<float, float> position, size;
	cv::Scalar color = cv::Scalar(209, 209, 36);
private:
	std::pair<float, float> _velocity, _cameraSpeed;
	int _lifeTime, _currentLifetime = 0;
};

class WaveParticle {
	friend Wave;
public:
	WaveParticle(cv::Point cameraSpeed, std::pair<float, float> position, std::pair<float, float> velocity, cv::Scalar color = cv::Scalar(0, 255, 163), bool lightParticle = false);
	bool CollidingWith(Hitbox *hitbox);

private:
	~WaveParticle() { }
	void UpdatePosition();
	void Draw(cv::Mat img);
	void Collide(Hitbox *hitbox);
	void MultiplyVelocity(int multiplier) { _velocity = _velocity * multiplier; }
	void SetPosition(std::pair<float, float> pos) { _position = pos; }
	void SetSize(int size) { waveSize = size; }
	void SetColor(cv::Scalar color) { _color = color; }
	void SetID(int waveID) { _waveID = waveID; }
	void SetCameraSpeed(cv::Point cameraSpeed) { _cameraSpeed = cameraSpeed; }

	cv::Scalar _color;
	cv::Point _cameraSpeed;
	std::pair<float, float> _velocity, _position;
	int waveSize = 3, _waveID;
	bool _lightParticle;
};


class Wave {
public:
	Wave(cv::Point cameraSpeed, cv::Point position, int sizeIncrease, int lifetime, int *particlesPerWave, int waveID, int size = 3, std::vector<cv::Scalar>* lightColor = nullptr); //wave constructor
	Wave(cv::Point cameraSpeed, std::pair<float, float> position, int sizeIncrease, int lifetime, int *particlesPerWave, int waveID, std::vector<WaveParticle*> *particles, int size = 3, std::vector<cv::Scalar>* lightColor = nullptr); //particle wave constructor
	
	~Wave();
	void Frame(std::vector<Hitbox*>* hitboxes);
	void Draw(cv::Mat img);

	void SetSizeIncrease(int sizeIncrease) { _sizeIncrease = sizeIncrease; }
	void SetSpeed(cv::Point speed) { _cameraSpeed = speed; }
	void LifetimeDecrease(int frameDelay) { _lifetime -= frameDelay; _currentLifetime += frameDelay; }
	int GetID() { return _waveID; }
	int GetCurrentLifetime() { return _currentLifetime; }
	bool IsDead() { return _lifetime <= 0; }
	bool CollidingWith(Hitbox* hitbox);

private:
	
	const cv::Scalar *WaveColor = new cv::Scalar(0, 255, 163);
	cv::Point _position, _cameraSpeed;
	int _size = 1, _sizeIncrease, waveSize, _waveID;
	int _lifetime, _currentLifetime;
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
//WaveSource ended up being more of the simulation and WaveSimulation became a simulation host (might re-organize later)
public:
	WaveSource(cv::Point position, cv::Point screenSize, cv::Point sourceSpeed = cv::Point(0, 0), cv::Point cameraSpeed = cv::Point(0, 0), int wavedelay = 1000, int waveLifetime = 5000);

	inline void SetSourceSpeed(cv::Point sourceSpeed) { _sourceSpeed = sourceSpeed; }
	inline void LockCamera(bool lockCamera) { _cameraSpeed = lockCamera ? _sourceSpeed : cv::Point(0, 0); }
	void SetWaveSizeIncrease(int newIncrease);
	void Frame();
	void Draw(cv::Mat img);
private:
	void UpdatePositions();
	void UpdateLifetime();
	void UpdateFrequency();
	void SpawnWave();
	void SpawnHitbox();
	void UpdateWaveSpeed() { _waveSpeed = _waveSpeed > 0 ? _waveSpeed : 1; }

	const int frameDelay = 30;
	const cv::Scalar WaveSourceColor = cv::Scalar(200, 29, 0);

	cv::Point _position, _sourceSpeed, _cameraSpeed;
	cv::Point _screenSize;
	//frequency is in ms
	int _waveFrequency, currentWaveDelay = 0, _waveLifetime, _waveSpeed = 5, waveID = 0;
	int particlesPerWave = 200;

	int _hitboxFrequency = 20000, _currentHitboxTimer = 0, _hitboxLifetime = 10000;
	bool _hitboxesEnabled = false;
	std::pair<int, int> hitboxVel = std::pair<int, int>(1, 0);

	bool _particleWave = false, _lightWave = false;

	//int waveBounces = 0; //deprecated and not used in this program anymore
	std::vector<Wave*> waves;
	std::vector<Hitbox*> hitboxes;
	std::vector<WaveParticle*>* waveParticles;
	std::vector<std::pair<float, float>> particleVelocities;
	std::map<float, cv::Scalar> *wavelengthMap;


	Hitbox sourceHitbox;
	int frequency, lastFrequency, lastID = INT_MIN;
	std::string frequencyText = "0ms";


	
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
