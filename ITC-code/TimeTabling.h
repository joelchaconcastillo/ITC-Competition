#ifndef __STEINER_TREE_H__
#define __STEINER_TREE_H__
#include <bits/stdc++.h>
using namespace std;
#define FOREACH(i, v) for (__typeof((v).begin()) i = (v).begin(); i != (v).end(); i++)

void printBest();



class TimeTablingProblem{
	public:
              struct Time
              {
                 unsigned long int days; //Days of week binary representation (7 bits)
                 int start;
                 int length;
                 unsigned long weeks; //binary representation weeks of the semester problem-depend
                 long long int penalty;
              };
              struct Room
              {
                 int capacity;
                 vector<Time> unavailable;
                 unordered_map< int, int > p_travel_to_room; //<id_room, penalty>
              };
              struct Distribution
              {
                 string type;
                 bool required;
                 long long int penalty;
                 vector<int> classes; //classes with this kind of constrint..
              };
              struct Class
              {
                 int Parent_id;
                 int limit;
                 vector<Time> times;
                 unordered_map <int, int>  p_room_penalty; // penality to asign room <id_room, penalty>
                 bool rooms; // a class could have an unset room ...
              };
		TimeTablingProblem(string file);
		~TimeTablingProblem(){
		}
		void Load(string file);
		///problem information header
		int nrDays, slotsPerDay, nrWeeks;
		string name;
		///optimization information header
		int time_w, room_w, distribution_w, student_w; //specifications of weights for the optimization criteria, i.e. each sum has a weight factor..

		vector <Room> rooms;
		vector < vector <int> > courses; // to configuration;
		vector < vector <int> > configuration; //to Subpart;
		vector < vector <int> > subpart; //to Classes
		vector <Class> classes;
		vector <Distribution> distributions;
		vector < vector <int> > students; //to courses..
		unordered_map<string, vector<int> > distributions_by_type;
};
class TimeTabling{
	public:
		TimeTabling(){
		
		}
		~TimeTabling(){
		
		}
		int getDistance(TimeTabling &ind);
		void Mutation(double pm);
		void Crossover(TimeTabling &ind);
		void localSearch();
		long long calculateFitness();
		void print();
		long long fitness;
		static TimeTablingProblem *TimeTablingproblem;
};

#endif
