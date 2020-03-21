#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 3 /* Size of shared buffer */
#define MAX_NUM 10
#define NUM_WRITER_THREADS 5
#define NUM_READER_THREADS 20

int buffer[BUF_SIZE]; /* shared buffer */
int add = 0;          /* place to add next element */
int rem = 0;          /* place to remove next element */
int num = 0;          /* number elements in buffer */

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;      /* mutex lock for buffer */
pthread_cond_t c_reader = PTHREAD_COND_INITIALIZER; /* reader waits on this cond var */
pthread_cond_t c_writer = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */

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

/* Produce value(s) */
void *writer(void *param)
{
    while (num < MAX_NUM)
    {

        /* Insert into buffer */
        pthread_mutex_lock(&m);
        /* overflow */
        if (add > BUF_SIZE)
        {
            exit(1);
        }

        /* block if buffer is full */
        while (add == BUF_SIZE)
        {
            add = 0;
            printf("writer filled buffer, waiting\n");
            pthread_cond_wait(&c_writer, &m);
        }
        if (num >= MAX_NUM)
        {
            return 0;
        }

        /* if executing here, buffer not full so add element */
        // sleep(2);
        buffer[add] = num;
        printf("wrote: %d on pos: %d\n", num, add);
        add = add + 1;
        num++;
        fflush(stdout);
        pthread_mutex_unlock(&m);
        pthread_cond_signal(&c_reader);
    }

    printf("writer quiting\n");
    fflush(stdout);
    return 0;
}

/* Consume value(s); Note the reader never terminates */
void *reader(void *param)
{

    int i;
    while (num < MAX_NUM)
    {
        pthread_mutex_lock(&m);
        /* underflow */
        if (rem < 0)
        {
            exit(1);
        }

        /* block if buffer empty */
        while (rem == BUF_SIZE || rem > add)
        {
            printf("reader end of buffer\n");
            rem = 0;
            pthread_cond_wait(&c_reader, &m);
        }

        /* if executing here, buffer not empty so remove element */
        if (num >= MAX_NUM)
        {
            return 0;
        }
        i = buffer[rem];
        printf("read: %d on pos: %d \n", i, rem);
        rem = rem + 1;
        fflush(stdout);
        pthread_mutex_unlock(&m);
        pthread_cond_signal(&c_writer);
    }
    printf("reader quiting\n");
    fflush(stdout);
    return 0;
}
