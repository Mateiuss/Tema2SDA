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

void FreeTLG(TLG list)
{
    TLG aux;
    while (list) {
        aux = list;
        list = list->urm;
        free(aux->info);
        free(aux);
    }
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

Task *new_task(int exec_time, int priority, int *used_ids)
{
    Task *task = (Task*)malloc(sizeof(Task));
    if (!task) {
        return NULL;
    }

    int i = 1;
    while (i <= 32677) {
        if (!used_ids[i]) {
            used_ids[i] = 1;
            break;
        }
        ++i;
    }
    task->id = i;
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

void FreeS(AS *stiva)
{
    FreeTLG((*stiva)->vf);
    free(*stiva);
    *stiva = NULL;
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

void *Pop(AS s)
{
    if (s->vf) {
        TLG aux = s->vf;
        s->vf = aux->urm;
        aux->urm = NULL;
        return aux;
    }
    return NULL;
}

int VidaS(AS stiva)
{
    if (!stiva->vf) {
        return 1;
    }
    return 0;
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

void FreeC(AC *coada)
{
    FreeTLG((*coada)->ic);
    free(*coada);
    *coada = NULL;
}

int VidaC(AC coada)
{
    if (!coada->ic && !coada->sc) {
        return 1;
    }
    return 0;
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

TLG ExtrC(AC coada)
{
    if (!VidaC(coada)) {
        TLG aux = coada->ic;
        coada->ic = coada->ic->urm;
        aux->urm = NULL;
        if (coada->ic == NULL) {
            coada->sc = NULL;
        }

        return aux;
    }
    return NULL;
}

int comp(void *el1, void *el2)
{
    Task *var1 = ((TLG)el1)->info;
    Task *var2 = ((TLG)el2)->info;

    if (var1->priority < var2->priority) {
        return 1;
    } else if (var1->priority > var2->priority) {
        return 0;
    }

    if (var1->exec_time > var2->exec_time) {
        return 1;
    } else if (var1->exec_time < var2->exec_time) {
        return 0;
    }

    if (var1->id > var2->id) {
        return 1;
    } else if (var1->id < var2->id) {
        return 0;
    }

    return 0;
}

void InsrOrdonata(AC coada, void *el)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        return;
    }
    if (comp(el, coada->sc)) {
        InsrC(coada, el);
        return;
    }
    AC aux = InitC(sizeof(TNod));
    void *element = ExtrC(coada);
    do
    {
        if (!comp(el, element)) {
            break;
        }
        InsrC(aux, element);
        element = ExtrC(coada);
    } while (!VidaC(coada));
    InsrC(aux, el);
    InsrC(aux, element);
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        InsrC(aux, element);
    }
    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);
}

void add_tasks(AC waitingC, int nr, int exec_time, unsigned char priority, int *used_ids, char *out_file)
{
    FILE *fp = fopen(out_file, "a");

    for (int i = 0; i < nr; ++i) {
        Task *task = new_task(exec_time, priority, used_ids);
        TLG cell = new_cell((void*)task);
        InsrOrdonata(waitingC, cell);

        fprintf(fp, "Task created successfully : ID %d.\n", task->id);
    }
    
    fclose(fp);
}

int get_task_search(AC coada, int id)
{
    AC aux = InitC(sizeof(TNod));
    void *element;
    int exec_time = -1;
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        if (((Task*)((TLG)element)->info)->id == id && exec_time == -1) {
            exec_time = ((Task*)((TLG)element)->info)->exec_time;
        }
        InsrC(aux, element);
    }

    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);

    return exec_time;
}

