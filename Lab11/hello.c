#include <stdio.h>
#include <omp.h>

int main() {
  # pragma omp parallel
  {
    int thread_ID = omp_get_thread_num();
    printf("Hello World %d\n", thread_ID);
  }
}
