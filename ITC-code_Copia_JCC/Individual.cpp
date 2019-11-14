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

void printBest(){
	
}
void Individual::iterated_forward_search_vns()
{
    
    vector<pair<int, int>> current_indiv = x_var, best_indiv = x_var;
    long long best_f = mix_penalizations(calculateFitness(best_indiv)); 


    vector<set<int>> state_rooms;
                                                                                     
    while(true)
    {
        //Making conflicting neighbourhood 
  	vector<vector<int>> component = TTP.link_hard_distributions_variables(current_indiv, state_rooms);

	if(component.empty()) break;
	bool improved=false;
//	int idx = rand()%component.size();
	for(int idx = 0; idx < component.size(); idx++)
	local_search_neighborhood(component[idx], current_indiv, 1, improved, state_rooms);

    	long long current_f = mix_penalizations(calculateFitness(current_indiv)); 
	if(current_f < best_f)
	{
	   best_indiv = current_indiv;
	   best_f = current_f;
	   x_var = best_indiv;
	   cout << "improved...: "<<best_f <<endl; 
           cout  << mix_penalizations(calculateFitness(best_indiv)) <<" "<<best_f<<endl;
	}
	else current_indiv = best_indiv;

	if(!improved) //perturb..
 	{
	   cout << "no"<<endl;
//	   for(int idx =0; idx < component.size(); idx++)
	   {
	   int idx =  rand()%component.size();
	   int i = rand()%component[idx].size();
///	   for(int i = 0; i < component[idx].size(); i++)
 		current_indiv[component[idx][i]] = random_domain(component[idx][i]);
////	   int idx = rand()%current_indiv.size();
 //		current_indiv[idx] = random_domain(idx);
	   }
	}
    }
  x_var = best_indiv;
}
void Individual::local_search_neighborhood(vector<int> & variables,vector<pair<int, int>> &original_indiv, int Nvariables, bool &improved,  vector<set<int>> &state_rooms)
{
  vector<set<int>> state_rooms_best = state_rooms, state_rooms_current = state_rooms;
  vector<pair<int, int>> current_indiv = original_indiv, best_indiv = original_indiv;
  int maxite = 10;
  int cont = 0;
  vector<int> perm = variables;
  int v = 0;
  vector<int> var(Nvariables);
  while(cont++ < maxite)
  {
    random_shuffle(perm.begin(), perm.end());
    for(int i = 0; i < perm.size(); i++)
    {  
       var[0] = perm[i];
       current_indiv[perm[i]] =  random_domain(perm[i]);
//        if(current_indiv[perm[i]].second!= NOT_ROOM)
//	{
//          state_rooms_current[current_indiv[perm[i]].second].insert(perm[i]);
//          state_rooms_current[best_indiv[perm[i]].second].erase(perm[i]);
//	}
    	long long best_f = mix_penalizations(incremental_evaluation(var, best_indiv, state_rooms_best)); 
        long long current_f = mix_penalizations(incremental_evaluation(var, current_indiv, state_rooms_current)); 

        if( current_f < best_f)  
        {
           best_f = current_f;
           best_indiv[perm[i]] = current_indiv[perm[i]];
	//   int r = best_indiv[perm[i]].second;
	//   if( r!=NOT_ROOM)
	//   state_rooms_best[r] = state_rooms_current[r];
        ///   best_value_indiv(perm[i], current_indiv, best_indiv, state_rooms_best);
           cout  << mix_penalizations(calculateFitness(best_indiv)) <<" "<<best_f<<endl;
           cont = 0;
	   improved = true;
        }
//        else current_indiv[perm[i]] = best_indiv[perm[i]];
    }
  }
  original_indiv = best_indiv;
  state_rooms = state_rooms_best;
}
void Individual::best_value_indiv(int id, vector<pair<int, int>> &current_indiv, vector<pair<int, int>> &best_indiv, vector<set<int>> &state_rooms )
{
  vector<int> v(1);v[0] = id;
  vector<set<int>> state_rooms_best = state_rooms, state_rooms_current = state_rooms;
  long long best_f = mix_penalizations(incremental_evaluation(v, best_indiv, state_rooms_best)); 
  for(int i = 0; i < domain[id].size(); i++)
  {
     current_indiv[id] = domain[id][i]; 
     long long current_f = mix_penalizations(incremental_evaluation(v, current_indiv, state_rooms_current)); 
	if( current_f < best_f)
	{
//	   int r = domain[id][i].second;
//	   if( r != NOT_ROOM)
//	   state_rooms_best[r] = state_rooms_current[r];
	   best_f = current_f;
	   best_indiv[id] =  domain[id][i];
	}
  }
  state_rooms = state_rooms_best;
}
void Individual::iterated_local_search()
{
//  int maxite = 100000;
//  //Apply local search to the individual
//  vector<pair<int, int>> current_indiv = x_var, best_indiv = x_var;
//  long long best_f = mix_penalizations(calculateFitness(best_indiv)); 
//  while(true)
//  {
//     //perturb..
//  //      current_indiv = iterated_forward_search(maxite, current_indiv);
//     //apply local-search..
//        current_indiv = localSearch_for_ILS(maxite, current_indiv);
//          cout << best_f << "----" <<endl;
//        long long current_f = mix_penalizations(calculateFitness(current_indiv)); 
//        if( current_f < best_f)
//        {
//           current_indiv = best_indiv;
//           best_f = current_f;
//          cout << best_f << "----" <<endl;
//        }
//	current_indiv = best_indiv;
//	perturb(current_indiv);
//  }
}
void Individual::perturb(vector<pair<int, int>> &current_indiv)
{
///  vector<int> unassigned = TTP.unassign_hard_distributions(current_indiv);
///  int idx_var, idx_unassigned=-1;
///  ///select a variable..
///  if( !unassigned.empty() ) 
///    {
///       idx_unassigned = rand()%unassigned.size();
///       idx_var = unassigned[idx_unassigned];
///    }
///    else idx_var =  rand()%domain.size();
///   //select a value 
///   pair<int, int> value = random_domain(idx_var);
///   current_indiv[idx_var] = value;
}
vector<pair<int, int>> Individual::localSearch_for_ILS(int maxite, vector<pair<int, int>> &base_var)
{
//  vector<pair<int, int>> current_indiv=base_var, best_indiv = base_var;
// vector<int> perm;
// for(int i = 0; i < domain.size(); i++)perm.push_back(i);
//  random_shuffle(perm.begin(), perm.end());
// int variables_to_modifiy=2;
// vector<int> variables(variables_to_modifiy); 
//  int cont = 0;
//int v=0;
// while(cont++  < maxite)
// {
//    for(int i = 0; i < 2; i ++)
//    {
//       //looking a variable 
//       int idx_var = perm[v++];
//        v %= domain.size();
//       variables[i]= idx_var;
//       pair<int, int> opc = random_domain(idx_var);
//       current_indiv[idx_var] = opc;
//    }
//
//
//    long long current_f = mix_penalizations(incremental_evaluation(variables, current_indiv)); 
//    long long best_f = mix_penalizations(incremental_evaluation(variables, best_indiv)); 
//    if(best_f > current_f)
//    {
//	//check all options of var idx_var
//        for(int j = 0; j < variables.size(); j++)
//        {
//	   int idx_var = variables[j];
//	for(int i = 0; i < domain[idx_var].size(); i++)
//	{
// 	   current_indiv[idx_var] = domain[idx_var][i];
//	   long long current_f = mix_penalizations(incremental_evaluation(variables, current_indiv)); 
//    	   long long best_f = mix_penalizations(incremental_evaluation(variables, best_indiv)); 
//
//    	   if(best_f > current_f)
//	   {
//             for(int i = 0; i < 2; i++)
//             {
//	        best_indiv[variables[i]] = current_indiv[variables[i]];
//             }
//             pair<long long, long long> p = calculateFitness(best_indiv);
//             cout << mix_penalizations(p)<<endl;
//		if(p.second==0) exit(0);
//	   }
//	}
//  	}
//	cont = 0;
//    }
// }
//  return best_indiv;
}
vector<pair<int, int> > Individual::iterated_forward_search(int maxite, vector<pair<int, int >> &base_indiv)
{
//     vector<pair<int, int>> current_indiv=base_indiv, best_indiv = base_indiv;
//   int cont = 0;
//   
//  //get the initial classes in conflict..
//  vector<int> unassigned = TTP.unassign_hard_distributions(base_indiv);
//  vector<int> variables(1);
//  while(cont++ < maxite)
//  {
//    int idx_var, idx_unassigned=-1;
//    ///select a variable..
//    if( !unassigned.empty() ) 
//    {
//       idx_unassigned = rand()%unassigned.size();
//       idx_var = unassigned[idx_unassigned];
//    }
//    else idx_var =  rand()%domain.size();
//   //select a value 
//   pair<int, int> value = random_domain(idx_var);
//   current_indiv[idx_var] = value;
//   variables[0]=idx_var;
//   pair<long long, long long> current_f = incremental_evaluation(variables, current_indiv); 
//
//   pair<long long, long long> best_f = incremental_evaluation(variables, best_indiv); 
//   if( best_f.first > current_f.first )
//   {
//      best_indiv[idx_var] = current_indiv[idx_var];
//      pair<long long, long long> p = calculateFitness(best_indiv);
//             cout << p.first << " " << p.second<<endl;
//      if(current_f.second)
//         unassigned = TTP.unassign_hard_distributions(best_indiv);
//	else exit(0);
//      cont =0;
//   }
//  }  
//  return best_indiv;
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
   this->x_var.resize(TTP.classes.size());
   for(int i = 0;i < this->x_var.size(); i++)
   {
	if(domain[i].empty())
	   continue;
	int size_domain = domain[i].size();
	this->x_var[i] = domain[i][rand()%size_domain];
   }
}
