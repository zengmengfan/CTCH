#include "ctmodel.h"

CtSolver::CtSolver(int strength, vector<int>& parameters, int candidates){
		t=strength;
		factors=parameters;
		M=candidates;
		init();
		constraintInvolvedByLevel=constraintInvolved(factors,forbiddenConstraints);
}

CtSolver::CtSolver(int strength, vector<int>& parameters,vector<vector<PV> >& forbidden, int candidates){
	t=strength;
	factors=parameters;
	M=candidates;
	forbiddenConstraints=forbidden;
	init();
	//printTuples(forbiddenConstraints);
	//update coverstate
	for(int i=0;i<forbiddenConstraints.size();i++){
		if(forbiddenConstraints[i].size()==t){
			vector<int > p=getPosition(forbiddenConstraints[i]);
			coverState[p[0]][p[1]]=true;
			uncoveredCount--;
			groupUncover[p[0]]-=1;
		} 
		if(forbiddenConstraints[i].size()<t) partialCoverstate(forbiddenConstraints[i]);
	}
	
	constraintInvolvedByLevel=constraintInvolved(factors,forbiddenConstraints);
	
}

void CtSolver::partialCoverstate(vector<PV>& partial){
	vector <PV> temp;
	int pi=0;
	for(int i=0;i<factors.size();i++){
		if(partial[pi].para!=i){
			PV pv;
			pv.para=i;
			temp.push_back(pv);
		}else if(pi<partial.size()-1) pi++;	
	}
	//get all t-tuples which cover that partial tuple and change their coverstate
	vector<PV > continuous;
	for(int i=0;i<temp.size();i++){
		PV pv;
		pv.para=i;
		continuous.push_back(pv);
	}
	vector<vector<PV> >keep=getTuples(continuous,t-partial.size());
	vector<vector<PV> > all;
	for(int i=0;i<keep.size();i++){
		vector<PV> tp;
		for(int j=0;j<keep[i].size();j++)
			tp.push_back(temp[keep[i][j].para]);
		all.push_back(tp);
	}
	for(int i=0;i<all.size();i++){
		vector<int> fc;
		for(int j=0;j<all[i].size();j++)
			fc.push_back(all[i][j].para);
		for(int j=0;j<partial.size();j++) fc.push_back(partial[j].para);
		sort(fc.begin(),fc.end());
		int fp=getPaIndex(fc,factors.size());
		int* map=new int[t-partial.size()];
		int p=0;
		for(int j=0;j<fc.size();j++){
			if(all[i][p].para==fc[j]) {
				map[p]=j;
				if(p<t-partial.size()-1) p++;
			}
		}
		vector<vector<int> > levelList;
		for(int j=0;j<all[i].size();j++){
			vector<int > level;
			for(int k=0;k<factors[all[i][j].para];k++)
				level.push_back(k);
			levelList.push_back(std::move(level));
		}
		vector<vector<int> > levelComb=directProduct(levelList);
		//construct unassigned t-tuples
		vector<PV> tuple=partial;
		for(int j=0;j<all[i].size();j++){
			tuple.push_back(all[i][j]);
		}
		sort(tuple.begin(),tuple.end(),comp);

		//modify coverstate
		for(int j=0;j<levelComb.size();j++){
			for(int k=0;k<t-partial.size();k++){
				tuple[map[k]].value=levelComb[j][k];
			}
			if(!coverState[fp][getLeIndex(tuple)]){
				coverState[fp][getLeIndex(tuple)]=true;
				groupUncover[fp]-=1;
				uncoveredCount--;
			}
		}
		delete [] map;

	}
}

