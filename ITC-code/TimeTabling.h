#ifndef __TIME_TABLING_PROBLEM_H__
#define __TIME_TABLING_PROBLEM_H__
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
#define ROOM_OVERLAP 24
#define NOT_CHECK -2 //this indicates that a variable its OK 
#define TIMES 0
#define ROOMS 1
#define NOT_ROOM -1
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
                 vector<int> classes; //classes with this kind of constrain.
		 unordered_map<int, int> order_classes; // id , order for Precedence distribution
		 bool pair;
              };
              struct Class
              {
                 int Parent_id;
                 int limit;
                 //vector<Time> times;
		 vector<int > times;
                 unordered_map <int, int>  p_room_penalty; // penalty to asign room <id_room, penalty>
		 vector<int> rooms_c, penalty_c;
                 bool rooms; // a class could have an unset room ...
              };
		TimeTablingProblem(string file);
		~TimeTablingProblem(){
		}
		void Load(string file);
		void Parsing_type(const char *, Distribution &str_distribution);


		long long penalize_pair( int id_class_i, int id_class_j, int id_distribution);
		long long penalize_overall( int id_distribution);
		bool conflicts_student(int id_student);

		int implicit_room_constraints();
		long long Hard_constraints();
		long long Soft_constraints();

		long long time_penalization();

		long long room_penalization();
		
		int student_penalization();
		pair<long long, long long> incremental_evaluation_by_classes(vector<int> &selected_classes);

		vector<int> unassign_hard_distributions(vector<pair<int, int>> &x_var);


		void feasible_space();


		void split_in_blocks(vector<vector<int> > &blocks, vector<int> &start_b, vector<int> &end_b, priority_queue< pair<int, pair<int, int> > > &pq, int S);




		inline bool SameStart(Time &C_ti, Time &C_tj){return (C_ti.start == C_tj.start); }
		inline bool SameTime(Time &C_ti, Time &C_tj){return ( (C_ti.start <= C_tj.start && C_tj.end <= C_ti.end) || (C_tj.start <= C_ti.start && C_ti.end <= C_tj.end) ) ; }
		inline bool DifferentTime(Time &C_ti, Time &C_tj){return ((C_ti.end <= C_tj.start )  || (C_tj.end <= C_ti.start)  );}
		inline bool SameDays(Time &C_ti, Time &C_tj){return (((C_ti.days | C_tj.days) == C_ti.days)  || ((C_ti.days | C_tj.days) == C_tj.days) ); }
		inline bool DifferentDays(Time &C_ti, Time &C_tj){return ( (C_ti.days & C_tj.days) == 0); }
		inline bool SameWeeks(Time &C_ti, Time &C_tj){return  ( ((C_ti.weeks | C_tj.weeks) == C_ti.weeks)  || ((C_ti.weeks | C_tj.weeks) == C_tj.weeks) ); }
		inline bool DifferentWeeks(Time &C_ti, Time &C_tj){return  ((C_ti.weeks & C_tj.weeks) == 0); }
		inline bool Overlap(Time &C_ti, Time &C_tj){return  ( (C_tj.start < C_ti.end) && (C_ti.start < C_tj.end) && ( ( (C_ti.days & C_tj.days)!=0) && ((C_ti.weeks & C_tj.weeks) != 0) ) ); }
		inline bool NotOverlap(Time &C_ti, Time &C_tj){return ((C_ti.end <= C_tj.start) || (C_tj.end <= C_ti.start) || ( ((C_ti.days & C_tj.days)==0) || ((C_ti.weeks & C_tj.weeks) == 0) ) ); }
		inline bool SameRoom(int id_room_i, int id_room_j){  return ( id_room_i == id_room_j);}
		inline bool DifferentRoom(int id_room_i, int id_room_j){  return ( id_room_i != id_room_j);}
		inline bool SameAttendees(Time &C_ti, Time &C_tj, int id_room_i, int id_room_j)
		{
//			if( id_room_i == NOT_ROOM || id_room_j==NOT_ROOM) return false;
			int  traveling_time_i_j = (id_room_i==NOT_ROOM)?0:rooms[id_room_i].time_travel_to_room[id_room_j];
			int  traveling_time_j_i = (id_room_j==NOT_ROOM)?0:rooms[id_room_j].time_travel_to_room[id_room_i];
			bool c1 = (C_ti.end + traveling_time_i_j ) <= C_tj.start ;
			bool c2 = (C_tj.end + traveling_time_j_i) <= C_ti.start ;
        		bool c3 = ((C_ti.days & C_tj.days)==0);
			bool c4 = ((C_ti.weeks & C_tj.weeks)==0);
//			cout << C_ti.start << " " << C_tj.start <<" " << C_ti.end << " " << C_tj.end <<" " << traveling_time_i_j << " " <<traveling_time_j_i << C_ti.days << " " <<C_tj.days<<endl;
//			cout << c1 << c2<<c3<<c4<<endl;
			return (c1 || c2 || c3 || c4   );
		}
		inline bool Precedence(Time &C_ti, Time &C_tj)
		{
			bool c1 = (nrWeeks-ffsll(C_ti.weeks)) < (nrWeeks-ffsll(C_tj.weeks));
		        bool c2 = ffsll(C_ti.weeks) == ffsll(C_tj.weeks);
		        bool c3 = (nrDays-ffsll(C_ti.days)) < (nrDays-ffsll(C_tj.days));
		        bool c4 = ffsll(C_ti.days) == ffsll(C_tj.days);
		        bool c5 = C_ti.end <= C_tj.start;
//			cout << c1 << c2<<c3<<c4<<c5<<endl;
		        return ( c1 || ( c2 && ( c3 || ( c4 && c5  )  ) ) );
		}
		inline bool WorkDay(Time &C_ti, Time &C_tj, int S)
	        {
		        bool c1 = ( (C_ti.days & C_tj.days) == 0);
		        bool c2 = ( (C_ti.weeks & C_tj.weeks) == 0);
		        bool c3 = ( (max(C_ti.end, C_tj.end) - min(C_ti.start, C_tj.start)) <= S );
		        return ( c1 || c2 || c3);
		}
		inline bool MinGap(Time &C_ti, Time &C_tj, int G)
		{
		        bool c1 = ( (C_ti.days & C_tj.days)==0 );
		        bool c2 = ( (C_ti.weeks & C_tj.weeks)==0 );
		        bool c3 = ( (C_ti.end + G) <= C_tj.start  );
		        bool c4 = ( (C_tj.end + G) <= C_ti.start  );
			return ( c1 || c2 || c3 || c4);
		}



		long long int get_var_time_size();
		long long int get_var_room_size();
		void loading_example();
		void save_xml(vector<int> &x_var_room, vector<int> &x_var_time, vector< vector<int> > &x_var_student);

		///problem information header
		int nrDays, slotsPerDay, nrWeeks;
		string name;
		///optimization information header
		int time_w, room_w, distribution_w, student_w; //specifications of weights for the optimization criteria, i.e. each sum has a weight factor..

		unordered_map<int, int> idx_class, class_idx;

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
	        vector<int> x_var_time, x_var_room;
		vector< vector<int> > x_var_student;
 		vector<int> x_var;
		vector<vector<pair<int,int>>> domain;
		vector< vector<int>> from_table_to_class, linear_domain;

//		vector<pair<int, int>>

};
#endif
