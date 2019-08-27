#include "Simulation.h"


int main(int argc, char* argv[]) {

	cv::Mat img;
	cv::Point topVal(50, 50), botVal(-75, -50);
	double valOne = 10, valTwo = 20;
	bool myBool = false;
	ControlPanel ctrlP = ControlPanel("Control Panel numba one", cv::Size(500, 500));
	ctrlP.AddTwoDTrackBar(cv::Point(50, 50), cv::Point(400, 400), cv::Point(-50, -50), cv::Point(50, 50), ctrlP.GetTopLayer(), &valOne, &valTwo);
	ctrlP.Draw();

	cv::waitKey(0);

	//WaveSimulation waves = WaveSimulation();
	//waves.RunSimulation();



	
}