void CtSolver::init(){
	//calculate L
	vector<int> forSort=factors;
	sort(forSort.begin(),forSort.end());
	L=1;
	for(int i=forSort.size()-1;i>forSort.size()-t-1;i--)
		L=L*forSort[i];

	//initialize levelAppear
	for(int i=0;i<factors.size();i++){
		vector<int> temp(factors[i],0);
		levelAppear.push_back(std::move(temp));
	}
	vector<PV> factorInfo;
	for(int i=0;i<factors.size();i++){
		PV temp;
		temp.para=i;
		temp.value=factors[i];
		factorInfo.push_back(temp);
	}

	//initialize coverState
	factorComb=getTuples(factorInfo,t);
	for(int i=0;i<factorComb.size();i++){
		vector<bool> temp;
		coverState.push_back(temp);
	}
	int sum=0;
	for(int i=0;i<factorComb.size();i++){
		int temp=1;
		for(int j=0;j<t;j++)
			temp*=factorComb[i][j].value;

		groupUncover.push_back(temp);
		for(int j=0;j<temp;j++){
			coverState[i].push_back(false);
		}
		sum+=temp;
	}
	
	//initialize the uncoveredCount
	uncoveredCount=sum;

	//initialize the partialCombs

	for(int i=0;i<factors.size()-t;i++){
		vector<PV>  partial;
		for(int j=0;j<t+i;j++){
			PV temp;
			temp.para=j;
			partial.push_back(temp);
		}
		partialCombs.push_back(getTuples(partial,t-1));
	}

}

vector<vector<PV> > CtSolver::getTuples(vector<PV>& partialCase, int tau){
	vector<vector<PV> > singleTuples;
	for(int i=0;i<partialCase.size();i++){
			vector<PV> tuple;
			tuple.push_back(partialCase[i]);
			singleTuples.push_back(tuple);
	}
	if(tau==1) return singleTuples;
	vector<vector<PV> > oldTuples=std::move(singleTuples);
	for(int i=0;i<tau-1;i++){
		vector<vector<PV> > newTuples;
		for(int j=0;j<oldTuples.size();j++){
			for(int k=oldTuples[j][i].para+1;k<=partialCase[partialCase.size()-1].para;k++){
				vector<PV> tuple=oldTuples[j];
				tuple.push_back(partialCase[k]);
				newTuples.push_back(tuple);
			}
		}
		oldTuples=std::move(newTuples);
	}
	return oldTuples;		
}

vector<vector<PV> > CtSolver::getAllTuples(vector<PV>& partialCase,int tau){
	int n=partialCase.size();
	vector<vector<PV> > res=partialCombs[n-tau-1];
	for(int i=0;i<res.size();i++){
		for(int j=0;j<tau;j++){
			res[i][j].value=partialCase[res[i][j].para].value;
			res[i][j].para=partialCase[res[i][j].para].para;
			
		}
	}
	return res;
}

vector<int> CtSolver::getPosition(vector<PV>& tuple){
	vector<int> pos;
	vector<int> fcomb;
	vector<int> lcomb;
	for(int i=0;i<tuple.size();i++){
		fcomb.push_back(tuple[i].para);
		lcomb.push_back(tuple[i].value);
	}
	pos.push_back(getPaIndex(fcomb,factors.size()));
	int level_pos=0;
	int weight=1;
	for(int i=lcomb.size()-1;i>=0;i--){
		level_pos+=weight*lcomb[i];
		weight*=factors[fcomb[i]];
	}
	pos.push_back(level_pos);
	return pos;
}

int CtSolver::getLeIndex(vector<PV>& tuple){
	int level_pos=0;
	int weight=1;
	for(int i=tuple.size()-1;i>=0;i--){
		level_pos+=weight*tuple[i].value;
		weight*=factors[tuple[i].para];
	}
	return level_pos;
}

bool CtSolver::isValid(vector<PV> partial,int factor,int level){
	bool flag=true;
	for(int i=0;i<constraintInvolvedByLevel[factor][level].size();i++){
		vector<PV> constraint=forbiddenConstraints[constraintInvolvedByLevel[factor][level][i]];
		if(isSubTuple(constraint,partial)) {
			flag=false;
			break;
		}
	}
	return flag;
}

