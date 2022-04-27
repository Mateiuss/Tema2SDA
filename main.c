#include "structures.h"
#define MAX 256

int main(int argc, char *argv[])
{
    FILE *fp = fopen(argv[1], "r");
    char read_line[MAX], *parser;

    int Q;
    unsigned char C, N;
    fscanf(fp, "%d %hhd\n", &Q, &C);
    N = 2 * C;

    AS pool = InitPool(N);
    AC waitingC = InitC(sizeof(TNod));
    AC runningC = InitC(sizeof(TNod));
    AC finishedC = InitC(sizeof(TNod));
    AC times = InitC(sizeof(TNod));

    int used_ids[32678] = {0};
    int total_time = 0;

    while (fgets(read_line, MAX, fp)) {
        if (strstr(read_line, "add_tasks")) {
            parser = strtok(read_line, " ");
            parser = strtok(NULL, " ");

            int nr_of_tasks = atoi(parser);
            int exec_time = atoi(strtok(NULL, " "));
            unsigned char priority = atoi(strtok(NULL, " "));

            add_tasks(waitingC, nr_of_tasks, exec_time, priority, used_ids, argv[2]);
        } else if (strstr(read_line, "get_task")) {
            parser = strtok(read_line, " ");
            int id = atoi(strtok(NULL, " "));

            get_task(runningC, waitingC, finishedC, id, used_ids, argv[2]);
        } else if (strstr(read_line, "get_thread")) {
            parser = strtok(read_line, " ");
            int id = atoi(strtok(NULL, " "));

            get_thread(pool, runningC, id, argv[2]);
        } else if (strstr(read_line, "print waiting")) {
            print_waiting(waitingC, argv[2]);
        } else if (strstr(read_line, "print running")) {
            print_running(runningC, argv[2]);
        } else if (strstr(read_line, "print finished")) {
            print_finished(finishedC, argv[2]);
        } else if (strstr(read_line, "run")) {
            parser = strtok(read_line, " ");
            int T = atoi(strtok(NULL, " "));

            FILE *fp = fopen(argv[2], "a");
            if (!fp) {
                break;
            }
            fprintf(fp, "Running tasks for %d ms...\n", T);
            run(waitingC, runningC, finishedC, times, pool, T, Q, &total_time, used_ids);

            fclose(fp);
        }
    }

    FreeC(&waitingC);
    FreeC(&runningC);
    FreeC(&finishedC);
    FreeC(&times);
    FreeS(&pool);

    fclose(fp);
    return 0;
}