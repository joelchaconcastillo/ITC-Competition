#include "MA.h"

int main(int argc, char **argv){
	int N = 50;
	double pc = 0.9;
	double pm = 0.01;
	double finalTime = 25 * 60;
	srand(time(NULL));

	///string file = string(argv[1]);
//	string file = "Instances/public/iku-fal17.xml";
	//string file = "Instances/public/wbg-fal10.xml";
//	string file = "Instances/public/tg-fal17.xml";
	//string file = "Instances/public/yach-fal17.xml";
	string file = "Instances/public/agh-fis-spr17.xml";
//	string file = "Instances/public/muni-pdf-spr16.xml";
	TimeTablingProblem TTP(file);
//	Individual::TimeTablingproblem = &TTP;

	MA ma(N, pc, pm, finalTime, TTP);
	ma.run();
}
