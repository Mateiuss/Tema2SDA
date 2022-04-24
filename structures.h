#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct thread {
    unsigned char id;
} Thread;

typedef struct task {
    unsigned short id;
    Thread *thread;
    unsigned char priority;
    unsigned int exec_time;
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
