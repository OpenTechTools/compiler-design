%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    void yyerror(const char *s);
    int yylex(void);
    %}
    
    %union {
        int num;
        char* str;
    }
    
    %token <str> STRING ID
    %token <num> NUMBER
    
    %token WORKFLOW NAME TASKS COMMAND DEPENDS_ON
    
    %type <str> name_field command_field depends_on_field
    
    %%
    
    program:
        workflow_block
        ;
    
    workflow_block:
        WORKFLOW ':' STRING task_section
        {
            printf("Parsed workflow: %s\n", $3);
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
            printf("  Task: %s\n", $1);
            printf("  Command: %s\n", $3);
        }
        |
        name_field ',' command_field ',' depends_on_field
        {
            printf("  Task: %s\n", $1);
            printf("  Command: %s\n", $3);
            printf("  Depends On: %s\n", $5);
        }
        ;
    
    name_field:
        NAME ':' STRING { $$ = $3; }
        ;
    
    command_field:
        COMMAND ':' STRING { $$ = $3; }
        ;
    
    depends_on_field:
        DEPENDS_ON ':' '[' STRING ']' { $$ = $4; }
        ;
    
    %%
    
    void yyerror(const char *s) {
        extern int yylineno;
        fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    }
    