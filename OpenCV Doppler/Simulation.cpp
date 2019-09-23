#include "Simulation.h"
#include <opencv2/core/ocl.hpp>
#define _USE_MATH_DEFINES

void Hitbox::Draw(cv::Mat img) {
	cv::Point pos = Resources::PairToPoint(position);
	cv::Point boxSize = Resources::PairToPoint(size);
	cv::rectangle(img, cv::Rect(pos, pos + boxSize), color, cv::FILLED, cv::LINE_8);
}

WaveParticle::WaveParticle(cv::Point cameraSpeed, std::pair<float, float> position, std::pair<float, float> velocity, cv::Scalar color) : _position(position), _velocity(velocity), _color(color) {
}
void WaveParticle::Draw(cv::Mat img) {
	//will crash if size = 0
	cv::Point pointPos = cv::Point(static_cast<int>(_position.first), static_cast<int>(_position.second));
	cv::rectangle(img, cv::Rect(pointPos, pointPos + cv::Point(waveSize - 1, waveSize - 1)), cv::Scalar(200, 200, 200), -1);
	//cv::line(img, _position, _position + cv::Point(1, 1), _color);
}
void WaveParticle::UpdatePosition() { 
	_position += _velocity;
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


Wave::Wave(cv::Point cameraSpeed, cv::Point position, int sizeIncrease, int lifetime, int *particlesPerWave, int size, std::vector<cv::Scalar>* lightColor) : _cameraSpeed(cameraSpeed), _position(position),  _lifetime(lifetime), _particlesPerWave(particlesPerWave), waveSize(size), _lightColor(lightColor) {
	
	light = lightColor != nullptr;
	if (light) {
		if (*particlesPerWave != lightColor->size()) {
			std::cout << "Error, lightColor size does equal 360\n";
		}
	}
	_sizeIncrease = sizeIncrease > 0 ? sizeIncrease : 1;
	angleStep = 360 / static_cast<float>(*_particlesPerWave);

}
Wave::Wave(cv::Point cameraSpeed, std::pair<float, float> position, int sizeIncrease, int lifetime, int *particlesPerWave, std::vector<WaveParticle*>* particles, int size, std::vector<cv::Scalar>* lightColor) : _cameraSpeed(cameraSpeed), _lifetime(lifetime), _particlesPerWave(particlesPerWave), waveParticles(particles), waveSize(size), _lightColor(lightColor) {
	if (*particlesPerWave != particles->size()) {
		std::cout << "Error, particles size does not match ParticlesPerWave\n";
	}
	light = lightColor != nullptr;
	if (light) {
		if (*particlesPerWave != lightColor->size()) {
			std::cout << "Error, lightColor size does not match particlesPerWave\n";
		}
	}

	_particles = true;
	if (light) {
		for (int i = 0; i < particles->size(); i++) {
			(*particles)[i]->SetColor((*lightColor)[i]);
		}
	}
	for (WaveParticle* p : *particles) {
		p->MultiplyVelocity(sizeIncrease);
		p->SetPosition(position);
		p->SetSize(size);
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
void Wave::Frame(std::vector<Hitbox*>* hitboxes) {
	if (_particles) {
		for (WaveParticle *wp : *waveParticles) {
			for (Hitbox* h : *hitboxes) {
				wp->Collide(h);
			}
			wp->UpdatePosition();
		}
	}
	else {
		IncreaseSize();
		UpdateSize();
	}
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


WaveSource::WaveSource(cv::Point position, cv::Point screenSize, cv::Point sourceSpeed, cv::Point cameraSpeed, int wavedelay, int waveLifetime) : _position(position), _sourceSpeed(sourceSpeed), _cameraSpeed(cameraSpeed), _waveFrequency(wavedelay), _waveLifetime(waveLifetime), _screenSize(screenSize) {
	wavelengthMap = Resources::GetWavelengthToRgbMap(2);

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
	UpdateLifetime();
	UpdatePositions();
}
void WaveSource::UpdatePositions() {
	_position.x += -_cameraSpeed.x + _sourceSpeed.x;
	_position.y += _cameraSpeed.y -_sourceSpeed.y; //reverse y to create a mathematically logical coordinate system (pos y is up and pos x is right)
	_position.x = _position.x < 0 ? 0 : _position.x;
	_position.x = _position.x > _screenSize.x ? _screenSize.x : _position.x;
	_position.y = _position.y < 0 ? 0 : _position.y;
	_position.y = _position.y > _screenSize.y ? _screenSize.y : _position.y;
	for (int i = 0; i < waves.size(); i++) {
		waves[i]->SetSpeed(cv::Point(-_cameraSpeed.x, _cameraSpeed.y));
		waves[i]->Frame(&hitboxes);
	}
}
void WaveSource::UpdateLifetime() {
	for (int i = 0; i < waves.size(); i++) {
		waves[i]->LifetimeDecrease(FrameDelay);
		if (waves[i]->IsDead()) {
			Wave &tempW = (*waves[i]);
			waves.erase(waves.begin() + i);
			tempW.~Wave();
			i--;
		}
	}
}
void WaveSource::SpawnWave() {
	currentWaveDelay += FrameDelay;

	


	if (_particleWave) {
		if (waveParticles == nullptr) {
			waveParticles = new std::vector<WaveParticle*>();
		}
		int currentParticleVectorSize = static_cast<int>(waveParticles->size());
		int waveSpawnDelayLeft = currentWaveDelay > _waveFrequency ? 0 : _waveFrequency - currentWaveDelay;
		float framesLeft = static_cast<float>(waveSpawnDelayLeft) / FrameDelay;
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
			if (waveParticles->size() < particlesPerWave) {
				std::pair<float, float> pos(static_cast<float>(0), static_cast<float>(0));
				int currentParticleVectorSize = static_cast<int>(waveParticles->size());
				for (int i = 0; i < particlesPerWave - currentParticleVectorSize; i++) {
					waveParticles->push_back(new WaveParticle(_cameraSpeed, pos, particleVelocities[i + currentParticleVectorSize]));
				}
			}
			waves.push_back(new Wave(_cameraSpeed, std::pair<float, float>(static_cast<float>(_position.x), static_cast<float>(_position.y)), _waveSpeed, _waveLifetime, &particlesPerWave, waveParticles, 5));
			waveParticles = nullptr; 
		}
		else {
			if (!_lightWave) {
				waves.push_back(new Wave(cv::Point(0, 0), _position, _waveSpeed, _waveLifetime, &particlesPerWave));
			}
			else {
				waves.push_back(new Wave(cv::Point(0, 0), _position, _waveSpeed, _waveLifetime, &particlesPerWave, 3, lightColor));

			}
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
void WaveSource::Draw(cv::Mat img) {
	cv::circle(img, _position, 10, WaveSourceColor, 5);
	//cv::rectangle(img, cv::Point(_position.x - 10, _position.y - 10), cv::Point(_position.x + 10, _position.y + 10), WaveSourceColor, -1);
	

	std::vector<std::thread> drawThreads;
	std::vector<Wave*> *wavePackage = new std::vector<Wave*>();
	
	int wavesPerThread = 2;// waves.size() > 9 ? 4 : 3;

	for (int i = 0; i < waves.size(); i++) {
		wavePackage->push_back(waves[i]);
		if (i % wavesPerThread == wavesPerThread - 1) {
			drawThreads.push_back(std::thread::thread(Resources::DrawWaves, img, wavePackage));
			wavePackage = new std::vector<Wave*>();
		}
		//waves[i]->Draw(img);
	}
	if (wavePackage->size() != 0) {
		drawThreads.push_back(std::thread::thread(Resources::DrawWaves, img, wavePackage));
	}

	for (int i = 0; i < drawThreads.size(); i++) {
		drawThreads[i].join();
	}

	//for (Wave *wave : waves) {
	//	//drawThreads.push_back(std::thread::thread(&Wave::Draw, wave, img));
	//	wave->Draw(img);
	//}
	
	//for (int i = 0; i < drawThreads.size(); i++) {
	//	drawThreads[i].join();
	//}


	//previous time: 6ms / wave, 
	//first update: roughly 14ms for several waves, but goes over 25 at 5 waves
	//second update: roughly 10-11 ms per wave, now deprecated

	for (Hitbox* h : hitboxes) {
		h->Draw(img);
	}
}
void WaveSource::AddHitbox(cv::Point position, cv::Point size) {
	hitboxes.push_back(new Hitbox(Resources::PointToPair(position), Resources::PointToPair(size)));
}


void CreateTrackbars(int* sourceSpeedX, int* reverseX, int* sourceSpeedY, int* reverseY, int* waveDelay, int* waveSpeed, int* waveLifetime) {
	//old and deprecated (should be removed)
	const char* commandWindowName = "Control Panel (deprecated)";

	cv::namedWindow(commandWindowName, cv::WINDOW_FREERATIO);
	cv::resizeWindow(commandWindowName, cv::Size(500, 500));
	cv::createTrackbar("Source Speed X", commandWindowName, sourceSpeedX, 50);
	cv::createTrackbar("Reverse X Speed", commandWindowName, reverseX, 1);
	cv::createTrackbar("Wave Source Speed Y", commandWindowName, sourceSpeedY, 50);
	cv::createTrackbar("Reverse Y Speed", commandWindowName, reverseY, 1);
	cv::createTrackbar("Wave Spawn Delay", commandWindowName, waveDelay, 5000);
	cv::createTrackbar("Wave Speed", commandWindowName, waveSpeed, 75);
	cv::createTrackbar("Wave Lifetime", commandWindowName, waveLifetime, 10000);
}


WaveSimulation::WaveSimulation() {
	_img = cv::Mat::zeros(cv::Size(windowWidth, windowHeight), CV_8UC4);
	cv::namedWindow("Doppler effect", cv::WINDOW_FULLSCREEN);
	cv::resizeWindow("Doppler effect", cv::Size(windowWidth, windowHeight));

	ws = new WaveSource(cv::Point((windowWidth - controlPanelWidth ) / 2, windowHeight / 2), cv::Point(windowWidth - controlPanelWidth, windowHeight));
	
	//CreateTrackbars(&ws->_sourceSpeed.x, &ws->reverseX, &ws->_sourceSpeed.y, &ws->reverseY, &ws->_waveFrequency, &ws->_waveSpeed, &ws->_waveLifetime); //old and deprecated (should be removed)
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

	cv::putText(controlPanelDesign, "Pause:", cv::Point(15, 760), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
	cv::putText(controlPanelDesign, "Simulate Particles:", cv::Point(15, 810), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);

	//adding userinput elements
	TwoDTrackbar* tdtb = ctrlP->AddTwoDTrackBar(cv::Point(25, 100), cv::Point(350, 350), cv::Point(-10, -10), cv::Point(10, 10), ctrlP->GetTopLayer(), &ws->_sourceSpeed.x, &ws->_sourceSpeed.y);
	Trackbar *waveSpeedTB = ctrlP->AddTrackbar(cv::Point(25, 500), cv::Point(350, 50), 0, 10, ctrlP->GetTopLayer(), &ws->_waveSpeed);
	Trackbar *waveFrequencyTB = ctrlP->AddTrackbar(cv::Point(25, 585), cv::Point(350, 50), 100, 7500, ctrlP->GetTopLayer(), &ws->_waveFrequency);
	Trackbar *waveLifetimeTB = ctrlP->AddTrackbar(cv::Point(25, 670), cv::Point(350, 50), 100, 5000, ctrlP->GetTopLayer(), &ws->_waveLifetime);

	ctrlP->AddCheckBox(cv::Point(87, 52), ctrlP->GetTopLayer(), tdtb->GetLockX(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(187, 52), ctrlP->GetTopLayer(), tdtb->GetLockY(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(317, 52), ctrlP->GetTopLayer(), tdtb->GetToCenter(), cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(142, 72), ctrlP->GetTopLayer(), &lockCamera, cv::Point(15, 15));
	ctrlP->AddCheckBox(cv::Point(135, 735), ctrlP->GetTopLayer(), &pause, cv::Point(30, 30));
	ctrlP->AddCheckBox(cv::Point(335, 785), ctrlP->GetTopLayer(), &ws->_particleWave, cv::Point(30, 30));

	//adding dynamic texts
	ctrlP->AddDynamicText(new DynamicText<int>(tdtb->GetValOne(), cv::Point(260, 35), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(tdtb->GetValTwo(), cv::Point(320, 35), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveSpeedTB->GetValue(), cv::Point(340, 490), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveFrequencyTB->GetValue(), cv::Point(330, 575), 1.2));
	ctrlP->AddDynamicText(new DynamicText<int>(waveLifetimeTB->GetValue(), cv::Point(320, 660), 1.2));
	
	//showing the controlpanel
	ctrlP->Draw();
}

void WaveSimulation::RunSimulation() {
	cv::TickMeter frameTimer;
	cv::TickMeter lagTimer;
	for (;;) {
		frameTimer.start();
		_img = cv::Mat::zeros(cv::Point(windowWidth, windowHeight), CV_8UC4);
		if (!pause) {
			ws->LockCamera(lockCamera);
			ws->Frame();
		}

		lagTimer.start();
		ws->Draw(_img); //this is creating lag, probably because of all the angles that need to be written, averaging on 10 ms per currently existing wave
		lagTimer.stop();
		//std::cout << "lagtimer, milliseconds: " << lagTimer.getTimeMilli() << "\n";
		lagTimer.reset();

		ctrlP->Draw();

		cv::imshow("Doppler effect", _img);
		//supposed to smooth the framerate, but since the ws->draw() is unoptimized, it doesn't do anything
		frameTimer.stop();
		int timer = frameTimer.getTimeMilli();
		int frameDelay = timer >= 25 ? 5 : 30 - timer;
		std::cout << frameDelay << "\n";
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


