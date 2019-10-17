#ifndef __STEINER_TREE_H__
#define __STEINER_TREE_H__
#include <bits/stdc++.h>
using namespace std;
#define FOREACH(i, v) for (__typeof((v).begin()) i = (v).begin(); i != (v).end(); i++)

void printBest();

class TimeTablingProblem{
	public:
		TimeTablingProblem(string file);
		~TimeTablingProblem(){
		
		}
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
