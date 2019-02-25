#include "shared.h"
bool comp(const PV& a,const PV& b){
	return a.para<b.para;
}

long long comb(int n,int m){
	if(n<m) return 0;
	if(m>n/2) m=n-m;
	long long value=1;
	for(int i=0;i<m;i++){
		value=value*(n-i)/(1+i);
	}
	return value;
}

/* this function can be used if the above function overflows
long long comb(int n,int m){
	if(n<m) return 0;
	if(m>n/2) m=n-m;
	vector<vector<long long> > v;
	for(int i=0;i<=m;i++){
		vector<long long> temp(n+1,1);
		v.push_back(temp);
	}
	for(int i=1;i<=m;i++)
		for(int j=i+1;j<=n;j++)
			v[i][j]=v[i][j-1]+v[i-1][j-1];
	return v[m][n];	
}
*/

long long combN2K(int n,int m,int k){
	if(n<m) return 0;
	long long value=1;
	long long sum=0;
	if(m>n/2){
		m=n-m;
		for(int i=0;i<m;i++){
			value=value*(n-i)/(1+i);
		}
		for(int i=0;i<k;i++){
			sum+=value;
			value=value*(n-i-m+1)/(n-i);
		}
	}else{
		for(int i=0;i<k;i++){
			sum+=comb(n-i,m);
		}
	}
	return sum;
} 
/* ditto
long long combN2K(int n, int m, int k){
	if(n<m) return 0;
	if(k<=0) return 0;
	vector<vector<long long> > v;
	for(int i=0;i<=m;i++){
		vector<long long> temp(n+1,1);
		v.push_back(temp);
	}
	long long sum=0;
	for(int i=1;i<=m;i++)
		for(int j=i+1;j<=n;j++)
			v[i][j]=v[i][j-1]+v[i-1][j-1];
	for(int j=n;j>n-k;j--)
		sum+=v[m][j];
	return sum;
}
*/
int getPaIndex(vector<int>& v,int n){
	int index=0;
	index+=combN2K(n-1,v.size()-1,v[0]);
	for(int i=1;i<v.size();i++){
		index+=combN2K(n-v[i-1]-2,v.size()-i-1,v[i]-v[i-1]-1);
	}
	return index;
}

int str2num(string s){   
    int num;
    stringstream ss(s);
    ss>>num;
    return num;
}

vector<string> split(string source, char delimiter){
	vector<string > res;
 	int n=source.size();
	if(source[n-1]==delimiter) {
		source=source.substr(0,n-1);
		n=n-1;
	}
	int pre;
	pre=0;
	for(int i=0;i<n;i++){
		if(source[i]==delimiter){
			res.push_back(source.substr(pre,i-pre));
			pre=i+1;
		}
	}
	res.push_back(source.substr(pre,n-pre));
	return res;
}

PV translate(vector<int> model,int raw){
	PV res;
	vector<int > sum(model.size(),0);
	sum[0]=model[0];
	for(int i=1;i<model.size();i++)
		sum[i]=model[i]+sum[i-1];
	for(int i=0;i<model.size();i++){
		if(raw+1<=sum[i]){
			res.para=i;
			res.value=model[i]-(sum[i]-raw);
			break;
		}
		
	}
	return res;
}

vector<int > processInputString(string input){
	vector<int> values;
	vector<int> times;
	int keep=0; 
	for(int i=0;i<input.size();i++){
		if(input[i]==','){
			string temp=input.substr(keep,i);
			keep=i+1;
			values.push_back(str2num(temp));
		}
		if(input[i]==';'){
			string temp=input.substr(keep,i);
			keep=i+1;
			times.push_back(str2num(temp));
		}
	}
	vector<int > res;
	for(int i=0;i<times.size();i++){
		for(int j=0;j<times[i];j++)
			res.push_back(values[i]);
	}
	return res;
}

void adjust(vector<PV>& tuple){
	PV keep=tuple[tuple.size()-1];
	for(int i=0;i<tuple.size();i++){
		if(keep.para<tuple[i].para){
			for(int j=tuple.size()-1;j>i;j--) tuple[j]=tuple[j-1];
			tuple[i]=keep;
			break;
		}
	}
}

