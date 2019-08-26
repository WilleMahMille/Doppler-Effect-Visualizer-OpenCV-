#include "Simulation.h"


int main(int argc, char* argv[]) {

	cv::Mat img;
	bool myBool = true;
	ControlPanel ctrlP = ControlPanel("Control Panel numba one", cv::Size(500, 500));
	ctrlP.AddCheckBox(cv::Point(40, 40), ctrlP.GetTopLayer(), &myBool);
	ctrlP.Draw();

	cv::waitKey(0);

	//WaveSimulation waves = WaveSimulation();
	//waves.RunSimulation();



	
}
