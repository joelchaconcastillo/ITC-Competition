#include <signal.h>
#include "TimeTabling.h"
#include "Individual.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

////////////////////////////Individual information ////////////////////////////////
pair<long long, int> Individual::calculateFitness(vector<pair<int,int>> &x_ind){

   long long hard_constraints_violated = 0;
 //  for(int i = 0; i < TTP->from_table_to_class[TIMES].size() ; i++)
 //  {
 //    TTP->x_var_time[TTP->from_table_to_class[TIMES][i]] = x_ind[i];
 //  }

 //  for(int i = TTP->from_table_to_class[TIMES].size(), j = 0; i < TTP->from_table_to_class[TIMES].size()+ TTP->from_table_to_class[ROOMS].size()  ; i++, j++)
 //  {
 //    TTP->x_var_room[TTP->from_table_to_class[ROOMS][j]] = x_ind[i];
 //  }
    for(int i = 0; i < x_var.size(); i++)
    {
	TTP->x_var_time[i] = x_ind[i].first;
	TTP->x_var_room[i] = x_ind[i].second;
    }
    vector<bool> invalid_variables(TTP->x_var_room.size(), false); 
    hard_constraints_violated += TTP->implicit_room_constraints(invalid_variables);
    hard_constraints_violated += TTP->hard_constraints_by_pairs(invalid_variables);
    hard_constraints_violated += TTP->overall_hard_constraints(invalid_variables);

 long long distribution_soft_penalizations= 0;
    distribution_soft_penalizations+= TTP->soft_constraints_by_pairs(invalid_variables);
    distribution_soft_penalizations+= TTP->overall_soft_constraints(invalid_variables);

 long long room_penalization_v = TTP->room_penalization(invalid_variables);
 long long time_penalization_v = TTP->time_penalization(invalid_variables);
long long student_penalization_v = student_penalization_v = TTP->student_penalization();
///
  long long TotalFitness = distribution_soft_penalizations*this->TTP->distribution_w;

   TotalFitness += room_penalization_v*this->TTP->room_w;
   TotalFitness += time_penalization_v*this->TTP->time_w;
   TotalFitness += student_penalization_v*this->TTP->student_w;
//   vector<int> hard_classes;
//  for(int i = 0; i < invalid_variables.size(); i++) if(invalid_variables[i]) hard_classes.push_back(i);
///
  return make_pair( TotalFitness, hard_constraints_violated);
}

void printBest(){
	
}

void Individual::localSearch(){
 
bool hardsolved = false;
 pair<long long, int > x_p = calculateFitness(x_var);
 long long best = x_p.first + x_p.second*10000;
 bool good_variable = false;
 int idx_var = -1;//rand()%x_var.size();
 int maxite = 1000000;
 int cont = 0;
 while(cont < maxite )
 {
    vector<pair<int, int>> cu_var = x_var;

 for(int i = 0; i < (rand()%5) + 1; i++) 
 {
    idx_var = rand()%x_var.size();
    cu_var[idx_var] =  domain[idx_var][rand()%domain[idx_var].size()];
 }
    pair<long long,int> p = calculateFitness(cu_var);
    long long current = p.first + p.second*10000;
     
//	 cout << best << " "<<p.first << " "<<p.second  <<endl;
//	 cout << current<< " "<<p.first << " "<<p.second  <<endl;
     if(current < best)
	{
	   best = current;
	   this->x_var = cu_var;
	 cout << best << " "<<p.first << " "<<p.second  <<endl;
	cont = 0;
	}
  if( p.second == 0 ) break;
//    cont++;
 }
}

int Individual::getDistance(Individual &ind){
return 0;
}
void Individual::Mutation(double pm){
}
void Individual::Crossover(Individual &ind){
}
void Individual::print(){
}
void Individual::initialization()
{
   this->x_var.resize(TTP->classes.size());
   for(int i = 0;i < this->x_var.size(); i++)
   {
	if(domain[i].empty())
	   continue;
	int size_domain = domain[i].size();
	this->x_var[i] = domain[i][rand()%size_domain];
   }
}