void get_task(AC run, AC wait, AC finish, int id, int *used_ids, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    int rez;
    if ((rez = get_task_search(run, id)) != -1) {
        fprintf(fp, "Task %d is running (remaining_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    if ((rez = get_task_search(wait, id)) != -1) {
        fprintf(fp, "Task %d is waiting (remaining_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    if ((rez = get_task_search(finish, id)) != -1) {
        fprintf(fp, "Task %d is finished (executed_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    fprintf(fp, "Task %d not found.\n", id);

    fclose(fp);
}

int get_thread_search_stack(AS pool, int id)
{
    AS aux = InitS(sizeof(TNod));
    void *element;
    int found = 0;

    while (!VidaS(pool)) {
        element = Pop(pool);
        if (((Thread*)((TLG)element)->info)->id == id) {
            found = 1;
        }
        Push(aux, element);
    }

    while(!VidaS(aux)) {
        element = Pop(aux);
        Push(pool, element);
    }

    FreeS(&aux);

    return found;
}

void get_thread_search_queue(AC coada, int id, FILE *fp)
{
    AC aux = InitC(sizeof(TNod));
    void *element;
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        Task *task = (Task*)((TLG)element)->info;
        if (task->id == id) {
            fprintf(fp, "Thread %d is running task %i (remaining_time = %d).\n", id, task->id, task->exec_time);
        }
        InsrC(aux, element);
    }

    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);
}

void get_thread(AS pool, AC run, int id, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    if (get_thread_search_stack(pool, id)) {
        fprintf(fp, "Thread %d is idle.\n", id);
        fclose(fp);
        return;
    }

    get_thread_search_queue(run, id, fp);

    fclose(fp);
}

void print_waiting(AC wait, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    fprintf(fp, "====== Waiting queue =======\n[");
    AC aux = InitC(sizeof(TNod));
    void *element;

    int entered = 0;
    while (!VidaC(wait)) {
        element = ExtrC(wait);
        Task *task = (Task*)(((TLG)element)->info);

        fprintf(fp, "(%d: priority = %d, remaining_time = %d)", task->id, task->priority, task->exec_time);
        if (!wait->ic) {
            fprintf(fp, "]\n");
        } else {
            fprintf(fp, ",\n");
        }

        InsrC(aux, element);
        entered = 1;
    }

    if (!entered) {
        fprintf(fp, "]\n");
    } 

    wait->ic = aux->ic;
    wait->sc = aux->sc;

    free(aux);

    fclose(fp);
}

void print_running(AC run, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    fprintf(fp, "====== Running in parallel =======\n[");
    AC aux = InitC(sizeof(TNod));
    void *element;

    int entered = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);
        Task *task = (Task*)(((TLG)element)->info);

        fprintf(fp, "(%d: priority = %d, remaining_time = %d, running_thread = %d)", task->id, task->priority, task->exec_time, task->thread->id);
        if (!run->ic) {
            fprintf(fp, "]\n");
        } else {
            fprintf(fp, ",\n");
        }

        InsrC(aux, element);
        entered = 1;
    }

    if (!entered) {
        fprintf(fp, "]\n");
    } 

    run->ic = aux->ic;
    run->sc = aux->sc;

    free(aux);

    fclose(fp);
}

void print_finished(AC run, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    fprintf(fp, "====== Finished queue =======\n[");
    AC aux = InitC(sizeof(TNod));
    void *element;

    int entered = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);
        Task *task = (Task*)(((TLG)element)->info);

        fprintf(fp, "(%d: priority = %d, executed_time = %d)", task->id, task->priority, task->exec_time);
        if (!run->ic) {
            fprintf(fp, "]\n");
        } else {
            fprintf(fp, ",\n");
        }

        InsrC(aux, element);
        entered = 1;
    }

    if (!entered) {
        fprintf(fp, "]\n");
    } 

    run->ic = aux->ic;
    run->sc = aux->sc;

    free(aux);

    fclose(fp);
}

void InsrOrdonataRun(AC coada, AC times, void *el, void *time)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        times->ic = times->sc = time;
        return;
    }
    if (comp(el, coada->sc)) {
        InsrC(coada, el);
        InsrC(times, time);
        return;
    }
    AC aux = InitC(sizeof(TNod));
    AC time_aux = InitC(sizeof(TNod));
    void *element = ExtrC(coada);
    void *time_element = ExtrC(times);
    do
    {
        if (!comp(el, element)) {
            break;
        }
        InsrC(aux, element);
        InsrC(time_aux, time_element);
        element = ExtrC(coada);
        time_element = ExtrC(times);
    } while (!VidaC(coada));
    InsrC(aux, el);
    InsrC(aux, element);
    InsrC(time_aux, time);
    InsrC(time_aux, time_element);
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        time_element = ExtrC(times);
        InsrC(aux, element);
        InsrC(time_aux, time_element);
    }
    coada->ic = aux->ic;
    coada->sc = aux->sc;
    times->ic = time_aux->ic;
    times->sc = time_aux->sc;
    free(aux);
    free(time_aux);
}

