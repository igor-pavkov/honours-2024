// Source: https://stanford-cs242.github.io/f18/lectures/06-2-concurrency.html


#include <stdio.h>
#include <pthreads.h>


typedef struct {
  int cash;
} Bank;

static Bank the_bank;


void deposit(int n) {
  the_bank.cash += n;
}

void withdraw(int n) {
  if (the_bank.cash >= n) {
    the_bank.cash -= n;
  }
}


void* customer(void* args) {
  for (int i = 0; i < 100; ++i) {
    deposit(2);
  }

  for (int i = 0; i < 100; ++i) {
    withdraw(2);
  }

  return NULL;
}


int main() {
  the_bank.cash = 0;

  // Create a thread for 32 customers.
  int N = 32;
  pthread_t tids[N];
  for (int i = 0; i < N; ++i) {
    pthread_create(&tids[i], NULL, &customer, NULL);
  }

  // Wait til each on is done.
  for (int i = 0; i < N; ++i) {
    pthread_join(tids[i], NULL);
  }

  printf("Total: %d\n", the_bank.cash);

  return 0;
}