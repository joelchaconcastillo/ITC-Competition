#include "MA.h"

int main(int argc, char **argv){
	int N = 1;
	double pc = 0.9;
	double pm = 0.01;
	double finalTime = 25 * 60;
//      unsigned long l = 1572372098;//   1572370994;  //1572357183;//time(NULL);
      //unsigned long l =    1572370994;  //1572357183;//time(NULL);
      unsigned long l = time(NULL);
	cout << l <<endl;
	srand(l);
	//srand(1);

	///string file = string(argv[1]);
//	string file = "Instances/public/iku-fal17.xml";
//	string file = "Instances/public/wbg-fal10.xml";
//	string file = "Instances/public/tg-fal17.xml";
	//string file = "Instances/public/agh-fis-spr17.xml";
//	string file = "Instances/public/muni-pdf-spr16.xml";

	//maxdayload
//	string file = "Instances/public/yach-fal17.xml";
	string file = "Instances/public/muni-pdf-spr16.xml";
	//string file = "Instances/public/agh-h-spr17.xml";
	//string file = "Instances/public/bet-fal17.xml";
	//string file = "Instances/public/muni-pdf-spr16c.xml";
//	string file = "Instances/public/bet-fal17.xml";
	

	TimeTablingProblem TTP(file);
	//MA ma(N, pc, pm, finalTime, TTP);
	//ma.run();
	//
	///Temporal para probar la busqueda local...
	  Individual indiv(TTP);
	  pair<long long, int > p = indiv.calculateFitness();
	  cout <<  p.first<< " " <<  p.second<<endl;
          TTP.save_xml(indiv.x_var_room, indiv.x_var_time, indiv.x_var_student);
	  return 0;
}
