#include "Simulation.h"
#include <opencv2/core/ocl.hpp>
#define _USE_MATH_DEFINES

void Hitbox::Draw(cv::Mat img) {
	cv::Point pos = Resources::PairToPoint(position);
	cv::Point boxSize = Resources::PairToPoint(size);
	cv::rectangle(img, cv::Rect(pos, pos + boxSize), color, cv::FILLED, cv::LINE_8);
}
void Hitbox::UpdateLifetime(int framedelay) {
	_currentLifetime += framedelay;
}


WaveParticle::WaveParticle(cv::Point cameraSpeed, std::pair<float, float> position, std::pair<float, float> velocity, cv::Scalar color, bool lightParticle)
	: _position(position), _velocity(velocity), _color(color), _lightParticle(lightParticle) {
}
void WaveParticle::Draw(cv::Mat img) {
	//will crash if size = 0
	cv::Point pointPos = cv::Point(static_cast<int>(_position.first), static_cast<int>(_position.second));
	cv::rectangle(img, cv::Rect(pointPos, pointPos + cv::Point(waveSize - 1, waveSize - 1)), _color, -1);
	//cv::line(img, _position, _position + cv::Point(1, 1), _color);
}
void WaveParticle::UpdatePosition() { 
	_position += _velocity;
	_position += Resources::PointToPair(_cameraSpeed);
}
void WaveParticle::Collide(Hitbox *hitbox) {
	if (CollidingWith(hitbox)) {
		//update velocity
		
		float xDist, yDist;
		std::pair<float, float> hitboxCenter = std::pair<float, float>(hitbox->position.first + hitbox->size.first / 2, hitbox->position.second + hitbox->size.second / 2);
		xDist = (_position.first - hitboxCenter.first) / (hitbox->size.second / 2);
		yDist = (_position.second - hitboxCenter.second) / (hitbox->size.second / 2);

		//move the waveparticle out of the hitbox the same amount that it's in (enforce hitbox is what I would call it)

		if (abs(xDist) > abs(yDist)) {
			//collision on x-edge
			//"enforce" hitbox
			if (xDist > 0) {
				
				_position.first -= 2 * (_position.first - hitbox->position.first - hitbox->size.first);
			}
			else {
				_position.first -= 2 * (_position.first + waveSize - hitbox->position.first);
			}
			//update velocity
			if (!((_velocity.first > 0 && xDist > 0) || (_velocity.first < 0 && xDist < 0))) {
				_velocity.first *= -1;
			}
		}
		else if (abs(yDist) > abs(xDist)) {
			//collision on y-edge
			//"enforce" hitbox
			if (yDist > 0) {
				_position.second -= 2 * (_position.second - hitbox->position.second - hitbox->size.second);
			}
			else {
				_position.second -= 2 * (_position.second + waveSize - hitbox->position.second);
			}
			//update velocity
			if (!((_velocity.second > 0 && yDist > 0) || (_velocity.second < 0 && yDist < 0))) {
				_velocity.second *= -1;
			}
		}
		else if (abs(yDist) == abs(xDist)) {
			//collision on corner (should probably remove)
			_velocity = _velocity * -1;
		}
	}
}
bool WaveParticle::CollidingWith(Hitbox *hitbox) {
	return 
		hitbox->position.first <= _position.first + waveSize && 
		hitbox->position.first + hitbox->size.first >= _position.first && 
		hitbox->position.second <= _position.second + waveSize && 
		hitbox->position.second + hitbox->size.second >= _position.second;
}


