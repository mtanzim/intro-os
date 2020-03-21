#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_WRITER_THREADS 5
#define NUM_READER_THREADS 5
#define MAX_ACCESSES 2
#define UPPER 6
#define LOWER 1

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t read_phase = PTHREAD_COND_INITIALIZER;
pthread_cond_t write_phase = PTHREAD_COND_INITIALIZER;
int resource_counter = 0;
int shared_val = 0;

void *writer(void *param);
void *reader(void *param);

int main(int argc, char *argv[])
{

    /* thread identifiers */
    int i, j;
    pthread_t writer_tid[NUM_WRITER_THREADS];
    pthread_t reader_tid[NUM_READER_THREADS];

    /* create/fork threads */
    for (i = 0; i < NUM_WRITER_THREADS; i++)
    {
        if (pthread_create(&writer_tid[i], NULL, writer, NULL) != 0)
        {
            fprintf(stderr, "Unable to create writer thread %d\n", i);
            exit(1);
        }
    }
    for (j = 0; j < NUM_READER_THREADS; j++)
    {
        if (pthread_create(&reader_tid[j], NULL, reader, NULL) != 0)
        {
            fprintf(stderr, "Unable to create reader thread %d\n", j);
            exit(1);
        }
    }

    /* wait/join threads */
    for (i = 0; i < NUM_WRITER_THREADS; i++)
    {
        pthread_join(writer_tid[i], NULL);
    }
    for (j = 0; j < NUM_READER_THREADS; j++)
    {
        pthread_join(reader_tid[j], NULL);
    }

    printf("Parent quiting\n");

    return 0;
}

void *reader(void *param)
{

    for (int reads_per_thread = 0; reads_per_thread < MAX_ACCESSES; reads_per_thread++)
    {

        pthread_mutex_lock(&counter_mutex);
        while (resource_counter == -1)
        {
            pthread_cond_wait(&read_phase, &counter_mutex);
        }
        resource_counter++;
        pthread_mutex_unlock(&counter_mutex);

        // read data phase
        int num = (rand() %
                   (UPPER - LOWER + 1)) +
                  LOWER;
        sleep(num);
        printf("Read data: %d, readers: %d \n", shared_val, resource_counter);
        // end reading

        pthread_mutex_lock(&counter_mutex);
        resource_counter--;
        if (resource_counter == 0)
        {
            // printf("Reader: resource free\n");
            pthread_cond_signal(&write_phase);
        }
        pthread_mutex_unlock(&counter_mutex);
    }
}

void *writer(void *param)
{
    for (int writes_per_thread = 0; writes_per_thread < MAX_ACCESSES; writes_per_thread++)
    {

        pthread_mutex_lock(&counter_mutex);
        while (resource_counter != 0)
        {
            pthread_cond_wait(&write_phase, &counter_mutex);
        }
        resource_counter = -1;
        pthread_mutex_unlock(&counter_mutex);

        // write data phase
        int num = (rand() %
                   (UPPER - LOWER + 1)) +
                  LOWER;
        sleep(num);
        shared_val++;
        printf("Wrote data: %d, readers: %d \n", shared_val, resource_counter);
        // end writing

        pthread_mutex_lock(&counter_mutex);
        resource_counter = 0;
        if (resource_counter == 0)
        {
            // printf("Writer: resource free\n");
            pthread_cond_broadcast(&read_phase);
            pthread_cond_signal(&write_phase);
        }
        pthread_mutex_unlock(&counter_mutex);
    }
}
