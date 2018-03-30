#include <sys/time.h>
#include "type.h"
#include <iostream>

using namespace std;

__global__ void mainIteration(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		int td = idx;
		int di, test = 1;
		
		for (int i=0; i<m; i++){
			di = idx % dev_cap[i];
			if (di < dev_weight[i]){
				test = 0;
				break;
			}	
			td = td / dev_cap[i];
		}		

		if(test){
			//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
			//d-w_k
			maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
		dev_array2[idx] = maxprofit;
	}
}

__global__ void mainIteration1(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		
		if (idx >= dev_weight[0]){
			//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
			//d-w_k
			maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
		dev_array2[idx] = maxprofit;
	}
}

__global__ void mainIteration2(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		int d0, d1;
		
		d0 = idx % dev_cap[0];
		d1 = idx / dev_cap[0];

		if(d0>=dev_weight[0] && d1>= dev_weight[1]){
			maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
		dev_array2[idx] = maxprofit;
	}
}

__global__ void mainIteration3(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		int d0, d1, d2, d;
		d0 = idx % dev_cap[0];
		d = idx / dev_cap[0];
		d1 = d % dev_cap[1];
		d2 = d / dev_cap[1];
			
		if(d0>=dev_weight[0] && d1>=dev_weight[1] && d2>=dev_weight[2]){
			maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
		dev_array2[idx] = maxprofit;
	}
}

__global__ void mainIteration4(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		int d0, d1, d2, d3;
		int d = idx / dev_cap[0];
		d0 = idx % dev_cap[0];
		d1 = d % dev_cap[1];
		d = d / dev_cap[1];
		d2 = d % dev_cap[2];
		d3 = d / dev_cap[2];

		if(d0>=dev_weight[0] && d1>=dev_weight[1] && d2>=dev_weight[2] && d3>=dev_weight[3]){
			maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
		dev_array2[idx] = maxprofit;
	}
}
/*
__global__ void mainIteration(int m, UINT64 total_weight, int blockSize, int gridSize, 
			      int idxOffset, int profit, int *dev_array1, int *dev_array2,
			      int *dev_cap, int *dev_weight, int item){
	int thread = blockIdx.x * blockDim.x + threadIdx.x;
	//for all possible MKP vector capacities between 0 and cap
	
	for (UINT64 idx = thread; idx < total_weight; idx += (blockSize*gridSize) ){
		
		int maxprofit = dev_array1[idx];
		int td = dev_array1[idx];
		int di, test = 1;
		
		for (int i=0; i<m; i++){
			di = idx % dev_cap[i];
			if (di < dev_weight[item * m + i]){
				test = 0;
				break;
			}	
			td = td / dev_cap[i];
		}		

		if(test){
			//T_(k)(d) = max(T_(k-1)(d), T_(k-1)(d-w_k)+p_k);
			//d-w_k
				maxprofit = max(maxprofit, dev_array1[idx - idxOffset] + profit);
		}
	}
}
*/

int MKPoffset(int *weight, int *cap, int m){
	int offset = 0;
	int ww = 1;
	for (int i=0; i<m; i++){
		offset += (weight[i] * ww);
		ww *= cap[i];
	}

	return offset;
}

int DPiteration(int m, int n, int *weight, int *profit, int *cap){
	
	struct timeval tbegin, tend;	
	int maxvalue;
	//MKP is a table consist of all constraints and items. Constraints includes no-constraint; item includes 0 item.
	UINT64 total_weight = 1;
	for (int i=0; i<m; i++){
		total_weight *= (UINT64)(cap[i]+1);
	}
	
	int blockSize, gridSize;
	blockSize = 512;
	gridSize = 16;

	int *dev_array1 = 0, *dev_array2 = 0;
	int *dev_cap = 0, *dev_weight = 0;
	
	cudaMalloc((void**)dev_array1, total_weight*sizeof(int) );
	cudaMalloc((void**)dev_array2, total_weight*sizeof(int) );
	cudaMalloc((void**)dev_cap, m*sizeof(int) );
	cudaMalloc((void**)dev_weight, n*m*sizeof(int) );
	cudaMemset(dev_array1, 0, total_weight*sizeof(int));
	cudaMemset(dev_array2, 0, total_weight*sizeof(int));	
	cudaMemcpy(dev_cap, cap, m * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_weight, weight, n * m * sizeof(int), cudaMemcpyHostToDevice);

	gettimeofday(&tbegin, NULL);
	
	for (int k=0; k<n; k++){
		int idxOffset = MKPoffset(&weight[k*m], cap, m);

		switch(m){	
			case 1: mainIteration1<<<gridSize, blockSize>>>(m, total_weight, blockSize, gridSize, 
						       idxOffset, profit[k], dev_array1, dev_array2, 
						       dev_cap, &dev_weight[k], k);
			break;
			case 2: mainIteration2<<<gridSize, blockSize>>>(m, total_weight, blockSize, gridSize, 
						       idxOffset, profit[k], dev_array1, dev_array2, 
						       dev_cap, &dev_weight[k+k], k);
			break;
			case 3: mainIteration3<<<gridSize, blockSize>>>(m, total_weight, blockSize, gridSize, 
						       idxOffset, profit[k], dev_array1, dev_array2, 
						       dev_cap, &dev_weight[k+k+k], k);
			break;
			case 4: mainIteration4<<<gridSize, blockSize>>>(m, total_weight, blockSize, gridSize, 
						       idxOffset, profit[k], dev_array1, dev_array2, 
						       dev_cap, &dev_weight[k*4], k);
			break;
			default: mainIteration<<<gridSize, blockSize>>>(m, total_weight, blockSize, gridSize, 
						       idxOffset, profit[k], dev_array1, dev_array2, 
						       dev_cap, &dev_weight[k*m], k);
			break;
		}

		cudaDeviceSynchronize();
		int *temp = dev_array1;
		dev_array1 = dev_array2;
		dev_array2 = temp;
	}
	
	gettimeofday(&tend, NULL);

	cout << "DP iteration on GPU run time: " <<(tend.tv_usec - tbegin.tv_usec)/1E6 << " second." << endl;
		
	cudaMemcpy(&maxvalue, &dev_array2[total_weight-1], sizeof(int), cudaMemcpyDeviceToHost);
	
	cudaFree(dev_array1);
	cudaFree(dev_array2);
	cudaFree(dev_cap);
	cudaFree(dev_weight);
	
	return maxvalue;
}


