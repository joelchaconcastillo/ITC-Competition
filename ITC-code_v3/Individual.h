#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__
#include <bits/stdc++.h>
using namespace std;
#define FOREACH(i, v) for (__typeof((v).begin()) i = (v).begin(); i != (v).end(); i++)


void printBest();

class Individual{
	public:
		Individual(TimeTablingProblem &TTP_){
		  this->TTP= &TTP_;
		  x_var_time.resize(TTP->classes.size(), NOT_SET);
		  x_var_room.resize(TTP->classes.size(), NOT_SET);
		  x_var_student.resize(TTP->students.size());
		  // exit(0);
		  //initialization....
		  initialization();
		  //Load example solution to check the evaluator...
		// cout << "Feasible space (Domain size) by room.... in this individual " << get_var_room_size() <<endl;
		 //cout << "Feasible space (Domain size) by time.... in this individual " << get_var_time_size() <<endl;
                  //loading_example(); // solution-wbg-fal10.xml ...
		  //cout << this->calculateFitness().first<<endl;
		//   save_xml();
		}
		Individual(){}
		~Individual(){
		}
//	        inline int first(long long int bin){ int pos =0; while( !(bin & (1<<pos)) )pos++; return pos;  }		
		void initialization();
		int getDistance(Individual &ind);
		void Mutation(double pm);
		void Crossover(Individual &ind);
		void localSearch();
		pair<long long, int> calculateFitness(vector<int> &x_ind);
		void print();
		long long fitness;
		//static TimeTablingProblem *TimeTablingproblem;
		TimeTablingProblem *TTP;
 		int dist;
		vector<int> x_var, x_var_time, x_var_room;
		vector< vector<int> > x_var_student;
};

#endif
