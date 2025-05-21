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
        ;
    
    workflow_block:
        WORKFLOW ':' STRING task_section
        {
            workflow = create_workflow($3);
        }
        ;
    
    task_section:
        TASKS ':' '[' task_list ']'
        ;
    
    task_list:
        task_item
        | task_list ',' task_item
        ;
    
    task_item:
        '{' task_body '}'
        ;
    
    task_body:
        name_field ',' command_field
        {
            current_task = create_task($1, $3);
            add_task(workflow, current_task);
        }
        |
        name_field ',' command_field ',' depends_on_field
        {
            current_task = create_task($1, $3);
            current_task->depends_on = $5.array;
            current_task->depends_count = $5.count;
            add_task(workflow, current_task);
        }
        ;
    
    name_field:
        NAME ':' STRING { $$ = $3; }
        ;
    
    command_field:
        COMMAND ':' STRING { $$ = $3; }
        ;
    
    depends_on_field:
        DEPENDS_ON ':' '[' STRING ']'
        {
            $$.array = malloc(sizeof(char *));
            $$.array[0] = $4;
            $$.count = 1;
        }
    ;

    string_list:
        STRING
        {
            $$.array = malloc(sizeof(char *));
            $$.array[0] = $1;
            $$.count = 1;
        }
        |
        string_list ',' STRING
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
    }
    