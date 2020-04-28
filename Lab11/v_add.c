#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ARRAY_SIZE 10000000
#define REPEAT     100



//original
void v_add(double* x, double* y, double* z)
{
    #pragma omp parallel shared(z,x,y)
	{
	for(int i=0; i<ARRAY_SIZE; i++)
		z[i] = x[i] + y[i];
	}
}


void v_add_false(double* x, double* y, double* z)
{
  int i, thread_num, thread_tot; 
  #pragma omp parallel shared(z,x,y)
  {
    int nums = omp_get_num_threads();
    int num = omp_get_thread_num();
    for(int i=0; i<ARRAY_SIZE; i+=nums)
      z[i+num] = x[i+num] + y[i+num];
  }
}

void v_add_block(double* x, double* y, double* z)
{
  int nums;
  int block_size;
  #pragma omp parallel shared(z,x,y)
  {
    nums = omp_get_num_threads();
    block_size = ARRAY_SIZE/nums;
    int num = omp_get_thread_num();
    int block_offset = block_size * num;
    for(int i=0; i<block_size; i++)
      z[i+block_offset] = x[i+block_offset] + y[i+block_offset];
  }
  // tails
  int total = block_size * nums;
  for(int i=total; i<ARRAY_SIZE; i++)
    z[i] = x[i] + y[i];
}

void v_add_for(double* x, double* y, double* z)
{
	int num_threads = omp_get_max_threads();	
	int indicators[num_threads];
	for(int i = 0; i < num_threads; i++) indicators[i] = 0;
	omp_set_num_threads(4);

	bool k = true;

  #pragma omp parallel shared(indicators) ruduction(&:k)
  {
  	printf("%d: %d\n", omp_get_thread_num(), k);
  	indicators[omp_get_thread_num()] = 1;
  	int n = 100 * omp_get_thread_num();
  	int m = 0;
    #pragma omp for
    for(int i=0; i<n; i++){
    	m++;
  		indicators[omp_get_thread_num()] = 1;
	}
	printf("%d For %d times, need %d\n", omp_get_thread_num(), m, n);
  }
  for(int i = 0; i < num_threads; i++) printf("%d ", indicators[i]);
}




double* gen_array(int n) {
	double* array = (double*) malloc(n*sizeof(double));
	for(int i=0; i<n; i++)
		array[i] = drand48();
	return array;
}

// Double check if it is correct
int verify(double* x, double* y) {
	double *z_v_add = (double*) malloc(ARRAY_SIZE*sizeof(double));
	double *z_oracle = (double*) malloc(ARRAY_SIZE*sizeof(double));
	v_add(x, y, z_v_add);
	for(int i=0; i<ARRAY_SIZE; i++)
		z_oracle[i] = x[i] + y[i];
	for(int i=0; i<ARRAY_SIZE; i++)
		if(z_oracle[i] != z_v_add[i])
			return 0;	
	return 1;
}


int main() {
	// Generate input vectors and destination vector
	double *x = gen_array(ARRAY_SIZE);
	double *y = gen_array(ARRAY_SIZE);
	double *z = (double*) malloc(ARRAY_SIZE*sizeof(double));

	// Test framework that sweeps the number of threads and times each run
	double start_time, run_time;
	int num_threads = omp_get_max_threads();	
	for(int i=1; i<=num_threads; i++) {
		omp_set_num_threads(num_threads);		
		start_time = omp_get_wtime();
		for(int j=0; j<1; j++)
			// v_add(x,y,z);
			// v_add_false(x,y,z);
			// v_add_block(x,y,z);
            v_add_for(x,y,z);
		run_time = omp_get_wtime() - start_time;

	  // verify that output of v_add is correct
		if(!verify(x,y)) {
			printf("v_add does not match oracle\n");
			return -1;
		}
  	printf(" %d thread(s) took %f seconds\n",i,run_time);
	}
  return 0;
}
