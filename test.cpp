#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

#include <semaphore.h>
#include <pthread.h>

using namespace std;

const int S = 0;
const int P1 = 1;
const int P2 = 2;
const int P3 = 3;

const int NONE = 0;
const int SUGAR = 1;
const int WATER = 2;
const int ESSENCE = 3;

int container;

sem_t sem;

int timer;
char tabs[][6]={"", "\t", "\t\t", "\t\t\t"};

void print(int type, const char *res)
{
	printf("%s%s\n", tabs[type], res);
}

void init()
{
	sem_init(&sem, 0, 1);
	container = NONE;
	puts("S\tP1\tP2\tP3");
}

void *work_P1(void *argv)
{
	print(P1, "START");
	while (1)
	{
		if (container != ESSENCE) continue;
		sem_wait(&sem);
		print(P1, "Psem");
		container = NONE;
		print(P1, "Vsem");
		sem_post(&sem);
		break;
	}
	print(P1, "EXIT");
}

void *work_P2(void *argv)
{
	print(P2, "START");
	while (1)
	{
		if (container != SUGAR) continue;
		sem_wait(&sem);
		print(P2, "Psem");
		container = NONE;
		print(P2, "Vsem");
		sem_post(&sem);
		break;
	}
	print(P2, "EXIT");
}

void *work_P3(void *argv)
{
	print(P3, "START");
	while (1)
	{
		if (container != WATER) continue;
		sem_wait(&sem);
		print(P3, "Psem");
		container = NONE;
		print(P3, "Vsem");
		sem_post(&sem);
		break;
	}
	print(P3, "EXIT");
}

void *work_S(void *argv)
{
	static int cur = NONE;
	print(S, "START");
	while (1)
	{
		if (container != NONE) continue;
		sem_wait(&sem);
		print(S, "Psem");
		container = ++cur;
		print(S, "Vsem");
		sem_post(&sem);
		if (cur>3) break;
	}
	print(S, "EXIT");
}

int main()
{
	init();
	pthread_t tP1, tP2, tP3, tS;
	pthread_create(&tS, NULL, work_S, NULL);
	pthread_create(&tP1, NULL, work_P1, NULL);
	pthread_create(&tP2, NULL, work_P2, NULL);
	pthread_create(&tP3, NULL, work_P3, NULL);
	
	pthread_join(tS, NULL);
	pthread_join(tP1, NULL);
	pthread_join(tP2, NULL);
	pthread_join(tP3, NULL);
	return 0;
}

