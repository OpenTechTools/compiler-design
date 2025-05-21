#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "executor.h"

typedef struct IRTaskNode {
    IRTask *task;
    int visited;
    struct IRTaskNode *next;
} IRTaskNode;

// Helper to find a task by name
IRTask* find_task(IRWorkflow *ir, const char *name) {
    for (IRTask *t = ir->tasks; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            return t;
        }
    }
    return NULL;
}

// Topological sort + simulate execution
void execute_workflow(IRWorkflow *ir) {
    int total = 0;

    // Count tasks
    for (IRTask *t = ir->tasks; t; t = t->next) total++;

    // Maps task name to in-degree
    int *in_degree = calloc(total, sizeof(int));
    IRTask **task_list = malloc(sizeof(IRTask*) * total);

    int index = 0;
    for (IRTask *t = ir->tasks; t; t = t->next, index++) {
        task_list[index] = t;

        for (int i = 0; i < t->depends_count; i++) {
            IRTask *dep = find_task(ir, t->depends_on[i]);
            if (dep) {
                in_degree[index]++;
            }
        }
    }

    // Execute using topological order
    int executed = 0;
    while (executed < total) {
        int progress = 0;

        for (int i = 0; i < total; i++) {
            IRTask *t = task_list[i];
            if (in_degree[i] == 0) {
                // Simulate execution
                printf("Executing Task: %s\n", t->name);
                printf("  Command: %s\n", t->command);

                in_degree[i] = -1;  // Mark as executed
                executed++;
                progress = 1;

                // Reduce in-degree of tasks depending on this
                for (int j = 0; j < total; j++) {
                    IRTask *other = task_list[j];
                    for (int k = 0; k < other->depends_count; k++) {
                        if (strcmp(other->depends_on[k], t->name) == 0) {
                            in_degree[j]--;
                        }
                    }
                }
            }
        }

        if (!progress) {
            fprintf(stderr, "Cycle detected! Cannot execute workflow.\n");
            break;
        }
    }

    free(in_degree);
    free(task_list);
}