Wave::Wave(cv::Point cameraSpeed, cv::Point position, int sizeIncrease, int lifetime, int *particlesPerWave, int waveID, int size, std::vector<cv::Scalar>* lightColor) 
	: _cameraSpeed(cameraSpeed), _position(position),  _lifetime(lifetime), _particlesPerWave(particlesPerWave), _waveID(waveID), waveSize(size), _lightColor(lightColor) {
	
	light = lightColor != nullptr;
	if (light) {
		if (*particlesPerWave != lightColor->size()) {
			std::cout << "Error, lightColor size does equal 360\n";
		}
	}
	_sizeIncrease = sizeIncrease > 0 ? sizeIncrease : 1;
	angleStep = 360 / static_cast<float>(*_particlesPerWave);

}
Wave::Wave(cv::Point cameraSpeed, std::pair<float, float> position, int sizeIncrease, int lifetime, int *particlesPerWave, int waveID, std::vector<WaveParticle*>* particles, int size, std::vector<cv::Scalar>* lightColor) 
	: _cameraSpeed(cameraSpeed), _lifetime(lifetime), _particlesPerWave(particlesPerWave), waveParticles(particles), _waveID(waveID), waveSize(size), _lightColor(lightColor) {
	if (*particlesPerWave != particles->size()) {
		std::cout << "Error, particles size does not match ParticlesPerWave\n";
	}
	light = lightColor != nullptr;
	if (light && (*particlesPerWave != lightColor->size())) {
		std::cout << "Error, lightColor size does not match particlesPerWave\n";
	}

	_particles = true;
	if (light) {
		for (int i = 0; i < particles->size(); i++) {
			(*particles)[i]->SetColor((*lightColor)[i]);
			(*particles)[i]->SetID(_waveID);
		}
	}
	for (int i = 0; i < particles->size(); i++){ 
		if (light) {
			(*particles)[i]->SetColor((*lightColor)[i]);
		}
		(*particles)[i]->MultiplyVelocity(sizeIncrease);
		(*particles)[i]->SetPosition(position);
		(*particles)[i]->SetSize(size);
	}
}
Wave::~Wave() {
	if (waveParticles != nullptr) {
		for (int i = 0; i < waveParticles->size(); i++) {
			delete (*waveParticles)[i];
		}
		delete waveParticles;
	}
	delete WaveColor;
}
void Wave::UpdatePosition() {
	_position += _cameraSpeed;
}
void Wave::Frame(std::vector<Hitbox*>* hitboxes) {
	if (_particles) {
		for (WaveParticle *wp : *waveParticles) {
			for (Hitbox* h : *hitboxes) {
				wp->Collide(h);
			}
			wp->SetCameraSpeed(_cameraSpeed);
			wp->UpdatePosition();
		}
	}
	else {
		IncreaseSize();
		UpdatePosition();
	}
}
bool Wave::CollidingWith(Hitbox* hitbox) {
	if (!_particles) {
		return false;
	}
	for (WaveParticle *wp : (*waveParticles)) {
		if (wp->CollidingWith(hitbox)) {
			return true;
		}
	}
	return false;
}
void Wave::Draw(cv::Mat img) {
	if (_particles) {
		for (WaveParticle *wp : *waveParticles) {
			wp->Draw(img);
		}
	}
	else {
		if (light) {
			for (int i = 0; i < *_particlesPerWave; i++) {
				cv::ellipse(img, _position, cv::Size(_size, _size), 0, i * angleStep, (i + 1) * angleStep, (*_lightColor)[i], waveSize);
			}
			
		}
		else {
			cv::ellipse(img, _position, cv::Size(_size, _size), 0, 0, 360, *WaveColor, waveSize);

		}
	}
}


