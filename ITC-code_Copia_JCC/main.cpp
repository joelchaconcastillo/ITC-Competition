#include "MA.h"

int main(int argc, char **argv){
	int N = 1;
	double pc = 0.9;
	double pm = 0.01;
	double finalTime = 25 * 60;
	unsigned long l = 1572386715;
	l = 1572386933;
l = 1572566617;
      l = time(NULL);
//l=1572467997;
//	l = 1572464917;
//	l = 1572389008;
////	l = 1572439927;
	cout << l <<endl;
	srand(l);

//////no student instances..

	string file = "Instances/public/iku-fal17.xml";
	//string file = "Instances/public/tg-fal17.xml";

	///string file = string(argv[1]);
 //Test instances..
	//string file = "Instances/public/wbg-fal10.xml";
	//string file = "Instances/public/lums-sum17.xml";
	//string file = "Instances/public/bet-sum18.xml";
	//string file = "Instances/public/pu-cs-fal07.xml";
	//string file = "Instances/public/pu-llr-spr07.xml"; //***** wrong instance
	//string file = "Instances/public/pu-c8-spr07.xml"; //****
 //Early instances..
	//string file = "Instances/public/agh-fis-spr17.xml";
	//string file = "Instances/public/agh-ggis-spr17.xml";
	//string file = "Instances/public/bet-fal17.xml";
	//string file = "Instances/public/iku-fal17.xml";
	//string file = "Instances/public/mary-spr17.xml";
	//string file = "Instances/public/muni-fi-spr16.xml";
	//string file = "Instances/public/muni-fsps-spr17.xml";
	//string file = "Instances/public/muni-pdf-spr16c.xml";
	//string file = "Instances/public/pu-llr-spr17.xml";
	//string file = "Instances/public/tg-fal17.xml";
 //Middle
//	string file = "Instances/public/yach-fal17.xml";




//	string file = "Instances/public/wbg-fal10.xml";
//	string file = "Instances/public/tg-fal17.xml";
	//string file = "Instances/public/agh-fis-spr17.xml";
	//string file = "Instances/public/muni-pdf-spr16.xml";

	//maxdayload
	//string file = "Instances/public/yach-fal17.xml";
//	string file = "Instances/public/muni-pdf-spr16.xml";
	//string file = "Instances/public/agh-h-spr17.xml"; //***
	//string file = "Instances/public/muni-pdf-spr16c.xml";
//	string file = "Instances/public/bet-fal17.xml";
	

	TimeTablingProblem TTP(file);
	//MA ma(N, pc, pm, finalTime, TTP);
	//ma.run();
	//
	///Temporal para probar la busqueda local...
	  Individual indiv(TTP);
        //  cout << TTP.get_var_time_size()<< endl;
	//  cout << TTP.get_var_room_size()<< endl;
//	  pair<long long, int > p = indiv.calculateFitness(indiv.x_var);
  	  //indiv.iterated_local_search();
  	  indiv.iterated_forward_search_vns();
  	  //indiv.iterated_forward_search(100000, indiv.x_var);
//for(int i = 0; i < 10000; i++)
	//   p = indiv.calculateFitness(indiv.x_var);
	 // cout <<  p.first<< " " <<  p.second<<endl;
	  TTP.save_xml(indiv.x_var, TTP.x_var_student);
       

	  return 0;
}
