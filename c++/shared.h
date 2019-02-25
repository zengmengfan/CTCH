#ifndef SHARED_H
#define SHARED_H
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include <utility>
#include <fstream>
using std::ifstream;
using std::cout;
using std::cin;
using std::cerr;
using std::stringstream;
using std::vector;
using std::endl;
using std::sort;
using std::string;
using std::set;

struct PV{
	int para;
	int value;
};

struct TestCase{
	vector<PV> testCase;
	int fitness;
};
bool comp(const PV& a,const PV& b);
long long comb(int n,int m);
long long combN2K(int n,int m,int k);
int getPaIndex(vector<int>& v,int n);
int str2num(string s);
vector<string> split(string source, char delimiter);
PV translate(vector<int> model,int raw);
vector<int > processInputString(string input);
void adjust(vector<PV>& tuple);
bool isSubTuple(vector<PV> tuple1, vector<PV> tuple2);
vector<int> findRepeat(vector<vector<PV> > pairs);
vector<vector<PV> > simplify(vector<vector<PV> > origional);
vector<vector<vector<int> > > constraintInvolved(vector<int> factors,vector<vector<PV> > &origional);
vector<vector<int> > directProduct(vector<vector<int> >& allList);
void printTuples(vector<vector<PV> >& res);
void printTestSuite(vector<TestCase>& res);
void printTestcase(TestCase& tc);

#endif