#include "MA.h"

int main(int argc, char **argv){
	int N = 50;
	double pc = 0.9;
	double pm = 0.01;
	double finalTime = 25 * 60;
	srand(time(NULL));

	//MA ma(N, pc, pm, finalTime);
	///string file = string(argv[1]);
	string file = "Instances/public/iku-fal17.xml";
	TimeTablingProblem TTP(file);
	TimeTabling::TimeTablingproblem = &TTP;
	//ma.run();
}