WaveSource::WaveSource(cv::Point position, cv::Point screenSize, cv::Point sourceSpeed, cv::Point cameraSpeed, int wavedelay, int waveLifetime)
	: _position(position), _sourceSpeed(sourceSpeed), _cameraSpeed(cameraSpeed), _waveFrequency(wavedelay), _waveLifetime(waveLifetime), _screenSize(screenSize), 
	sourceHitbox(Resources::PointToPair(position - cv::Point(10, 10)), std::pair<float, float>(0, 0), std::pair<float, float>(21, 21), -1) {


	wavelengthMap = Resources::GetWavelengthToRgbMap(2);
	_currentHitboxTimer = _hitboxFrequency - frameDelay;
	//Resources::SetMultiplier(lightMultiplier);
	for (int i = 0; i < particlesPerWave; i++) {
		float rad = 2 * pi * i / particlesPerWave;
		particleVelocities.push_back(std::make_pair<float, float>(cos(rad), sin(rad)));
	}
}
void WaveSource::SetWaveSizeIncrease(int newIncrease) {
	for (int i = 0; i < waves.size(); i++) {
		waves[i]->SetSizeIncrease(newIncrease);
	}
}
void WaveSource::Frame() {
	
	SpawnWave();
	SpawnHitbox();
	UpdateLifetime();
	UpdatePositions();
	UpdateFrequency();
}
void WaveSource::UpdatePositions() {
	_position.x += -_cameraSpeed.x + _sourceSpeed.x;
	_position.y += _cameraSpeed.y -_sourceSpeed.y; //reverse y to create a mathematically logical coordinate system (pos y is up and pos x is right)
	_position.x = _position.x < 0 ? 0 : _position.x;
	_position.x = _position.x > _screenSize.x ? _screenSize.x : _position.x;
	_position.y = _position.y < 0 ? 0 : _position.y;
	_position.y = _position.y > _screenSize.y ? _screenSize.y : _position.y;
	sourceHitbox.position = Resources::PointToPair(_position - cv::Point(10, 10));

	for (int i = 0; i < waves.size(); i++) {
		waves[i]->SetSpeed(cv::Point(-_cameraSpeed.x, _cameraSpeed.y));
		waves[i]->Frame(&hitboxes);
	}
	for (Hitbox *h : hitboxes) {
		h->SetVelocity(hitboxVel);
		h->UpdatePosition();
	}
}
void WaveSource::UpdateLifetime() {
	for (int i = 0; i < waves.size(); i++) {
		waves[i]->LifetimeDecrease(frameDelay);
		if (waves[i]->IsDead()) {
			Wave &tempW = (*waves[i]);
			waves.erase(waves.begin() + i);
			tempW.~Wave();
			i--;
		}
	}
}
void WaveSource::UpdateFrequency() {
	frequency += frameDelay;
	for (Wave *w : waves) {
		if (w->CollidingWith(&sourceHitbox) && lastID != w->GetID() && w->GetCurrentLifetime() > 200) {
			if (lastID != INT_MIN) {
				lastFrequency = frequency;
				frequencyText = std::to_string(lastFrequency) + "ms";
				
			}
			frequency = 0;
			lastID = w->GetID();
		}
	}
}
void WaveSource::SpawnWave() {
	currentWaveDelay += frameDelay;

	std::vector<cv::Scalar>* lightColor = new std::vector<cv::Scalar>();
	if (_lightWave) {
		for (int i = 0; i < particlesPerWave; i++) {

			float wavelength = Resources::GetWavelengthFromVelocity(particleVelocities[i], _sourceSpeed);

			//const float wavelength = Resources::GetWavelengthFromVelocity(particleVelocities[i], _sourceSpeed);
			if (wavelength < 380) {
				wavelength = 380;
			}
			if (wavelength > 779) {
				wavelength = 779;
			}
			cv::Scalar rgb = wavelengthMap->at(round(wavelength));
			lightColor->push_back(rgb);

		}
	}
	if (_particleWave) {
		if (waveParticles == nullptr) {
			waveParticles = new std::vector<WaveParticle*>();
		}
		int currentParticleVectorSize = static_cast<int>(waveParticles->size());
		int waveSpawnDelayLeft = currentWaveDelay > _waveFrequency ? 0 : _waveFrequency - currentWaveDelay;
		float framesLeft = static_cast<float>(waveSpawnDelayLeft) / frameDelay;
		int wavesLeft = particlesPerWave - currentParticleVectorSize;
		int wavesThisFrame = static_cast<int>(wavesLeft / ceil(framesLeft));
		std::pair<float, float> pos = std::pair<float, float>(static_cast<float>(0), static_cast<float>(0));
		
		for (int i = 0; i < wavesThisFrame; i++) {
			waveParticles->push_back(new WaveParticle(_cameraSpeed, pos, particleVelocities[i + currentParticleVectorSize]));
		}
	}
	else {
		if (waveParticles != nullptr) {
			delete waveParticles;
			waveParticles = nullptr;
		}
	}


	if (currentWaveDelay >= _waveFrequency) {
		currentWaveDelay = 0;
		
		if (_particleWave) {
			if (waveParticles->size() < particlesPerWave) {
				std::pair<float, float> pos(static_cast<float>(0), static_cast<float>(0));
				int currentParticleVectorSize = static_cast<int>(waveParticles->size());
				for (int i = 0; i < particlesPerWave - currentParticleVectorSize; i++) {
					waveParticles->push_back(new WaveParticle(_cameraSpeed, pos, particleVelocities[i + currentParticleVectorSize]));
				}
			}
			if (_lightWave) {
				waves.push_back(new Wave(_cameraSpeed, std::pair<float, float>(static_cast<float>(_position.x), static_cast<float>(_position.y)), _waveSpeed, _waveLifetime, &particlesPerWave, waveID, waveParticles, 5, lightColor));
			}
			else {
				waves.push_back(new Wave(_cameraSpeed, std::pair<float, float>(static_cast<float>(_position.x), static_cast<float>(_position.y)), _waveSpeed, _waveLifetime, &particlesPerWave, waveID, waveParticles, 5));
			}
			waveParticles = nullptr;
		}
		else {
			if (!_lightWave) {
				waves.push_back(new Wave(cv::Point(0, 0), _position, _waveSpeed, _waveLifetime, &particlesPerWave, waveID));
			}
			else {
				waves.push_back(new Wave(cv::Point(0, 0), _position, _waveSpeed, _waveLifetime, &particlesPerWave, waveID, 3, lightColor));

			}
		}
		waveID++;
		if (waveID > 1000) {
			waveID = 0;
		}
		
		//wave bounces, entertaining concept, but physically incorrect and not used in this program (anymore)
		/*for (int i = 0; i < waveBounces; i++) {
			
			waves.push_back(Wave(cv::Point(0, 0), cv::Point((i % 2 == 0 ? _screenSize.x - _position.x : _position.x) + _screenSize.x * (i + 1) * (i % 2 == 0 ? -1 : 1), _position.y), _waveSpeed, _waveLifetime));
			waves.push_back(Wave(cv::Point(0, 0), cv::Point((i % 2 == 0 ? _screenSize.x - _position.x : _position.x) + _screenSize.x * (i + 1) * (i % 2 == 0 ? 1 : -1), _position.y), _waveSpeed, _waveLifetime));
			
			waves.push_back(Wave(cv::Point(0, 0), cv::Point(_position.x, (i % 2 == 0 ? _screenSize.y - _position.y : _position.y) + _screenSize.y * (i + 1) * (i % 2 == 0 ? 1 : -1)), _waveSpeed, _waveLifetime));
			waves.push_back(Wave(cv::Point(0, 0), cv::Point(_position.x, (i % 2 == 0 ? _screenSize.y - _position.y : _position.y) + _screenSize.y * (i + 1) * (i % 2 == 0 ? -1 : 1)), _waveSpeed, _waveLifetime));
		}*/	
	}
}
void WaveSource::SpawnHitbox() {
	if (!_hitboxesEnabled) {
		return;
	}
	if (_currentHitboxTimer >= _hitboxFrequency) {
		_currentHitboxTimer -= _hitboxFrequency;
		hitboxes.push_back(new Hitbox(std::make_pair<float, float>(static_cast<float>(windowWidth - controlPanelWidth), windowHeight * 2 / static_cast<float>(3)), hitboxVel, std::make_pair<float, float>(75, 75), _hitboxLifetime));
		std::cout << "adding hitbox\n";
	}
	_currentHitboxTimer += frameDelay;
}
void WaveSource::Draw(cv::Mat img) {
	cv::circle(img, _position, 10, WaveSourceColor, 5);
	
	Resources::multiplier = c / (2 * _waveSpeed);

	std::vector<std::thread> drawThreads;
	std::vector<Wave*> *wavePackage = new std::vector<Wave*>();
	
	int wavesPerThread = 2;

	for (int i = 0; i < waves.size(); i++) {
		wavePackage->push_back(waves[i]);
		if (i % wavesPerThread == wavesPerThread - 1) {
			drawThreads.push_back(std::thread::thread(Resources::DrawWaves, img, wavePackage));
			wavePackage = new std::vector<Wave*>();
		}
	}
	if (wavePackage->size() != 0) {
		drawThreads.push_back(std::thread::thread(Resources::DrawWaves, img, wavePackage));
	}

	for (int i = 0; i < drawThreads.size(); i++) {
		drawThreads[i].join();
	}
	for (Hitbox* h : hitboxes) {
		h->Draw(img);
	}

}


