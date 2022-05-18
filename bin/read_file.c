/* DrewDotCo, 18/5/2022
 *
 * Simple program to read a file of values into a graph and sort them
 *
 * Call with read_file <file>
 */
#include <stdlib.h>
#include <stdio.h>

#include "../src/graph.h"
#include "../src/dbg.h"

#define BUFFER_SIZE 255

int main(int argc, char *argv[])
{

    if(argc != 2) {
        fprintf(stderr, "Usage: read_file <file>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if(!file) {
        fprintf(stderr, "Could not open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    Graph *graph = new_graph();
    if(!graph) {
        log_err("Out of memory.");
        fclose(file);
        return EXIT_FAILURE;
    }

    char *buf = malloc(BUFFER_SIZE); 
    char *buf_1 = malloc(BUFFER_SIZE);
    char *buf_2 = malloc(BUFFER_SIZE);
    char relate;

    if(!buf || !buf_1 || !buf_2) {
        log_err("Out of memory.");
        fclose(file);
        free(buf);
        free(buf_1);
        free(buf_2);
        return EXIT_FAILURE;
    }

    while(fgets(buf, BUFFER_SIZE, file)) {
        int err = sscanf(buf, "%s %c %s\n", buf_1, &relate, buf_2);

        if(err == EOF) {
            log_err("Error reading line: %s\n", buf);
            fclose(file);
            free(buf);
            free(buf_1);
            free(buf_2);
            return EXIT_FAILURE;
        }

        switch(relate) {
            case '<':
                g_apply_relation(graph, buf_2, buf_1);
                break;
            case '>':
                g_apply_relation(graph, buf_1, buf_2);
                break;
            default:
                log_warn("'%c' is not a valid comparison in %s\n", relate, buf);
                break;
        }
    }

    free(buf);
    free(buf_1);
    free(buf_2);

    int size = 0;
    char **sorted = g_sorted(graph, &size);

    printf("Done. Sorted list:\n");
    for(int i = 0; i < size; i++) {
        printf("%s, ", sorted[i]);
    }
    printf("\n");

    free(sorted);
    fclose(file);
    g_free(graph);
    return 0;
}
