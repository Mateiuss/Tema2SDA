#include "structures.h"
#include "headerCoada.h"
#include "headerStiva.h"

int main(int argc, char *argv[])
{
    FILE *fp = fopen(argv[1], "r");
    // Sirul de caractere folosit pentru citirea liniilor din fisier si 
    // pointerul utilizat pentru parsarea instructiunilor
    char read_line[MAX], *parser;

    // Citirea si determinarea pentru variabilele: Q, C, N
    int Q;
    unsigned char C, N;
    fscanf(fp, "%d %hhd\n", &Q, &C);
    N = 2 * C;

    // Initializarea pool-ului, a cozilor de waiting, running si finished
    // si a cozii in care retin timpii de executie a task-urilor inainte de
    // a introduce task-ul in coada running
    AS pool = InitPool(N);
    AC waitingC = InitC(sizeof(TNod));
    AC runningC = InitC(sizeof(TNod));
    AC finishedC = InitC(sizeof(TNod));
    AC times = InitC(sizeof(TNod));

    // Vectorul de frecventa folosit pentru a determina ID-urile libere, 
    // respectiv variabila in care retin timpul total de executie
    int used_ids[MAX_TASKS] = {0};
    int total_time = 0;

    while (fgets(read_line, MAX, fp)) {
        read_line[strcspn(read_line, "\n")] = 0;
        if (strstr(read_line, "add_tasks")) { // add_tasks
            parser = strtok(read_line, " ");
            parser = strtok(NULL, " ");

            int nr_of_tasks = atoi(parser);
            int exec_time = atoi(strtok(NULL, " "));
            unsigned char priority = atoi(strtok(NULL, " "));

            if(!add_tasks(waitingC, nr_of_tasks, exec_time, priority, used_ids, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "get_task")) { // get_task
            parser = strtok(read_line, " ");
            int id = atoi(strtok(NULL, " "));

            if(!get_task(runningC, waitingC, finishedC, id, used_ids, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "get_thread")) { // get_thread
            parser = strtok(read_line, " ");
            int id = atoi(strtok(NULL, " "));

            if(!get_thread(pool, runningC, id, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "print waiting")) { // print waiting
            if (!print_waiting(waitingC, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "print running")) { // print running
            if (!print_running(runningC, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "print finished")) { // print finished
            if (!print_finished(finishedC, argv[2])) {
                break;
            }
        } else if (strstr(read_line, "run")) { // run
            parser = strtok(read_line, " ");
            int T = atoi(strtok(NULL, " "));

            FILE *fout = fopen(argv[2], "a");
            if (!fout) {
                break;
            }

            fprintf(fout, "Running tasks for %d ms...\n", T);
            fclose(fout);

            if (!run(waitingC, runningC, finishedC, times, pool, T, Q, &total_time, used_ids)) {
                break;
            }

            
        } else if (strstr(read_line, "finish")) { // finish
            if (!finish(waitingC, runningC, finishedC, times, pool, Q, &total_time)) {
                break;
            }

            FILE *fout = fopen(argv[2], "a");
            if (!fout) {
                break;
            }

            fprintf(fout, "Total time: %d", total_time);

            fclose(fout);
        }
    }

    // Eliberarea memoriei alocate si inchiderea fisierului deschis pentru 
    // citirea instructiunilor
    FreeC(&waitingC, FreeTLG);
    FreeC(&runningC, FreeTask);
    FreeC(&finishedC, FreeTLG);
    FreeC(&times, FreeTLG);
    FreeS(&pool);
    fclose(fp);

    return 0;
}