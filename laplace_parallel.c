#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "matrix.h"
#include "stdlib.h"



/*
	Idea:
	split the array into two parts and run a thread on each part.
	thread1 will run from row0 to row 512
	and thread 2 will run from row 1024 to row 512

	use barrier at start of each iteration to synchronize start 
	use barrier at the tolerance check to make sure that both threads finish updating their part of the array
	


*/

#define EPSILON   (0.001)
#define N         (1024)
#define M         (1024)
#define MAX_ITER  (100000)
#define RES_STEPS (100)
#define MAX_THREADS (2)


double **matOldValues;
double **matNewValues;
double dResiduum;
int check;		//1 when (t%RES_STEPS) == 0, 0 otherwise
double dResiduum_thread1 = -1; 
double dResiduum_thread2 = -1; 

typedef struct
{

int start;
int increment;
int stopVal;
int whichThread;
int currentResd;

} thread_arg;



pthread_barrier_t barrier;
pthread_barrier_t barrierEndIteration;
pthread_mutex_t lock =  PTHREAD_MUTEX_INITIALIZER;


//calculate largest difference between element at pos(i,j) between  two successive iterations t and t+1
/*
we already calculated matrix[i][j] and we have the old matrix, so we could computer residum while computing laplace -> however we have two threads and they have different values -> use global variables to hold the current residum for each thread and compare maximum at the end of iteration
*/

double calc_residual(double **matNewValues, double **matOldValues) 
{
	double dResiduum=-1;
	double temp;
	for (int i=1; i<N+1; i++) {
		for (int j=1; j<M+1; j++) {
			
			temp=fabs(matNewValues[i][j]-matOldValues[i][j]);
			if (temp > dResiduum) {
				dResiduum=temp;
				
			}
		}
	}
	return dResiduum;
}

void init(double ***matNewValues,  double ***matOldValues) 
{
	mat_alloc(matNewValues, N, M);    
	mat_alloc(matOldValues, N, M);    
	mat_init(*matNewValues, N, M);
	mat_init(*matOldValues, N, M);
}




void* calc_thread(void* arg)
{
	int i ,j,t;
	
	
	thread_arg* my_arg = (thread_arg*) arg; 
	
	i = my_arg->start;
	
	for (t=0; t<MAX_ITER; t++) {
		
		
		
		dResiduum = -1;
		double resd = -1;
		check = 0 ; 
		pthread_barrier_wait(&barrier);	//barrier to synchronize start of current iteration between both threads
		i = my_arg->start;
		
		if(t%RES_STEPS == 0) check = 1;
		
		for (i = my_arg->start ; i != my_arg->stopVal; i+= my_arg->increment) {
			
			for (j=1; j<M+1; j++) {
			
				matNewValues[i][j]= 0.25*(matOldValues[i-1][j] + matOldValues[i+1][j] +  matOldValues[i][j-1] + matOldValues[i][j+1]);
				
				if(check)
				{
				double temp = fabs(matNewValues[i][j] - matOldValues[i][j]);
				if(temp > resd) resd = temp;
				}
			}

		}  
		
		double mx = 0; 
		
		if(t % RES_STEPS == 0)
		{

			if(resd > dResiduum)mx = resd;
			
			
			//barrier to syncrhonize the computation of end condition which requires that both threads finish the current iteration
			pthread_barrier_wait(&barrierEndIteration);	

			
			if(mx <= EPSILON)
			{
				t++;

				return 0;
			
			}
		}

		
		if(my_arg ->whichThread == 1)	//change matrix only once in thread 1(don't change in thread 2 because it would just give you the original configuation)
		{
		double** old = matOldValues;
		matOldValues = matNewValues;
		matNewValues = old;
		}	
	}
	return 0;
}



int main(int argc, char* argv[])
{
	
	

	/* initialize the matrices with 100 on boundary and 0 inside */
	init(&matNewValues, &matOldValues);
	

	//use only two threads
	pthread_t thread1;
	pthread_t thread2;
	
	//barrier to allow two threads to run simultaneously
	pthread_barrier_init(&barrier , NULL , MAX_THREADS);
	pthread_barrier_init(&barrierEndIteration , NULL , MAX_THREADS);
	

	
	thread_arg* th1_arg = malloc(sizeof(thread_arg));
	thread_arg* th2_arg = malloc(sizeof(thread_arg));
	
	//thread 1 will move from first row to row N/2
	th1_arg -> start = 1;
	th1_arg -> stopVal = N/2+1;
	th1_arg->increment = 1;
	th1_arg-> whichThread = 1;
	
	
	//thread 2 will move from row N to row N/2 
	th2_arg ->start = N;
	th2_arg ->stopVal = N/2 -1;
	th2_arg->increment = -1;
	th2_arg -> whichThread = 2;
	
	
	
	
	if( pthread_create(&thread1 , NULL , calc_thread , th1_arg) != 0 ) printf("ERROR CREATING THREAD1 \n");
	if( pthread_create(&thread2, NULL , calc_thread , th2_arg) != 0 ) printf("ERROR CREATING THREAD2 \n");

	
	
	if( pthread_join(thread1 , NULL) != 0 ) printf("Error join thread1 \n");
	if( pthread_join(thread2 , NULL) != 0 ) printf("Error join thread2 \n");

	pthread_barrier_destroy(&barrier);
	pthread_barrier_destroy(&barrierEndIteration);
	
	

	
	

	return 0;
}