PV CtSolver::getBestLevel(TestCase& partialCase,int factor,bool consider){
	vector<PV>& partial=partialCase.testCase;
	vector<vector<PV> > subTuples=getAllTuples(partial,t-1);
	//insert current factor and sort
	vector<int> factorPos;
	for(int i=0;i<subTuples.size();i++){
		PV temp;
		temp.para=factor;
		subTuples[i].push_back(temp);
		//sort
		adjust(subTuples[i]);
		vector<int> fcomb;
		for(int j=0;j<t;j++)
			fcomb.push_back(subTuples[i][j].para);
		int fp=getPaIndex(fcomb,factors.size());
		factorPos.push_back(fp);
	}
	int bestLevel=0;
	int maxCover=-1;
	for(int i=0;i<factors[factor];i++){
		//check whether a  value  satisfy constraints
		if(consider){
			vector<PV> forCkeck=partial;
			PV current;
			current.para=factor;
			current.value=i;
			forCkeck.push_back(current);
			adjust(forCkeck);
			if(!isValid(forCkeck,factor,i)) continue;
		}

		int count=0;
		for(int j=0;j<subTuples.size();j++){
			//modify the value of the factor to i 
			for(int k=0;k<t;k++){
				if(subTuples[j][k].para==factor){
					subTuples[j][k].value=i;
					break;
				}
			}
			//
			if(!coverState[factorPos[j]][getLeIndex(subTuples[j])]) count++;	
		}
		if(count>maxCover||(count==maxCover&&levelAppear[factor][i]<levelAppear[factor][bestLevel])){
			bestLevel=i;
			maxCover=count;
		}
	}
	PV res;
	res.para=bestLevel;
	res.value=maxCover+partialCase.fitness;
	return res;
}

void CtSolver::shuffle(vector<int>& parameters){
	int n=parameters.size();
	for(int i=0;i<n;i++){
		int j=rand()%n;
		int temp=parameters[j];
		parameters[j]=parameters[i];
		parameters[i]=temp;
	}
}

vector<int> CtSolver::getRemainOrder(vector<PV>& tuple){
	vector<int> order;
	for(int i=0;i<factors.size();i++)
		order.push_back(i);
	auto it=order.begin();
	while(it!=order.end()){
		bool flag=false;
		for(int i=0;i<tuple.size();i++){
			if(*it==tuple[i].para){
				it=order.erase(it);
				flag=true;
				break;
		 	}
		}	
		if(!flag) it++;
	}
	shuffle(order);
	return order;

}

vector<PV> CtSolver::getFirstTuple(){
	int index=rand()%(factorComb.size());
	for(int i=0;i<groupUncover.size();i++){
		if(groupUncover[index]){
			break;
		}else index=(index+1)%factorComb.size();
	}
	vector<PV> firstTuple=factorComb[index];
	vector<int> fcomb;
	int weight=1;
	for(int i=0;i<firstTuple.size();i++){
		fcomb.push_back(firstTuple[i].para);
		weight=weight*firstTuple[i].value;
	}
	int fp=getPaIndex(fcomb,factors.size());//the position of factor combination
	int level_pos;
	for(int i=0;i<coverState[fp].size();i++){
		if(!coverState[fp][i]) {
			level_pos=i;
			break;
		}
	}
	//get the level of factors
	for(int i=0;i<firstTuple.size();i++){
		weight=weight/firstTuple[i].value;
		firstTuple[i].value=level_pos/weight;
		level_pos=level_pos%weight;
	}
	return firstTuple;
}


void CtSolver::updateCoverState(TestCase& testCase){
	uncoveredCount-=testCase.fitness;
	for(int i=0;i<factorComb.size();i++){
		vector<PV> tuple;
		for(int j=0;j<t;j++)
			tuple.push_back(testCase.testCase[factorComb[i][j].para]);
		int level=getLeIndex(tuple);
		if(!coverState[i][level]){
			groupUncover[i]-=1;
			coverState[i][level]=true;
			//update levelAppear
			for(int j=0;j<t;j++){
				levelAppear[tuple[j].para][tuple[j].value]+=1;
			}
		}
	}
}

TestCase CtSolver::generateTest(bool consider){
	vector<PV> firstTuple=getFirstTuple();
	TestCase tc;
	tc.testCase=firstTuple;
	tc.fitness=1;
	vector<int> order=getRemainOrder(firstTuple);
	for(int i=0;i<order.size();i++){
		PV bestNext=getBestLevel(tc,order[i],consider);
		tc.fitness=bestNext.value;
		bestNext.value=bestNext.para;
		bestNext.para=order[i];
		tc.testCase.push_back(bestNext);
		adjust(tc.testCase);//sort
	}
	return tc;
}

vector<TestCase> CtSolver::solve(){
	cout<<"Parameter number:"<<factors.size()<<endl<<"Uncovered Pairs:"<<uncoveredCount<<endl;
	vector<TestCase> testSuite;
	//int last=0;
	while(uncoveredCount!=0){
		TestCase best=generateTest(true);
		for(int i=0;i<M;i++){
			TestCase tc=generateTest(true);
			if(tc.fitness>best.fitness) best=tc;
			if(best.fitness>=uncoveredCount/L) break;
		}
		testSuite.push_back(best);
		//last=best.fitness;
		updateCoverState(best);
	}
	return testSuite;
}

