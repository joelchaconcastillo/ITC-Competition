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

   //////reading courses, configurations, subpart and classes...
    this->courses.resize(1); //first index is one...
    this->configuration.resize(1);
    this->subpart.resize(1);
    this->classes.resize(1);
    for (pugi::xml_node course: doc.child("problem").child("courses")) //for each course ...
    {
	vector< int > course_to_configuration;
        for (pugi::xml_node config: course) //for each configuration
	{
	   course_to_configuration.push_back(config.attribute("id").as_int());
	   vector< int > configuration_to_subpart;
           for (pugi::xml_node subpart:config) //for each subpart
	   {
	      vector <int> subpart_to_class;
              for (pugi::xml_node iclass:subpart) //for each class
	      {
		 Class str_class;
		 str_class.limit = iclass.attribute("limit").as_int();
		 str_class.Parent_id = iclass.attribute("parent").as_int();
                 for (pugi::xml_node inside_class:iclass) //for each attribute inside to class..
		 {
		      if( !strcmp(inside_class.name(), "room") )
		      {
			str_class.p_room_penalty[inside_class.attribute("id").as_int()] = inside_class.attribute("penalty").as_int();
                      }
		      else if( !strcmp(inside_class.name(),"time"))
		      {
			  Time str_time;
		          string days = inside_class.attribute("days").value();
                          str_time.days = stoll(days, nullptr, 2); //convert binary string to long long integer
		          str_time.start = inside_class.attribute("start").as_int();
		          str_time.length = inside_class.attribute("length").as_int();
		          string weeks = inside_class.attribute("weeks").value();
		          str_time.weeks = stoll(weeks, nullptr, 2);
		          str_time.penalty = inside_class.attribute("penalty").as_llong();
			  str_class.times.push_back(str_time);
	              }
		 }
		 this->classes.push_back(str_class);
		 subpart_to_class.push_back(iclass.attribute("id").as_int());
              }
	      this->subpart.push_back(subpart_to_class);
	      configuration_to_subpart.push_back(subpart.attribute("id").as_int());
	   }
	   this->configuration.push_back(configuration_to_subpart);
	   course_to_configuration.push_back(config.attribute("id").as_int());
	}
	this->courses.push_back(course_to_configuration);
    }
    ////// reading distributions..
    distributions.resize(1);
    for (pugi::xml_node distribution_i: doc.child("problem").child("distributions")) //for each course ...
    {
       Distribution str_distribution;
       str_distribution.type =  distribution_i.attribute("type").value();
       str_distribution.required = distribution_i.attribute("required").as_bool();
       str_distribution.penalty = distribution_i.attribute("penalty").as_llong();
       
       for (pugi::xml_node class_in_distribution:distribution_i)
       {
          str_distribution.classes.push_back(class_in_distribution.attribute("id").as_int());
       }
      // distributions_by_type[distribution_i.attribute("type").value()].push_back(distributions.size());
       distributions.push_back(str_distribution);
    }
    //reading students...
    //
    students.resize(1);
    for (pugi::xml_node student_i: doc.child("problem").child("students")) //for each course ...
    {
       vector<int> idx_courses;
       for(pugi::xml_node course_i:student_i)
       {
	idx_courses.push_back(course_i.attribute("id").as_int());
       }
       students.push_back(idx_courses);
    }

	cout << "rooms... " << rooms.size() <<endl;
	cout << "courses... " << courses.size() <<endl;
	cout << "configuration... " << configuration.size() <<endl;
	cout << "subpart... " << subpart.size() <<endl;
	cout << "classes... " << classes.size() <<endl;
	cout << "distributions... " << distributions.size() <<endl;
	cout << "student... " << students.size() <<endl;

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