int get_max_remaining_time(AC run)
{
    AC aux = InitC(sizeof(TNod));
    void *element;

    int max = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);

        if (((Task*)((TLG)element)->info)->exec_time > max) {
            max = ((Task*)((TLG)element)->info)->exec_time;
        }

        InsrC(aux, element);
    }

    run->ic = aux->ic;
    run->sc = aux->sc;

    free(aux);
    return max;
}

void run(AC wait, AC run, AC finish, AC times, AS pool, int T, int Q, int *total_time, int *used_ids)
{
    int steps = T / Q;
    steps += (T % Q) ? 1 : 0;
    for (int i = 0; i < steps && (!VidaC(wait) || !VidaC(run)); ++i) {
        while (!VidaS(pool) && !VidaC(wait)) {
            void *element = Pop(pool);
            void *element2 = ExtrC(wait);
            ((Task*)((TLG)element2)->info)->thread = ((TLG)element)->info;
            free(element);
            void *info = malloc(sizeof(int));
            memcpy(info, &((Task*)((TLG)(element2))->info)->exec_time, sizeof(int));
            TLG cell = new_cell(info);
            InsrOrdonataRun(run, times, element2, cell);
        }

        int dec = MIN(T, Q);
        dec = MIN(dec, get_max_remaining_time(run));
        *total_time += dec;

        AC aux = InitC(sizeof(TNod));
        AC time_aux = InitC(sizeof(TNod));
        while (!VidaC(run)) {
            void *element = ExtrC(run);
            void *time_element = ExtrC(times);
            Task *task = (Task*)((TLG)(element))->info;
            task->exec_time -= dec;
            if (task->exec_time <= 0) {
                used_ids[task->id] = 0;
                TLG cell = new_cell(task->thread);
                task->thread = NULL;
                Push(pool, cell);
                memcpy(&task->exec_time, ((TLG)(time_element))->info, sizeof(int));
                free(((TLG)time_element)->info);
                free(time_element);
                InsrC(finish, element);
            } else {
                InsrC(aux, element);
                InsrC(time_aux, time_element);
            }
        }
        run->ic = aux->ic;
        run->sc = aux->sc;
        times->ic = time_aux->ic;
        times->sc = time_aux->sc;
        free(aux);
        free(time_aux);

        T -= Q;
    }
}

void finish(AC wait, AC run, AC finish, AC times, AS pool, int Q, int *total_time)
{
    while (!VidaC(wait) || !VidaC(run)) {
        while (!VidaS(pool) && !VidaC(wait)) {
            void *element = Pop(pool);
            void *element2 = ExtrC(wait);
            ((Task*)((TLG)element2)->info)->thread = ((TLG)element)->info;
            free(element);
            void *info = malloc(sizeof(int));
            memcpy(info, &((Task*)((TLG)(element2))->info)->exec_time, sizeof(int));
            TLG cell = new_cell(info);
            InsrOrdonataRun(run, times, element2, cell);
        }

        int dec = get_max_remaining_time(run);
        *total_time += dec;
        AC aux = InitC(sizeof(TNod));
        AC time_aux = InitC(sizeof(TNod));
        while (!VidaC(run)) {
            void *element = ExtrC(run);
            void *time_element = ExtrC(times);
            Task *task = (Task*)((TLG)(element))->info;
            task->exec_time -= dec;
            if (task->exec_time <= 0) {
                TLG cell = new_cell(task->thread);
                task->thread = NULL;
                Push(pool, cell);
                memcpy(&task->exec_time, ((TLG)(time_element))->info, sizeof(int));
                free(((TLG)time_element)->info);
                free(time_element);
                InsrC(finish, element);
            } else {
                InsrC(aux, element);
                InsrC(time_aux, time_element);
            }
        }
        run->ic = aux->ic;
        run->sc = aux->sc;
        times->ic = time_aux->ic;
        times->sc = time_aux->sc;
        free(aux);
        free(time_aux);
    }
}
