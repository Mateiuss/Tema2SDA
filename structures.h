#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN(a, b) (a) < (b) ? (a) : (b)
#define TASK_ID(el) ((Task*)((TLG)(el))->info)->id
#define TASK_EXEC_TIME(el) ((Task*)((TLG)(el))->info)->exec_time
#define TASK_PRIORITY(el) ((Task*)((TLG)(el))->info)->priority
#define THREAD_ID(el) ((Thread*)((TLG)(el))->info)->id
#define TASK_TID(el) ((Task*)((TLG)(el))->info)->thread->id
#define TASK_THREAD(el) ((Task*)((TLG)(el))->info)->thread
#define MAX_TASKS 32678
#define MAX 256
#pragma once

typedef struct thread {
    unsigned char id;
} Thread;

typedef struct task {
    unsigned short id;
    Thread *thread;
    unsigned char priority;
    int exec_time;
} Task;

typedef struct lista {
    void *info;
    struct lista *urm;
} TNod, *TLG;

typedef struct coada {
    size_t dime;
    TLG ic, sc;
} TCoada, *AC;

typedef struct stiva {
    size_t dime;
    TLG vf;
} TStiva, *AS;

AS InitPool(unsigned char N);
void add_tasks(AC waitingC, int nr, int exec_time, unsigned char priority, int *curr_id, char *out_file);
void InsrOrdonata(AC coada, void *el);
void get_task(AC run, AC wait, AC finish, int id, int *used_ids, char *out_file);
void get_thread(AS pool, AC run, int id, char *out_file);
int print_waiting(AC wait, char *out_file);
int print_running(AC run, char *out_file);
int print_finished(AC run, char *out_file);
int run(AC wait, AC run, AC finish, AC times, AS pool, int T, int Q, int *total_time, int *used_ids);
void finish(AC wait, AC run, AC finish, AC times, AS pool, int Q, int *total_time);
void FreeTask(TLG list);
void FreeTLG(TLG list);
int comp(void *el1, void *el2);
