#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Workflow* create_workflow(const char *name) {
    Workflow *wf = malloc(sizeof(Workflow));
    wf->name = strdup(name);
    wf->tasks = NULL;
    return wf;
}

Task* create_task(const char *name, const char *command) {
    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->command = strdup(command);
    t->depends_on = NULL;
    t->depends_count = 0;
    t->next = NULL;
    return t;
}

void add_task(Workflow *wf, Task *task) {
    if (!wf->tasks) {
        wf->tasks = task;
    } else {
        Task *curr = wf->tasks;
        while (curr->next) curr = curr->next;
        curr->next = task;
    }
}

void free_workflow(Workflow *wf) {
    if (!wf) return;
    free(wf->name);
    Task *t = wf->tasks;
    while (t) {
        Task *next = t->next;
        free(t->name);
        free(t->command);
        for (int i = 0; i < t->depends_count; i++) {
            free(t->depends_on[i]);
        }
        free(t->depends_on);
        free(t);
        t = next;
    }
    free(wf);
}

void print_workflow(const Workflow *wf) {
    printf("Workflow: %s\n", wf->name);
    Task *t = wf->tasks;
    while (t) {
        printf(" Task: %s\n", t->name);
        printf("  Command: %s\n", t->command);
        if (t->depends_count > 0) {
            printf("  Depends on: ");
            for (int i = 0; i < t->depends_count; i++) {
                printf("%s%s", t->depends_on[i], (i == t->depends_count - 1) ? "\n" : ", ");
            }
        }
        t = t->next;
    }
}
