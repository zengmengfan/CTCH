#include "shared.h"
#include "deduce.h"
#include "ctmodel.h"
int main(int argc,char *argv[]){
	string file=argv[1];
	string strength=argv[2];
	string repeat=argv[3];
	int t=str2num(strength);
	int M=str2num(repeat);
	ifstream ifile;
	ifile.open(file);
    string line;
    getline(ifile, line);
    vector<string> raw=split(line,' ');
    vector<int> model;
    for(int i=0;i<raw.size();i++)
    	model.push_back(str2num(raw[i]));
    //constraints
    vector<vector<PV> > constraints;
    while (getline(ifile, line)) {
        vector<string > temp=split(line,' ');
        vector<PV> tuple;
        for(int i=0;i<temp.size();i++){
        	tuple.push_back(translate(model,str2num(temp[i])));
        }
        constraints.push_back(tuple);
    }
    ifile.close();
    constraints=deduceConstraint(model,constraints);
    CtSolver solver=CtSolver(t,model,constraints,M);
    int begin,end;
    begin=clock();
    vector<TestCase> res=solver.solve();
    res=solver.optimizeImproved(res);
    end=clock();
    printTestSuite(res);
    cout<<"size="<<res.size()<<"\ttime="<<(1000.0*(end-begin)/CLOCKS_PER_SEC)<<"ms"<<endl;
    return 0;
}