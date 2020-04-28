#ifndef _PARSE_H_
#define _PARSE_H_

#include <string.h>

#define BUF_SIZE    512
#define MAX_CMDS    100
#define MAX_ARGC    100
#define WHITE_CHARS " \f\n\r\t\v"
#define SEP_CHARS   " \f\n\r\t\v,()"

#define CAT_CONST_STR(STR1, STR2)   (STR1 STR2)

typedef int         bool;
#define true        (1)
#define false       (0)

/* This macro are useful to suppress the unsued variable warnings */
#define UNUSED(VAR) (void)(VAR)

/******************************************************************************
 * String Utilities
 *****************************************************************************/

char* strltrim(char* src, const char* chars);
char* strrtrim(char* src, const char* chars);
char* strtrim(char* src, const char* chars);

bool strstartswith(const char* str, const char* tar);
bool strendswith(const char* str, const char* tar);

int strjoin(char* dest, char* strv[], int strc, const char* sep);

/******************************************************************************
 * Path Utilities
 *****************************************************************************/

char* path_cat(char* dest, char* src);
char* path_change_directory(char* path, char* parent, char* target);
char* path_eliminate_begin_slash(char* path);
char* path_eliminate_tail_slash(char* path);
char* path_ensure_tail_slash(char* path);
bool path_file_exists(const char* path);

/******************************************************************************
 * Command Utilities
 *****************************************************************************/

typedef struct
{
    int             argc;
    char*           argv[MAX_ARGC];

    char*           input;
    char*           output;
    bool            append;

} Command;

typedef struct
{
    int             cmdc;
    Command*        cmdv;
    bool            bg;
} CommandLine;

void parse_command_line(CommandLine* command_line, char* line);

void free_command_line(CommandLine* command_line);

int format_command_line(char* dest, CommandLine* command_line, bool bg);

#endif /* _PARSE_H_ */
