#include <signal.h>
#include "TimeTabling.h"
#include "Individual.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

////////////////////////////Individual information ////////////////////////////////
///This function computes constraints in the following order:
// *implicit distributions by rooms: no classes assigned to the same room at overlaping time, and without unavalibility time by room.
// *hard constraints (distributions) by pairs: just comparison of distributions between paris.
// *overall constraints (distributions) by all: comparison of all the class in the distributions.
// *soft pair constraints (distributions).
// *soft overall distributions.
// *room penalization: a penalization assigned to each room in relation with each classs.
// *time penalization: a penalization assigned to each time in relation with each class. 
// *student penalization: a penalization to each student conflict.
pair<long long, long long> Individual::calculateFitness(vector<pair<int,int>> &x_ind){

    for(int i = 0; i < x_var.size(); i++)
    {
	TTP->x_var_time[i] = x_ind[i].first;
	TTP->x_var_room[i] = x_ind[i].second;
    }
    long long hard_constraints_violated = TTP->Hard_constraints();
    long long distribution_soft_penalizations = TTP->Soft_constraints();
    long long room_penalization_v = TTP->room_penalization();
    long long time_penalization_v = TTP->time_penalization();
    long long student_penalization_v = student_penalization_v = TTP->student_penalization();
    long long TotalFitness = distribution_soft_penalizations*this->TTP->distribution_w;

    TotalFitness += room_penalization_v*this->TTP->room_w;
    TotalFitness += time_penalization_v*this->TTP->time_w;
    TotalFitness += student_penalization_v*this->TTP->student_w;
    return make_pair( TotalFitness, hard_constraints_violated);
}
pair<long long, long long> Individual::incremental_evaluation(vector<int> &classes_to_check, vector<pair<int,int>> &x_ind){
    for(int i = 0; i < classes_to_check.size(); i++)
    {
	int id_class = classes_to_check[i];
	TTP->x_var_time[id_class] = x_ind[id_class].first;
	TTP->x_var_room[id_class] = x_ind[id_class].second;
   }
   return TTP->incremental_evaluation_by_classes(classes_to_check);
}
void printBest(){
	
}
void Individual::iterated_local_search()
{
  int maxite = 100000;
  //Apply local search to the individual
  vector<pair<int, int>> current_indiv = x_var, best_indiv = x_var;
  long long best_f = mix_penalizations(calculateFitness(best_indiv)); 
  while(true)
  {
     //perturb..
  //      current_indiv = iterated_forward_search(maxite, current_indiv);
     //apply local-search..
        current_indiv = localSearch_for_ILS(maxite, current_indiv);
          cout << best_f << "----" <<endl;
        long long current_f = mix_penalizations(calculateFitness(current_indiv)); 
        if( current_f < best_f)
        {
           current_indiv = best_indiv;
           best_f = current_f;
          cout << best_f << "----" <<endl;
        }
	current_indiv = best_indiv;
	perturb(current_indiv);
  }
}
void Individual::perturb(vector<pair<int, int>> &current_indiv)
{
  vector<int> unassigned = TTP->unassign_hard_distributions(current_indiv);
  int idx_var, idx_unassigned=-1;
  ///select a variable..
  if( !unassigned.empty() ) 
    {
       idx_unassigned = rand()%unassigned.size();
       idx_var = unassigned[idx_unassigned];
    }
    else idx_var =  rand()%domain.size();
   //select a value 
   pair<int, int> value = random_domain(idx_var);
   current_indiv[idx_var] = value;
}
vector<pair<int, int>> Individual::localSearch_for_ILS(int maxite, vector<pair<int, int>> &base_var)
{
  vector<pair<int, int>> current_indiv=base_var, best_indiv = base_var;
 vector<int> perm;
 for(int i = 0; i < domain.size(); i++)perm.push_back(i);
  random_shuffle(perm.begin(), perm.end());
 int variables_to_modifiy=1;
 vector<int> variables(variables_to_modifiy); 
  int cont = 0;
int v=0;
 while(cont++  < maxite)
 {
    //looking a variable 
    int idx_var = perm[v++];
     v %= domain.size();
    variables[0]= idx_var;
    pair<int, int> opc = random_domain(idx_var);
    current_indiv[idx_var] = opc;
    long long current_f = mix_penalizations(incremental_evaluation(variables, current_indiv)); 
    long long best_f = mix_penalizations(incremental_evaluation(variables, best_indiv)); 
    if(best_f > current_f)
    {
	//check all options of var idx_var
	for(int i = 0; i < domain[idx_var].size(); i++)
	{
 	   current_indiv[idx_var] = domain[idx_var][i];
	   long long current_f = mix_penalizations(incremental_evaluation(variables, current_indiv)); 
    	   long long best_f = mix_penalizations(incremental_evaluation(variables, best_indiv)); 
    	   if(best_f > current_f)
	   {
	     best_indiv[idx_var] = domain[idx_var][i];
             pair<long long, long long> p = calculateFitness(best_indiv);
             cout << mix_penalizations(p)<<endl;
		if(p.second==0) exit(0);
	   }
	}
	cont = 0;
    }
 }
  return best_indiv;
}
vector<pair<int, int> > Individual::iterated_forward_search(int maxite, vector<pair<int, int >> &base_indiv)
{
     vector<pair<int, int>> current_indiv=base_indiv, best_indiv = base_indiv;
   int cont = 0;
   
  //get the initial classes in conflict..
  vector<int> unassigned = TTP->unassign_hard_distributions(base_indiv);
  vector<int> variables(1);
  while(cont++ < maxite)
  {
    int idx_var, idx_unassigned=-1;
    ///select a variable..
    if( !unassigned.empty() ) 
    {
       idx_unassigned = rand()%unassigned.size();
       idx_var = unassigned[idx_unassigned];
    }
    else idx_var =  rand()%domain.size();
   //select a value 
   pair<int, int> value = random_domain(idx_var);
   current_indiv[idx_var] = value;
   variables[0]=idx_var;
   pair<long long, long long> current_f = incremental_evaluation(variables, current_indiv); 

   pair<long long, long long> best_f = incremental_evaluation(variables, best_indiv); 
   if( best_f.first > current_f.first )
   {
      best_indiv[idx_var] = current_indiv[idx_var];
      pair<long long, long long> p = calculateFitness(best_indiv);
             cout << p.first << " " << p.second<<endl;
      if(current_f.second)
         unassigned = TTP->unassign_hard_distributions(best_indiv);
	else exit(0);
      cont =0;
   }
  }  
  return best_indiv;
}
void Individual::localSearch()
{

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
