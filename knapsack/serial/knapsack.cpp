#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

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

void readInputData(int *profit, int *weight, int *cap, string str1){
	
	ifstream inputfile;

	inputfile.open(str1.c_str());

	if (!inputfile){
		cout <<"ERROR: Input file cannot be opened!" << endl;
		exit(EXIT_FAILURE);
	}

	int trash;
	inputfile >> trash >> trash >> trash;

	for (int i=0; i<n; i++)	
		inputfile >> profit[i];

	for (int i=0; i<m; i++){
		for (int j=0; j<n; j++){
			inputfile >> weight[j*m+i];
		}
		//inputfile >> profit[i];
	}
	
	for (int i=0; i<m; i++)
		inputfile >> cap[i];	
}

bool ifAleqB(int *d, int *cap, int len){
	
	for (int i=0; i<len; i++){
		if (cap[i] < d[i]){
			return false;
		}
	}
	
	return true;
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
	int *mkp = new int[n*total_weight];

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
		int mkpIdx = k*total_weight;

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
				mkp[idx_k_d] = mkp[idx_k_d-total_weight];
			}
			else{
				//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
				//d-w_k
				for (int i=0; i<m; i++){
					d_w[i] = d[i] - weight[k*m+i];
				}
				int idx_dw = mkpToIndex(cap, d_w, m);
				mkp[idx_k_d] = max( mkp[idx_k_d-total_weight], mkp[mkpIdx - total_weight + idx_dw]+ profit[k]);
			}
			//update vector d
		}
		cout << "item: " << k << ", mkpIdx: " << mkpIdx << ", lowBIdx: " << lowBIdx << ", maxIdx: " << maxIdx << ", totalweight: " << total_weight << endl;			
	}

	int maxvalue = mkp[n*total_weight-1];

	delete[] maxw;
	delete[] d;
	delete[] d_w;
	delete[] lowB;
	delete[] mkp;

	return maxvalue;
}


int main(int argc, char *argv[]){
	string str1 = "./Data/Sample_BergerPaper/mkp_";
	string str2 = ".txt";
	string str3 = ".xls";

	if (argc < 2){
		cout << "Not enough parameters have been passed." << endl;
		exit(0);
	}
	else{
		num_item = atoi(argv[1]);
		num_constraint = atoi(argv[2]);
	}
	
	ostringstream convert1, convert2;
	convert1 << num_item;
	convert2 << num_constraint;
	str1.append( convert1.str() );
	str1.append("_constraints");
	str1.append( convert2.str() );	
	
	str1.assign("./Data/Sample_BergerPaper/mkp_2_1_600_20_3000_0.txt");	
	
	//input file format: same as the paper author's format (Berger's paper).
	//inputfile >> num_item;

	int *weight, *profit, *cap;
	int *OPT;

	//load m,n from the input configuration file.
	m = num_constraint;
	n = num_item;


	profit = new int[n];
	cap = new int[m];
	weight = new int[n*m];

	//load array weight, profit, cap from the configuration file.
	readInputData(profit, weight, cap, str1);

	displayInputArray(profit, weight, cap, n, m);

	int total_weight = 1;
	for (int i=0; i<m; i++)
		total_weight *= (cap[i] + 1);

	OPT = new int[n*total_weight];

	int maxprofit = DPiteration(m, n, total_weight, weight, profit, cap, OPT);

	cout << "Maximum profit: " << maxprofit << endl;

	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;
	delete[] OPT;

	return 0;
}
