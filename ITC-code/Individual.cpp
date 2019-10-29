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
pair<long long, int> Individual::calculateFitness(){



int cont = 0;
   int hard_constraints_violated = 0;

//////////////////Hard distributions//////////////////////////////////////////////////
   vector< vector<int> > Graph_Hard_Constraints(x_var_time.size());
    ///A class cannot be placed in a room when its assigned time overlaps with an unavailability of the room or when there is other class placed in the room at an overlapping time.
    ///When it happend a room is dropped out
    vector<bool> invalid_variables(x_var_room.size(), false); 
    hard_constraints_violated += TTP->implicit_room_constraints(x_var_time, x_var_room, Graph_Hard_Constraints, invalid_variables);

    hard_constraints_violated += TTP->hard_constraints_by_pairs(x_var_time, x_var_room, Graph_Hard_Constraints, invalid_variables);



     int cont_unassigned_variables = 0;
    ////Summary of unavailable variables...
    for(int i = 0; i < x_var_time.size(); i++) //for each class
    {
	if(invalid_variables[i]) continue;

	for(int j = 0; j < Graph_Hard_Constraints[i].size(); j++)
	{
//	  if( Graph_Hard_Constraints[i][j] < i) continue;
	  invalid_variables[Graph_Hard_Constraints[i][j]] = true;
	}
    }


    hard_constraints_violated += TTP->overall_hard_constraints(x_var_time, x_var_room, Graph_Hard_Constraints, invalid_variables);

    for(int i = 0; i < x_var_time.size(); i++) //for each class
	if(invalid_variables[i]) cont_unassigned_variables++;
    cout <<"unasigned " <<cont_unassigned_variables<<endl;

    for(int i = 0; i < x_var_time.size(); i++) //for each class
	if(invalid_variables[i]) cout << i+1 <<endl;


 long long distribution_soft_penalizations= 0;


    distribution_soft_penalizations+= TTP->soft_constraints_by_pairs(x_var_time, x_var_room, invalid_variables);
    distribution_soft_penalizations+= TTP->overall_soft_constraints(x_var_time, x_var_room, invalid_variables, Graph_Hard_Constraints);
	cout <<"soft "<< distribution_soft_penalizations <<endl;
///  ///Checking distributions between pair classes..

///  ///Checking room penalizations..
///  int assigned_variables = 0;
  long long room_penalization_v = TTP->room_penalization(x_var_room, invalid_variables);
    cout << "room " << room_penalization_v <<endl;
///
///
    long long time_penalization_v = TTP->time_penalization(x_var_time, invalid_variables);
  
cout << "time" << " " <<time_penalization_v <<endl;
    long long student_penalization = 0; //check student conflicts...
//  for(int i = 0; i < this->x_var_student.size(); i++)
//  {
//     if(TTP->conflicts_student(i,x_var_time, x_var_room ))
//      student_penalization++;
//  }
///
///  cout << "Total distributions penalty: " <<distribution_soft_penalizations <<endl;
///  cout << "Total room penalty: "<< room_penalization <<endl;//*this->TTP->room_w  <<endl;
///  cout << "Total time penalty: " << time_penalization*this->TTP->time_w <<endl;
///  cout << "Total student conflicts: " << student_penalization*this->TTP->student_w <<endl;
///
  return make_pair(distribution_soft_penalizations*this->TTP->distribution_w + room_penalization_v*this->TTP->room_w + time_penalization_v*this->TTP->time_w + student_penalization*this->TTP->student_w, cont_unassigned_variables);

}

//Individual bestI;

void printBest(){
	
}
void Individual::localSearch(){
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

   //Preparing domain-space... PENDIENTE

   //////
   for(int i = 0; i < this->x_var_time.size(); i++)
   {
      x_var_time[i] = this->TTP->classes[i].times[rand()% this->TTP->classes[i].times.size()];
      if( this->TTP->classes[i].rooms_c.empty()  )
	x_var_room[i] = NOT_SET;
      else
        x_var_room[i] = this->TTP->classes[i].rooms_c[rand()% this->TTP->classes[i].rooms_c.size()].first;
   }
//   for(int i = 0; i < this->x_var_student.size(); i++)
//   {
//        
//   }
}
