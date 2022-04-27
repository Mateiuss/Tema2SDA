#include "structures.h"
#include "headerCoada.h"
#include "headerStiva.h"

// Aloc memorie pentru o celula de tip TLG, celula careia ii atribui informatia
// pasata ca parametru la functie
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

// Aloc memorie pentru un thread, avand ID-ul pasat ca parametru al functiei
Thread *new_thread(unsigned char id)
{
    Thread *thread = (Thread*)malloc(sizeof(Thread));
    if (!thread) {
        return NULL;
    }

    thread->id = id;

    return thread;
}

// Aloc memorie pentru un task, avand timpul de executie si prioritatea
// pasate ca parametru. Determin ID-ul cu ajutorul vectorului used_ids
Task *new_task(int exec_time, int priority, int *used_ids)
{
    Task *task = (Task*)malloc(sizeof(Task));
    if (!task) {
        return NULL;
    }

    int i = 1;
    while (i < MAX_TASKS) {
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

// Functie de eliberare a memoriei pentru o lista de task-uri care 
// pointeaza si catre un thread (pentru coada running)
void FreeTask(TLG list)
{
    TLG aux;
    while (list) {
        aux = list;
        list = list->urm;
        if (((Task*)aux->info)->thread) {
            free(((Task*)aux->info)->thread);
        }
        free(aux->info);
        free(aux);
    }
}

// Functie de eliberare a memoriei pentru o lista 
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

// Functie de alocare a memoriei pentru pool-ul de thread-uri
AS InitPool(unsigned char N)
{
    AS pool = InitS(sizeof(TNod));
    if (!pool) {
        return NULL;
    }

    for (unsigned char i = N - 1; ; --i) {
        Thread *thread = new_thread(i);
        if (!thread) {
            if (!pool->vf) {
                free(pool);
                return NULL;
            }
            FreeTLG(pool->vf);
            free(pool);
            return NULL;
        }
        TLG cell = new_cell((void*)thread);
        if (!cell) {
            if (!pool->vf) {
                free(pool);
                return NULL;
            }
            FreeTLG(pool->vf);
            free(pool);
            return NULL;
        }
        Push(pool, (void*)cell);

        if (i == 0) {
            break;
        }
    }

    return pool;
}

// Functia de comparare utilizata pentru sortarea cozilor waiting si running
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

// Functia de generare si de adaugare a task-urilor in coada waiting
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

// Functia de cautare a unui task dupa ID intr-o coada
int get_task_search(AC coada, int id)
{
    AC aux = InitC(sizeof(TNod));
    void *element;
    int exec_time = -1;
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        if (TASK_ID(element) == id && exec_time == -1) {
            exec_time = TASK_EXEC_TIME(element);
        }
        InsrC(aux, element);
    }

    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);

    return exec_time;
}

