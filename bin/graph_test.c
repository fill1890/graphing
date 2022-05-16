#include "../src/graph.h"
#include "../src/dbg.h"

int main(void)
{
    log_info("Making new graph...");
    Graph *graph = new_graph();

    g_print(graph);

    log_info("adding one > two");
    g_apply_relation(graph, "one", "two");

    log_info("adding one > three");
    g_apply_relation(graph, "one", "three");

    log_info("adding three > two");
    g_apply_relation(graph, "three", "two");
    g_print(graph);

    g_free(graph);

    return 0;
}
