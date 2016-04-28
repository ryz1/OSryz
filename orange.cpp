#include <cstdio>
#include <semaphore.h>
#include <pthread.h>

sem_t sem;
int now;

void init() {
  sem_init(&sem,0,1);
  now = 0;
  puts("P1\tP2\tP3\tS");
}

void *main_P1(void *argv) {
  printf("START\n");
  while (1) {
    if (now != 3) continue;
    sem_wait(&sem);
    printf("Psem\n");
    now = 0;
    printf("Vsem\n");
    sem_post(&sem);
    break;
  }
  printf("EXIT\n");
}

void *main_P2(void *argv) {
  printf("\tSTART\n");
  while (1) {
    if (now != 1) continue;
    sem_wait(&sem);
    printf("\tPsem\n");
    now = 0;
    printf("\tVsem\n");
    sem_post(&sem);
    break;
  }
  printf("\tEXIT\n");
}

void *main_P3(void *argv) {
  printf("\t\tsSTART\n");
  while (1) {
    if (now != 2) continue;
    sem_wait(&sem);
    printf("\t\tPsem\n");
    now = 0;
    printf("\t\tVsem\n");
    sem_post(&sem);
    break;
  }
  printf("\t\tEXIT\n");
}

void *main_S(void *argv) {
  int current = 0;
  printf("\t\t\tSTART");
  while (1) {
    if (now != 0) continue;
    sem_wait(&sem);
    printf("\t\tPsem\n");
    now = ++current;
    printf("\t\tVsem\n");
    sem_post(&sem);
    if (current > 3) break;
  }
  printf("\t\t\tEXIT\n");
}

int main() {
  init();
  pthread_t P1,P2,P3,S;
  pthread_create(&S,NULL,main_S,NULL);
  pthread_create(&S,NULL,main_P1,NULL);
  pthread_create(&S,NULL,main_P2,NULL);
  pthread_create(&S,NULL,main_P3,NULL);
  
  pthread_join(S,NULL);
  pthread_join(P1,NULL);
  pthread_join(P2,NULL);
  pthread_join(P3,NULL);
  return 0;
}
