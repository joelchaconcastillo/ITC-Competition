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
    vector<bool> grid2(x_var_room.size(), false); 
    vector<bool> grid(x_var_room.size(), false); 
    hard_constraints_violated += TTP->implicit_room_constraints(x_var_time, x_var_room, Graph_Hard_Constraints, grid);
for(int i = 0; i < Graph_Hard_Constraints.size(); i++)
    {
	if(grid2[i]) continue;
	for(int j = 0; j < Graph_Hard_Constraints[i].size(); j++)
	{
//	if( grid[Graph_Hard_Constraints[i][j]]  ) continue;
	 //cout << i+1<< " " << Graph_Hard_Constraints[i][j]+1 <<endl;
	  grid2[Graph_Hard_Constraints[i][j]] = true;
	}
    }
    hard_constraints_violated += TTP->hard_constraints_by_pairs(x_var_time, x_var_room, Graph_Hard_Constraints, grid2);

    for(int i = 0; i < Graph_Hard_Constraints.size(); i++)
    {
	if(grid[i]) continue;
	for(int j = 0; j < Graph_Hard_Constraints[i].size(); j++)
	{
//	if( grid[Graph_Hard_Constraints[i][j]]  ) continue;
	 //cout << i+1<< " " << Graph_Hard_Constraints[i][j]+1 <<endl;
	  grid[Graph_Hard_Constraints[i][j]] = true;
	}
    }
     int cont2 = 0;
    for(int i = 0; i < Graph_Hard_Constraints.size(); i++)
	
	if(grid[i]) {cont2++;cout <<i+1<<endl;}
cout <<"unasigned " <<cont2<<endl;

     //cont_hard_distributions_violated += penalize_overall(this->TTP->all_hard_distributions[k]);
/// 
///  //
//
///
///    for(int i = 0; i < this->TTP->classes.size(); i++)
///    {
///	for(int d = 0; d < this->TTP->distributions_by_class[i].size();d++)
///	{
///	   int id_distribution = this->TTP->distributions_by_class[i][d];
///           TimeTablingProblem::Distribution distribution_k = this->TTP->distributions[id_distribution];
///	   if(! distribution_k.required) continue;
///	   for(int j = 0; j < distribution_k.classes.size(); j++)
///	   {
///	        int id_class = distribution_k.classes[j];
///
///		   //if(x_var_time[i] == NOT_SET) continue;
///		   if(i == id_class)continue;
///			
///	      int isviolated = (int)penalize_pair(i, id_class, id_distribution);
///	      cout << i+1 << " " << id_class +1 << " " <<distribution_k.type << " "<<isviolated<< " " << x_var_room[i]<< " "<<x_var_room[id_class]<< endl;
///	      if( isviolated)
///	      {
///		this->x_var_room[id_class] = 	NOT_SET;
///		cout <<"qaaa" << d << " "<< i+1 << " " << id_class+1 << " " << distribution_k.type << "***" << x_var_time[i] << " "<<x_var_time[id_class] << "***" << x_var_room[i] << " "<<x_var_room[id_class] <<endl;
///
///	      }
///	      cont_hard_distributions_violated += isviolated;
///	   }
///	}	
/////	    if( x_var_room[i] == NOT_SET) 
/////			cout << i+1 << "---"<<endl;
///    }
///
///
///    for(int i = 0; i < this->TTP->classes.size(); i++)
///	    if( x_var_room[i] == NOT_SET) cout << i+1 <<endl;
///
/////exit(0);
///
/// // for(int k = 0; k < this->TTP->pair_hard_distributions.size(); k++)
/// // {
/// //    TimeTablingProblem::Distribution distribution_k = this->TTP->distributions[ this->TTP->pair_hard_distributions[k]];
/// //    for(int i = 0; i < distribution_k.classes.size(); i++)
/// //    {
/// //        bool anyconflict = false;
/// //        int id_class_i = distribution_k.classes[i];
/// //       for(int j = i+1; j < distribution_k.classes.size(); j++)
/// //       {
/// //          int id_class_j = distribution_k.classes[j];
///////	   if( x_var_time[id_class_j] == NOT_SET || x_var_room[id_class_j] == NOT_SET) continue;
///////	   cout << id_class_i+1 <<  " " << id_class_j+1 <<endl;
/// //          int isviolated = (int)penalize_pair(id_class_i, id_class_j, this->TTP->pair_hard_distributions[k]);
/// //          if(isviolated)
/// //          {
/// //         	cout << distribution_k.type <<" " <<id_class_i << " -- " << id_class_j <<endl; 
/// //       	anyconflict=true;
///////  	   this->x_var_time[id_class_i] = 	NOT_SET;
///////	   this->x_var_room[id_class_i] = 	NOT_SET;
///////	   continue;
/// //          }
/// //          cont_hard_distributions_violated += isviolated;
/// //       }	
/// //       if( anyconflict = true)
/// //       {
/// // 	//   this->x_var_time[id_class_j] = 	NOT_SET;
/// //       //   this->x_var_room[id_class_j] = 	NOT_SET;
///
/// //       }
///
/// //    }
/// // }
///  ///Checking overall distributions...
/// for(int k = 0; k < this->TTP->all_hard_distributions.size(); k++)
///  {
///  }
///  /////////////!end-Hard distributions////////////////////////////////////////////////////

 long long distribution_soft_penalizations= 0;


    distribution_soft_penalizations+= TTP->soft_constraints_by_pairs(x_var_time, x_var_room, grid);
	cout <<"soft "<< distribution_soft_penalizations <<endl;
