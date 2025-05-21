#ifndef IR_H
#define IR_H

typedef struct IRTask {
    char *name;
    char *command;
    char **depends_on;
    int depends_count;
    struct IRTask *next;
} IRTask;

typedef struct IRWorkflow {
    char *name;
    IRTask *tasks;
} IRWorkflow;

#include "ast.h"

IRWorkflow* generate_ir(Workflow *wf);
void print_ir(IRWorkflow *ir);
void free_ir(IRWorkflow *ir);

#endif
