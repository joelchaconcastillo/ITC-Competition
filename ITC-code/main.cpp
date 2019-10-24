#include "MA.h"

int main(int argc, char **argv){
	int N = 1;
	double pc = 0.9;
	double pm = 0.01;
	double finalTime = 25 * 60;
	srand(time(NULL));

	///string file = string(argv[1]);
	string file = "Instances/public/iku-fal17.xml";
//	string file = "Instances/public/wbg-fal10.xml";
//	string file = "Instances/public/tg-fal17.xml";
	//string file = "Instances/public/yach-fal17.xml";
	//string file = "Instances/public/agh-fis-spr17.xml";
	//string file = "Instances/public/muni-pdf-spr16.xml";
	//string file = "Instances/public/yach-fal17.xml";
	

	TimeTablingProblem TTP(file);
	//MA ma(N, pc, pm, finalTime, TTP);
	//ma.run();
	//
	///Temporal para probar la busqueda local...
	  Individual indiv(TTP);
	  cout << indiv.calculateFitness().first <<endl;
	  cout << indiv.calculateFitness().second <<endl;
	  indiv.save_xml();
}
