#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/ast.h"
#include "ir.h"
#include "executor.h"

#define WHITE 0
#define GRAY 1
#define BLACK 2

extern int yyparse();
extern FILE *yyin;
extern Workflow *workflow;

int yydebug = 1;

int check_dependencies(Workflow *wf) {
    Task *t = wf->tasks;
    while (t) {
        for (int i = 0; i < t->depends_count; i++) {
            int found = 0;
            Task *search = wf->tasks;
            while (search) {
                if (strcmp(search->name, t->depends_on[i]) == 0) {
                    found = 1;
                    break;
                }
                search = search->next;
            }
            if (!found) {
                fprintf(stderr, "Semantic error: Task '%s' depends on undefined task '%s'\n", t->name, t->depends_on[i]);
                return 0;
            }
        }
        t = t->next;
    }
    return 1;
}

Task* find_task_by_name(Workflow *wf, const char *name) {
    for (Task *t = wf->tasks; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

int dfs_check(Task *task, Workflow *wf, int *colors, char **task_names, int task_count) {
    int index = -1;
    for (int i = 0; i < task_count; ++i) {
        if (strcmp(task->name, task_names[i]) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) return 0; // Task not found (should not happen)

    if (colors[index] == GRAY) return 1;     // cycle found
    if (colors[index] == BLACK) return 0;    // already visited

    colors[index] = GRAY;

    for (int i = 0; i < task->depends_count; i++) {
        Task *dep = find_task_by_name(wf, task->depends_on[i]);
        if (dep && dfs_check(dep, wf, colors, task_names, task_count)) {
            return 1; // cycle found
        }
    }

    colors[index] = BLACK;
    return 0;
}

int check_cycles(Workflow *wf) {
    int task_count = 0;
    for (Task *t = wf->tasks; t; t = t->next) task_count++;

    int *colors = calloc(task_count, sizeof(int));
    char **task_names = malloc(sizeof(char *) * task_count);

    int idx = 0;
    for (Task *t = wf->tasks; t; t = t->next) {
        task_names[idx++] = t->name;
    }

    for (int i = 0; i < task_count; i++) {
        Task *t = find_task_by_name(wf, task_names[i]);
        if (colors[i] == WHITE && dfs_check(t, wf, colors, task_names, task_count)) {
            fprintf(stderr, "Semantic error: Cycle detected involving task '%s'\n", task_names[i]);
            free(colors);
            free(task_names);
            return 0;
        }
    }

    free(colors);
    free(task_names);
    return 1;
}

int check_duplicate_tasks(Workflow *wf) {
    for (Task *t1 = wf->tasks; t1 != NULL; t1 = t1->next) {
        for (Task *t2 = t1->next; t2 != NULL; t2 = t2->next) {
            if (strcmp(t1->name, t2->name) == 0) {
                fprintf(stderr, "Semantic error: Duplicate task name '%s'\n", t1->name);
                return 0;
            }
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.yaml>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    yyin = file;

    if (yyparse() == 0) {
        if (!workflow) {
            fprintf(stderr, "No workflow parsed.\n");
            fclose(file);
            return 1;
        }

        if (!check_dependencies(workflow)) {
            fclose(file);
            free_workflow(workflow);
            return 1;
        }

        if (!check_cycles(workflow)) {
            fclose(file);
            free_workflow(workflow);
            return 1;
        }

        if (!check_duplicate_tasks(workflow)) {
            fclose(file);
            free_workflow(workflow);
            return 1;
        }

        print_workflow(workflow);
        IRWorkflow *ir = generate_ir(workflow);
        if (!ir) {
            fprintf(stderr, "IR generation failed.\n");
            fclose(file);
            free_workflow(workflow);
            return 1;
        }

        printf("\n--- IR ---\n");
        print_ir(ir);
        printf("\n--- Execution ---\n");
        execute_workflow(ir);

        free_ir(ir);

        free_workflow(workflow);
    } else {
        fprintf(stderr, "Parsing failed.\n");
    }
    
    fclose(file);
    return 0;
}
