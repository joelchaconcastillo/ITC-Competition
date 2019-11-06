#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__
#include <bits/stdc++.h>
using namespace std;
#define FOREACH(i, v) for (__typeof((v).begin()) i = (v).begin(); i != (v).end(); i++)


void printBest();

class Individual{
	public:
		struct fitness
		{
		   long long fitness_v;
		   long long room_penalization_v, time_penalization_v, student_penalization_v, soft_distributions_v, hard_distributions_v;
		};
		Individual(TimeTablingProblem &TTP_){
		  this->TTP= &TTP_;
		  this->domain = TTP->domain;
		
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

		vector<pair<int, int>> localSearch_for_ILS(int maxite, vector<pair<int, int>> &base_var);
		vector<pair<int,int>>iterated_forward_search(int maxite, vector<pair<int, int >> &base_indiv);
		inline long long mix_penalizations(pair<long long, long long> p){ return p.first+p.second*10000;}
		void perturb(vector<pair<int, int>> &current_indiv);
		void iterated_local_search();
		void localSearch();

		inline pair<int, int> random_domain(int idx_var){return domain[idx_var][rand()%domain[idx_var].size()];}

		pair<long long, long long> calculateFitness(vector<pair<int,int>> &x_ind);
		pair<long long, long long> incremental_evaluation(vector<int> &classes_to_check, vector<pair<int,int>> &x_ind);
		void print();
		long long fitness;
		//static TimeTablingProblem *TimeTablingproblem;
		TimeTablingProblem *TTP;
 		int dist;
		vector<pair<int, int>> x_var;
		vector<vector<pair<int,int>>> domain;
};

#endif
