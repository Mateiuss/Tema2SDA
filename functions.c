#include "structures.h"

TLG new_cell(void *info)
{
    TLG cell = (TLG)malloc(sizeof(TNod));
    if (!cell) {
        return NULL;
    }

    cell->info = info;
    cell->urm = NULL;

    return cell;
}

Thread *new_thread(unsigned char id)
{
    Thread *thread = (Thread*)malloc(sizeof(Thread));
    if (!thread) {
        return NULL;
    }

    thread->id = id;

    return thread;
}

Task *new_task(int exec_time, int priority, int *curr_id)
{
    Task *task = (Task*)malloc(sizeof(Task));
    if (!task) {
        return NULL;
    }

    ++(*curr_id);
    task->id = *curr_id;
    task->exec_time = exec_time;
    task->priority = priority;
    task->thread = NULL;

    return task;
}

AS InitS(size_t dime)
{
    AS stiva = (AS)malloc(sizeof(TStiva));
    if (!stiva) {
        return NULL;
    }

    stiva->dime = dime;
    stiva->vf = NULL;

    return stiva;
}

void Push(AS s, void *el)
{
    if (!s->vf) {
        s->vf = el;
        return;
    }

    ((TLG)el)->urm = s->vf;
    s->vf = el;
}

AS InitPool(unsigned char N)
{
    AS pool = InitS(sizeof(TNod));

    for (unsigned char i = N - 1; ; --i) {
        Thread *thread = new_thread(i);
        TLG cell = new_cell((void*)thread);
        Push(pool, (void*)cell);

        if (i == 0) {
            break;
        }
    }

    return pool;
}

AC InitC(size_t dime)
{
    AC coada = (AC)malloc(sizeof(TCoada));
    if (!coada) {
        return NULL;
    }

    coada->dime = dime;
    coada->ic = coada->sc = NULL;

    return coada;
}

void InsrC(AC coada, void *el)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        return ;
    }

    coada->sc->urm = el;
    coada->sc = el;
}

void add_tasks(AC waitingC, int nr, int exec_time, unsigned char priority, int *curr_id, char *out_file)
{
    FILE *fp = fopen(out_file, "a+");

    for (int i = 0; i < nr; ++i) {
        Task *task = new_task(exec_time, priority, curr_id);
        TLG cell = new_cell((void*)task);
        InsrC(waitingC, cell);

        fprintf(fp, "Task created successfully : ID %d.\n", task->id);
    }
    
    fclose(fp);
}
