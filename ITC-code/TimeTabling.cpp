#include <signal.h>
#include "TimeTabling.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

//////////////////////////Problem information///////////////////////////////////////
TimeTablingProblem* TimeTabling::TimeTablingproblem;
long long best = 1e16;
TimeTablingProblem::TimeTablingProblem(string file){
  //loading the instance infor..
  Load(file);
}
void TimeTablingProblem::Load(string file)
{
     pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file.c_str());
    if (!result)
    {
        cout << "error reading xml-instance "<<endl;
	exit(0);
    }
    //reading dimentions of each structure...
    this->name = doc.child("problem").attribute("name").value();
    this->nrDays = doc.child("problem").attribute("nrDays").as_int();
    this->slotsPerDay= doc.child("problem").attribute("slotsPerDay").as_int();
    this->nrWeeks= doc.child("problem").attribute("nrWeeks").as_int();
   // cout << this->name << " "<< this->nrDays << " " << this->slotsPerDay << " " << this->nrWeeks <<endl; 
    //reading weights for the optimization criteria...
    this->time_w = doc.child("problem").child("optimization").attribute("time").as_int();
    this->room_w= doc.child("problem").child("optimization").attribute("room").as_int();
    this->distribution_w= doc.child("problem").child("optimization").attribute("distribution").as_int();
    this->student_w= doc.child("problem").child("optimization").attribute("student").as_int();
 //   cout << this->time_w << " " << this->room_w << " " << this->distribution_w << " " << this->student_w <<endl;

    //computing the number of rooms... it can be a user parameter..
    int Nrooms = 0; 
    for (pugi::xml_node room: doc.child("problem").child("rooms")) //for each room ...
       Nrooms++; 
     rooms.resize(Nrooms+1); // zero-index...

    //reading rooms configurations...
    for (pugi::xml_node room: doc.child("problem").child("rooms")) //for each room ...
    {
        int id = room.attribute("id").as_int();
	rooms[id].capacity = room.attribute("capacity").as_int();
	for (pugi::xml_node in_room: room) //for each travel...
	{
	      if( !strcmp(in_room.name(),"room"))
	      {
                 int travel = in_room.attribute("room").as_int();
                 int value = in_room.attribute("value").as_int();
                 this->rooms[id].p_travel_to_room[travel] = value;
	      }
	      else if( !strcmp(in_room.name(), "unavailable"))
	      {
		Time str_time;
		string days = in_room.attribute("days").value();
                str_time.days = stoll(days, nullptr, 2); //convert binary string to long long integer
		str_time.start = in_room.attribute("start").as_int();
		str_time.length = in_room.attribute("length").as_int();
		string weeks = in_room.attribute("weeks").value();
		str_time.weeks = stoll(weeks, nullptr, 2);
		str_time.penalty = in_room.attribute("penalty").as_llong();
		this->rooms[id].unavailable.push_back(str_time);
    	      }
	}
    }

   //////reading courses configurations...
    ///this->courses.resize(1); //first index is one...
    ///this->configuration.resize(1);
    ///this->subpart.resize(1);

    for (pugi::xml_node course: doc.child("problem").child("courses")) //for each course ...
    {
//	this->course.push_back(course.attribute("id"))
        for (pugi::xml_node config: course) //for each configuration
	{
           for (pugi::xml_node subpart:config) //for each subpart
	   {
              for (pugi::xml_node iclass:subpart) //for each class
	      {
                 for (pugi::xml_node inside_class:iclass) //for each
		 {
		      if( !strcmp(inside_class.name(), "room") )
		      {
			cout << inside_class.attribute("id").value() <<endl;
                      }
		      else if( !strcmp(inside_class.name(),"time"))
		      {

	              }
		 }
              }
	   }
	}
    }


}
////////////////////////////Individual information ////////////////////////////////

long long TimeTabling::calculateFitness(){
  return 0;
}
TimeTabling bestI;

void printBest(){
	
}
void TimeTabling::localSearch(){
}

int TimeTabling::getDistance(TimeTabling &ind){
return 0;
}
void TimeTabling::Mutation(double pm){
}
void TimeTabling::Crossover(TimeTabling &ind){
}
void TimeTabling::print(){
}