bool isSubTuple(vector<PV> tuple1, vector<PV> tuple2){
	bool res=true;
	if(tuple1.size()>tuple2.size()) return false;
	if(tuple1.size()==tuple2.size()){
		for(int i=0;i<tuple1.size();i++){
			if(tuple1[i].para!=tuple2[i].para||(tuple1[i].value!=tuple2[i].value))
				res=false;
		}
		return res;
	}else{
		int p1=0,p2=0;
		while(p1<tuple1.size()&&p2<tuple2.size()){
			if(tuple1[p1].para<tuple2[p2].para||(tuple1[p1].para==tuple2[p2].para&&tuple1[p1].value!=tuple2[p2].value)){
				res=false;
				return res;
			}
			if(tuple1[p1].para==tuple2[p2].para&&tuple1[p1].value==tuple2[p2].value){
				p1++;
				p2++;
			}
			if(tuple1[p1].para>tuple2[p2].para) p2++;
		}
		if(p1!=tuple1.size()) res=false;
		return res;
	}

}

vector<int> findRepeat(vector<vector<PV> > pairs){
	set<int > repeat;
	//find repeated tuples
	for(int i=0;i<pairs.size();i++)
		for(int j=i+1;j<pairs.size();j++){
			vector<PV> small=pairs[i],big=pairs[j];
			int smallIndex=i,bigIndex=j;
			if(small.size()>big.size()) {
				small.swap(big);
				int temp=smallIndex;
				smallIndex=bigIndex;
				bigIndex=temp;
			}
			if(isSubTuple(small,big)) repeat.insert(bigIndex);	
		}
	//return all repeated tuples
	vector<int> res;
	for(auto it=repeat.begin();it!=repeat.end();it++)
		res.push_back(*it);
	return res;
}

vector<vector<PV> > simplify(vector<vector<PV> > origional){
	vector<int > useless=findRepeat(origional);
	
	
	if(useless.size()==0) return origional;
	vector<vector<PV> > mfts;
	int p=0;
	for(int i=0;i<origional.size();i++){
		if(i!=useless[p]) mfts.push_back(origional[i]);
		else if(p<useless.size()-1) p++;
	}
	return mfts;
}

vector<vector<vector<int> > > constraintInvolved(vector<int> factors,vector<vector<PV> > &origional){
	//define a auxiliary table
	vector<vector<vector<int > > > auxiliary;
	for(int i=0;i<factors.size();i++){
		vector<vector<int > > temp;
		auxiliary.push_back(temp);
		for(int j=0;j<factors[i];j++){
			vector<int> list;
			auxiliary[i].push_back(list);
		}
	}
	//initialize the auxiliary table
	for(int i=0;i<origional.size();i++){
		for(int j=0;j<origional[i].size();j++){
			auxiliary[origional[i][j].para][origional[i][j].value].push_back(i);
		}
	}
	return auxiliary;
}

vector<vector<int> > directProduct(vector<vector<int> >& allList){
	vector<vector<int> > old;
	for(int i=0;i<allList[0].size();i++){
		vector<int > temp;
		temp.push_back(allList[0][i]);
		old.push_back(temp);
	}
	for(int i=1;i<allList.size();i++){
		vector<vector<int> > keep;
		for(int j=0;j<allList[i].size();j++)
			for(int k=0;k<old.size();k++){
				vector<int> temp=old[k];
				temp.push_back(allList[i][j]);
				keep.push_back(temp);
			}
		old=keep;
	}
	return old;
}

void printTuples(vector<vector<PV> >& res){
	for(int i=0;i<res.size();i++){
		for(int j=0;j<res[i].size();j++){
			cout<<res[i][j].para<<"."<<res[i][j].value<<";";
		}
		cout<<endl;
	}
}

void printTestSuite(vector<TestCase>& res){
	for(int i=0;i<res.size();i++){
		for(int j=0;j<res[i].testCase.size();j++)
			cout<<(res[i].testCase[j].value)<<",";
		cout<<res[i].fitness<<endl;
	}
}

void printTestcase(TestCase& tc){
	for(int i=0;i<tc.testCase.size();i++)
		cout<<tc.testCase[i].para<<"."<<tc.testCase[i].value<<",";
	cout<<tc.fitness<<endl;
}
