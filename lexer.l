%{
    #include "parser.h"
    #include <stdlib.h>
    #include <string.h>

    char *copy_string(const char *text);
%}

%%

"workflow" {
    return WORKFLOW;
}
"name" {
    return NAME;
}
"tasks" {
    return TASKS;
}
"command" {
    return COMMAND;
}
"depends_on" {
    return DEPENDS_ON;
}
\"[^\"]+\" {
    char *raw = yytext;
    size_t len = strlen(raw);
    char *unquoted = (char *)malloc(len - 1);  // len - 2 for quotes + 1 for \0
    strncpy(unquoted, raw + 1, len - 2);
    unquoted[len - 2] = '\0';
    yylval.str = unquoted;
    return STRING;
}
[a-zA-Z_][a-zA-Z0-9_-]* {
    yylval.str = copy_string(yytext);
    return ID;
}
[0-9]+ {
    yylval.num = atoi(yytext);
    return NUMBER;
}
[\[\]{},:] {
    return yytext[0];
}
[ \t\r\n]+ {
    // skip whitespace
}
. {
    return yytext[0];
}

%%

char* copy_string(const char* text) {
    size_t len = strlen(text);
    char* result = (char*)malloc(len + 1);
    strncpy(result, text, len);
    result[len] = '\0';
    return result;
}

int yywrap() {
    return 1;
}
