#include <stdio.h>
#include <stdlib.h>

#include "tasks_queue.h"
#include <pthread.h>

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_cond_2 = PTHREAD_COND_INITIALIZER;

tasks_queue_t *create_tasks_queue(void)
{

    tasks_queue_t *q = (tasks_queue_t *)malloc(sizeof(tasks_queue_t));

    q->task_buffer_size = QUEUE_SIZE;
    q->task_buffer = (task_t **)malloc(sizeof(task_t *) * q->task_buffer_size);

    q->index = 0;

    return q;
}

void free_tasks_queue(tasks_queue_t *q)
{
    /* IMPORTANT: We chose not to free the queues to simplify the
     * termination of the program (and make debugging less complex) */

    /* free(q->task_buffer); */
    /* free(q); */
}

void enqueue_task(tasks_queue_t *q, task_t *t)
{

    pthread_mutex_lock(&queue_mutex);
    while (q->index == q->task_buffer_size)
    {
        pthread_cond_wait(&queue_cond_2, &queue_mutex);
    }
    q->task_buffer[q->index] = t;
    q->index++;
    printf("Task index %d \n",q->index);
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond_1);

    // if (q->index == q->task_buffer_size)
    // {
    //     fprintf(stderr, "ERROR: the queue of tasks is full\n");
    //     exit(EXIT_FAILURE);
    // }

    // q->task_buffer[q->index] = t;
    // q->index++;
}

task_t *dequeue_task(tasks_queue_t *q)
{

    pthread_mutex_lock(&queue_mutex);
    while (q->index == 0)
    {
        pthread_cond_wait(&queue_cond_1, &queue_mutex);
    }
    task_t *t = q->task_buffer[q->index - 1];
    q->index--;
    printf("Task index %d \n",q->index);
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond_2);

    // if(q->index == 0){
    //     return NULL;
    // }

    // task_t *t = q->task_buffer[q->index-1];
    // q->index--;

    return t;
}