///  ///Checking distributions between pair classes..

///  ///Checking overall distributions...
/// for(int k = 0; k < this->TTP->all_soft_distributions.size(); k++)
///  {
///     distribution_soft_penalizations += penalize_overall(this->TTP->all_soft_distributions[k]);
///  }
///
///
///  ///Checking room penalizations..
///  int assigned_variables = 0;
  long long room_penalization = 0;
///  vector<bool> checked(this->x_var_time.size(), false);
  for(int i = 0; i < this->x_var_room.size(); i++)
  {
	if( grid[i]) continue;
     room_penalization += this->TTP->classes[i].p_room_penalty[this->x_var_room[i]];
//    if( this->TTP->classes[i].p_room_penalty[this->x_var_room[i]] )
//     cout << "C"<< i+1 << " " << this->TTP->classes[i].p_room_penalty[this->x_var_room[i]] <<endl;
  }
cout << "room " << room_penalization <<endl;
///
///
    long long time_penalization = 0;
  for(int i = 0; i < this->x_var_time.size(); i++)
  {
     if( grid[i] ) 
	     continue;
	     long long value = this->TTP->times[this->x_var_time[i]].penalty;
//	     if(value > 0) cout << i+1 << " " << value <<endl;
     time_penalization += value;
  }
cout << "time" << " " <<time_penalization <<endl;
///    long long student_penalization = 0; //check student conflicts...
///  for(int i = 0; i < this->x_var_student.size(); i++)
///  {
///     if(conflicts_student(i))
///      student_penalization++;
///  }
///  cout << "Variables assigned: " << assigned_variables <<endl;
///
///  cout << "Total distributions penalty: " <<distribution_soft_penalizations <<endl;
///  cout << "Total room penalty: "<< room_penalization <<endl;//*this->TTP->room_w  <<endl;
///  cout << "Total time penalty: " << time_penalization*this->TTP->time_w <<endl;
///  cout << "Total student conflicts: " << student_penalization*this->TTP->student_w <<endl;
///
///  return make_pair(distribution_soft_penalizations*this->TTP->distribution_w + room_penalization*this->TTP->room_w + time_penalization*this->TTP->time_w + student_penalization*this->TTP->student_w, cont_hard_distributions_violated);




 return make_pair(1,cont2);
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
