#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN(a, b) (a) < (b) ? (a) : (b)

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
AC InitC(size_t dime);
void add_tasks(AC waitingC, int nr, int exec_time, unsigned char priority, int *curr_id, char *out_file);
void InsrOrdonata(AC coada, void *el);
void get_task(AC run, AC wait, AC finish, int id, int *used_ids, char *out_file);
void FreeC(AC *coada);
void FreeS(AS *stiva);
void get_thread(AS pool, AC run, int id, char *out_file);
void print_waiting(AC wait, char *out_file);
void print_running(AC run, char *out_file);
void print_finished(AC run, char *out_file);
void run(AC wait, AC run, AC finish, AC times, AS pool, int T, int Q, int *total_time, int *used_ids);
