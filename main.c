//
//  dp_main.c
//  dining_philosophers
//

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_PHILO    5
#define MAX_SLEEP_TIME     2
#define MAX_EATEN          3

int total_iterations = 0;

typedef enum {THINKING, HUNGRY, EATING} STATE;

pthread_t tid[NUM_PHILO];
STATE state[NUM_PHILO];
long thread_id[NUM_PHILO];
int haveEaten[NUM_PHILO];

// condition variables and associated mutex lock
pthread_cond_t      cond_vars[NUM_PHILO];
pthread_mutex_t     mutex_lock;


const char* toString(STATE state, int haveEaten) {
    static char buf[20];
    
    switch(state) {
        case THINKING:  return "thinking";
        case HUNGRY:    return "*Hungry*";
        case EATING:
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "EATING (%d)", haveEaten);
            return buf;
        default:        return "-------- invalid state --------";
    }
}
void showAllPhilosophers() {
    printf("-------------------------------------------------------------------------\n");
    for (long i = 0; i < NUM_PHILO; ++i) { printf("%10s  |  ", toString(state[i], haveEaten[i])); }
    printf("\n");
}
void nap(const char* msg, int time) { /* printf("%s\n", msg);  */   sleep(time); }
void thinking(int thinking_time) { nap("thinking", thinking_time); }
void eating(  int eating_time)   { nap("eating",   eating_time);   }

long onleft(long number)  { return (number + NUM_PHILO - 1) % NUM_PHILO; }
long onright(long number) { return (number + 1) % NUM_PHILO; }

void hungry_chopsticksAvail_eat(long i) {      // eat when I'm hungry and my neighbors aren't eating
    if (state[i] == HUNGRY && state[onleft(i)] != EATING && state[onright(i)] != EATING) {
        ++haveEaten[i];/Users/williammccarthy/dev/CSUF/cs 351/dining_philosophers/dining_philosophers/dp_main.c
        state[i] = EATING;
        showAllPhilosophers();
        pthread_cond_signal(&cond_vars[i]);     // signal thread i it is now eating
    }
}
void wants_to_pickup_chopsticks(long number) {
    pthread_mutex_lock(&mutex_lock);
    state[number] = HUNGRY;
    hungry_chopsticksAvail_eat(number);
    // wait for signal to eat
    while (state[number] != EATING) { pthread_cond_wait(&cond_vars[number], &mutex_lock); }
    pthread_mutex_unlock(&mutex_lock);
}
void wants_to_return_chopsticks(long number) {
    pthread_mutex_lock(&mutex_lock);
    state[number] = THINKING;
    hungry_chopsticksAvail_eat(onleft(number));     // signal HUNGRY left/right neighbors to eat
    hungry_chopsticksAvail_eat(onright(number));
    pthread_mutex_unlock(&mutex_lock);
}
void* philosopher(void* param) {
    long number = *(long *)param;
    int sleep_time;
    int times_through_loop = 0;
    
    srandom((unsigned)time(NULL));
    while (haveEaten[number] < MAX_EATEN) {
        sleep_time = (int)((random() % MAX_SLEEP_TIME) + 1);
        thinking(sleep_time);
        wants_to_pickup_chopsticks(number);
        
        sleep_time = (int)((random() % MAX_SLEEP_TIME) + 1);
        eating(sleep_time);
        
        wants_to_return_chopsticks(number);
        ++times_through_loop;
    }
    return NULL;
}

void init() {               // requires -std=C99 or -std=C11 compile-flag
    for (int i = 0; i < NUM_PHILO; i++) {
        state[i] = THINKING;
        haveEaten[i] = 0;
        thread_id[i] = i;
        pthread_cond_init(&cond_vars[i], NULL);
    }
    pthread_mutex_init(&mutex_lock, NULL);
}

int main(int argc, const char** argv) {
    init();
    printf("=========================================================================\n");
    printf("|                    ====== PHILOSOPHERS ======                         |\n");
    printf("=========================================================================\n");
    printf("   Phil_0   |     Phil_1   |     Phil_2   |     Phil_3   |    Phil_4    |\n");
    showAllPhilosophers();
    for (int i = 0; i < NUM_PHILO; i++) {
        pthread_create(&tid[i], 0, philosopher, (void *)&thread_id[i]);
    }
    for (int i = 0; i < NUM_PHILO; i++) {
        pthread_join(tid[i], NULL);
    }
    
    printf("\n\t\t...done.\n");
    return 0;
}
