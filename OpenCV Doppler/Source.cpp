#include "Simulation.h"


int main(int argc, char* argv[]) {
	
	std::pair<float, float> pairOne(40, 40);

	std::pair<float, float> pairTwo(60, 60);

	pairOne += pairTwo;
	std::cout << "First: " << pairOne.first << " second: " << pairOne.second << "\n";

	while (true) {
		
	}

	//WaveSimulation waves = WaveSimulation();
	//waves.RunSimulation();



	
}
