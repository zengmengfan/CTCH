#ifndef DEDUCE_H
#define DEDUCE_H
#include "shared.h"
vector<vector<vector<int> > > mergeTuple(vector<int> factors,int factor,vector<vector<vector<int > > > &auxiliary,vector<vector<PV> >& origional);
vector<vector<PV> > deduceConstraint(vector<int >factors,vector<vector<PV> > origional);
#endif