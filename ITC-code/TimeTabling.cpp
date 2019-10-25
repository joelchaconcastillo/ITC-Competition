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
	      if( !strcmp(in_room.name(),"travel"))
	      {
                 int travel = in_room.attribute("room").as_int();
                 int value = in_room.attribute("value").as_int();
                 this->rooms[id-1].time_travel_to_room[travel] = value;
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
		this->rooms[id-1].unavailable.push_back(times.size());
		times.push_back(str_time);
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
		          str_time.end = str_time.start + str_time.length;
		          string weeks = inside_class.attribute("weeks").value();
		          str_time.weeks = stoll(weeks, nullptr, 2);
		          str_time.penalty = inside_class.attribute("penalty").as_llong();
			  str_class.times.push_back(times.size());
			  this->times.push_back(str_time);
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
    distributions_by_class.resize(this->classes.size());
    for (pugi::xml_node distribution_i: doc.child("problem").child("distributions")) //for each course ...
    {
       //parsing distribution constraints..
       Distribution str_distribution;
       Parsing_type(distribution_i.attribute("type").value(), str_distribution);
       str_distribution.required = distribution_i.attribute("required").as_bool();
       str_distribution.penalty = distribution_i.attribute("penalty").as_llong();

       if( str_distribution.required) str_distribution.penalty = 1; //hard constraints are only one 
       
       for (pugi::xml_node class_in_distribution:distribution_i)
       {
	  int id_class = class_in_distribution.attribute("id").as_int()-1;
          str_distribution.classes.push_back(id_class);
          distributions_by_class[id_class].push_back(distributions.size());
       }
	
       distributions_by_type[str_distribution.type].push_back(distributions.size());

       distributions_by_feasibility[str_distribution.required][str_distribution.pair].push_back(distributions.size());
	//////hierarchical storing of the distributions...
       if( str_distribution.required) //by hardness
	{
	   hard_distributions.push_back(distributions.size());
           if(str_distribution.pair) //by pair
	      pair_hard_distributions.push_back(distributions.size());
	   else
	      all_hard_distributions.push_back(distributions.size());
	}
	else //by softness
	{
	   soft_distributions.push_back(distributions.size());
           if( str_distribution.pair) //by pair
	     pair_soft_distributions.push_back(distributions.size());
	   else
	     all_soft_distributions.push_back(distributions.size());
	}

       if( str_distribution.pair) //by pair
	   pair_comparison_distributions.push_back(distributions.size());
       else
	   all_comparison_distributions.push_back(distributions.size());
	
       distributions.push_back(str_distribution);
       
    }
    //reading students...
    //
    for (pugi::xml_node student_i: doc.child("problem").child("students")) //for each course ...
    {
       vector<int> idx_courses;
       for(pugi::xml_node course_i:student_i)
       {
	idx_courses.push_back(course_i.attribute("id").as_int()-1);
       }
       students.push_back(idx_courses);
    }


double average = 0.0;
for(int i = 0 ; i < distributions.size(); i++)
{
   average += distributions[i].classes.size();
}
average /= distributions.size();
	cout << "rooms... " << rooms.size() <<endl;
	cout << "courses... " << courses.size() <<endl;
	cout << "configuration... " << configuration.size() <<endl;
	cout << "subpart... " << subpart.size() <<endl;
	cout << "classes... " << classes.size() <<endl;
	cout << "student... " << students.size() <<endl;
	cout << "distributions... " << distributions.size() <<endl;
	cout << "distributions pairs, all " <<  pair_comparison_distributions.size() << " "<<all_comparison_distributions.size() <<endl;
	cout << "distributions required, penalized "<< hard_distributions.size() << " "<< soft_distributions.size() <<endl;
	cout << "average classes by distribution "<< average <<endl;
}
void TimeTablingProblem::Parsing_type(const char *type_, Distribution &str_distribution)
{
   str_distribution.pair = true;

   if( !strcmp(type_, "SameStart")) str_distribution.type = SAMESTART;
   else if(!strcmp(type_, "SameTime")) str_distribution.type = SAMETIME;
   else if(!strcmp(type_, "DifferentTime")) str_distribution.type = DIFFERENTTIME;
   else if(!strcmp(type_, "SameDays")) str_distribution.type = SAMEDAYS;
   else if(!strcmp(type_, "DifferentDays")) str_distribution.type = DIFFERENTDAYS;
   else if(!strcmp(type_, "SameWeeks")) str_distribution.type = SAMEWEEKS;
   else if(!strcmp(type_, "DifferentWeeks")) str_distribution.type = DIFFERENTWEEKS;
   else if(!strcmp(type_, "SameRoom")) str_distribution.type = SAMEROOM;
   else if(!strcmp(type_, "DifferentRoom")) str_distribution.type = DIFFERENTROOM;
   else if(!strcmp(type_, "Overlap")) str_distribution.type = OVERLAP;
   else if(!strcmp(type_, "SameAttendees"))   str_distribution.type = SAMEATTENDEES;
   else if(!strcmp(type_, "Precedence"))   str_distribution.type = PRECEDENCE;
   else if(!strcmp(type_, "NotOverlap"))   str_distribution.type = NOTOVERLAP;
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
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
pair<long long, int> Individual::calculateFitness(){

 
  //////////////////Hard distributions//////////////////////////////////////////////////
  int cont_hard_distributions_violated = 0;
////  vector<bool> checked1(this->TTP->classes.size(), false);
////    for(int i = 0; i < this->TTP->classes.size(); i++)
////    {
////	  bool anyconflict = false;
////	for(int d = 0; d < this->TTP->distributions_by_class[i].size();d++)
////	{
////	   int id_distribution = this->TTP->distributions_by_class[i][d];
////           TimeTablingProblem::Distribution distribution_k = this->TTP->distributions[id_distribution];
////	   for(int j = 0; j < distribution_k.classes.size(); j++)
////	   {
////	        int id_class = distribution_k.classes[j];
////		   //if(x_var_time[i] == NOT_SET) continue;
////		   if(checked1[id_class] || i == id_class)continue;
////			
////	      int isviolated = (int)penalize_pair(i, id_class, id_distribution);
////	      if( isviolated)
////	      {
////			anyconflict = true;
////			cout << i+1 << " " << id_class+1<<endl;
////	      }
////	      cont_hard_distributions_violated += isviolated;
////	   }
////	}	
////	    if(anyconflict) {
////		this->x_var_time[i] = 	NOT_SET;
////		this->x_var_room[i] = 	NOT_SET;
////		cout << i+1 <<endl;
////	    }
////	    checked1[i] = true;
////    }


  for(int k = 0; k < this->TTP->pair_hard_distributions.size(); k++)
  {
     TimeTablingProblem::Distribution distribution_k = this->TTP->distributions[ this->TTP->pair_hard_distributions[k]];
     for(int i = 0; i < distribution_k.classes.size(); i++)
     {
         bool anyconflict = false;
	 int id_class_i = distribution_k.classes[i];
        for(int j = i+1; j < distribution_k.classes.size(); j++)
	{
	   int id_class_j = distribution_k.classes[j];
	   if( x_var_time[id_class_j] == NOT_SET || x_var_room[id_class_j] == NOT_SET) continue;
//	   cout << id_class_i+1 <<  " " << id_class_j+1 <<endl;
	   int isviolated = (int)penalize_pair(id_class_i, id_class_j, this->TTP->pair_hard_distributions[k]);
	   if(isviolated)
	   {
//	  	cout << distribution_k.type <<" " << distribution_k.classes[i]+1 << " " <<distribution_k.classes[j]+1 <<endl;
		anyconflict=true;
//  	   this->x_var_time[id_class_i] = 	NOT_SET;
	   this->x_var_room[id_class_i] = 	NOT_SET;
	   cout << k << " " <<id_class_i +1 <<endl;
//	   continue;
	   }
	   cont_hard_distributions_violated += isviolated;
	}	
	if( anyconflict = true)
	{
  	//   this->x_var_time[id_class_j] = 	NOT_SET;
	//   this->x_var_room[id_class_j] = 	NOT_SET;

	}

     }
  }
  ///Checking overall distributions...
 for(int k = 0; k < this->TTP->all_hard_distributions.size(); k++)
  {
     cont_hard_distributions_violated += penalize_overall(this->TTP->all_hard_distributions[k]);
  }
  /////////////!end-Hard distributions////////////////////////////////////////////////////
 long long distribution_soft_penalizations= 0;
  ///Checking distributions between pair classes..
  for(int k = 0; k < this->TTP->pair_soft_distributions.size(); k++)
  {
     TimeTablingProblem::Distribution distribution_k = this->TTP->distributions[ this->TTP->pair_soft_distributions[k]];
     for(int i = 0; i < distribution_k.classes.size(); i++)
     {
        for(int j = i+1; j < distribution_k.classes.size(); j++)
	{
	   long long current_value = penalize_pair(distribution_k.classes[i], distribution_k.classes[j], this->TTP->pair_soft_distributions[k]);

	   distribution_soft_penalizations +=  current_value;
	}	
     }
  }
  ///Checking overall distributions...
 for(int k = 0; k < this->TTP->all_soft_distributions.size(); k++)
  {
     distribution_soft_penalizations += penalize_overall(this->TTP->all_soft_distributions[k]);
  }


  ///Checking room penalizations..
  int assigned_variables = 0;
  long long room_penalization = 0;
  vector<bool> checked(this->x_var_time.size(), false);
  for(int i = 0; i < this->x_var_room.size(); i++)
  {
     if( x_var_room[i] == NOT_SET)
	    	continue;
     assigned_variables++;
     room_penalization += this->TTP->classes[i].p_room_penalty[this->x_var_room[i]];
//    if( this->TTP->classes[i].p_room_penalty[this->x_var_room[i]] )
//     cout << "C"<< i+1 << " " << this->TTP->classes[i].p_room_penalty[this->x_var_room[i]] <<endl;
  }


    long long time_penalization = 0;
  for(int i = 0; i < this->x_var_time.size(); i++)
  {
     if( x_var_time[i] == NOT_SET) 
	     continue;
	     long long value = this->TTP->times[this->x_var_time[i]].penalty;
//	     if(value > 0) cout << i+1 <<endl;
     time_penalization += value;
  }
    long long student_penalization = 0; //check student conflicts...
  for(int i = 0; i < this->x_var_student.size(); i++)
  {
     if(conflicts_student(i))
      student_penalization++;
  }
  cout << "Variables assigned: " << assigned_variables <<endl;

  cout << "Total distributions penalty: " <<distribution_soft_penalizations <<endl;
  cout << "Total room penalty: "<< room_penalization <<endl;//*this->TTP->room_w  <<endl;
  cout << "Total time penalty: " << time_penalization*this->TTP->time_w <<endl;
  cout << "Total student conflicts: " << student_penalization*this->TTP->student_w <<endl;

  return make_pair(distribution_soft_penalizations*this->TTP->distribution_w + room_penalization*this->TTP->room_w + time_penalization*this->TTP->time_w + student_penalization*this->TTP->student_w, cont_hard_distributions_violated);
}
bool Individual::conflicts_student(int id_student)
{
   vector<int> classes_by_student = this->x_var_student[id_student]; 
   for(int i = 0; i < classes_by_student.size(); i++) //checking each class...
   {
      for(int j = i+1; j < classes_by_student.size(); j++)
      {
	int id_class_i = classes_by_student[i];
	int id_class_j = classes_by_student[j];
        TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class_i]];
        TimeTablingProblem::Time C_tj = this->TTP->times[this->x_var_time[id_class_j]];
	TimeTablingProblem::Room C_ri = this->TTP->rooms[this->x_var_room[id_class_i]];
  	TimeTablingProblem::Room C_rj = this->TTP->rooms[this->x_var_room[id_class_j]];
	bool c1 = (C_tj.start < C_ti.end + C_ri.time_travel_to_room[this->x_var_room[id_class_j]]);
	bool c2 = (C_ti.start <  C_tj.end + C_rj.time_travel_to_room[this->x_var_room[id_class_i]]);
        bool c3 = ((C_ti.days & C_tj.days)!=0);
	bool c4 = ((C_ti.weeks & C_tj.weeks)!=0);
	if( c1 && c2 && c3 && c4 )
          return true;	
      }
   }
  return false;
}
long long Individual::penalize_overall(int id_distribution)
{
  TimeTablingProblem::Distribution dist = this->TTP->distributions[id_distribution];
  long long int totalpenalization = 0;

  if(dist.type == MAXDAYS)
  {
     unsigned long int days = 0;
      
     for(int i = 0; i < dist.classes.size(); i++)
     {
        int id_class = dist.classes[i]; 
	if( this->x_var_time[id_class] == NOT_SET) continue;

        TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class]];
           days |= C_ti.days;
     } 
     int nonzerobits = __builtin_popcountll(days);
     if( !( nonzerobits <= dist.D) && dist.required )
     {
        for(int i = 0; i < dist.classes.size(); i++)
	     {
		int id_class = dist.classes[i]; 
		this->x_var_time[id_class] = NOT_SET;
		this->x_var_room[id_class] = NOT_SET;
	     }
     }
    //   return dist.penalty*(dist.D-nonzerobits);
       return dist.penalty*max(0, dist.D-nonzerobits);
  }
  else if( dist.type == MAXDAYLOAD)
  {
     int TotalDayLoad = 0;
     for(int week_i = 0; week_i < this->TTP->nrWeeks; week_i++)
     { 
        for(int day_i = 0; day_i < this->TTP->nrDays; day_i++)
        {
	  int day_load = 0;
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
	     if( this->x_var_time[id_class] == NOT_SET) continue;

             TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  ) day_load += C_ti.length;
          }
	  if( (day_load > dist.S) && dist.required )
	   for(int i = 0; i < dist.classes.size(); i++)
           {
             int id_class = dist.classes[i]; 
		this->x_var_time[id_class] = NOT_SET;
		this->x_var_room[id_class] = NOT_SET;
	   }


	  TotalDayLoad += max(0, day_load-dist.S);
	}
     }
     return (dist.penalty*TotalDayLoad)/this->TTP->nrWeeks;
  }
  else if( dist.type == MAXBREAKS)
  {
     int Totalbreaks = 0;
     for(int week_i = 0; week_i < this->TTP->nrWeeks; week_i++)
     { 
        for(int day_i = 0; day_i < this->TTP->nrDays; day_i++)
        {
	  priority_queue< pair<int, int > > pq; ///kind of pre-sort by starts and ends..
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
	     if( this->x_var_time[id_class] == NOT_SET) continue;
             TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  ) pq.push(make_pair(-C_ti.start, C_ti.end));
          }
	  int NBreaks = 0; 
	  int Block_start = -pq.top().first, Block_end = pq.top().second;
	  pq.pop();
	  while(!pq.empty())
	  {
	     int current_start = -pq.top().first;
	     int current_end = pq.top().second;
	     pq.pop();
	     if( !(Block_end + dist.S > current_start ) ) //is the same than (Block_end + dist.S <= current_start )
	     {
		NBreaks++;
	     }
	     Block_end = current_end;
	  }
	  if( NBreaks > dist.R+1 && dist.required )
	  {

          for(int i = 0; i < dist.classes.size(); i++)
	  {
             int id_class = dist.classes[i]; 
	     this->x_var_time[id_class] = NOT_SET;
	     this->x_var_room[id_class] = NOT_SET;
	  }
	  }
	  Totalbreaks += max(0, NBreaks-dist.R+1);
	}
     }
    return Totalbreaks/this->TTP->nrWeeks;
  }
  else if( dist.type == MAXBLOCK)
  {
     int TotalBlocksOver = 0;
     for(int week_i = 0; week_i < this->TTP->nrWeeks; week_i++)
     { 
        for(int day_i = 0; day_i < this->TTP->nrDays; day_i++)
        {
	  priority_queue< pair<int, int > > pq; ///kind of pre-sort by starts and ends..
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
	     if( this->x_var_time[id_class] == NOT_SET) continue;
             TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  ) pq.push(make_pair(-C_ti.start, C_ti.end));
          }
	  int Block_start = -pq.top().first, Block_end = pq.top().second;
	  int lengthBlock = Block_end-Block_start;
	  pq.pop();
	  while(!pq.empty())
	  {
	     int current_start = -pq.top().first;
	     int current_end = pq.top().second;
	     pq.pop();
	     if( !(Block_end + dist.S > current_start ) ) //is the same than (Block_end + dist.S <= current_start )
	     {
		lengthBlock = Block_end - Block_start;	
		if(lengthBlock > dist.M) TotalBlocksOver++;
		Block_start = current_start;
	     }
	     Block_end = current_end;
	  }
	}
     }
	if( TotalBlocksOver > 0 && dist.required)
	  {

          for(int i = 0; i < dist.classes.size(); i++)
	  {
             int id_class = dist.classes[i]; 
	     this->x_var_time[id_class] = NOT_SET;
	     this->x_var_room[id_class] = NOT_SET;
	  }
	  }
     return TotalBlocksOver*dist.penalty/this->TTP->nrWeeks;
  }
  return 0;
}
long long Individual::penalize_pair( int id_class_i, int id_class_j, int id_distribution)
{
  if(this->x_var_room[id_class_i] == NOT_SET || this->x_var_room[id_class_j] == NOT_SET ) return 0;
  if(this->x_var_time[id_class_i] == NOT_SET || this->x_var_time[id_class_j] == NOT_SET ) return 0;

  TimeTablingProblem::Time C_ti = this->TTP->times[this->x_var_time[id_class_i]];
  TimeTablingProblem::Time C_tj = this->TTP->times[this->x_var_time[id_class_j]];

  TimeTablingProblem::Room C_ri = this->TTP->rooms[this->x_var_room[id_class_i]];
  TimeTablingProblem::Room C_rj = this->TTP->rooms[this->x_var_room[id_class_j]];

  TimeTablingProblem::Distribution dist = this->TTP->distributions[id_distribution];
  bool violatedRoom = false, violatedTime = false;
   if( dist.type == SAMESTART  )
   {
      if(!(C_ti.start == C_tj.start)) 
	 violatedTime = true;
   }
   else if( dist.type == SAMETIME  )
   {
       if( !( (C_ti.start <= C_tj.start && C_tj.end <= C_ti.end) || (C_tj.start <= C_ti.start && C_ti.end <= C_tj.end) ) )
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTTIME )
   { 
	if( !((C_ti.end <= C_tj.start )  || (C_tj.end <= C_ti.start)  ))
	 violatedTime = true;
   }
  else if(dist.type == SAMEDAYS) 
   {
	if( !(((C_ti.days | C_tj.days) == C_ti.days)  || ((C_ti.days | C_tj.days) == C_tj.days) ))
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTDAYS )
   {
	if(!( (C_ti.days & C_tj.days) == 0))
	 violatedTime = true;
   }
   else if(dist.type == SAMEWEEKS) 
   {
	if(!( ((C_ti.weeks | C_tj.weeks) == C_ti.weeks)  || ((C_ti.weeks | C_tj.weeks) == C_tj.weeks) ))
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTWEEKS )
   {
	if( !((C_ti.weeks & C_tj.weeks) == 0))
	 violatedTime = true;
   }
   else if( dist.type == OVERLAP)
   {
	if(!( (C_tj.start < C_ti.end) && (C_ti.start < C_tj.end) && ( (C_ti.days & C_tj.days)!=0 && (C_ti.weeks & C_tj.weeks) != 0 ) ))
	 violatedTime = true;
   }
   else if( dist.type == NOTOVERLAP)
   { 
	if( !((C_ti.end <= C_tj.start) || (C_tj.end <= C_ti.start) || ( ((C_ti.days & C_tj.days)==0) || ((C_ti.weeks & C_tj.weeks) == 0) ) ) )
	 violatedTime = true;
  }
  else if(dist.type == SAMEROOM)
  {
	if(!(this->x_var_room[id_class_i] == this->x_var_room[id_class_j]))
	 violatedRoom = true;
  }
  else if(dist.type == DIFFERENTROOM)
  {
	if(!(this->x_var_room[id_class_i] != this->x_var_room[id_class_j]))
	 violatedRoom = true;
  }
  else if(dist.type == SAMEATTENDEES)
  {
	bool c1 = (C_ti.end + C_ri.time_travel_to_room[this->x_var_room[id_class_j]] ) <= C_tj.start ;
	bool c2 = (C_tj.end + C_rj.time_travel_to_room[this->x_var_room[id_class_i]] ) <= C_ti.start ;
        bool c3 = ((C_ti.days & C_tj.days)==0);
	bool c4 = ((C_ti.weeks & C_tj.weeks)==0);
	if( !(c1 || c2 || c3 || c4   ))
	 violatedTime = true;
  }
  else if(dist.type == PRECEDENCE)
  {
     bool c1 = ffsll(C_ti.weeks) < ffsll(C_tj.weeks);
     bool c2 = ffsll(C_ti.weeks) == ffsll(C_tj.weeks);
     bool c3 = ffsll(C_ti.days) < ffsll(C_tj.days);
     bool c4 = ffsll(C_ti.days) == ffsll(C_tj.days);
     bool c5 = C_ti.end <= C_tj.start;
     if(!( c1 || ( c2 && ( c3 || ( c4 && c5  )  ) )))
	 violatedTime = true;
  }
  else if(dist.type == WORKDAY)
  {
     bool c1 = ( (C_ti.days & C_tj.days) == 0);
     bool c2 = ( (C_ti.weeks & C_tj.weeks) == 0);
     bool c3 = ( (max(C_ti.end, C_tj.end) - min(C_ti.start, C_tj.start)) <= dist.S );
     if( !( c1 || c2 || c3))
	 violatedTime = true;
  }
  else if(dist.type == MINGAP)
  {
    bool c1 = ( (C_ti.days & C_tj.days)==0 );
    bool c2 = ( (C_ti.weeks & C_tj.weeks)==0 );
    bool c3 = ( (C_ti.end + dist.G) <= C_tj.start  );
    bool c4 = ( (C_tj.end + dist.G) <= C_ti.start  );
    if( !( c1 || c2 || c3 || c4))
	 violatedTime = true;
  }
  if(violatedTime || violatedRoom)
  {
	  if(dist.required)
	  {
//		  if( violatedTime)
//		{
//		this->x_var_time[id_class_i] = 	NOT_SET;
//		this->x_var_time[id_class_j] = 	NOT_SET;
//		this->x_var_room[id_class_i] = 	NOT_SET;
//		this->x_var_room[id_class_j] = 	NOT_SET;
//
//		}
//		  else if(violatedRoom)
//		  {
//	        this->x_var_room[id_class_i] = 	NOT_SET;
//		this->x_var_room[id_class_j] = 	NOT_SET;
//		}
	  }
	  
         return dist.penalty;
  }
  return 0;	
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
void Individual::save_xml()
{
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("solution");
    node.append_attribute("name") = this->TTP->name.c_str();
    node.append_attribute("technique") = "Alternative";
    node.append_attribute("authors") = "cs and jcc";
    node.append_attribute("institution") = "CIMAT";
    node.append_attribute("country") = "Mexico";
    node.append_attribute("runtime") = "0";
    node.append_attribute("cores") = "1";

    vector< vector<int> > class_to_student(this->x_var_room.size());
    
    for(int i = 0; i < this->x_var_student.size(); i++)
    {
	for(int j = 0; j < this->x_var_student[i].size(); j++)
	{
	    int id_class = this->x_var_student[i][j];
	    class_to_student[id_class].push_back(i);
	}
    }


    for(int i = 0; i < class_to_student.size(); i++)
    {
        pugi::xml_node class_i = node.append_child("class");
        class_i.append_attribute("id") = i+1;

	if(this->x_var_time[i] != NOT_SET )
	{
	unsigned long int days = this->TTP->times[this->x_var_time[i]].days;
	unsigned long int weeks = this->TTP->times[this->x_var_time[i]].weeks;
	int start = this->TTP->times[this->x_var_time[i]].start;
	string str_days = "";
	for(int k = this->TTP->nrDays-1; k >=0 ; k--) str_days.push_back(( (days&(1<<k)) != 0)? '1':'0');
        class_i.append_attribute("days") = str_days.c_str();


        class_i.append_attribute("start") = start;
	string str_weeks = "";
	for(int k = this->TTP->nrWeeks-1; k >=0 ; k--) str_weeks.push_back(( (weeks&(1<<k)) != 0)? '1':'0');

        class_i.append_attribute("weeks") = str_weeks.c_str();
	}
	if(this->x_var_room[i] != NOT_SET )
        class_i.append_attribute("room") = this->x_var_room[i]+1;

	for(int j = 0; j < class_to_student[i].size(); j++)
	{
	   pugi::xml_node student_i = class_i.append_child("student");
           student_i.append_attribute("id") = class_to_student[i][j]+1;
	}
    }

//    pugi::xml_node descr = node.append_child("description");
//    descr.append_child(pugi::node_pcdata).set_value("Simple node");
//    // add param node before the description
//    pugi::xml_node param = node.insert_child_before("param", descr);
//    
//    // add attributes to param node
//    param.append_attribute("name") = "version";
//    param.append_attribute("value") = 1.1;
//    param.insert_attribute_after("type", param.attribute("name")) = "float";
    doc.save_file("save_file_output.xml");

}
void Individual::loading_example()
{
     pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("Instances/public/solution-wbg-fal10.xml");
    if (!result)
    {
        cout << "error reading xml-instance "<<endl;
	exit(0);
    }
    for (pugi::xml_node class_i: doc.child("solution")) 
    {
	int id_class = class_i.attribute("id").as_int()-1;
	int id_room= class_i.attribute("room").as_int()-1;
	TimeTablingProblem::Time str_time;
        string days = class_i.attribute("days").value();
        str_time.days = stoll(days, nullptr, 2); //convert binary string to long long integer
        str_time.start = class_i.attribute("start").as_int();
        string weeks = class_i.attribute("weeks").value();
        str_time.weeks = stoll(weeks, nullptr, 2);
	//getting back the id-time......
        int id_time=-1;
	for(int i = 0; i < this->TTP->classes[id_class].times.size(); i++)
	{
	   TimeTablingProblem::Time str_time2 = this->TTP->times[this->TTP->classes[id_class].times[i]];
	   if( (str_time2.days == str_time.days)   )
	   if( (str_time2.start == str_time.start)   )
	   if( (str_time2.weeks == str_time.weeks)   )
	   {
		id_time = this->TTP->classes[id_class].times[i];
		break;
           }
	}
	
        this->x_var_time[id_class] = id_time;
        this->x_var_room[id_class] = id_room;

        for (pugi::xml_node student_i: class_i) 
        {
	   int id_student = student_i.attribute("id").as_int()-1;
	   this->x_var_student[id_student].push_back(id_class);
	}
        
    }

}
long long int Individual::get_var_time_size()
{ 
   long long int prod = 0;
   for(int i = 0; i < x_var_time.size(); i++)
   {    
	if(this->TTP->classes[i].times.empty()) continue;
	prod += this->TTP->classes[i].times.size();
   }
   return prod;
}
long long int Individual::get_var_room_size()
{
   long long int prod = 0;
   for(int i = 0; i < x_var_room.size(); i++)
   {
	if(this->TTP->classes[i].rooms_c.empty()) continue;
	prod += this->TTP->classes[i].rooms_c.size();
   }
   return prod;
}

void Individual::initialization()
{
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
