%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "include/ast.h"

    void yyerror(const char *s);
    int yylex(void);
    
    Workflow *workflow = NULL;
    Task *current_task = NULL;
    char **depends_array = NULL;
    int depends_count = 0;

    void cleanup_parser() {
        if (depends_array) {
            for (int i = 0; i < depends_count; i++) {
                free(depends_array[i]);
            }
            free(depends_array);
            depends_array = NULL;
            depends_count = 0;
        }
    }
%}

%union {
    int num;
    char* str;
    struct {
        char **array;
        int count;
    } str_list;
}

%token <str> STRING ID
%token <num> NUMBER
%token WORKFLOW NAME TASKS COMMAND DEPENDS_ON
%type <str> name_field command_field 
%type <str_list> depends_on_field string_list

%%

program:
    workflow_block
    { cleanup_parser(); }
    ;

workflow_block:
    WORKFLOW ':' STRING
    {
        if (workflow) free_workflow(workflow);
        workflow = create_workflow($3);
        printf("Created workflow: %s\n", $3);
    }
    task_section
    ;

task_section:
    TASKS ':' '[' task_list ']'
    {
        if (!workflow) {
            fprintf(stderr, "Error: Workflow not initialized before task_section\n");
            YYABORT;
        }
    }
    ;

task_list:
    task_item
    { printf("Parsed one task item\n"); }
    | task_list ',' task_item
    { printf("Parsed another task item\n"); }
    ;

task_item:
    '{' task_body '}'
    { printf("Completed task item\n"); }
    ;

task_body:
    name_field ',' command_field
    {
        if (!workflow) {
            fprintf(stderr, "Error: Workflow not initialized in task_body\n");
            free($1);
            free($3);
            YYABORT;
        }
        printf("Creating task: %s\n", $1);
        current_task = create_task($1, $3);
        add_task(workflow, current_task);
        printf("Task added to workflow: %s\n", workflow->name);
    }
    | name_field ',' command_field ',' depends_on_field
    {
        if (!workflow) {
            fprintf(stderr, "Error: Workflow not initialized in task_body\n");
            free($1);
            free($3);
            for (int i = 0; i < $5.count; i++) {
                free($5.array[i]);
            }
            free($5.array);
            YYABORT;
        }
        printf("Creating task with dependencies: %s\n", $1);
        current_task = create_task($1, $3);
        current_task->depends_on = $5.array;
        current_task->depends_count = $5.count;
        add_task(workflow, current_task);
        printf("Task added to workflow: %s\n", workflow->name);
    }
    ;

name_field:
    NAME ':' STRING { $$ = $3; }
    ;

command_field:
    COMMAND ':' STRING { $$ = $3; }
    ;

depends_on_field:
    DEPENDS_ON ':' '[' string_list ']'
    {
        $$ = $4;
    }
    ;

string_list:
    STRING
    {
        $$.array = malloc(sizeof(char *));
        $$.array[0] = $1;
        $$.count = 1;
    }
    | string_list ',' STRING
    {
        $1.array = realloc($1.array, sizeof(char *) * ($1.count + 1));
        $1.array[$1.count] = $3;
        $1.count++;
        $$ = $1;
    }
    ;

%%

void yyerror(const char *s) {
    extern int yylineno;
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    cleanup_parser();
}