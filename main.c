#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/ast.h"
#include "ir.h"
#include "executor.h"


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
