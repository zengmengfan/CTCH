#include "shared.h"
#include "deduce.h"
vector<vector<vector<int> > > mergeTuple(vector<int> factors,int factor,vector<vector<vector<int > > > &auxiliary,vector<vector<PV> >& origional){
	vector<vector<int> > validCombine;
	vector<vector<int> > validCombineIndex;
	//initialize a temp array which is use for jduge valid 
	vector<int> temp(factors.size(),-1);
	validCombine.push_back(temp);
	validCombineIndex.push_back(vector<int>());
	for(int i=0;i<factors[factor];i++){
		//expand and judge 
		vector<vector<int> > newValidCombine;
		vector<vector<int> > newValidCombineIndex;
		vector<int> tupleList=auxiliary[factor][i];
		for(int j=0;j<tupleList.size();j++){
			vector<PV> tuple=origional[tupleList[j]];
			for(int k=0;k<validCombine.size();k++){
				//judge whether they can combine
				bool flag=true;
				for(int p=0;p<tuple.size();p++){
					if(validCombine[k][tuple[p].para]!=-1&&validCombine[k][tuple[p].para]!=tuple[p].value){
						flag=false;
						break;
					}
				}
				if(flag){
					vector <int> combineAdd=validCombine[k];
					vector <int> indexAdd=validCombineIndex[k];
					//combine
					for(int p=0;p<tuple.size();p++){
						if(tuple[p].para!=factor)
							combineAdd[tuple[p].para]=tuple[p].value;
					}
					newValidCombine.push_back(combineAdd);
					//index
					indexAdd.push_back(tupleList[j]);
					newValidCombineIndex.push_back(indexAdd); 

				}
			}
		}
		validCombine=newValidCombine;
		validCombineIndex=newValidCombineIndex;
	}
	vector<vector<vector<int> > > res;
	res.push_back(validCombine);
	res.push_back(validCombineIndex);
	return res;

}


vector<vector<PV> > deduceConstraint(vector<int >factors,vector<vector<PV> > origional){
	vector<vector<vector<int > > > auxiliary=constraintInvolved(factors,origional);

	//calculate constraint parameters
	vector<bool> CPstatus(factors.size(),false);
	for(int i=0;i<factors.size();i++){
		bool isCP=true;
		for(int j=0;j<factors[i];j++){
			if(auxiliary[i][j].size()==0){
				isCP=false;
				break;
			}
		}
		if(isCP){
			CPstatus[i]=true;
		}
	}
	//calculate tuples that can combine
	
	vector<vector<vector<int> > > CPValid;
	for(int i=0;i<factors.size();i++)
		CPValid.push_back(vector<vector<int> >());
	vector<vector<int> > nextList;
	for(int i=0;i<CPstatus.size();i++){
		if(CPstatus[i]){
			vector<vector<vector<int> > >merge=mergeTuple(factors,i,auxiliary,origional);
			CPValid[i]=merge[1];
			vector<vector<int> > deducedTuples=merge[0];
			for(int j=0;j<deducedTuples.size();j++){
				vector<PV> tuple;
				for(int k=0;k<factors.size();k++){
					if(deducedTuples[j][k]!=-1&&k!=i) {
						PV pv;
						pv.para=k;
						pv.value=deducedTuples[j][k];
						tuple.push_back(pv);
					}
				}
				//judge redundancy
				bool valuable=true;
				for(int k=0;k<origional.size();k++){
					if(isSubTuple(origional[k],tuple)){
						valuable=false;
						break;
					}
					if(isSubTuple(tuple,origional[k])){
						for(int l=0;l<origional[k].size();l++){
							vector<int>& plist=auxiliary[origional[k][l].para][origional[k][l].value];
							for(auto it=plist.begin();it!=plist.end();){
								if((*it)==k){
									plist.erase(it);
								}else it++;
							}
							vector<vector<int> >& cpvalid=CPValid[origional[k][l].para];
							for(auto it=cpvalid.begin();it!=cpvalid.end();){
								if((*it)[origional[k][l].value]==k){
									it=cpvalid.erase(it);
								}else it++;
							}
						}
					}
				}
				if(valuable){
					for(int k=0;k<tuple.size();k++){
						auxiliary[tuple[k].para][tuple[k].value].push_back(origional.size());
						if(CPstatus[tuple[k].para]){
							vector<int > need;
							need.push_back(tuple[k].para);
							need.push_back(tuple[k].value);
							need.push_back(origional.size());
							nextList.push_back(need);
						} 
					}
					origional.push_back(tuple);
				}

			}
		}
	}
	//deduce constraints iteratively

	while(nextList.size()){
		vector<vector<int> > keepList;
		for(int i=0;i<nextList.size();i++){
			for(int j=0;j<CPValid[nextList[i][0]].size();j++){
				vector<int> comb=CPValid[nextList[i][0]][j];
				comb[nextList[i][1]]=nextList[i][2];

				int* merge=new int[factors.size()];
				bool  canMerge=true;
				for(int k=0;k<factors.size();k++)
					merge[k]=-1;
				for(int k=0;k<comb.size();k++){
					for(int c=0;c<origional[comb[k]].size();c++){
						if(merge[origional[comb[k]][c].para]!=-1&&merge[origional[comb[k]][c].para]!=origional[comb[k]][c].value){
							canMerge=false;
							break;
						}else{
							merge[origional[comb[k]][c].para]=origional[comb[k]][c].value;
						}
					}
				}
				if(canMerge){
					vector<PV> tuple;
					for(int k=0;k<factors.size();k++){
						if(merge[k]!=-1&&k!=nextList[i][0]){
							PV pv;
							pv.para=k;
							pv.value=merge[k];
							tuple.push_back(pv);
						}
					}
					bool valuable=true;
					for(int k=0;k<origional.size();k++){
						if(isSubTuple(origional[k],tuple)){
							valuable=false;
							break;
						}
						if(isSubTuple(tuple,origional[k])){
							for(int l=0;l<origional[k].size();l++){
								vector<int>& plist=auxiliary[origional[k][l].para][origional[k][l].value];
								for(auto it=plist.begin();it!=plist.end();){
									if((*it)==k){
										plist.erase(it);
									}else it++;
								}
								vector<vector<int> >& cpvalid=CPValid[origional[k][l].para];
								for(auto it=cpvalid.begin();it!=cpvalid.end();){
									if((*it)[origional[k][l].value]==k){
										it=cpvalid.erase(it);
									}else it++;
								}
							}

						}
					}
					if(valuable){
						for(int k=0;k<tuple.size();k++){
							auxiliary[tuple[k].para][tuple[k].value].push_back(origional.size());
							CPValid[tuple[k].para].push_back(comb);
							if(CPstatus[tuple[k].para]){
								vector<int > need;
								need.push_back(tuple[k].para);
								need.push_back(tuple[k].value);
								need.push_back(origional.size());
								keepList.push_back(need);
							} 
						}
						origional.push_back(tuple);
						cout<<nextList[i][0]<<endl;
						
					}

				}
			}
		}
		nextList=keepList;
	}
	//simplify all the constraints
	vector<vector<PV> >mfts=simplify(origional);

	return mfts;
}
