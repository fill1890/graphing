#include <malloc.h>

#include "../src/graph.h"
#include "../src/dbg.h"

int main(void)
{
    log_info("Making new graph...");
    Graph *graph = new_graph();

    g_apply_relation(graph, "five", "two");
    g_apply_relation(graph, "five", "zero");
    g_apply_relation(graph, "two", "three");
    g_apply_relation(graph, "three", "one");
    g_apply_relation(graph, "four", "one");
    g_apply_relation(graph, "four", "zero");

    int size = 0;
    char **sorted = g_sorted(graph, &size);

    for(int i = 0; i < size; i++) {
        printf("%s\n", sorted[i]);
    }

    free(sorted);
    g_free(graph);

    return 0;
}