// Functia de cautare a unui task dupa ID-ul cerut
void get_task(AC run, AC wait, AC finish, int id, int *used_ids, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    int rez;
    // Cautare in running
    if ((rez = get_task_search(run, id)) != -1) {
        fprintf(fp, "Task %d is running (remaining_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    // Cautare in waiting
    if ((rez = get_task_search(wait, id)) != -1) {
        fprintf(fp, "Task %d is waiting (remaining_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    // Cautare in finished
    if ((rez = get_task_search(finish, id)) != -1) {
        fprintf(fp, "Task %d is finished (executed_time = %d).\n", id, rez);
        fclose(fp);
        return;
    }

    // Daca task-ul nu a fost gasit in niciuna dintre cozi, atunci se afiseaza
    // mesajul de mai jos
    fprintf(fp, "Task %d not found.\n", id);

    fclose(fp);
}

// Functie de cautare a thread-ului in pool
int get_thread_search_stack(AS pool, int id)
{
    AS aux = InitS(sizeof(TNod));
    void *element;
    int found = 0;

    while (!VidaS(pool)) {
        element = Pop(pool);
        if (THREAD_ID(element) == id) {
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

// Functie de cautare a thread-ului in coada running
void get_thread_search_queue(AC coada, int id, FILE *fp)
{
    AC aux = InitC(sizeof(TNod));
    void *element;
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        if (TASK_ID(element) == id) {
            fprintf(fp, "Thread %d is running task %i (remaining_time = %d).\n"
            , id, TASK_ID(element), TASK_EXEC_TIME(element));
        }
        InsrC(aux, element);
    }

    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);
}

// Functie de cautare a unui thread in functie de ID
void get_thread(AS pool, AC run, int id, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return;
    }

    // Cautare in pool-ul de thread-uri
    if (get_thread_search_stack(pool, id)) {
        fprintf(fp, "Thread %d is idle.\n", id);
        fclose(fp);
        return;
    }

    // Cautare in coada running
    get_thread_search_queue(run, id, fp);

    fclose(fp);
}

// Functie de afisare a cozii waiting
int print_waiting(AC wait, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return 0;
    }

    fprintf(fp, "====== Waiting queue =======\n[");
    AC aux = InitC(sizeof(TNod));
    if (!aux) {
        fclose(fp);
        return 0;
    }
    void *element;

    int entered = 0;
    while (!VidaC(wait)) {
        element = ExtrC(wait);

        fprintf(fp, "(%d: priority = %d, remaining_time = %d)"
        , TASK_ID(element), TASK_PRIORITY(element), TASK_EXEC_TIME(element));
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
    return 1;
}

// Functie de afisare a cozii running
int print_running(AC run, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return 0;
    }

    fprintf(fp, "====== Running in parallel =======\n[");
    AC aux = InitC(sizeof(TNod));
    if (!aux) {
        fclose(fp);
        return 0;
    }
    void *element;

    int entered = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);

        fprintf(fp, "(%d: priority = %d, remaining_time = %d, running_thread = %d)"
        , TASK_ID(element), TASK_PRIORITY(element), TASK_EXEC_TIME(element), TASK_TID(element));
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
    return 1;
}

// Functie de afisare a cozii running
int print_finished(AC run, char *out_file)
{
    FILE *fp = fopen(out_file, "a");
    if (!fp) {
        printf("Nu s-a putut deschide fisierul pentru scriere!\n");
        return 0;
    }

    fprintf(fp, "====== Finished queue =======\n[");
    AC aux = InitC(sizeof(TNod));
    if (!aux) {
        fclose(fp);
        return 0;
    }
    void *element;

    int entered = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);

        fprintf(fp, "(%d: priority = %d, executed_time = %d)"
        , TASK_ID(element), TASK_PRIORITY(element), TASK_EXEC_TIME(element));
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
    return 1;
}

// Functie care primeste ca parametru o coada(running) si returneaza 
// timpul cel mai mare de executie din aceasta.
int get_max_remaining_time(AC run)
{
    AC aux = InitC(sizeof(TNod));
    if (!aux) {
        return -1;
    }
    void *element;

    int max = 0;
    while (!VidaC(run)) {
        element = ExtrC(run);

        if (TASK_EXEC_TIME(element) > max) {
            max = TASK_EXEC_TIME(element);
        }

        InsrC(aux, element);
    }

    run->ic = aux->ic;
    run->sc = aux->sc;

    free(aux);
    return max;
}

// Functia apelata pentru executarea instructiunii run.
int run(AC wait, AC run, AC finish, AC times, AS pool, int T, int Q, int *total_time, int *used_ids)
{
    // Numarul de unitati de timp de executat pe procesor
    int steps = T / Q;
    steps += (T % Q) ? 1 : 0;
    // Functia executa se opreste atunci cand s-a executat numarul de unitati
    // de timp specificat sau atunci cand nu mai are procese de adaugat sau
    // de executat
    for (int i = 0; i < steps && (!VidaC(wait) || !VidaC(run)); ++i, T -= Q) {
        // Aici sunt introduse procesele in coada running (atat timp cat sunt
        // thread-uri disponibile sau procese suficiente in coada waiting)
        while (!VidaS(pool) && !VidaC(wait)) {
            void *element = Pop(pool);
            void *element2 = ExtrC(wait);
            TASK_THREAD(element2) = ((TLG)element)->info;
            free(element);
            void *info = malloc(sizeof(int));
            if (!info) {
                return 0;
            }
            memcpy(info, &TASK_EXEC_TIME(element2), sizeof(int));
            TLG cell = new_cell(info);
            if (!cell) {
                free(info);
                return 0;
            }
            InsrOrdonataRun(run, times, element2, cell);
        }

        // Aici este calculat timpul de executie pentru pasul curent
        int dec = MIN(T, Q);
        dec = MIN(dec, get_max_remaining_time(run));

        // Maresc timpul total cu timpul cu timpul curent de executie
        *total_time += dec;

        // Initializez cozi auxiliare pentru parcurgere
        AC aux = InitC(sizeof(TNod));
        if (!aux) {
            return 0;
        }
        AC time_aux = InitC(sizeof(TNod));
        if (!time_aux) {
            free(aux);
            return 0;
        }

        // Se executa procesele din coada running, iar daca s-au terminat
        // sunt adaugate in in coada finished.
        while (!VidaC(run)) {
            void *element = ExtrC(run);
            void *time_element = ExtrC(times);
            TASK_EXEC_TIME(element) -= dec;
            if (TASK_EXEC_TIME(element) <= 0) {
                // Eliberez ID-ul procesului abia terminat
                used_ids[TASK_ID(element)] = 0;

                // Aloc o celula pentru thread-ul acum eliberat
                TLG cell = new_cell(TASK_THREAD(element));
                if (!cell) {
                    free(TASK_THREAD(element));
                    free(aux);
                    free(time_aux);
                    free(((TLG)time_element)->info);
                    free(time_element);
                    free(((TLG)element)->info);
                    free(element);
                    return 0;
                }
                TASK_THREAD(element) = NULL;

                // Adaug inapoi in pool thread-ul
                Push(pool, cell);
                // Pun la loc timpul initial de executie al procesului 
                // abia terminat
                memcpy(&TASK_EXEC_TIME(element), ((TLG)(time_element))->info, sizeof(int));
                free(((TLG)time_element)->info);
                free(time_element);
                // Adaug procesul in coada finished
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
        
        // Eliberez memoria alocata pentru cozile auxiliare
        free(aux);
        free(time_aux);
    }
    return 1;
}

// Functia apelata pentru executarea instructiunii finish
void finish(AC wait, AC run, AC finish, AC times, AS pool, int Q, int *total_time)
{
    // Aceasta ruleaza atat timp cat exista task-uri de adaugat si de rulat
    // in coada running
    while (!VidaC(wait) || !VidaC(run)) {
        // Aici sunt introduse procesele in coada running (atat timp cat sunt
        // thread-uri disponibile sau procese suficiente in coada waiting)
        while (!VidaS(pool) && !VidaC(wait)) {
            void *element = Pop(pool);
            void *element2 = ExtrC(wait);
            TASK_THREAD(element2) = ((TLG)element)->info;
            free(element);
            void *info = malloc(sizeof(int));
            memcpy(info, &TASK_EXEC_TIME(element2), sizeof(int));
            TLG cell = new_cell(info);
            if (!cell) {
                free(info);
                return 0;
            }
            InsrOrdonataRun(run, times, element2, cell);
        }

        int dec = get_max_remaining_time(run);
        *total_time += dec;

        AC aux = InitC(sizeof(TNod));
        if (!aux) {
            return 0;
        }
        AC time_aux = InitC(sizeof(TNod));
        if (!time_aux) {
            free(aux);
            return 0;
        }

        // Se executa procesele din coada running, iar daca s-au terminat
        // sunt adaugate in in coada finished.
        while (!VidaC(run)) {
            void *element = ExtrC(run);
            void *time_element = ExtrC(times);
            TASK_EXEC_TIME(element) -= dec;
            if (TASK_EXEC_TIME(element) <= 0) {
                TLG cell = new_cell(TASK_THREAD(element));
                if (!cell) {
                    free(TASK_THREAD(element));
                    free(aux);
                    free(time_aux);
                    free(((TLG)time_element)->info);
                    free(time_element);
                    free(((TLG)element)->info);
                    free(element);
                    return 0;
                }
                TASK_THREAD(element) = NULL;
                Push(pool, cell);
                memcpy(&TASK_EXEC_TIME(element), ((TLG)(time_element))->info, sizeof(int));
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

        // Eliberez memoria alocata
        free(aux);
        free(time_aux);
    }
}
