#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <sys/time.h>

using namespace std;

typedef uint64_t UINT64;
typedef uint32_t UINT;

//#define debug


int n,m;	//n=number of items,m=number of weights
int num_constraint, num_item;

void displayInputArray(int *profit, int *weight, int *cap, int n, int m){
	for (int i=0; i<n; i++)
		cout << profit[i] << " ";
	cout << endl;
	for (int i=0; i<m; i++){
		for (int j=0; j<n; j++){
			cout << weight[j*m+i] << " ";
		}
		cout << endl;
	}
	for (int i=0; i<m; i++)
		cout << cap[i] << " ";
	cout << endl;
}

void readInputData(int **profit, int **weight, int **cap, string str1, int &n, int &m){
	
	ifstream inputfile;

	inputfile.open(str1.c_str());

	if (!inputfile){
		cout <<"ERROR: Input file cannot be opened!" << endl;
		exit(EXIT_FAILURE);
	}

	int p;
	inputfile >> n >> m >> p;

	*profit = new int[n];
	*weight = new int[n*m];
	*cap = new int[m];

	for (int i=0; i<n; i++)	
		inputfile >> (*profit)[i];

	for (int i=0; i<m; i++){
		for (int j=0; j<n; j++){
			//weight array structure: array of item weights
			//However, item of array weights is more efficient.
			inputfile >> (*weight)[j*m+i];
		}
		//inputfile >> profit[i];
	}
	
	for (int i=0; i<m; i++)
		inputfile >> (*cap)[i];	
}

bool ifAlesB(int *d, int *cap, int len){
	
	for (int i=0; i<len; i++){
		if (d[i] < cap[i]){
			return true;
		}
	}
	
	return false;
}


void indexToMKP(int *d, int *cap, int m, int index){	
	int residual = index;
	for (int i = 0; i < m; i++)
	{
		d[i] = 0;
		int div = 1;
		//this if statement can be removed if cap[i]>0 is guaranteed
		//if (cap[i] != 0){				
			for (int j=i+1; j<m; j++){
				//if (cap[j] != 0){
					div *= (cap[j] + 1);
				//}
			}
			d[i] = residual / div;
			residual -= (div * d[i]);
		//}
	}
}

int mkpToIndex(int *cap, int *d, int m){
	int index = 0;
	for (int i=0; i<m; i++){
		int resid = 1;
		//problems occur when there is a constraint of value 0
		for (int j=i+1; j<m; j++){
			resid *= (cap[j]+1);	
		}
		index += (d[i] * resid);
	}
	return index;
}


