#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <sys/time.h>

#include "DP.h"
#include "type.h"

using namespace std;


//#define debug


int n,m;	//n=number of items,m=number of weights

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
	//str1.append( convert1.str() );
	//str1.append("_constraints");
	//str1.append( convert2.str() );
	
#ifndef debug	
	str1.assign("/wsu/home/et/et80/et8023/MulDimDP/knapsack/serial/Data/Sample_BergerPaper/mkp_3_4_200_12_600_0.txt");	
#else
	str1.assign("/wsu/home/et/et80/et8023/MulDimDP/knapsack/serial/Data/test.txt");
#endif
	//input file format: same as the paper author's format (Berger's paper).
	//inputfile >> num_item;

	int *weight, *profit, *cap;

	//load array weight, profit, cap from the configuration file.
	readInputData(&profit, &weight, &cap, str1, n, m);

//	displayInputArray(profit, weight, cap, n, m);

	int maxprofit = DPiteration(m, n, weight, profit, cap);

	cout << "Maximum profit: " << maxprofit << endl;

	//free arrays
	delete[] weight;
	delete[] profit;
	delete[] cap;

	return 0;
}
