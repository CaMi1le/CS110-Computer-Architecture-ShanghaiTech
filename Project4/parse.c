#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

/******************************************************************************
 * String Utilities
 *****************************************************************************/

char* strltrim(char* src, const char* chars)
{
    char ch;
    while((ch = *src) != '\0'){
        char* found = strchr(chars, ch);
        if(found == NULL) break;
        src++;
    }
    return src;
}

char* strrtrim(char* src, const char* chars)
{
    char* rev = src + strlen(src) - 1;
    while(rev >= src){
        char* found = strchr(chars, *rev);
        if (found == NULL) break;
        (*rev) = '\0';
        rev--;
    }
    return src;
}

char* strtrim(char* src, const char* chars)
{
    return strrtrim(strltrim(src, chars), chars);
}

bool strstartswith(const char* str, const char* tar)
{
    if(strlen(str) < strlen(tar)) return false;
    return strncmp(str, tar, strlen(tar)) == 0;
}

bool strendswith(const char* str, const char* tar)
{
    int pos = strlen(str) - strlen(tar);
    if(pos < 0) return false;
    return strcmp(str + pos, tar) == 0;
}

int strjoin(char* dest, char* strv[], int strc, const char* sep)
{
    int i, len = 0;
    if (dest != NULL) {
        dest[0] = '\0';
    }
    for(i = 0; i < strc; i++){
        len += strlen(strv[i]);
        if(dest != NULL) dest = strcat(dest, strv[i]);
        if(i != strc - 1){
            len += strlen(sep);
            if(dest != NULL) dest = strcat(dest, sep);
        }
    }
    return len;
}

/******************************************************************************
 * Path Utilities
 *****************************************************************************/

char* path_cat(char* dest, char* src)
{
    dest = path_ensure_tail_slash(dest);
    src = strltrim(src, "/");
    return strcat(dest, src);
}

char* path_change_directory(char* path, char* parent, char* target)
{
    parent = path_eliminate_tail_slash(parent);
    if(strstartswith(path, parent)){
        target = path_cat(target, path + strlen(parent));
    }
    return target;
}

char* path_eliminate_begin_slash(char* path)
{
    return strltrim(path, "/");
}

char* path_eliminate_tail_slash(char* path)
{
    return strrtrim(path, "/");
}

char* path_ensure_tail_slash(char* path)
{
    if(!strendswith(path, "/")){
        strcat(path, "/");
    }
    return path;
}

bool path_file_exists(const char* path)
{
    FILE* file = fopen(path, "r");
    if(file != NULL){
        fclose(file);
        return true;
    }
    return false;
}

/******************************************************************************
 * Command Utilities
 *****************************************************************************/

void parse_command_line(CommandLine* command_line, char* line)
{
    const char* pipe_delimiters = "|";
    char* token;
    char* commands[MAX_CMDS];
    int i;

    line = strtrim(line, WHITE_CHARS);
    /* Is background command? */
    command_line->bg = strendswith(line, "&");
    /* Eliminate the end &, after it's handled */
    line = strtrim(line, CAT_CONST_STR(WHITE_CHARS, "&"));

    /* Tokenize by vertical line(|) */
    i = 0;
    token = strtok(line, pipe_delimiters);
    while(token != NULL){
        commands[i++] = token;
        token = strtok(NULL, pipe_delimiters);
    }
    /* Now i equals to the count of arguments */
    command_line->cmdc = i;
    command_line->cmdv = malloc(sizeof(Command) * i);
    for(i = 0; i < command_line->cmdc; i++){
        Command* cmd = &command_line->cmdv[i];
        cmd->argc = 0;
        cmd->output = NULL;
        cmd->input = NULL;

        /* Traverse each token */
        token = strtok(commands[i], WHITE_CHARS);
        while(token != NULL){
            if(strcmp(token, ">") == 0){
                /* Write */
                token = strtok(NULL, WHITE_CHARS);
                cmd->output = malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(cmd->output, token);
                cmd->append = false;
            }else if(strcmp(token, ">>") == 0){
                /* Append */
                token = strtok(NULL, WHITE_CHARS);
                cmd->output = malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(cmd->output, token);
                cmd->append = true;
            }else if(strcmp(token, "<") == 0){
                /* Read */
                token = strtok(NULL, WHITE_CHARS);
                cmd->input = malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(cmd->input, token);
                cmd->append = false;
            }else{
                cmd->argv[cmd->argc] = malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(cmd->argv[cmd->argc], token);
                cmd->argc++;
            }
            token = strtok(NULL, WHITE_CHARS);
        }
        /* Ending identifier */
        cmd->argv[cmd->argc] = NULL;
    }
}

void free_command_line(CommandLine* command_line)
{
    int i, j;
    for(i = 0; i < command_line->cmdc; i++){
        Command* cmd = &command_line->cmdv[i];
        if(cmd->input != NULL) free(cmd->input);
        if(cmd->output != NULL) free(cmd->output);
        for(j = 0; j < cmd->argc; j++){
            free(cmd->argv[j]);
        }
    }
    free(command_line->cmdv);
}

int format_command_line(char* dest, CommandLine* command_line, bool bg)
{
    int i, len = 0;
    char** command_strs = malloc(command_line->cmdc * sizeof(char*));
    for(i = 0; i < command_line->cmdc; i++){
        Command* cmd = &command_line->cmdv[i];
        char* cmd_str;
        int cmd_str_len = strjoin(NULL, cmd->argv, cmd->argc, " ");
        if(cmd->input){
            /* 3 for ' < ' */
            cmd_str_len += 3;
            cmd_str_len += strlen(cmd->input);
        }
        if(cmd->output){
            if(cmd->append){
                /* 4 for ' >> ' */
                cmd_str_len += 4;
            }else{
                /* 3 for ' > ' */
                cmd_str_len += 3;
            }
            cmd_str_len += strlen(cmd->output);
        }
        cmd_str = malloc(sizeof(char) * (cmd_str_len + 1));
        strjoin(cmd_str, cmd->argv, cmd->argc, " ");
        if(cmd->input){
            strcat(cmd_str, " < ");
            strcat(cmd_str, cmd->input);
        }
        if(cmd->output){
            if(cmd->append){
                strcat(cmd_str, " >> ");
            }else{
                strcat(cmd_str, " > ");
            }
            strcat(cmd_str, cmd->output);
        }
        command_strs[i] = cmd_str;
    }
    len = strjoin(dest, command_strs, command_line->cmdc, " | ");
    for(i = 0; i < command_line->cmdc; i++){
        free(command_strs[i]);
    }
    free(command_strs);

    /* If require the tailing background flag */
    if(bg && command_line->bg){
        /* 2 for ' &' */
        len += 2;
        if(dest){
            strcat(dest, " &");
        }
    }
    return len;
}
