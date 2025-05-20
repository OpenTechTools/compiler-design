#ifndef AST_H
#define AST_H

typedef struct Task {
    char *name;
    char *command;
    char **depends_on;  // array of strings (task names)
    int depends_count;
    struct Task *next;
} Task;

typedef struct Workflow {
    char *name;
    Task *tasks;
} Workflow;

Workflow* create_workflow(const char *name);
Task* create_task(const char *name, const char *command);
void add_task(Workflow *wf, Task *task);
void free_workflow(Workflow *wf);
void print_workflow(const Workflow *wf);

#endif // AST_H