vector<int> CtSolver::checkTestcase(vector<PV>& tc){
	set<int> pset;
	for(int i=0;i<forbiddenConstraints.size();i++){
		if(isSubTuple(forbiddenConstraints[i],tc)){
			for(int j=0;j<forbiddenConstraints[i].size();j++) pset.insert(forbiddenConstraints[i][j].para);
		}
	}
	vector<int> res;
	for(auto it=pset.begin();it!=pset.end();it++)
		res.push_back(*it);
	return res;
}

void CtSolver::modifyTestcase(TestCase& tc){
	vector<int> cps=checkTestcase(tc.testCase);
	if(cps.size()!=0){
		vector<PV> tuple;
		for(int i=0;i<factorComb.size();i++){
			vector<PV> temp;
			for(int j=0;j<t;j++)
				temp.push_back(tc.testCase[factorComb[i][j].para]);
			int level=getLeIndex(temp);
			if(!coverState[i][level]){
				tuple=temp;
				break;
			}
		}
		//delete parameters in tuple from cps;
		for(auto it=cps.begin();it!=cps.end();){
			bool flag=false;
			for(int i=0;i<tuple.size();i++){
				if(tuple[i].para==*it){
					flag=true;
					break;
				}
			}
			if(flag) it=cps.erase(it);
			else it++;
		}
		for(int i=0;i<cps.size();i++){
			vector<PV> temp;
			for(int j=0;j<factors.size();j++){
				bool flag=true;
				for(int k=i;k<cps.size();k++){
					if(tc.testCase[j].para==cps[k]){
						flag=false;
						break;
					}
				}
				if(flag) temp.push_back(tc.testCase[j]);
			}
			//modify
			TestCase temp1;
			temp1.testCase=temp;
			temp1.fitness=0; 
			PV bestNext=getBestLevel(temp1,cps[i],true);
			tc.testCase[cps[i]].value=bestNext.para;
			//if testcase becomes valid break;
			if(!checkTestcase(tc.testCase).size())  break;
		}
	}
	//modify fitness
	int fit=0;
	for(int i=0;i<factorComb.size();i++){
		vector<PV> tuple;
		for(int j=0;j<t;j++)
			tuple.push_back(tc.testCase[factorComb[i][j].para]);
		int level=getLeIndex(tuple);
		if(!coverState[i][level]){
			fit++;
		}
	}
	tc.fitness=fit;
}

vector<TestCase> CtSolver::modify(){
	vector<TestCase> testSuite;
	while(uncoveredCount!=0){
		TestCase best=generateTest(false);
		//modify
		modifyTestcase(best);
		for(int i=0;i<M;i++){
			TestCase tc=generateTest(false);
			modifyTestcase(tc);
			if(tc.fitness>best.fitness) best=tc;
			if(best.fitness>=uncoveredCount/L) break;
		}
		testSuite.push_back(best);
		//last=best.fitness;
		updateCoverState(best);
	}
	return testSuite;
}

