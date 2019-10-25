#ifndef __STEINER_TREE_H__
#define __STEINER_TREE_H__
#include <bits/stdc++.h>
using namespace std;
#define FOREACH(i, v) for (__typeof((v).begin()) i = (v).begin(); i != (v).end(); i++)

#define HARD 1
#define SOFT 2
#define PAR 3
#define OVERALL 4
#define SAMESTART 5
#define SAMETIME 6
#define SAMEDAYS 7
#define SAMEWEEKS 8
#define SAMEROOM 9
#define OVERLAP 10
#define SAMEATTENDEES 11
#define PRECEDENCE 12
#define WORKDAY 13
#define MINGAP 14
#define MAXDAYS 15
#define MAXDAYLOAD 16
#define MAXBREAKS 17
#define MAXBLOCK 18
#define DIFFERENTTIME 19
#define DIFFERENTDAYS 20
#define DIFFERENTWEEKS 21
#define DIFFERENTROOM 22
#define NOTOVERLAP 23
#define NOT_SET -1 //this indicates that a variable room is not assigned
#define NOT_CHECK -2 //this indicates that a variable its OK 

void printBest();



class TimeTablingProblem{
	public:
              struct Time
              {
                 unsigned long int days; //Days of week binary representation (7 bits)
                 int start, end;
                 int length;
                 unsigned long int weeks; //binary representation weeks of the semester problem-depend
                 long long int penalty;
              };
              struct Room
              {
                 int capacity;
                 vector<int> unavailable;
                 unordered_map< int, int > time_travel_to_room; // time to travel from this room to room with id
              };
              struct Distribution
              {
                 int type;
                 bool required;
                 long long int penalty;
		 int S, G, D, R, M; //slots over S, gaps, daysover D, breaks over R, blocks over M
                 vector<int> classes; //classes with this kind of constrint.
		 bool pair;
              };
              struct Class
              {
                 int Parent_id;
                 int limit;
                 //vector<Time> times;
		 vector<int > times;
                 unordered_map <int, int>  p_room_penalty; // penalty to asign room <id_room, penalty>
		 vector<pair<int, int> > rooms_c; // id room and penalty
                 bool rooms; // a class could have an unset room ...
              };
		TimeTablingProblem(string file);
		~TimeTablingProblem(){
		}
		void Load(string file);
		void Parsing_type(const char *, Distribution &str_distribution);
		///problem information header
		int nrDays, slotsPerDay, nrWeeks;
		string name;
		///optimization information header
		int time_w, room_w, distribution_w, student_w; //specifications of weights for the optimization criteria, i.e. each sum has a weight factor..

		vector <Room> rooms;
		vector <Time> times; 
		vector < vector <int> > courses; // to configuration;
		vector < vector <int> > configuration; //to Subpart;
		vector < vector <int> > subpart; //to Classes
		vector <Class> classes;
		vector <Distribution> distributions;
		vector < vector <int> > students; //to courses..
		/////distributions///////////////////////////////////////////////
		unordered_map<int, vector<int> > distributions_by_type; //the key is the type of distribution
		vector<int> hard_distributions, soft_distributions; //relation to feasibility
		vector<int> pair_hard_distributions, pair_soft_distributions, all_hard_distributions, all_soft_distributions; //relation to feasibility
		vector<int> pair_comparison_distributions, all_comparison_distributions;
		vector< vector<int> > distributions_by_class;

		unordered_map<int, unordered_map< bool, vector<int> > > distributions_by_feasibility;
};
class Individual{
	public:
		Individual(TimeTablingProblem &TTP_){
		  this->TTP= &TTP_;
		  x_var_time.resize(TTP->classes.size(), -1);
		  x_var_room.resize(TTP->classes.size(), -1);
		  x_var_student.resize(TTP->students.size());
		  // exit(0);
		  //initialization....
		  initialization();
		  //Load example solution to check the evaluator...
		// cout << "Feasible space (Domain size) by room.... in this individual " << get_var_room_size() <<endl;
		 //cout << "Feasible space (Domain size) by time.... in this individual " << get_var_time_size() <<endl;
                  //loading_example(); // solution-wbg-fal10.xml ...
		  //cout << this->calculateFitness().first<<endl;
		//   save_xml();
		}
		Individual(){}
		~Individual(){
		}
//	        inline int first(long long int bin){ int pos =0; while( !(bin & (1<<pos)) )pos++; return pos;  }		
		void loading_example();
		void save_xml();
		void initialization();
		long long penalize_pair( int id_class_i, int id_class_j, int id_distribution);
		long long penalize_overall( int id_distribution);
		bool conflicts_student(int id_student);
		int getDistance(Individual &ind);
		void Mutation(double pm);
		void Crossover(Individual &ind);
		void localSearch();
		pair<long long, int> calculateFitness();
		void print();
		long long int get_var_time_size();
		long long int get_var_room_size();
		long long fitness;
		//static TimeTablingProblem *TimeTablingproblem;
		TimeTablingProblem *TTP;
 		int dist;
		vector<int> x_var_time, x_var_room;
		vector< vector<int> > x_var_student;
};

#endif
