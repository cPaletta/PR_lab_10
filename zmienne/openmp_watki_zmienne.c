#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int f_threadprivate =0;
#pragma omp threadprivate(f_threadprivate)

int main(){
  
#ifdef   _OPENMP
  printf("\nKompilator rozpoznaje dyrektywy OpenMP\n");
#endif

  int liczba_watkow;
  
  int a_shared = 1;
  int b_private = 2;
  int c_firstprivate = 3;
  int e_atomic=5;
  
  
  printf("przed wejsciem do obszaru rownoleglego -  nr_threads %d, thread ID %d\n",
	 omp_get_num_threads(), omp_get_thread_num());
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
    

omp_set_num_threads(5);

#pragma omp parallel default(none) shared(a_shared, e_atomic) private(b_private) firstprivate(c_firstprivate )
  {
    int i;
    int d_local_private;
    d_local_private = a_shared + c_firstprivate; //WAR, a jest modyfikowana w 45;

    f_threadprivate = omp_get_thread_num();
    
    #pragma omp barrier

    #pragma omp critical 
    {
      for(int i=0;i<10;i++){  //RAW, zapisujemy w iteracji i, a odczytujemy w i+1
        a_shared=a_shared+1; //WAR, odczytujemy i zapisujemy powiększone
      }                      //WAR w 36
    //WAW
    }

    for(i=0;i<10;i++){
      c_firstprivate += omp_get_thread_num();
    }

    for(i=0;i<10;i++){
      #pragma omp atomic
      e_atomic+=omp_get_thread_num(); //WAR, odczytujemy i zapisujemy powiększone
    }
    #pragma omp barrier    

    #pragma omp critical
    {
      
      printf("\nw obszarze równoległym: aktualna liczba watkow %d, moj ID %d\n",
	     omp_get_num_threads(), omp_get_thread_num());
      
      printf("\ta_shared \t= %d\n", a_shared);  //RAW, a jest modyfikowana w 45 
      printf("\tb_private \t= %d\n", b_private);
      printf("\tc_firstprivate \t= %d\n", c_firstprivate);  
      printf("\td_local_private = %d\n", d_local_private);  
      printf("\te_atomic \t= %d\n", e_atomic);  //RAW, e jest modyfikowane w 56
      
    }
    
    //#pragma omp single
/* #pragma omp master */
/*         { */
    
/*           printf("\ninside single: nr_threads %d, thread ID %d\n", */
/*     	     omp_get_num_threads(), omp_get_thread_num()); */
/*           /\* Get environment information *\/ */
/*           int procs = omp_get_num_procs(); */
/*           int nthreads = omp_get_num_threads(); */
/*           int maxt = omp_get_max_threads(); */
/*           int inpar = omp_in_parallel(); */
/*           int dynamic = omp_get_dynamic(); */
/*           int nested = omp_get_nested(); */
    
/*           /\* Print environment information *\/ */
/*           printf("Number of processors = %d\n", procs); */
/*           printf("Number of threads = %d\n", nthreads); */
/*           printf("Max threads = %d\n", maxt); */
/*           printf("In parallel? = %d\n", inpar); */
/*           printf("Dynamic threads enabled? = %d\n", dynamic); */
/*           printf("Nested parallelism supported? = %d\n", nested); */
    
/*         } */
    
  }


#pragma omp parallel default(none)
{
  printf("\nDrugi obszar równoległy: aktualna liczba watkow %d, moj ID %d, f_threadprivate = %d\n",
         omp_get_num_threads(), omp_get_thread_num(), f_threadprivate); 
}
  
  printf("po zakonczeniu obszaru rownoleglego:\n");
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
  
}
