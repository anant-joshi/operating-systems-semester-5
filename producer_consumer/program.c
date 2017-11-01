#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  sem_t *mutex;
  sem_t *full;
  sem_t *empty;
  int *buffer;
  int bufsize;
  int index;
  int signal;
} s_args;

void wait_for_exit() {
  char c;
  printf("Press any key to continue\n");
  scanf("%c", c);
}

void init_args(s_args *args, int size) {
  args->mutex = malloc(sizeof(sem_t));
  args->full = malloc(sizeof(sem_t));
  args->empty = malloc(sizeof(sem_t));
  sem_init((args->mutex), 0, 1);
  sem_init((args->empty), 0, size);
  sem_init((args->full), 0, 0);
  args->bufsize = size;
  args->buffer = malloc(sizeof(int) * size);
  args->index = 0;
  args->signal = 1;
}

void print_buffer(int *buffer, int bufsize) {
  int i;
  for (i = 0; i < bufsize; i++) {
    printf("%d ", buffer[i]);
  }
  printf("\n");
}

void *producer(void *arguments) {
  int mutex_val, full_val, empty_val;
  mutex_val = 42;
  time_t t;
  srand((unsigned) time(&t));
  s_args *args = (s_args *)arguments;
  while (args->signal) {
    sem_wait(args->empty);
    sem_wait(args->mutex);
    printf("Producer: ");
    args->buffer[args->index] = ((rand()%50)+10);
    args->index++;
    print_buffer(args->buffer, args->bufsize);
    sem_post(args->mutex);
    sem_post(args->full);
    sleep((rand()%7)+1);
  }
}

void *consumer(void *arguments) {
  time_t t;
  srand((unsigned) time(&t));
  s_args *args = (s_args *)arguments;
  while (args->signal) {
    sem_wait(args->full);
    sem_wait(args->mutex);
    printf("Consumer: ");
    args->index--;
    args->buffer[args->index] = 0;
    print_buffer(args->buffer, args->bufsize);
    sem_post(args->mutex);
    sem_post(args->empty);
    sleep((rand()%7)+1);
  }
}

int main(void) {
  s_args *args = malloc(sizeof(s_args));
  int buffer_size;
  pthread_t *prod, *cons;
  prod = malloc(sizeof(pthread_t));
  cons = malloc(sizeof(pthread_t));
  printf("Enter buffer size ");
  scanf("%d", &buffer_size);
  printf("Get Buffer Size\n");
  init_args(args, buffer_size);
  printf("Index: %d\n", args->index);
  printf("Initialize Arguments\n");
  pthread_create(prod, NULL, producer, (void *)args);
  printf("Create Producer Thread\n");
  pthread_create(cons, NULL, consumer, (void *)args);
  printf("Create Consumer Thread\n");
  wait_for_exit();
  pthread_join(*prod, NULL);
  pthread_join(*cons, NULL);
  return 0;
}
