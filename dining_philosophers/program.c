/*
 *MIT License
 *
 *Copyright (c) 2017 Anant Joshi
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  sem_t *forks[5];
  int status[5];
} args_t;

typedef struct {
  args_t *arg;
  int num;
  char loop;
} args_p;

void print_status(sem_t **forks, int *status) {
  int i;
  int fork_val = 53;
  for (i = 0; i < 5; i++) {
    if (status[i] == 0) {
      printf("    sleeping    ");
    } else {
      printf("     eating     ");
    }
  }
  printf("\n");
}

void print_names() {
  int i;
  for (i = 1; i <= 5; i++) {
    printf("  Philosopher %d ", i);
  }
  printf("\n");
}

void philosopher(sem_t **forks, int *status, int num) {
  if (num % 2 == 0) {
    sem_wait(forks[num]);
    sem_wait(forks[(num + 1) % 5]);
  } else {
    sem_wait(forks[(num + 1) % 5]);
    sem_wait(forks[num]);
  }
  status[num] = 1;
  print_status(forks, status);
  sleep((rand() % 5) + 1);
  sem_post(forks[num]);
  sem_post(forks[(num + 1) % 5]);
  status[num] = 0;
  print_status(forks, status);
}

void *pre_philosopher(void *arguments) {
  int num;
  int *status;
  args_p *argue = (args_p *)arguments;
  args_t *args = argue->arg;
  num = argue->num;
  while (argue->loop) {
    philosopher(args->forks, args->status, num);
    sleep((rand() % 5) + 1);
  }
}

void initialize(args_t **args) {
  int i;
  args_t *arg = *args;
  for (i = 0; i < 5; i++) {
    arg->forks[i] = malloc(sizeof(sem_t));
    sem_init(arg->forks[i], 0, 1);
    arg->status[i] = 0;
  }
  *args = arg;
}

int main(void) {
  time_t t;
  int i;
  pthread_t *th[5];
  args_p *arguments[5];
  args_t *args;
  args = malloc(sizeof(args_t));
  srand((unsigned)time(&t));
  printf("Hello\n");
  print_names();
  initialize(&args);
  for (i = 0; i < 5; i++) {
    th[i] = malloc(sizeof(pthread_t));
    arguments[i] = malloc(sizeof(args_p));
    arguments[i]->arg = args;
    arguments[i]->num = i;
    arguments[i]->loop = 1;
  }
  for (i = 0; i < 5; i++) {
    pthread_create(th[i], NULL, pre_philosopher, (void *)arguments[i]);
  }
  sleep(15);
  for(i = 0; i<5; i++){
    sem_destroy(args->forks[i]);
  }
  return 0;
}