vector<TestCase> CtSolver::post(vector<TestCase>& ts){
	vector<vector<int> > coverTimes;
	for(int i=0;i<coverState.size();i++){
		vector<int > temp(coverState[i].size(),0);
		coverTimes.push_back(std::move(temp));
	}

	vector<vector<int> > keepLevel;
	//calculate covering times
	for(int i=0;i<ts.size();i++){
		vector<int> levelForTest;
		for(int j=0;j<factorComb.size();j++){
			vector<PV> tuple;
			for(int k=0;k<t;k++)
				tuple.push_back(ts[i].testCase[factorComb[j][k].para]);
			int level=getLeIndex(tuple);
			coverTimes[j][level]++;
			levelForTest.push_back(level);
		}
		keepLevel.push_back(std::move(levelForTest));
	}

	//keep or break up
	vector<int > valid;
	vector<vector<int> > add;
	for(int i=0;i<ts.size();i++){
		if(!checkTestcase(ts[i].testCase).size()) valid.push_back(i);
		else{
			for(int j=0;j<factorComb.size();j++){
				bool need=false;
				vector<PV> tuple;
				for(int k=0;k<t;k++)
					tuple.push_back(ts[i].testCase[factorComb[j][k].para]);
				if(coverTimes[j][keepLevel[i][j]]==1 && !checkTestcase(tuple).size()){
					need=true;
				}

				//add new test
				if(need){
					bool settled=false;
					for(int k=0;k<add.size();k++){
						bool flag=true;
						for(int p=0;p<t;p++){
							if(add[k][tuple[p].para]!=-1&&add[k][tuple[p].para]!=tuple[p].value){
								flag=false;
								break;
							}
						}
						//check after merging
						if(flag){
							vector<int> keep=add[k];
							for(int p=0;p<t;p++)
								add[k][tuple[p].para]=tuple[p].value;
							vector<PV> current;
							for(int p=0;p<add[k].size();p++)
								if(add[k][p]!=-1){
									PV pv;
									pv.para=p;
									pv.value=add[k][p];
									current.push_back(pv);
								}
							if(!checkTestcase(current).size()){
								settled=true;
							}else add[k]=keep;
						}
					}
					if(!settled){
						vector<int > newtest(factors.size(),-1);
						for(int p=0;p<t;p++)
							newtest[tuple[p].para]=tuple[p].value;
						add.push_back(newtest);
					}
				}else coverTimes[j][keepLevel[i][j]]--;

				/*cerr<<"add"<<endl;
				for(int l=0;l<add.size();l++){
					for(int n=0;n<add[l].size();n++)
						cerr<<add[l][n]<<",";
					cerr<<endl;
				}*/
			}
		} 
	}
	vector<TestCase> res;
	for(int i=0;i<valid.size();i++)
		res.push_back(ts[valid[i]]);
	for(int i=0;i<add.size();i++){
		TestCase temp;
		vector<PV> tc;
		for(int j=0;j<add[i].size();j++){
			PV pv;
			pv.para=j;
			pv.value=add[i][j];
			tc.push_back(pv);
		}
		temp.testCase=tc;
		temp.fitness=0;
		res.push_back(temp);
	}
	return res;
}

vector<TestCase> CtSolver::optimize(vector<TestCase>& ts){
	vector<vector<int> > coverTimes;
	for(int i=0;i<coverState.size();i++){
		vector<int > temp(coverState[i].size(),0);
		coverTimes.push_back(std::move(temp));
	}
	vector<vector<int> > keepLevel;
	for(int i=0;i<ts.size();i++){
		//calculate covering times
		vector<int> levelForTest;
		for(int j=0;j<factorComb.size();j++){
			vector<PV> tuple;
			for(int k=0;k<t;k++)
				tuple.push_back(ts[i].testCase[factorComb[j][k].para]);
			int level=getLeIndex(tuple);
			coverTimes[j][level]++;
			levelForTest.push_back(level);
		}
		keepLevel.push_back(std::move(levelForTest));
	}
	//judge redundancy
	vector<TestCase> res;
	for(int i=0;i<ts.size();i++){
		bool useless=true;
		for(int j=0;j<factorComb.size();j++){
			if(coverTimes[j][keepLevel[i][j]]==1){
				useless=false;
				break;
			}
		}
		if(!useless){
			res.push_back(ts[i]);
		}else{
			//update cover times
			for(int j=0;j<factorComb.size();j++)
				coverTimes[j][keepLevel[i][j]]--;	
		}
	} 
	return res;
}

TestCase  CtSolver::fusion(TestCase& tc1,TestCase& tc2){
	TestCase  res;
	vector<PV> temp;
	bool merge=true;
	for(int i=0;i<factors.size();i++){
		if(tc1.testCase[i].value==-1) temp.push_back(tc2.testCase[i]);
		else if(tc2.testCase[i].value==-1) temp.push_back(tc1.testCase[i]);
		else if(tc2.testCase[i].value==tc1.testCase[i].value) temp.push_back(tc1.testCase[i]);
		else{
			merge=false;
			break;
		}
	}
	if(merge && !checkTestcase(temp).size()){
		res.testCase=temp;
	}
	return res;
	
}

