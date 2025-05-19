#include <stdio.h>
extern int yyparse();
extern FILE *yyin;

int yydebug = 1;  // Enable parser debug output

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input.yaml>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    yyin = file;
    yyparse();
    fclose(file);
    return 0;
}
