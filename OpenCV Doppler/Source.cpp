#include "Simulation.h"


int main(int argc, char* argv[]) {
	/*
	std::pair<float, float> pairOne(40.86, 40);

	std::pair<float, float> pairTwo(60, 60);

	pairOne += pairTwo;
	std::cout << "First: " << pairOne.first << " second: " << pairOne.second << "\n";

	while (true) {
		
	}*/
	Resources::GetWavelengthToRgbMap(2);
	WaveSimulation waves = WaveSimulation();
	waves.RunSimulation();


	
}