WaveSimulation::WaveSimulation() {
	_img = cv::Mat::zeros(cv::Size(windowWidth, windowHeight), CV_8UC4);
	cv::namedWindow("Doppler effect", cv::WINDOW_FULLSCREEN);
	cv::resizeWindow("Doppler effect", cv::Size(windowWidth, windowHeight));

	ws = new WaveSource(cv::Point((windowWidth - controlPanelWidth ) / 2, windowHeight / 2), cv::Point(windowWidth - controlPanelWidth, windowHeight));
	
	ctrlP = new ControlPanel("Doppler effect", cv::Size(controlPanelWidth, windowHeight), cv::Point(windowWidth - controlPanelWidth, 0), _img);
	
	//setting the design for controlpanel
	cv::Mat controlPanelDesign = ctrlP->GetDesign();
	cv::putText(controlPanelDesign, "Velocity", cv::Point(25, 35), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "X:", cv::Point(240, 35), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Y:", cv::Point(300, 35), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Lock X", cv::Point(25, 65), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Lock Y", cv::Point(125, 65), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "To Center", cv::Point(225, 65), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Lock Camera", cv::Point(25, 85), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Wave speed", cv::Point(15, 490), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "Current:", cv::Point(250, 490), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Wave Frequency", cv::Point(15, 575), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "ms:", cv::Point(290, 575), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(210, 210, 210));
	cv::putText(controlPanelDesign, "Wave lifetime", cv::Point(15, 660), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "ms:", cv::Point(280, 660), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(210, 210, 210));

	cv::putText(controlPanelDesign, "Pause", cv::Point(35, 750), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "Simulate Particles", cv::Point(35, 780), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
	cv::putText(controlPanelDesign, "Simulate Light", cv::Point(35, 810), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
	cv::putText(controlPanelDesign, "Collider", cv::Point(35, 840), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
	cv::putText(controlPanelDesign, "Detected Frequency", cv::Point(15, 875), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
	cv::putText(controlPanelDesign, "Hitbox Velocity:", cv::Point(15, 900), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));

	//adding userinput elements
	TwoDTrackbar* tdtb = ctrlP->AddTwoDTrackBar(cv::Point(25, 100), cv::Point(350, 350), cv::Point(-10, -10), cv::Point(10, 10), ctrlP->GetTopLayer(), &ws->_sourceSpeed.x, &ws->_sourceSpeed.y);
	Trackbar *waveSpeedTB = ctrlP->AddTrackbar(cv::Point(25, 500), cv::Point(350, 50), 0, 10, ctrlP->GetTopLayer(), &ws->_waveSpeed);
	Trackbar *waveFrequencyTB = ctrlP->AddTrackbar(cv::Point(25, 585), cv::Point(350, 50), 100, 7500, ctrlP->GetTopLayer(), &ws->_waveFrequency);
	Trackbar *waveLifetimeTB = ctrlP->AddTrackbar(cv::Point(25, 670), cv::Point(350, 50), 100, 7500, ctrlP->GetTopLayer(), &ws->_waveLifetime);
	Trackbar *hitboxSpeedTB = ctrlP->AddTrackbar(cv::Point(25, 920), cv::Point(350, 50), 0, 5, ctrlP->GetTopLayer(), &ws->hitboxVel.first);

	ctrlP->AddCheckBox(cv::Point(87, 52), ctrlP->GetTopLayer(), tdtb->GetLockX(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(187, 52), ctrlP->GetTopLayer(), tdtb->GetLockY(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(317, 52), ctrlP->GetTopLayer(), tdtb->GetToCenter(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(142, 72), ctrlP->GetTopLayer(), &lockCamera, cv::Point(15, 15));

	ctrlP->AddCheckBox(cv::Point(15, 735), ctrlP->GetTopLayer(), &pause, cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(15, 765), ctrlP->GetTopLayer(), &ws->_particleWave, cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(15, 795), ctrlP->GetTopLayer(), &ws->_lightWave, cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(15, 825), ctrlP->GetTopLayer(), &ws->_hitboxesEnabled, cv::Point(15, 15));

	//adding dynamic texts
	ctrlP->AddDynamicText(new DynamicText<int>(tdtb->GetValOne(), cv::Point(260, 35), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(tdtb->GetValTwo(), cv::Point(320, 35), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveSpeedTB->GetValue(), cv::Point(340, 490), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveFrequencyTB->GetValue(), cv::Point(330, 575), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveLifetimeTB->GetValue(), cv::Point(320, 660), 1.2));
	ctrlP->AddDynamicText(new DynamicText<std::string>(&ws->frequencyText, cv::Point(235, 875), 1.2, cv::Scalar(255, 255, 255)));
	ctrlP->AddDynamicText(new DynamicText<int>(&ws->hitboxVel.first, cv::Point(175, 900), 1.2, cv::Scalar(255, 255, 255)));

	//showing the controlpanel
	ctrlP->Draw();
}

void WaveSimulation::RunSimulation() {
	cv::TickMeter frameTimer;
	for (;;) {
		frameTimer.start();
		_img = cv::Mat::zeros(cv::Point(windowWidth, windowHeight), CV_8UC4);
		if (!pause) {
			ws->LockCamera(lockCamera);
			ws->Frame();
		}
		ws->Draw(_img);
		ctrlP->Draw();

		cv::imshow("Doppler effect", _img);
		frameTimer.stop();
		//supposed to smooth the framerate
		int timer = frameTimer.getTimeMilli();
		int frameDelay = timer >= 25 ? 5 : 30 - timer;
		int temp = cv::waitKey(frameDelay);
		frameTimer.reset();
		
		if (temp == 27) {
			break;
		}
		if (temp == 32) {
			pause = !pause;
		}

	}
}