vector<TestCase> CtSolver::optimizeImproved(vector<TestCase>& ts){
	vector<vector<int> > coverTimes;
	for(int i=0;i<coverState.size();i++){
		vector<int > temp(coverState[i].size(),0);
		coverTimes.push_back(std::move(temp));
	}

	//calculate covering times
	for(int i=0;i<ts.size();i++){
		for(int j=0;j<factorComb.size();j++){
			vector<PV> tuple;
			for(int k=0;k<t;k++)
				tuple.push_back(ts[i].testCase[factorComb[j][k].para]);
			int level=getLeIndex(tuple);
			coverTimes[j][level]++;
		}
	}
	//combibation of the jth position
	vector<vector<int> > pointComb;
	for(int i=0;i<factors.size();i++){
		vector<int> temp; 
		pointComb.push_back(temp);
	}
 	for(int i=0;i<factorComb.size();i++){
 		for(int j=0;j<t;j++){
 			pointComb[factorComb[i][j].para].push_back(i);
 		}
	}
	vector<TestCase> old=ts;
	while(true){
		//calculate flexible points
		vector<TestCase> current=old;
		set<int> candidate;
		for(int i=0;i<current.size();i++){
			for(int j=0;j<factors.size();j++){
				bool flag=true;
				for(int k=0;k<pointComb[j].size();k++){
					vector<PV> tuple=factorComb[pointComb[j][k]];
					for(int v=0;v<t;v++)
						tuple[v].value=old[i].testCase[tuple[v].para].value;
					//judge cover times
					if(coverTimes[pointComb[j][k]][getLeIndex(tuple)]==1){
						flag=false;
						break;
					}
				}
				if(flag) {
					current[i].testCase[j].value=-1; 
					candidate.insert(i);
				}
			}
		}

		//judge merge
		vector<int> merge;
		for(auto it=candidate.begin();it!=candidate.end();it++) 
			merge.push_back(*it);
		//judge useless
		bool useless=false;
		for(int i=0;i<merge.size();i++){
			bool flag=true;
			for(int j=0;j<factors.size();j++){
				if(current[merge[i]].testCase[j].value!=-1) flag=false;
			}
			if(flag){
				useless=true;
				//delete and update covertimes
				for(int f=0;f<factorComb.size();f++){
					vector<PV> tuplei;
					for(int k=0;k<t;k++){
						tuplei.push_back(old[merge[i]].testCase[factorComb[f][k].para]);
					}
					int leveli=getLeIndex(tuplei);
					coverTimes[f][leveli]--;
				}
				auto it=old.begin()+merge[i];
				old.erase(it);
			}
		}
		if(useless) continue;

		//begin merge
		bool merged=false;
		for(int i=0;i<merge.size();i++){
			for(int j=i+1;j<merge.size();j++){
				TestCase s=fusion(current[merge[i]],current[merge[j]]);
				if(s.testCase.size()){
					vector<int> plist;
					//generate a new test
					for(auto it=s.testCase.begin();it!=s.testCase.end();){
						if((*it).value==-1){
							if(old[merge[i]].testCase[(*it).para].value==old[merge[j]].testCase[(*it).para].value){
								(*it).value=old[merge[i]].testCase[(*it).para].value;
								it++;
							}else{
								plist.push_back((*it).para);
								it=s.testCase.erase(it);
							}
						}else it++;
					}
					//assign for flexible points
					for(int k=0;k<plist.size();k++){
						PV bestNext=getBestLevel(s,plist[k],true);
						bestNext.value=bestNext.para;
						bestNext.para=plist[k];
						s.testCase.push_back(bestNext);
						adjust(s.testCase);//sort
					}
					
					//update covertimes
					for(int f=0;f<factorComb.size();f++){
						vector<PV> tuplei,tuplej,tuples;
						for(int k=0;k<t;k++){
							tuplei.push_back(old[merge[i]].testCase[factorComb[f][k].para]);
							tuplej.push_back(old[merge[j]].testCase[factorComb[f][k].para]);
							tuples.push_back(s.testCase[factorComb[f][k].para]);
						}
						int leveli=getLeIndex(tuplei);
						int levelj=getLeIndex(tuplej);
						int levels=getLeIndex(tuples);
						coverTimes[f][leveli]--;
						coverTimes[f][levelj]--;
						coverTimes[f][levels]++;
					}
					//update old 
					old[merge[j]].testCase=s.testCase;
					auto it=old.begin()+merge[i];
					old.erase(it);
					merged=true;
					break;
				}
				
			}
			if(merged) break;
		}
		if(!merged) break;
	}
	return old;
}
