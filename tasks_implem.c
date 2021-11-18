#include <stdio.h>
#include <stdlib.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "debug.h"
#include <pthread.h>
#include "tasks.h"

tasks_queue_t *tqueue = NULL;

void create_queues(void)
{
    tqueue = create_tasks_queue();
}

void delete_queues(void)
{
    free_tasks_queue(tqueue);
}

threads_t *create_thread_pool(void)
{
    threads_t *thread;

    thread = (threads_t *)malloc(sizeof(threads_t));

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (pthread_create(&thread->thread_created[i], NULL, thread_routine, NULL) != 0)
        {
            fprintf(stderr, "Failed to create the using thread\n");
            exit(EXIT_FAILURE);
        }
    }
    return thread;
}

void dispatch_task(task_t *t)
{
    enqueue_task(tqueue, t);
}

task_t *get_task_to_execute(void)
{
    return dequeue_task(tqueue);
}

unsigned int exec_task(task_t *t)
{
    t->step++;
    t->status = RUNNING;

    PRINT_DEBUG(10, "Execution of task %u (step %u)\n", t->task_id, t->step);

    unsigned int result = t->fct(t, t->step);

    return result;
}

void terminate_task(task_t *t)
{
    t->status = TERMINATED;

    PRINT_DEBUG(10, "Task terminated: %u\n", t->task_id);

#ifdef WITH_DEPENDENCIES
    if (t->parent_task != NULL)
    {
        task_t *waiting_task = t->parent_task;
        waiting_task->task_dependency_done++;

        task_check_runnable(waiting_task);
    }
#endif
}

void task_check_runnable(task_t *t)
{
#ifdef WITH_DEPENDENCIES
    if (t->task_dependency_done == t->task_dependency_count)
    {
        t->status = READY;
        dispatch_task(t);
    }
#endif
}
