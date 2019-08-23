#include "Simulation.h"


Wave::Wave(cv::Point speed, cv::Point position, int sizeIncrease, int lifetime) : _speed(speed), _position(position),  _lifetime(lifetime) {
	_sizeIncrease = sizeIncrease > 0 ? sizeIncrease : 1;
}

void Wave::Frame() {
	IncreaseSize();
	ApplySpeed();
}

void WaveSource::SetWaveSizeIncrease(float newIncrease) {
	for (int i = 0; i < waves.size(); i++) {
		waves[i].SetSizeIncrease(newIncrease);
	}
}
void WaveSource::Frame(cv::Mat img) {
	SpawnWave();
	UpdateLifetime();
	UpdatePositions();
	Draw(img);
}
void WaveSource::UpdatePositions() {
	
	_position.x += -_cameraSpeed.x + _sourceSpeed.x * (reverseX > 0 ? -reverseX : 1);
	_position.y += -_cameraSpeed.y + _sourceSpeed.y * (reverseY > 0 ? -reverseY : 1);
	for (int i = 0; i < waves.size(); i++) {
		waves[i].SetSpeed(-_cameraSpeed);
		waves[i].Frame();
	}
}
void WaveSource::UpdateLifetime() {
	for (int i = 0; i < waves.size(); i++) {
		waves[i].LifetimeDecrease(FrameDelay);
		if (waves[i].IsDead()) {
			waves.erase(waves.begin() + i);
			i--;
		}
	}
}
void WaveSource::SpawnWave() {
	currentWaveDelay += FrameDelay;
	if (currentWaveDelay >= _waveDelay) {
		currentWaveDelay -= _waveDelay;
		waves.push_back(Wave(cv::Point(0, 0), _position, _waveSpeed, _waveLifetime));
	}
}
void WaveSource::Draw(cv::Mat img) {
	cv::circle(img, _position, 10, WaveSourceColor, 5);
	//cv::rectangle(img, cv::Point(_position.x - 10, _position.y - 10), cv::Point(_position.x + 10, _position.y + 10), WaveSourceColor, -1);
	for (int i = 0; i < waves.size(); i++) {
		waves[i].Draw(img, WaveColor);
	}
}
void CreateTrackbars(int* sourceSpeedX, int* reverseX, int* sourceSpeedY, int* reverseY, int* waveDelay, int* waveSpeed, int* waveLifetime) {

	cv::createTrackbar("Source Speed X", "WaveSimulation", sourceSpeedX, 50);
	cv::createTrackbar("Reverse X speed", "WaveSimulation", reverseX, 1);
	cv::createTrackbar("Wave Source Speed Y", "WaveSimulation", sourceSpeedY, 50);
	cv::createTrackbar("Reverse Y speed", "WaveSimulation", reverseY, 1);
	cv::createTrackbar("Wave Spawn Delay", "WaveSimulation", waveDelay, 5000);
	cv::createTrackbar("Wave Speed", "WaveSimulation", waveSpeed, 75);
	cv::createTrackbar("Wave lifetime", "WaveSimulation", waveLifetime, 10000);

}
WaveSimulation::WaveSimulation() {
	cv::namedWindow("WaveSimulation", cv::WINDOW_AUTOSIZE);
	ws = new WaveSource(cv::Point(100, 400));
	CreateTrackbars(&ws->_sourceSpeed.x, &ws->reverseX, &ws->_sourceSpeed.y, &ws->reverseY, &ws->_waveDelay, &ws->_waveSpeed, &ws->_waveLifetime);
}

void WaveSimulation::SetCameraSpeed(cv::Point speed) {
	ws->SetCameraSpeed(speed);
}

void WaveSimulation::RunSimulation() {
	while (true) {
		cv::Mat img = cv::Mat::zeros(cv::Point(1900, 1000), CV_8UC3);
		ws->Frame(img);
		cv::imshow("Doppler effect", img);
		int temp = cv::waitKey(30);
		if (temp == 27) {
			break;
		}

	}
}


