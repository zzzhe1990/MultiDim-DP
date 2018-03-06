#include <iostream>
using namespace std;

int n,m;	//n=number of items,m=number of weights

void DPiteration(int m, int n, int *weight, 
		 int *profit, int *cap, int &OPT){
	
	int *maxw = new int[m];
	int *d = new int[m];
	int *lowB = new int[m];

	//initialize OPT for item 0 and maximum weight for each constraint
	for (int c=0; c<m; c++){
		for (int i=0; i<n; i++){
			maxw[c] += weight[i*m+c];
			OPT[i*m+c] = 0;
		}
	}

	
	for (int k=0; k<n; k++){
		for (int j=0; j<m; j++){
			maxw[j] -= weight[k*m+j];
			lowB[j] = max(0, cap[j] - maxw[j]);
		}

		//for all possible MKP vector capacities between lowB and C
		
			
	}

	

	delete[] maxw;
	delete[] d;

}


int main(){
	int *weight, *profit, *cap;
	int *OPT;

	//load m,n from the input configuration file.
	m = 5;
	n = 64;

	weight = new int[n*m];
	profit = new int[n];
	cap = new int[m];
	OPT = new int[n*m];	

	//load array weight, profit, cap from the configuration file.



	DPiteration(m, n, weight, profit, cap, OPT);



	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;
	delete[] OPT;

	return 0;
}
