#include <signal.h>
#include "TimeTabling.h"
#include "Individual.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

////////////////////////////Individual information ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
pair<long long, int> Individual::calculateFitness(vector<int> &x_ind){

   long long hard_constraints_violated = 0;
   for(int i = 0; i < TTP->from_table_to_class[TIMES].size() ; i++)
   {
     TTP->x_var_time[TTP->from_table_to_class[TIMES][i]] = x_ind[i];
   }

   for(int i = TTP->from_table_to_class[TIMES].size(), j = 0; i < TTP->from_table_to_class[TIMES].size()+ TTP->from_table_to_class[ROOMS].size()  ; i++, j++)
   {
     TTP->x_var_room[TTP->from_table_to_class[ROOMS][j]] = x_ind[i];
   }
  //////////////////Hard distributions//////////////////////////////////////////////////
    vector<bool> invalid_variables(TTP->x_var_room.size(), false); 
   // TTP->x_var_time = x_var_time;
   // TTP->x_var_room = x_var_room;

    hard_constraints_violated += TTP->implicit_room_constraints(invalid_variables);
    hard_constraints_violated += TTP->hard_constraints_by_pairs(invalid_variables);
    hard_constraints_violated += TTP->overall_hard_constraints(invalid_variables);


 long long distribution_soft_penalizations= 0;

    distribution_soft_penalizations+= TTP->soft_constraints_by_pairs(invalid_variables);
    distribution_soft_penalizations+= TTP->overall_soft_constraints(invalid_variables);
//	cout <<"soft "<< distribution_soft_penalizations <<endl;
///  ///Checking distributions between pair classes..

///  ///Checking room penalizations..
///  int assigned_variables = 0;
  long long room_penalization_v = TTP->room_penalization(invalid_variables);
  //  cout << "room " << room_penalization_v <<endl;
///
///
    long long time_penalization_v = TTP->time_penalization(invalid_variables);
  
//cout << "time" << " " <<time_penalization_v <<endl;
    long long student_penalization_v = 0; //check student conflicts...
    student_penalization_v = TTP->student_penalization();
///
///  cout << "Total distributions penalty: " <<distribution_soft_penalizations <<endl;
///  cout << "Total room penalty: "<< room_penalization <<endl;//*this->TTP->room_w  <<endl;
///  cout << "Total time penalty: " << time_penalization*this->TTP->time_w <<endl;
///  cout << "Total student conflicts: " << student_penalization*this->TTP->student_w <<endl;

  long long TotalFitness = distribution_soft_penalizations*this->TTP->distribution_w;
   TotalFitness += room_penalization_v*this->TTP->room_w;
   TotalFitness += time_penalization_v*this->TTP->time_w;
   TotalFitness += student_penalization_v*this->TTP->student_w;

///
  return make_pair( TotalFitness, hard_constraints_violated);

}

//Individual bestI;

void printBest(){
	
}
void Individual::localSearch(){
  
bool hardsolved = false;
 pair<long long, int> x_p = calculateFitness(x_var);
 long long best = x_p.first + x_p.second*10000;
 while(!hardsolved)
 {
    vector<int> cu_var = x_var;
    int idx_var = rand()%x_var.size();
    int size_domain =  TTP->linear_domain[idx_var].size();
    cu_var[idx_var] =  TTP->linear_domain[idx_var][rand()%size_domain];

     pair<long long, int> p = calculateFitness(cu_var);
     long long current = p.first + p.second*10000;
     if(current < best)
	{
	   best = current;
	   x_var = cu_var;
	 cout << best <<endl;
	}
    if(p.second ==0) hardsolved = true;

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
   this->x_var.resize(TTP->linear_domain.size());
   for(int i = 0;i < TTP->linear_domain.size(); i++)
   {
	int size_domain = TTP->linear_domain[i].size();
	this->x_var[i] = TTP->linear_domain[i][rand()%size_domain];
   }
}
