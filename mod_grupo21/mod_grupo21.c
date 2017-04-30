#include "module.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>

pthread_t thread[PROCESSORS_COUNT];
int cancelled;
int setup = 0;
// pthread_mutex_t mutex1;
// pthread_mutex_t mutex2;
// sem_t *semaphore1;
// sem_t *semaphore2;

#define N 10

sem_t *cheio, *vazio, *lock_prod, *lock_cons; 
oil_t buffer[N], i = 0, f = 0;

static void* worker(void* ignored) {
    while (cancelled == 0) {
        // pthread_mutex_lock(&mutex1);

        sem_wait(lock_prod);
        rock_t rock = pd_read();
        sem_post(lock_prod);
        
        // pthread_mutex_unlock(&mutex1);

        sem_wait(vazio);
        f = (f + 1) % N;
        buffer[f] = pd_process(rock);
        sem_post(cheio);

        // pthread_mutex_lock(&mutex2);
        sem_wait(cheio);
        sem_wait(lock_cons);
        i = (i + 1) % N;
        pd_deliver(buffer[i]); // consumidor
        // pthread_mutex_unlock(&mutex2);
        sem_post(lock_cons);
        sem_post(vazio);
    }
    return NULL;
}

void mod_setup() {
    assert(setup == 0);
    setup = 1;
    cancelled = 0;
    // pthread_mutex_init(&mutex1, NULL);
    // pthread_mutex_init(&mutex2, NULL);
    cheio = sem_open("/cheio", O_CREAT, 0644, 0);
    vazio = sem_open("/vazio", O_CREAT, 0644, N);
    lock_prod = sem_open("/lock_prod", O_CREAT, 0644, 1);
    lock_cons = sem_open("/lock_cons", O_CREAT, 0644, 1);
    for (int i = 0; i < PROCESSORS_COUNT; ++i) {
        pthread_create(&thread[i], NULL, &worker, NULL);
    }
}

char* mod_name() {
    return "grupo21";
}

void mod_shutdown() {
    assert(setup == 1);
    cancelled = 1;
    for (int i = 0; i < PROCESSORS_COUNT; ++i) {
        pthread_join(thread[i], NULL);
    }
    // pthread_mutex_destroy(&mutex1);
    // pthread_mutex_destroy(&mutex2);
    sem_close(cheio);
    sem_close(vazio);
    sem_close(lock_prod);
    sem_close(lock_cons);
}
