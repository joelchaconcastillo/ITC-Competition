#include <signal.h>
#include "TimeTabling.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

//////////////////////////Problem information///////////////////////////////////////
//TimeTablingProblem* Individual::TTP;

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
     rooms.resize(Nrooms); // zero-index...

    //reading rooms configurations...
    for (pugi::xml_node room: doc.child("problem").child("rooms")) //for each room ...
    {
        int id = room.attribute("id").as_int();
	rooms[id-1].capacity = room.attribute("capacity").as_int();
	for (pugi::xml_node in_room: room) //for each travel...
	{
	      if( !strcmp(in_room.name(),"room"))
	      {
                 int travel = in_room.attribute("room").as_int();
                 int value = in_room.attribute("value").as_int();
                 this->rooms[id-1].p_travel_to_room[travel] = value;
	      }
	      else if( !strcmp(in_room.name(), "unavailable"))
	      {
		Time str_time;
		string days = in_room.attribute("days").value();
                str_time.days = stoll(days, nullptr, 2); //convert binary string to long long integer
		str_time.start = in_room.attribute("start").as_int();
		str_time.length = in_room.attribute("length").as_int();
		str_time.end = str_time.start + str_time.length;
		string weeks = in_room.attribute("weeks").value();
		str_time.weeks = stoll(weeks, nullptr, 2);
		str_time.penalty = in_room.attribute("penalty").as_llong();
		this->rooms[id-1].unavailable.push_back(str_time);
    	      }
	}
    }
   //////reading courses, configurations, subpart and classes...
   
    for (pugi::xml_node course: doc.child("problem").child("courses")) //for each course ...
    {
	vector< int > course_to_configuration;
        for (pugi::xml_node config: course) //for each configuration
	{
	   course_to_configuration.push_back(config.attribute("id").as_int()-1);
	   vector< int > configuration_to_subpart;
           for (pugi::xml_node subpart:config) //for each subpart
	   {
	      vector <int> subpart_to_class;
              for (pugi::xml_node iclass:subpart) //for each class
	      {
		 Class str_class;
		 str_class.limit = iclass.attribute("limit").as_int();
		 str_class.Parent_id = iclass.attribute("parent").as_int()-1;
                 for (pugi::xml_node inside_class:iclass) //for each attribute inside to class..
		 {
		      if( !strcmp(inside_class.name(), "room") )
		      {
			str_class.p_room_penalty[inside_class.attribute("id").as_int()-1] = inside_class.attribute("penalty").as_int();
			str_class.rooms_c.push_back(make_pair(inside_class.attribute("id").as_int()-1, inside_class.attribute("penalty").as_int()));
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
		 subpart_to_class.push_back(iclass.attribute("id").as_int()-1);
              }
	      this->subpart.push_back(subpart_to_class);
	      configuration_to_subpart.push_back(subpart.attribute("id").as_int()-1);
	   }
	   this->configuration.push_back(configuration_to_subpart);
	   course_to_configuration.push_back(config.attribute("id").as_int()-1);
	}
	this->courses.push_back(course_to_configuration);
    }
    ////// reading distributions..
//    distributions.resize(1);
    for (pugi::xml_node distribution_i: doc.child("problem").child("distributions")) //for each course ...
    {
       //parsing distribution constraints..
       Distribution str_distribution;
       Parsing_type(distribution_i.attribute("type").value(), str_distribution);
       str_distribution.required = distribution_i.attribute("required").as_bool();
       str_distribution.penalty = distribution_i.attribute("penalty").as_llong();
       
       for (pugi::xml_node class_in_distribution:distribution_i)
       {
          str_distribution.classes.push_back(class_in_distribution.attribute("id").as_int()-1);
       }
	
       distributions_by_type[str_distribution.type].push_back(distributions.size());

       distributions_by_feasibility[str_distribution.required][str_distribution.pair].push_back(distributions.size());

       if( str_distribution.required)
	   hard_distributions.push_back(distributions.size());
	else
	   soft_distributions.push_back(distributions.size());

       if( str_distribution.pair)
	   pair_comparison_distributions.push_back(distributions.size());
       else
	   all_comparison_distributions.push_back(distributions.size());

       distributions.push_back(str_distribution);
       
    }
    //reading students...
    //
//    students.resize(1);
    for (pugi::xml_node student_i: doc.child("problem").child("students")) //for each course ...
    {
       vector<int> idx_courses;
       for(pugi::xml_node course_i:student_i)
       {
	idx_courses.push_back(course_i.attribute("id").as_int()-1);
       }
       students.push_back(idx_courses);
    }

	cout << "rooms... " << rooms.size() <<endl;
	cout << "courses... " << courses.size() <<endl;
	cout << "configuration... " << configuration.size() <<endl;
	cout << "subpart... " << subpart.size() <<endl;
	cout << "classes... " << classes.size() <<endl;
	cout << "student... " << students.size() <<endl;
	cout << "distributions... " << distributions.size() <<endl;
	cout << "distributions pairs, all " <<  pair_comparison_distributions.size() << " "<<all_comparison_distributions.size() <<endl;
	cout << "distributions required, penalized "<< hard_distributions.size() << " "<< soft_distributions.size() <<endl;

}
void TimeTablingProblem::Parsing_type(const char *type_, Distribution &str_distribution)
{
   str_distribution.pair = true;

   if( !strcmp(type_, "SameStart")) str_distribution.type = SAMESTART;
   else if(!strcmp(type_, "SameTime")) str_distribution.type = SAMETIME;
   else if(!strcmp(type_, "SameDays")) str_distribution.type = SAMEDAYS;
   else if(!strcmp(type_, "SameWeeks")) str_distribution.type = SAMEWEEKS;
   else if(!strcmp(type_, "SameRoom")) str_distribution.type = SAMEROOM;
   else if(!strcmp(type_, "Overlap")) str_distribution.type = OVERLAP;
   else if(!strcmp(type_, "SameAttendees"))   str_distribution.type = SAMEATTENDEES;
   else if(!strcmp(type_, "Precedence"))   str_distribution.type = PRECEDENCE;
   else
   {
	if(!strncmp(type_, "WorkDay", 7))
	{
	   str_distribution.type = WORKDAY;
	   sscanf(type_,"WorkDay(%d)", &(str_distribution.S));
	}
	else if(!strncmp(type_, "MinGap", 6))
	{
	   str_distribution.type = MINGAP;
	   sscanf(type_,"MinGap(%d)", &(str_distribution.G));
	}
	else if(!strncmp(type_, "MaxDays", 7))
	{
	   str_distribution.type = MAXDAYS;
           str_distribution.pair = false;
	   sscanf(type_,"MaxDays(%d)", &(str_distribution.D));
	}
	else if(!strncmp(type_, "MaxDayLoad", 10))
	{
	   str_distribution.type = MAXDAYLOAD;
           str_distribution.pair = false;
	   sscanf(type_,"MaxDayLoad(%d)", &(str_distribution.S));
	}
	else if(!strncmp(type_, "MaxBreaks", 9))
	{
	   str_distribution.type = MAXBREAKS;
           str_distribution.pair = false;
	   sscanf(type_,"MaxBreaks(%d,%d)", &(str_distribution.R), &(str_distribution.S));
	}
	else if(!strncmp(type_, "MaxBlock", 8))
	{
	   str_distribution.type = MAXBLOCK;
           str_distribution.pair = false;
	   sscanf(type_,"MaxBlock(%d,%d)", &(str_distribution.M), &(str_distribution.S));
	}
   }
}
////////////////////////////Individual information ////////////////////////////////

long long Individual::calculateFitness(){

  long long sum_penalization = 0;
  ///Checking distributions between pair classes..
  for(int i = 0; i < this->x_var_time.size(); i++)
  {
     for(int j = i+1; j < this->x_var_time.size(); j++)
     {
	for(int k = 0; k < this->TTP->pair_comparison_distributions.size(); k++)
	{
	   sum_penalization += penalize_pair(i, j, k);
	}	
     }
  }
  return sum_penalization;
}
long long Individual::penalize_pair( int id_class_i, int id_class_j, int id_distribution)
{

  TimeTablingProblem::Time C_ti = this->TTP->classes[id_class_i].times[this->x_var_time[id_class_i]];
  TimeTablingProblem::Time C_tj = this->TTP->classes[id_class_j].times[this->x_var_time[id_class_j]];

  pair< int, int> p_room_i = this->TTP->classes[id_class_i].rooms_c[this->x_var_room[id_class_i]];
  pair< int, int> p_room_j = this->TTP->classes[id_class_j].rooms_c[this->x_var_room[id_class_j]];

  TimeTablingProblem::Room C_ri = this->TTP->rooms[p_room_i.first];
  TimeTablingProblem::Room C_rj = this->TTP->rooms[p_room_j.first];

  TimeTablingProblem::Distribution dist = this->TTP->distributions[id_class_i];

   if( dist.type == SAMESTART  )
      if(C_ti.start == C_tj.start) 
         return dist.penalty;
   else if( dist.type == SAMETIME  )
       if((C_ti.start <= C_tj.start && C_tj.end <= C_ti.end) || (C_tj.start <= C_ti.start && C_ti.end <= C_tj.end) )
	 return dist.penalty;
   else if( dist.type == DIFFERENTTIME  )
	if( (C_ti.end <= C_tj.start )  || (C_tj.end <= C_ti.start)  )
	 return dist.penalty;

	
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
