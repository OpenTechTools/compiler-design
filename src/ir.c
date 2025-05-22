#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

IRTask* create_ir_task(Task *t) {
    IRTask *ir = malloc(sizeof(IRTask));
    ir->name = strdup(t->name);
    ir->command = strdup(t->command);
    ir->depends_count = t->depends_count;

    if (t->depends_count > 0) {
        ir->depends_on = malloc(sizeof(char *) * t->depends_count);
        for (int i = 0; i < t->depends_count; ++i) {
            ir->depends_on[i] = strdup(t->depends_on[i]);
        }
    } else {
        ir->depends_on = NULL;
    }

    ir->next = NULL;
    return ir;
}

IRWorkflow* generate_ir(Workflow *wf) {
    IRWorkflow *ir = malloc(sizeof(IRWorkflow));
    ir->name = strdup(wf->name);
    ir->tasks = NULL;
    printf("Generating IR for workflow: %s\n", ir->name);
    IRTask *tail = NULL;
    for (Task *t = wf->tasks; t != NULL; t = t->next) {
        printf("Processing task: %s\n", t->name);
        IRTask *ir_task = create_ir_task(t);
        if (!ir->tasks) {
            ir->tasks = ir_task;
            tail = ir_task;
        } else {
            tail->next = ir_task;
            tail = ir_task;
        }
    }
    return ir;
}

void print_ir(IRWorkflow *ir) {
    printf("IR Workflow: %s\n", ir->name);
    for (IRTask *t = ir->tasks; t; t = t->next) {
        printf("  Task: %s\n", t->name);
        printf("    Command: %s\n", t->command);
        if (t->depends_count > 0) {
            printf("    Depends On: ");
            for (int i = 0; i < t->depends_count; ++i) {
                printf("%s ", t->depends_on[i]);
            }
            printf("\n");
        }
    }
}

void free_ir(IRWorkflow *ir) {
    IRTask *t = ir->tasks;
    while (t) {
        IRTask *next = t->next;
        free(t->name);
        free(t->command);
        for (int i = 0; i < t->depends_count; ++i) {
            free(t->depends_on[i]);
        }
        free(t->depends_on);
        free(t);
        t = next;
    }
    free(ir->name);
    free(ir);
}
