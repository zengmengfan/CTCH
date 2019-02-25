#ifndef CTMODEL_H
#define CTMODEL_H
#include "shared.h"
#include "deduce.h"
class CtSolver{
public:
	CtSolver(){};
	CtSolver(int strength, vector<int>& parameters, int candidates);
	CtSolver(int strength, vector<int>& parameters, vector<vector<PV> >& forbidden, int candidates);
	vector<TestCase> solve();
	vector<TestCase> optimize(vector<TestCase>& ts);
	vector<TestCase> optimizeImproved(vector<TestCase>& ts);
	vector<TestCase> modify();
	vector<TestCase> post(vector<TestCase>& ts);
private:
	int t;
	vector<int> factors;
	int M;
	vector<vector<PV> > forbiddenConstraints;
	vector<vector<vector<int> > > constraintInvolvedByLevel;
	int L;
	vector<vector<bool> > coverState;
	int uncoveredCount;
	vector<int> groupUncover;
	vector<vector<PV> > factorComb;
	vector<vector<vector<PV> > >partialCombs;
	vector<vector<int> > levelAppear;

	void init();
	void partialCoverstate(vector<PV>& partial);
	TestCase  fusion(TestCase& tc1,TestCase& tc2);
	void modifyTestcase(TestCase& tc);
	vector<int> checkTestcase(vector<PV>& tc);
	vector<vector<PV> > getTuples(vector<PV>& partialCase, int tau);
	vector<vector<PV> > getAllTuples(vector<PV>& partialCase, int tau);
	vector<int> getPosition(vector<PV>& tuple);
	int getLeIndex(vector<PV>& tuple);
	bool isValid(vector<PV> partial,int factor,int level);
	PV getBestLevel(TestCase& partialCase,int factor,bool consider);
	vector<int> getRemainOrder(vector<PV>& tuple);
	void updateCoverState(TestCase& testCase);
	vector<PV> getFirstTuple();
	TestCase generateTest(bool consider);
	void shuffle(vector<int>& parameters);
};
#endif