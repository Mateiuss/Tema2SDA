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

    int curr_id = 0;

    while (fgets(read_line, MAX, fp)) {
        if (strstr(read_line, "add_tasks")) {
            parser = strtok(read_line, " ");
            parser = strtok(NULL, " ");

            int nr_of_tasks = atoi(parser);
            int exec_time = atoi(strtok(NULL, " "));
            unsigned char priority = atoi(strtok(NULL, " "));

            add_tasks(waitingC, nr_of_tasks, exec_time, priority, &curr_id, argv[2]);
        }
    }


    fclose(fp);
    return 0;
}