int DPiteration(int m, int n, int *weight, int *profit, int *cap){
	
	int *maxw = new int[m];
	int *d = new int[m];
	int *d_w = new int[m];
	int *lowB = new int[m];
	
	struct timeval tbegin, tend;	

	//MKP is a table consist of all constraints and items. Constraints includes no-constraint; item includes 0 item.
	UINT64 total_weight = 1;
	for (int i=0; i<m; i++){
		maxw[i] = 0;
		d[i] = 0;
		d_w[i] = 0;
		lowB[i] = 0;
		total_weight *= (UINT)(cap[i]+1);
	}
	
	
	vector<int> mkp1(total_weight, 0);
	vector<int> mkp2(total_weight, 0);

//	UINT64 size = (UINT64)(n+1) * (UINT64)total_weight;
/*	
	vector< vector<int> > mkp( (n+1), vector<int>());

	for (int i=0; i<=n; i++){
		for (int j=0; j<total_weight; j+=cap[0]){
			//mkp[i].push_back(0);
			mkp[i].insert(mkp[i].end(), cap[0]+1, 0);
		}
		cout << "i: " << i << ", this plain is initialized." << endl;
	}
	cout << "vector MKP is fully initialized to 0." << endl;
*/	
	gettimeofday(&tbegin, NULL);
	
	for (int k=0; k<n; k++){
		for (int i=0; i<m; i++)
			maxw[i] = 0;
		//initialize the vector maxw[] which is the addition of all the items' weight vectors
		for (int i=k; i<n; i++){
			for (int j=0; j<m; j++){
				maxw[j] += weight[i*m+j];
			}
		}
		//UINT64 mkpIdx = (k+1) * total_weight;	//MKP for 0 item is also stored, need to be skipped.
		int kidx = k+1;	
	
		bool startfromzero = false;
		for (int j=0; j<m; j++){
			lowB[j] = cap[j] - maxw[j];
			if (lowB[j] < 0){
				startfromzero = true;
				break;
			}
			d[j] = lowB[j];
		}
		if (startfromzero){
			for (int j=0; j<m; j++)
				d[j] = lowB[j] = 0;
		}

		//for all possible MKP vector capacities between lowB and cap
		//the index of each weight dimension is also the value of each unique weight
		//The MKP vectors that are between lowB and cap are the vectors that their indecies of each constraint dimension are between lowB[] and cap[]
		UINT64 lowBIdx = mkpToIndex(cap, d, m);
 	 	
		mkp1.swap(mkp2);
		for (UINT64 idx = lowBIdx; idx < total_weight; idx++){
			indexToMKP(d, cap, m, idx);
			
			if ( ifAlesB(d, &weight[k*m], m) ){
				//T_(k)(d) = T_(k-1)(d)
				//mkp[kidx][idx] = mkp[kidx-1][idx];
				mkp2[idx] = mkp1[idx];
			}
			else{
				//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
				//d-w_k
				for (int i=0; i<m; i++){
					d_w[i] = d[i] - weight[k*m+i];
				}
#ifdef debug
				cout << "d:" << endl;	
				cout << "(" << d[0] << ", " << d[1] << ") "<<endl;
				cout << "weight: "<<endl;	
				cout << "(" << weight[k*m] << ", " << weight[k*m+1] << ") "<<endl;
				cout << "d_w:"<<endl;
				cout << "(" << d_w[0] << ", " << d_w[1] << ") "<<endl<<endl;
#endif
				UINT64 idx_dw = mkpToIndex(cap, d_w, m);
				//mkp[kidx][idx] = max(mkp[kidx-1][idx], mkp[kidx-1][idx_dw] + profit[k]);
				mkp2[idx] = max(mkp1[idx], mkp1[idx_dw] + profit[k]);
#ifdef debug
				cout << "(" << idx << ", " << idx_dw << ") ";
#endif
			}
#ifdef debug	
			cout <<endl;
#endif
		}
#ifdef debug		
		cout << "item: " << k << ", lowBIdx: " << lowBIdx << endl;
		for (int i=0; i<cap[0]+1; i++){
			for(int j=0; j<cap[1]+1; j++){
				cout << mkp2[i*(cap[1]+1)+j] << " ";
			}
			cout << endl;
		}
		cout << endl;			
#endif
	}
	
	gettimeofday(&tend, NULL);
	//int maxvalue =mkp[n][total_weight-1];  
	int maxvalue = mkp2[total_weight-1];

	cout << "DP iteration time: " << tbegin.tv_sec - tend.tv_sec << endl;	

	delete[] maxw;
	delete[] d;
	delete[] d_w;
	delete[] lowB;

	return maxvalue;
}


int main(int argc, char *argv[]){
	string str1 = "./Data/Sample_BergerPaper/mkp_";
	string str2 = ".txt";
	string str3 = ".xls";
/*
	if (argc < 2){
		cout << "Not enough parameters have been passed." << endl;
		exit(0);
	}
	else{
		num_item = atoi(argv[1]);
		num_constraint = atoi(argv[2]);
	}
*/	
	ostringstream convert1, convert2;
	convert1 << num_item;
	convert2 << num_constraint;
	str1.append( convert1.str() );
	str1.append("_constraints");
	str1.append( convert2.str() );
	
#ifndef debug	
	str1.assign("/wsu/home/et/et80/et8023/MulDimDP/knapsack/serial/Data/Sample_BergerPaper/mkp_1_100000_0.txt");	
#else
	str1.assign("/wsu/home/et/et80/et8023/MulDimDP/knapsack/serial/Data/test.txt");
#endif
	//input file format: same as the paper author's format (Berger's paper).
	//inputfile >> num_item;

	int *weight, *profit, *cap;

	//load m,n from the input configuration file.
//	m = num_constraint;
//	n = num_item;


//	profit = new int[n];
//	cap = new int[m];
//	weight = new int[n*m];

	//load array weight, profit, cap from the configuration file.
	readInputData(&profit, &weight, &cap, str1, n, m);

	displayInputArray(profit, weight, cap, n, m);

	int maxprofit = DPiteration(m, n, weight, profit, cap);

	cout << "Maximum profit: " << maxprofit << endl;

	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;

	return 0;
}
