#include <signal.h>
#include "TimeTabling.h"
#include "Individual.h"
#include "pugixml.hpp"
#include "utils.h"
using namespace std;

//////////////////////////Problem information///////////////////////////////////////
//TimeTablingProblem* TimeTablingProblem::TTP;

long long best = 1e16;
TimeTablingProblem::TimeTablingProblem(string file){
  //loading the instance infor..
  Load(file);
  x_var.resize(classes.size());
  feasible_space();
///for(int i = 0 ; i  < domain.size(); i++)
/// cout << domain[i].size()<<endl; 
///
///for(int i = 0 ; i  < domain[class_idx[0]].size(); i++)
///   {
///	   cout << domain[class_idx[0]][i].first << " " << domain[class_idx[0]][i].second;
///   }
  x_var_time.resize(classes.size());
  x_var_room.resize(classes.size(), NOT_ROOM);

}
void TimeTablingProblem::feasible_space()
{
  domain.resize(classes.size());
  for(int ic = 0; ic < classes.size(); ic++)
  {
     for(int t = 0; t < classes[ic].times.size(); t++)
     {
	int id_time = classes[ic].times[t];
	Time C_ti = times[id_time];
	for(int r = 0; r < classes[ic].rooms_c.size(); r++)
	{
	  bool flag_feasible = true;
	  int id_room = classes[ic].rooms_c[r];
	  for(int l = 0; l < rooms[id_room].unavailable.size(); l++)
	  {
	    Time C_tj = times[rooms[id_room].unavailable[l]];
	     if( Overlap(C_ti, C_tj))
		{
		 flag_feasible = false;
		 break;
		}
	  }
	  if(flag_feasible) 
	  domain[ic].push_back(make_pair(id_time, id_room));
	}
	if(classes[ic].rooms_c.empty()) domain[ic].push_back(make_pair(id_time, NOT_ROOM));
     }
  }
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
    ////////////////////GETTING SIZES.....    
    int NRooms = 0, NCourses=0, NConfigurations=0, NSubparts=0, NClasses=0, NStudents=0; 
    for (pugi::xml_node room: doc.child("problem").child("rooms")) //for each room  getting the size..
	NRooms = max(NRooms, room.attribute("id").as_int());
   for (pugi::xml_node course: doc.child("problem").child("courses")) //for each course ...
    {
	NCourses = max(NCourses, course.attribute("id").as_int());
        for (pugi::xml_node config: course) //for each configuration
	{
	   NConfigurations = max(NConfigurations, config.attribute("id").as_int());
           for (pugi::xml_node isubpart:config) //for each subpart
	   {
		NSubparts = max(NSubparts, isubpart.attribute("id").as_int());
              for (pugi::xml_node iclass:isubpart) //for each class
	      {
		 idx_class[NClasses] = iclass.attribute("id").as_int();
		 class_idx[iclass.attribute("id").as_int()] = NClasses++;
              }
	   }
	}
    }
    for (pugi::xml_node student_i: doc.child("problem").child("students")) //for each course ...
    {
	NStudents = max(NStudents, student_i.attribute("id").as_int());
    }
    ////////////////////<----GETTING SIZES.....    

     rooms.resize(NRooms); // zero-index...
     courses.resize(NCourses); 
     configuration.resize(NConfigurations);	
     subpart.resize(NSubparts);
     classes.resize(NClasses);
    students.resize(NStudents);
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
                 this->rooms[id-1].time_travel_to_room[travel-1] = value;
                 this->rooms[travel-1].time_travel_to_room[id-1] = value;
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
	int id_course = course.attribute("id").as_int()-1;
        for (pugi::xml_node config: course) //for each configuration
	{
	   course_to_configuration.push_back(config.attribute("id").as_int()-1);
	   vector< int > configuration_to_subpart;
	   int id_configuration = config.attribute("id").as_int()-1;
           for (pugi::xml_node isubpart:config) //for each subpart
	   {
	      vector <int> subpart_to_class;
	      int id_subpart = isubpart.attribute("id").as_int()-1;
              for (pugi::xml_node iclass:isubpart) //for each class
	      {
	         int id_class = class_idx[iclass.attribute("id").as_int()];
		 Class str_class;
		 str_class.limit = iclass.attribute("limit").as_int(); 
		 str_class.Parent_id = class_idx[iclass.attribute("parent").as_int()];
		 str_class.rooms = ( !strcmp(iclass.attribute("room").value(), "false") )? false:true;
                 for (pugi::xml_node inside_class:iclass) //for each attribute inside to class..
		 {
		      if( !strcmp(inside_class.name(), "room") )
		      {
			str_class.p_room_penalty[inside_class.attribute("id").as_int()-1] = inside_class.attribute("penalty").as_int(); //travel time penalty
			str_class.rooms_c.push_back(inside_class.attribute("id").as_int()-1); 
			str_class.penalty_c.push_back(inside_class.attribute("penalty").as_int());
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
		 this->classes[id_class] = str_class;
//		 this->classes.push_back(str_class);
		 subpart_to_class.push_back(id_class);
              }
//	      this->subpart.push_back(subpart_to_class);
	      this->subpart[id_subpart] = subpart_to_class;
	      configuration_to_subpart.push_back(isubpart.attribute("id").as_int()-1);
	   }
//	   this->configuration.push_back(configuration_to_subpart);
	   this->configuration[id_configuration] = configuration_to_subpart;
	   course_to_configuration.push_back(config.attribute("id").as_int()-1);
	}
//	this->courses.push_back(course_to_configuration);
	this->courses[id_course] = course_to_configuration ;
    }
    ////// reading distributions..
    distributions_by_class.resize(this->classes.size());

    for (pugi::xml_node distribution_i: doc.child("problem").child("distributions")) //for each course ...
    {
       //parsing distribution constraints..
       Distribution str_distribution;
       Parsing_type(distribution_i.attribute("type").value(), str_distribution);
       str_distribution.required = distribution_i.attribute("required").as_bool();
       str_distribution.penalty = distribution_i.attribute("penalty").as_llong();

       if( str_distribution.required) str_distribution.penalty = 1; //hard constraints are only one 
       int order = 0;
       for (pugi::xml_node class_in_distribution:distribution_i)
       {
	  int id_class = class_idx[class_in_distribution.attribute("id").as_int()];
          str_distribution.classes.push_back(id_class);
          str_distribution.order_classes[id_class] = order++;
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
		{
	      all_hard_distributions.push_back(distributions.size());	
		}
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

long long TimeTablingProblem::penalize_overall(int id_distribution)
{
  Distribution dist = distributions[id_distribution];
  long long int totalpenalization = 0;
  if(dist.type == MAXDAYS)
  {
     unsigned long int days = 0;
     vector<int> class_c; 
     for(int i = 0; i < dist.classes.size(); i++)
     {
        int id_class = dist.classes[i]; 
//     if( classes[id_class].times.empty())  continue;
     if( dist.required && x_var_time[id_class] == NOT_CHECK) return 0;
        Time C_ti = times[x_var_time[id_class]];
           days |= C_ti.days;
	class_c.push_back(id_class);
     } 
     int nonzerobits = __builtin_popcountll(days);
///    	if(dist.required &&  nonzerobits > dist.D )
///	{
///     	   for(int i = 0; i < class_c.size(); i++)
///	   {
///		invalid_variables[class_c[i]]=true;
///	   }
///	} 
       return dist.penalty*max(0, nonzerobits - dist.D);
  }
  else if( dist.type == MAXDAYLOAD)
  {
     int TotalDayLoad = 0;
     for(unsigned long int week_i = 0; week_i < nrWeeks; week_i++)
     { 
        for(unsigned long int day_i = 0; day_i < nrDays; day_i++)
        {
	  int day_load = 0;
	  vector<int> class_c;
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
     	   if( dist.required && x_var_time[id_class] == NOT_CHECK) return 0;
             Time C_ti = times[x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  ) 
	      {
		day_load += C_ti.length;
		class_c.push_back(id_class);
	      }
          }
//	  if(  day_load > dist.S && dist.required)
//	  {
//		for(int i = 0; i < class_c.size(); i++) invalid_variables[class_c[i]]=true;
//	   }
	  TotalDayLoad += max(0, day_load-dist.S);
	}
     }
     return (dist.penalty*TotalDayLoad)/nrWeeks;
  }
  else if( dist.type == MAXBREAKS)
  {
     int Totalbreaks = 0;
     for(int week_i = 0; week_i < nrWeeks; week_i++)
     { 
        for(int day_i = 0; day_i < nrDays; day_i++)
        {
	  priority_queue< pair<int, pair<int, int> > > pq; ///kind of pre-sort by starts and ends..
	 vector<int> class_c;
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
     	     if( dist.required && x_var_time[id_class] == NOT_CHECK) return 0;
             Time C_ti = times[x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  )
             {
		 pq.push(make_pair(-C_ti.start, make_pair(C_ti.end, id_class)));
		  class_c.push_back(id_class);
	     }
          }
	  if(pq.empty()) continue;
          vector<int> start_b, end_b;
	  vector <vector<int> > blocks;
          split_in_blocks(blocks, start_b, end_b, pq, dist.S);
	  int NBreaks = blocks.size();
//	  if( NBreaks > dist.R+1 && dist.required)
//	  {
//		for(int i = 0; i  < class_c.size(); i++)
//		invalid_variables[class_c[i]] = true;
//           }
	  Totalbreaks += max(NBreaks - dist.R-1, 0);

	}
     }

    return (Totalbreaks*dist.penalty)/nrWeeks;
  }
  else if( dist.type == MAXBLOCK)
  {
     int TotalBlocksOver = 0;
     for(unsigned long int week_i = 0; week_i < nrWeeks; week_i++)
     { 
        for(unsigned long int day_i = 0; day_i < nrDays; day_i++)
        {
	  priority_queue< pair<int, pair<int, int> > > pq; /// pre-sort by starts and ends..
	  unordered_map<int, int> order_distribution;
	   int cont = 0;
          for(int i = 0; i < dist.classes.size(); i++)
          {
             int id_class = dist.classes[i]; 
     	     if( dist.required && x_var_time[id_class] == NOT_CHECK) return 0;
             Time C_ti = times[x_var_time[id_class]];
	     bool c1 = (C_ti.days & (1<<day_i))!=0;
	     bool c2 = (C_ti.weeks & (1<<week_i))!=0;
             if( c1 && c2  )
	     {
		 order_distribution[id_class] = cont++;	
		 pq.push(make_pair(-C_ti.start, make_pair(C_ti.end, id_class)));
	     }
          }
	  if(pq.empty()) continue;
          vector<int> start_b, end_b;
	  vector <vector<int> > blocks;
          split_in_blocks(blocks, start_b, end_b, pq, dist.S);
            //checking size....
            for(int i = 0; i < blocks.size(); i++)
            {
               if( (end_b[i] - start_b[i]  > dist.M)  && blocks[i].size() > 1)
               {
              ///    if( dist.required )
              ///     {
	      ///        for(int j = 0; j < blocks[i].size(); j++)
	      ///  	{
	      ///            invalid_variables[blocks[i][j]] = true;
	      ///  	}
              ///     }
                  TotalBlocksOver++;   
               }
            }    
        } //end days
     } //end weeks
     return (TotalBlocksOver*dist.penalty)/nrWeeks;
  }
  return 0;
}
long long TimeTablingProblem::penalize_pair( int id_class_i, int id_class_j, int id_distribution)
{
    Distribution dist = distributions[id_distribution];
  
   int id_time_i = x_var_time[id_class_i];
   int id_time_j = x_var_time[id_class_j];
   

  bool violatedRoom = false, violatedTime = false;
   if( dist.type == SAMESTART  )
   {
      if(!SameStart(times[id_time_i], times[id_time_j])) 
	 violatedTime = true;
   }
   else if( dist.type == SAMETIME  )
   {
       if( !SameTime(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTTIME )
   { 
	if( !DifferentTime(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
  else if(dist.type == SAMEDAYS) 
   {
	if( ! SameDays(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTDAYS )
   {
	if(!DifferentDays(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if(dist.type == SAMEWEEKS) 
   {
	if(!SameWeeks(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if( dist.type == DIFFERENTWEEKS )
   {
	if( !DifferentWeeks(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if( dist.type == OVERLAP)
   {
	if(! Overlap(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
   }
   else if( dist.type == NOTOVERLAP)
   { 
	if( ! NotOverlap(times[id_time_i], times[id_time_j]))
	 violatedTime = true;
  }
  else if(dist.type == SAMEROOM)
  {
	int id_room_i = x_var_room[id_class_i];
        int id_room_j = x_var_room[id_class_j];

	if(! SameRoom(id_room_i, id_room_j))
	 violatedRoom = true;
  }
  else if(dist.type == DIFFERENTROOM)
  {
	int id_room_i = x_var_room[id_class_i];
        int id_room_j = x_var_room[id_class_j];

	if(!DifferentRoom(id_room_i, id_room_j))
	 violatedRoom = true;
  }
  else if(dist.type == SAMEATTENDEES)
 {
        int id_room_i = x_var_room[id_class_i];
        int id_room_j = x_var_room[id_class_j];

      if( ! SameAttendees(times[id_time_i], times[id_time_j], id_room_i, id_room_j) )
	 violatedTime = true;
  }
  else if(dist.type == PRECEDENCE)
  {
     if(  dist.order_classes[id_class_i] > dist.order_classes[id_class_j] )
	swap(id_time_i, id_time_j);

     if( !Precedence(times[id_time_i], times[id_time_j]))
     {
////	if(id_class_j == 362 || id_class_j == 363)cout << "qqqq"<<endl;
	 violatedTime = true;
     }
  }
  else if(dist.type == WORKDAY)
  {
      if(! WorkDay(times[id_time_i], times[id_time_j], dist.S))
     	 violatedTime = true;
  }
  else if(dist.type == MINGAP)
  {
     if( ! MinGap(times[id_time_i], times[id_time_j], dist.G) )
	 violatedTime = true;
  }
  if(violatedTime || violatedRoom)
  {
         return dist.penalty;
  }
  return 0;	
}
void TimeTablingProblem::save_xml(vector<int> &x_var_room, vector<int> &x_var_time, vector< vector<int> > &x_var_student)
{
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("solution");
    node.append_attribute("name") = name.c_str();
    node.append_attribute("technique") = "Alternative";
    node.append_attribute("authors") = "cs and jcc";
    node.append_attribute("institution") = "CIMAT";
    node.append_attribute("country") = "Mexico";
    node.append_attribute("runtime") = "0";
    node.append_attribute("cores") = "1";

    vector< vector<int> > class_to_student(x_var_room.size());
   
    
    for(int i = 0; i < x_var_student.size(); i++)
    {
	for(int j = 0; j < x_var_student[i].size(); j++)
	{
	    int id_class = x_var_student[i][j];
	    class_to_student[id_class].push_back(i);
	}
    }
    for(int i = 0; i < class_to_student.size(); i++)
    {
        pugi::xml_node class_i = node.append_child("class");
        class_i.append_attribute("id") = idx_class[i];

	unsigned long int days = times[x_var_time[i]].days;
	unsigned long int weeks = times[x_var_time[i]].weeks;
	int start = times[x_var_time[i]].start;
	string str_days = "";
	for(int k = nrDays-1; k >=0 ; k--) str_days.push_back(( (days&(1<<k)) != 0)? '1':'0');
        class_i.append_attribute("days") = str_days.c_str();

        class_i.append_attribute("start") = start;
	string str_weeks = "";
	for(int k = nrWeeks-1; k >=0 ; k--) str_weeks.push_back(( (weeks&(1<<k)) != 0)? '1':'0');

        class_i.append_attribute("weeks") = str_weeks.c_str();

	if(x_var_room[i] != NOT_ROOM )
        class_i.append_attribute("room") = x_var_room[i]+1;

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

void TimeTablingProblem::loading_example()
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
	Time str_time;
        string days = class_i.attribute("days").value();
        str_time.days = stoll(days, nullptr, 2); //convert binary string to long long integer
        str_time.start = class_i.attribute("start").as_int();
        string weeks = class_i.attribute("weeks").value();
        str_time.weeks = stoll(weeks, nullptr, 2);
	//getting back the id-time......
        int id_time=-1;
	for(int i = 0; i < classes[id_class].times.size(); i++)
	{
	   Time str_time2 = times[classes[id_class].times[i]];
	   if( (str_time2.days == str_time.days)   )
	   if( (str_time2.start == str_time.start)   )
	   if( (str_time2.weeks == str_time.weeks)   )
	   {
		id_time = classes[id_class].times[i];
		break;
           }
	}
	
        x_var_time[id_class] = id_time;
        x_var_room[id_class] = id_room;

        for (pugi::xml_node student_i: class_i) 
        {
	   int id_student = student_i.attribute("id").as_int()-1;
	   x_var_student[id_student].push_back(id_class);
	}
    }
}
long long int TimeTablingProblem::get_var_time_size()
{ 
   long long int aveprod = 0;
   for(int i = 0; i < x_var_time.size(); i++)
   {    
	if(classes[i].times.empty()) continue;
	aveprod += classes[i].times.size();
   }
   aveprod /= x_var_time.size();
   return aveprod;
}
long long int TimeTablingProblem::get_var_room_size()
{
   long long int aveprod = 0;
   for(int i = 0; i < x_var_room.size(); i++)
   {
	if(classes[i].rooms_c.empty()) continue;
	aveprod += classes[i].rooms_c.size();
   }
   aveprod /= x_var_room.size();
   return aveprod;
}

////This function can be optimized with disjoint set or pre-sorting by times......
int TimeTablingProblem::implicit_room_constraints()
{
    vector<vector<int> > room_to_class(rooms.size());
    int Incumpled_room_constraints = 0;
    for(int i = 0; i < classes.size(); i++) // agrouping or clustering classes by rooms...
    {
	if( x_var_room[i]  == NOT_ROOM || x_var_time[i] == NOT_CHECK) continue;    
	room_to_class[x_var_room[i]].push_back(i);
    }

//    for(int r = 0; r < room_to_class.size(); r++)
//    {
//	for(int c1 = 0; c1 < room_to_class[r].size(); c1++)
//	{
//		int id_class_i = room_to_class[r][c1];
//		Time C_ti = times[id_class_i];
//	   for(int c2 = c1+1; c2 < room_to_class[r].size(); c2++)
//	   {
//		int id_class_j= room_to_class[r][c2];
//		if(id_class_i == id_class_j) continue;
//		Time C_tj = times[id_class_j];
//		if(Overlap(C_ti, C_tj))
//		{
//		   Incumpled_room_constraints++;
//		   invalid_variables[id_class_i]=true;
//		   invalid_variables[id_class_j]=true;
//		}
//
//	   }
//	}
//    }

    for(int id_class_i = 0; id_class_i < classes.size(); id_class_i++) //check each class
    {
	   if(x_var_room[id_class_i] == NOT_ROOM || x_var_time[id_class_i] == NOT_CHECK) continue;
             int id_room_i = x_var_room[id_class_i];
	     Time C_ti = times[x_var_time[id_class_i]];
	   ///////-->the unavailability constraints are removed in "feasible_space()" function
	   ///for(int j = 0; j < rooms[id_room_i].unavailable.size(); j++)
	   /// {
	   ///     Time C_tj = times[rooms[id_room_i].unavailable[j]];
	   ///     if(Overlap(C_ti, C_tj))
	   ///     {
	   ///        invalid_variables[id_class_i] = true;
	   ///        Incumpled_room_constraints++;
	   ///     }
	   /// }
	   //////-->two class cann't be in the same room at overlaping times..
	    for(int j = 0; j < room_to_class[id_room_i].size(); j++)
	    {
		int id_class_j = room_to_class[id_room_i][j];
		if( x_var_room[id_class_j] == NOT_ROOM || x_var_time[id_class_j] == NOT_CHECK) continue;
		if( id_class_i == id_class_j ) continue;
	
        	Time C_tj = times[x_var_time[id_class_j]];
		if(Overlap(C_ti, C_tj))
		{
		   Incumpled_room_constraints++;
		   //invalid_variables[id_class_i]=true;
		   //invalid_variables[id_class_j]=true;
		}
	    }
    }
//int cont2 = 0 ;
//for(int i = 0; i < invalid_variables.size(); i++)
//{
//   if(invalid_variables[i]){
//	 cont2++;
//	cout << i <<endl;
//	}
//}
//	cout << Incumpled_room_constraints << "-- " <<cont2 <<endl;
   return Incumpled_room_constraints;
}
long long TimeTablingProblem::room_penalization()
{
  long long room_penalization_v = 0;
for(int i = 0; i < classes.size(); i++)
  {
     room_penalization_v += classes[i].p_room_penalty[x_var_room[i]];
  }
  return room_penalization_v;
}
long long TimeTablingProblem::time_penalization()
{
  long long time_penalization_v =0;
  for(int i = 0; i < x_var_time.size(); i++)
  {
     if(classes[i].times.empty())    continue;
     time_penalization_v += times[x_var_time[i]].penalty;
  }
  return time_penalization_v;
}

void TimeTablingProblem::split_in_blocks(vector<vector<int> > &blocks, vector<int> &start_b, vector<int> &end_b, priority_queue< pair<int, pair<int, int> > > &pq, int S)
{
     vector<int> block;
       int Block_start = -pq.top().first, Block_end = pq.top().second.first;
	  int id_class = pq.top().second.second;
	  block.push_back(id_class);
	  pq.pop();

	  while(!pq.empty()) //building blocks by requirements..
	  {
	     int current_start = -pq.top().first;
	     int current_end = pq.top().second.first;
	     int id_class = pq.top().second.second;
	     pq.pop();
	     if( Block_end + S < current_start )
	     {
		blocks.push_back(block);
		start_b.push_back(Block_start);
		end_b.push_back(Block_end);
		
		block.clear();
		Block_start = current_start;
	     }
	     block.push_back(id_class);
	     Block_end = max(Block_end,current_end);
	  }
	  if(!block.empty()){
		 blocks.push_back(block);
	 	 start_b.push_back(Block_start);
		 end_b.push_back(Block_end);
	}

}
int TimeTablingProblem::student_penalization()
{
  int student_penalization = 0;
  for(int i = 0; i < this->x_var_student.size(); i++)
  {
     if(conflicts_student(i))
      student_penalization++;
  }
  return student_penalization;
}
bool TimeTablingProblem::conflicts_student(int id_student)
{
   vector<int> classes_by_student = x_var_student[id_student]; 
   for(int i = 0; i < classes_by_student.size(); i++) //checking each class...
   {
      for(int j = i+1; j < classes_by_student.size(); j++)
      {
	int id_class_i = classes_by_student[i];
	int id_class_j = classes_by_student[j];
        Time C_ti = times[x_var_time[id_class_i]];
        Time C_tj = times[x_var_time[id_class_j]];
	Room C_ri = rooms[x_var_room[id_class_i]];
  	Room C_rj = rooms[x_var_room[id_class_j]];
	bool c1 = (C_tj.start < C_ti.end + C_ri.time_travel_to_room[x_var_room[id_class_j]]);
	bool c2 = (C_ti.start <  C_tj.end + C_rj.time_travel_to_room[x_var_room[id_class_i]]);
        bool c3 = ((C_ti.days & C_tj.days)!=0);
	bool c4 = ((C_ti.weeks & C_tj.weeks)!=0);
	if( c1 && c2 && c3 && c4 )
          return true;	
      }
   }
  return false;
}
long long TimeTablingProblem::Hard_constraints()
{
   long long hard_constraints_violated = 0;
   hard_constraints_violated += implicit_room_constraints();
   /////////hard constarints by pair...
   for(int k = 0; k < pair_hard_distributions.size(); k++)
   {
      Distribution distribution_k = distributions[ pair_hard_distributions[k]];
      for(int i = 0; i < distribution_k.classes.size(); i++)	
      {
         int id_class_i = distribution_k.classes[i];
         if( x_var_time[id_class_i] == NOT_CHECK) continue;
         for(int j = i+1; j < distribution_k.classes.size(); j++)
         {
            int id_class_j = distribution_k.classes[j];
           if( x_var_time[id_class_j] == NOT_CHECK) continue;
            long long current_value = penalize_pair(id_class_i, id_class_j, pair_hard_distributions[k]);
          if(current_value) 
          {
             //invalid_variables[id_class_i] = true;
             //invalid_variables[id_class_j] = true;
             hard_constraints_violated +=  current_value;
           }
         }	
      }
   }
   /////////<-----hard constarints by pair...
   ////////distributions of groups...
   for(int k = 0; k < all_hard_distributions.size(); k++)
   {
	 Distribution distribution_k = distributions[all_hard_distributions[k]];
	 hard_constraints_violated += penalize_overall(all_hard_distributions[k]); 
   }
   ////////<--distributions of groups...
   return hard_constraints_violated;
}
long long TimeTablingProblem::Soft_constraints()
{
   long long distribution_soft_penalizations = 0;
  for(int k = 0; k < pair_soft_distributions.size(); k++)
  {
     Distribution distribution_k = distributions[ pair_soft_distributions[k]];
     for(int i = 0; i < distribution_k.classes.size(); i++)
     {
        for(int j = i+1; j < distribution_k.classes.size(); j++)
	{
	   int id_class_i = distribution_k.classes[i];
	   int id_class_j = distribution_k.classes[j];
	   distribution_soft_penalizations +=  penalize_pair(id_class_i, id_class_j, pair_soft_distributions[k]);
	}	
     }
  }
    for(int k = 0; k < all_soft_distributions.size(); k++)
    {
	 Distribution distribution_k = distributions[all_soft_distributions[k]];
	 distribution_soft_penalizations += penalize_overall(all_soft_distributions[k]);
    }
   return distribution_soft_penalizations;
}
//////////////////
pair<long long, long long> TimeTablingProblem::incremental_evaluation_by_classes(vector<int> &selected_classes)
{
   long long hard_constraints_violated = 0, soft_constraints_violated = 0;

   hard_constraints_violated += implicit_room_constraints();////pendiente...
   /////////hard constarints by pair...
   for(int c = 0; c < selected_classes.size(); c++)
   {
	int id_class = selected_classes[c];
	for(int d = 0; d < distributions_by_class[id_class].size(); d++)
	{
	     int id_distribution = distributions_by_class[id_class][d];
	     if( distributions[id_distribution].pair)
	     {
		 long long penalization = 0;
                 for(int i = 0; i < distributions[id_distribution].classes.size(); i++)
                 {
             	    int id_class_i = distributions[id_distribution].classes[i];
		   if( distributions[id_distribution].required && x_var_time[id_class_i] == NOT_CHECK) continue;
                   for(int j = i+1; j < distributions[id_distribution].classes.size(); j++)
             	   {
             	      int id_class_j = distributions[id_distribution].classes[j];
		      if( distributions[id_distribution].required && x_var_time[id_class_j] == NOT_CHECK) continue;
             	      penalization +=  penalize_pair(id_class_i, id_class_j, id_distribution);
             	   }	
                 }
		if( distributions[id_distribution].required) hard_constraints_violated += penalization;
		 else soft_constraints_violated +=penalization;
 	     }
	     else
	     {
		   long long penalization = penalize_overall(id_distribution);
	           if( distributions[id_distribution].required) hard_constraints_violated += penalization;
		   else soft_constraints_violated +=penalization;
             }
	}
   }
 //weighting the soft distributions according the problem 
  soft_constraints_violated *= distribution_w;
  soft_constraints_violated += room_penalization()*room_w;
  soft_constraints_violated += time_penalization()*time_w;
  soft_constraints_violated += student_penalization()*student_w;
  return make_pair(soft_constraints_violated, hard_constraints_violated); 
}
////return the list of violated hard constraints...
vector<int> TimeTablingProblem::unassign_hard_distributions(vector<pair<int, int>> &x_var)
{
for(int i = 0; i < x_var.size(); i++)
    {
	x_var_time[i] = x_var[i].first;
	x_var_room[i] = x_var[i].second;
    }

  vector<bool> conflict_classes(classes.size(), false); 

    vector<vector<int> > room_to_class(rooms.size());
   ////////rooms
   for(int id_class_i = 0; id_class_i < classes.size(); id_class_i++) //check each class
    {
	   if(x_var_room[id_class_i] == NOT_ROOM) continue;
             int id_room_i = x_var_room[id_class_i];
	     Time C_ti = times[x_var_time[id_class_i]];
	    for(int j = 0; j < room_to_class[id_room_i].size(); j++)
	    {
		int id_class_j = room_to_class[id_room_i][j];
		if( x_var_room[id_class_j] == NOT_ROOM ) continue;
		if( id_class_i == id_class_j ) continue;
        	Time C_tj = times[x_var_time[id_class_j]];
		if(Overlap(C_ti, C_tj))
		{
		   conflict_classes[id_class_i]=true;
		   conflict_classes[id_class_j]=true;
		}
	    }
    }


   /////////hard constarints by pair...
   for(int k = 0; k < pair_hard_distributions.size(); k++)
   {
      Distribution distribution_k = distributions[ pair_hard_distributions[k]];
      for(int i = 0; i < distribution_k.classes.size(); i++)	
      {
         int id_class_i = distribution_k.classes[i];
         for(int j = i+1; j < distribution_k.classes.size(); j++)
         {
            int id_class_j = distribution_k.classes[j];
            long long current_value = penalize_pair(id_class_i, id_class_j, pair_hard_distributions[k]);
          if(current_value) 
          {
	     conflict_classes[id_class_i] = true;
	     conflict_classes[id_class_j] = true;
           }
         }	
      }
   }
   /////////<-----hard constarints by pair...
   ////////distributions of groups...
   for(int k = 0; k < all_hard_distributions.size(); k++)
    {
	 Distribution distribution_k = distributions[all_hard_distributions[k]];
	 if(penalize_overall(all_hard_distributions[k]) > 0)
	 {
	  for(int i = 0 ; i < distribution_k.classes.size(); i++)
          {
	     conflict_classes[distribution_k.classes[i]] = true;
 	  }
         }
    }
   ////////<--distributions of groups...
   vector<int> idx_classes_conflict;
   for(int i =0; i < conflict_classes.size(); i++)if(conflict_classes[i]) idx_classes_conflict.push_back(i);
   return idx_classes_conflict;
}
