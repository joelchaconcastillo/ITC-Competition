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
		Individual(TimeTablingProblem TTP_){
		 	        	
		  this->TTP = TTP_;
		 domain = TTP.domain;
		
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
		inline pair<long long, long long> calculateFitness(vector<pair<int,int>> &x_ind){ return TTP.evaluator(x_ind);}
		inline pair<long long, long long> incremental_evaluation(vector<int> &classes_to_check, vector<pair<int,int>> &x_ind, vector<set<int>> &state_rooms){  return TTP.incremental_evaluation_by_classes(classes_to_check, x_ind, state_rooms);}
		inline pair<long long, long long> incremental_evaluation(vector<int> &classes_to_check, vector<pair<int,int>> &x_ind, vector<int> &N){

		long long cont = 0;
		//return TTP.incremental_evaluation_by_classes(classes_to_check, x_ind);
		for(int i = 0; i < N.size(); i++)
                {
                    int id_class_i = N[i];
                    TimeTablingProblem::Time C_ti = TTP.times[x_ind[id_class_i].first];
                   for(int j = i+1; j < N.size(); j++)
                    {
                    	int id_class_j = N[j];

                    	if(x_var[id_class_i].second != x_ind[id_class_j].second) continue;

                    	TimeTablingProblem::Time C_tj = TTP.times[x_ind[id_class_j].first];
                    	if(TTP.Overlap(C_ti, C_tj))
                    	{
                    	   cont++;
                    	}
                    }
                }
		return make_pair(0,cont);

		}
		vector<pair<int, int>> localSearch_for_ILS(int maxite, vector<pair<int, int>> &base_var);
		vector<pair<int,int>>iterated_forward_search(int maxite, vector<pair<int, int >> &base_indiv);
		inline long long mix_penalizations(pair<long long, long long> p){ return p.second*10000;}
		void perturb(vector<pair<int, int>> &current_indiv);
		void iterated_local_search();
		void localSearch();

		void iterated_forward_search_vns();

		void local_search_neighborhood(vector<int> & variables,vector<pair<int, int>> &original_indiv, int Nvariables, bool &improved,  vector<set<int>> &state_rooms);
		inline pair<int, int> random_domain(int idx_var){return domain[idx_var][rand()%domain[idx_var].size()];}

		void print();
		long long fitness;
		//static TimeTablingProblem *TimeTablingproblem;
		TimeTablingProblem TTP;
 		int dist;
		vector<pair<int, int>> x_var;
		vector<vector<pair<int,int>>> domain;

		void best_value_indiv(int id, vector<pair<int, int>> &current_indiv, vector<pair<int, int>> &best_indiv, vector<set<int>> & state_rooms);
};

#endif
