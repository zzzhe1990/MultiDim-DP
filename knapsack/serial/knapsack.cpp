#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int n,m;	//n=number of items,m=number of weights
int num_constraint, num_item;

bool ifAleqB(int *d, int *cap, int len){
	int t=0;
	for (int i=0; i<len; i++){
		t += (cap[i] - d[i]);
	}
	
	if (t >= 0)
		return true;
	else
		return false;
}


void indexToMKP(int *mkp, int *cap, int m, int index){	
	int residual = index;
	for (int i = 0; i < m; i++)
	{
		mkp[i] = 0;
		int div = 1;
		if (cap[i] != 0){				
			for (int j=i+1; j<m; j++){
				if (cap[j] != 0){
					div *= (cap[j] + 1);
				}
			}
			mkp[i] = residual / div;
			residual -= (div * mkp[i]);
		}
	}
}

int mkpToIndex(int *cap, int *mkp, int m){
	int index = 0;
	for (int i=0; i<m; i++){
		int resid = 1;
		//problems occur when there is a constraint of value 0
		for (int j=i+1; j<m; j++){
			resid *= (cap[j]+1);	
		}
		index += (mkp[i] * resid);
	}
	return index;
}


int DPiteration(int m, int n, int total_weight, int *weight, 
		 int *profit, int *cap, int *OPT){
	
	int *maxw = new int[m];
	int *d = new int[m];
	int *d_w = new int[m];
	int *lowB = new int[m];

	//how many MKP vector can be obtained in total?
	int *mkp = new int[n*totalweight];

	//initialize the vector maxw[] which is the addition of all the items' weight vectors
	for (int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			maxw[i*m+j] += weight[i*m+j];
		}
	}

	//initialize OPT for empty knapsack(0 item) and maximum weight for each constraint
	for (int i=0; i<m; i++){
		int offset = 0;
		for (int j=0; j<cap[i]; j++){
			OPT[offset+j] = 0;
		}
		offset+=cap[i];
	}

	
	for (int k=0; k<n; k++){
		mkpIdx = k*totalweight;

		bool startfromzero = false;
		for (int j=0; j<m; j++){
			maxw[j] -= weight[k*m+j];
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
		//while ( ifAleqB(d, cap, m) ){
		int lowBIdx = mkpToIndex(cap, lowB, m);
		int maxIdx = mkpToIndex(cap, cap, m);	//maxIdx maybe equal to totalweight
 	 	for (int index = lowBIdx; index <= maxIdx; index++){
			indexToMKP(d, cap, m, index);
			int idx_k_d = mkpIdx + index;
			
			if ( ifAleqB(d, &weight[k*m], m) ){
				//T_(k)(d) = T_(k-1)(d)
				mkp[idx_k_d] = mkp[idx_k_d-totalweight];
			}
			else{
				//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
				//d-w_k
				for (int i=0; i<m; i++){
					d_w[i] = d[i] - weight[k*m+i];
				}
				int idx_dw = mkpToIndex(cap, d_w, m);
				mkp[idx_k_d] = max( mkp[idx_k_d-totalweight], mkp[mkpIdx - totalweight + idx_dw]+ profit[k]);
			}
			//update vector d
		}
			
	}

	int maxvalue = mkp[n*totalweight-1];

	delete[] maxw;
	delete[] d;
	delete[] d_w;
	delete[] lowb;
	delete[] mkp;

	return maxvalue;
}


int main(int argc, char *argv[]){
	string str1 = "./Data/UniformData/item";
	string str2 = ".txt";
	string str3 = ".xls";

	if (argc < 2){
		cout << "Not enough parameters have been passed." << endl;
		exit(0);
	}
	else{
		num_item = atoi();
		num_constraint = atoi();
	}
	
	ostringstream convert1, convert2;
	convert1 << num_item;
	convert2 << num_constraint;
	str1.append( convert1.str() );
	str1.append("_constraints");
	str1.append( convert2.str() );	
	
	ifstream inputfile;

	inputfile.open(str1.c_str());

	if (!inputfile){
		cout <<"ERROR: Input file cannot be opened!" << endl;
		exit(EXIT_FAILURE);
	}
	
	//input file format: first line -- # of item; second line -- constraints cap; rest -- item weights + item profit
	//inputfile >> num_item;

	int *weight, *profit, *cap;
	int *OPT;

	//load m,n from the input configuration file.
	m = num_constranit;
	n = num_item;


	profit = new int[n];
	cap = new int[m];
	weight = new int[n*m];

	//load array weight, profit, cap from the configuration file.
	for (int i=0; i<m; i++)	
		inputfile >> cap[i];

	for (int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			inputfile >> weight[i*m+j];
		}
		inputfile >> profit[i];
	}

	int total_weight = 1;
	for (int i=0; i<m; i++)
		total_weight *= (cap[i] + 1);

	OPT = new int[n*total_weight];

	int maxprofit = DPiteration(m, n, total_weight, weight, profit, cap, OPT);



	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;
	delete[] OPT;

	return 0;
}
