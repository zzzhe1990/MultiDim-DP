#include <iostream>
using namespace std;

int n,m;	//n=number of items,m=number of weights

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

void DPiteration(int m, int n, int total_weight, int *weight, 
		 int *profit, int *cap, int OPT){
	
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
			d[j] = lowB[j];
		}

		//for all possible MKP vector capacities between lowB and C
		
		while ( ifAleqB(d, cap, m) ){
			
			if ( ifAleqB(d, &weight[k*m], m) ){
				//T_(k)(d) = T_(k-1)(d)
				OPT[k]
			}
			else{
				
			}
			//update vector d
		}
			
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


	profit = new int[n];
	cap = new int[m];
	weight = new int[n*m];

	//load array weight, profit, cap from the configuration file.
	
	int total_weight = 1;
	for (int i=0; i<m; i++)
		total_weight *= (cap[i] + 1);

	OPT = new int[n*total_weight];

	DPiteration(m, n, total_weight, weight, profit, cap, OPT);



	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;
	delete[] OPT;

	return 0;
}
