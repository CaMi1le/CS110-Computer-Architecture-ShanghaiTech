#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/unistd.h>
#include <fcntl.h>

#include "parse.h"

#define PROGRAM_NAME "shell"


/******************************************************************************
 * Enum of shell mode: interactive/noninteractive
 *****************************************************************************/
typedef enum {
    interactive,
    noninteractive
} ShellMode;


/******************************************************************************
 * Job and job list
 *****************************************************************************/
typedef struct {
    pid_t pid;
    CommandLine* cmd_ln;
    char* wc;
    bool available;
    int job_id;
    int flag;  /* 1 for -, 0 for + */
} Job;

typedef struct {
    Job* data;
    int top;
} JobList;

void init_job_list(JobList* list)
{
    int i;
    list->data = (Job*)malloc(sizeof(Job) * BUF_SIZE);
    list->top = -1;

    for (i = 0; i < BUF_SIZE; i++) {
        (list->data[i]).available = false;
        (list->data[i]).wc = NULL;
        (list->data[i]).cmd_ln = NULL;
        (list->data[i]).pid = -1;
        (list->data[i]).job_id = -1;
        (list->data[i]).flag = -1;
    }
}

void update_job_flag(JobList* list)
{
    int idx = list->top, idx2;

    if (list->top < 0) {
        return;
    }

    list->data[list->top].flag = 0;

    while (idx > 0 && (list->data[--idx]).available == false) {}
    if (idx < list->top) {
        list->data[idx].flag = 1;
    }

    idx2 = idx;
    while (idx2 > 0 && (list->data[--idx2]).available == false) {}
    if (idx2 < idx) {
        list->data[idx2].flag = -1;
    }
}

Job* append_job_list(JobList* list, pid_t pid, CommandLine* cmd_ln, char* wc)
{
    ++list->top;

    (list->data[list->top]).pid = pid;
    (list->data[list->top]).cmd_ln = cmd_ln;
    (list->data[list->top]).wc = (char*)malloc(strlen(wc) + 1);
    (list->data[list->top]).available = true;
    (list->data[list->top]).job_id = list->top;
    (list->data[list->top]).flag = 0;

    strcpy((list->data[list->top]).wc, wc);

    update_job_flag(list);

    return &(list->data[list->top]);
}

void remove_job_list(JobList* list, int idx)
{
    if (list == NULL || idx > list->top || !(list->data[idx]).available) {
        return;
    }

    (list->data[idx]).available = false;
    (list->data[idx]).pid = -1;

    if ((list->data[idx]).wc != NULL) {
        free((list->data[idx]).wc);
        (list->data[idx]).wc = NULL;
    }

    if ((list->data[idx]).cmd_ln != NULL) {
        free_command_line((list->data[idx]).cmd_ln);
        free((list->data[idx]).cmd_ln);
        (list->data[idx]).cmd_ln = NULL;
    }

    if (idx == list->top) {
        while (list->top >= 0 && (list->data[list->top]).available == false) {
            --list->top;
        }
    }

    update_job_flag(list);
}

bool get_job_status_name(pid_t pid, char* dest)
{
    pid_t w;
    bool ended = false;
    int stats;

    w = waitpid(pid, &stats, WNOHANG | WUNTRACED);

    if (w == 0) {
        strcpy(dest, "Running");

        ended = false;
    } else {
        if (WIFEXITED(stats)) {
            int es = WEXITSTATUS(stats);

            if (es == 0) {
                strcpy(dest, "Done");
            } else {
                sprintf(dest, "Exit %d", es);
            }
        } else if (WIFSIGNALED(stats)) {
            strcpy(dest, "Terminated");
        }

        ended = true;
    }

    return ended;
}

char get_flag_char(int flag)
{
    if (flag == 0) {
        return '+';
    } else if (flag == 1) {
        return '-';
    } else {
        return ' ';
    }
}

void print_job_list(JobList* list)
{
    int i;

    for (i = 0; i <= list->top; i++) {
        char stats_name[32];
        bool ended;
        char cmd_str[BUF_SIZE];

        Job p = list->data[i];

        if (!p.available) {
            continue;
        }

        ended = get_job_status_name(p.pid, stats_name);  /* whether the process is Done/Exit/Terminated */
        format_command_line(cmd_str, p.cmd_ln, !ended);  /* get command display name */

        printf("[%d]%c  %s%*s%s\n", p.job_id + 1, get_flag_char(p.flag), stats_name, (int)(24 - strlen(stats_name)), "", cmd_str);

        if (ended) {
            remove_job_list(list, i);
        }
    }
}

JobList job_list;  /* the job list */


/******************************************************************************
 * Utilities
 *****************************************************************************/
/* get username */
void get_username(char* dest)
{
    char* user;
    uid_t uid;
    struct passwd* pwd;
    
    /* get uid */
    uid = geteuid();
    /* get user profile */
    pwd = getpwuid(uid);
    if (pwd) {
        user = pwd->pw_name;
    } else {  /* failed to get username */
        fprintf(stderr, "%s: cannot find username for UID %u\n", PROGRAM_NAME, (unsigned)uid);
        exit(EXIT_FAILURE);
    }

    strcpy(dest, user);
}

/* get $HOME, e.g. /home/wyh */
void get_home_path(char* dest)
{
    char user[BUF_SIZE];
    char home_path[BUF_SIZE] = "/home/";

    /* get username */
    get_username(user);

    /* update home path */
    strcat(home_path, user);
    
    strcpy(dest, home_path);
}

/* replace $HOME with ~ */
void alias_home_path(char* dest, char* src)
{
    char home_path[BUF_SIZE];
    char alias_path[BUF_SIZE] = "~/";

    get_home_path(home_path);

    /* replace $HOME with ~ */
    if (strstartswith(src, home_path)) {
        path_change_directory(src, home_path, alias_path);
    }

    strcpy(dest, alias_path);
}

/* get current working directory where $HOME is replaced by ~ */
void get_cwd_with_alias_home(char* dest)
{
    char cwd[BUF_SIZE];
    getcwd(cwd, sizeof(cwd));
    alias_home_path(cwd, cwd);

    strcpy(dest, cwd);
}

/******************************************************************************
 * Parse and execute commands
 *****************************************************************************/
void kill_process(Command* cmd)
{
    int i;

    if (cmd->argc <= 1) {
        fprintf(stderr, "kill: usage: kill pid");

        return;
    }

    for (i = 1; i < cmd->argc; i++) {
        pid_t pid;
        bool is_job_id;
        char* arg = cmd->argv[i];

        is_job_id = strstartswith(arg, "%");

        arg = strltrim(arg, "%");  /* trim beginning % */
        pid = (pid_t)atoi(arg);

        if (is_job_id) {
            if (pid > job_list.top && !job_list.data[pid - 1].available) {
                continue;
            }
            pid = job_list.data[pid - 1].pid;
        }

        kill(pid, SIGKILL);
    }
}

bool exec_builtin(Command* cmd)
{
    bool builtin = true;
    char* command_name;

    if (cmd->argc <= 0) {
        return false;
    }
    
    command_name = cmd->argv[0];
    if (strcmp(command_name, "cd") == 0) {
        char* dir;
        if (cmd->argc == 1) {
            char home_dir[BUF_SIZE];
            get_home_path(home_dir);
            dir = home_dir;
        } else {
            dir = cmd->argv[1];
        }
        if (chdir(dir) < 0) {
            fprintf(stderr, "%s: cd: %s: No such file or directory\n", PROGRAM_NAME, dir);
        }
    } else if (strcmp(command_name, "jobs") == 0) {
        print_job_list(&job_list);
    } else if (strcmp(command_name, "kill") == 0) {
        kill_process(cmd);
    } else if (strcmp(command_name, "pwd") == 0) {
        char cwd[BUF_SIZE];
        getcwd(cwd, sizeof(cwd));

        printf("%s\n", cwd);
    } else if (strcmp(command_name, "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        builtin = false;
    }

    return builtin;
}

void exec_command(Command* cmd)
{
    if(cmd->argc <= 0) return;

    if(exec_builtin(cmd)){
        /* Exit child process */
        exit(EXIT_SUCCESS);
    }else{
        /*char command_name[BUF_SIZE]; strcpy(command_name, cmd->argv[0]);
        if(strchr(command_name, '/') == NULL){
            char* env_str = getenv("PATH");
            char* env_dir = strtok(env_str, ":");
            while(env_dir != NULL){
                char abs_path[BUF_SIZE]; strcpy(abs_path, env_dir);
                path_cat(abs_path, command_name);
                if(path_file_exists(abs_path)){
                    strcpy(command_name, abs_path);
                    break;
                }
                env_dir = strtok(NULL, ":");
            }
        }*/
        /*char** argv = (char**)malloc(sizeof(char*) * (cmd->argc + 1));
        int i;
        for (i = 0; i < cmd->argc; i++) {
            argv[i] = cmd->argv[i];
        }
        argv[cmd->argc] = NULL;*/

        if (execvp(cmd->argv[0], cmd->argv) < 0) {
            /* free(argv); */
            exit(EXIT_FAILURE);
        }

        /* free(argv); */

       /*  if(execv(command_name, cmd->argv) == -1){
            fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
            exit(EXIT_FAILURE);
        } */
    }
}

void do_child_process(CommandLine* command_line, int idx, int pfd_input, int pfd_output)
{
    Command* cmd;
    int pfds[] = {-1, -1};
    bool is_child_proc = false;

    /* First leave -1 to initialize with the end command */
    if(idx < 0) idx = command_line->cmdc - 1;
    
    cmd = &command_line->cmdv[idx--];
    if(idx >= 0){
        if(pipe(pfds) == 0){
            is_child_proc = (fork() == 0);
            if(is_child_proc){
                do_child_process(command_line, idx, pfds[0], pfds[1]);
            }
        }
    }
    if(!is_child_proc){
        char* input_file = cmd->input;
        char* output_file = cmd->output;

        if(input_file){
            /*  Input redirection */
            int input_fd = open(input_file, O_RDONLY);
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }else if(pfds[0] >= 0 && pfds[1] >= 0){
            /* Pipe stdin from child process */
            close(pfds[1]);
            dup2(pfds[0], STDIN_FILENO);
            close(pfds[0]);
        }

        if(output_file){
            /* Ouput redirection */
            int open_flags = O_RDWR | O_CREAT;
            int create_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
            int output_fd;
            
            if (cmd->append) {  /* append mode */
                open_flags |= O_APPEND;
            } else {
                open_flags |= O_TRUNC;
            }

            output_fd = open(output_file, open_flags, create_mode);
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }else if(pfd_input >= 0 && pfd_output >= 0){
            /* Pipe stdout to parent process */
            close(pfd_input);
            dup2(pfd_output, STDOUT_FILENO);
            close(pfd_output);
        }
        exec_command(cmd);
    }
}

void handle_line(char* line)
{
    bool child_process = false;
    bool free_cmd_ln = true;
    CommandLine* command_line = (CommandLine*)malloc(sizeof(CommandLine));

    parse_command_line(command_line, line);

    if (command_line->cmdc > 0) {
        bool single_builtin = (command_line->cmdc == 1) && exec_builtin(&(command_line->cmdv[0]));

        if (!single_builtin) {
            pid_t pid;
            
            fflush(stdout);
            pid = fork();
            child_process = (pid == 0);
            
            if (child_process) {  /* run in child process */
                do_child_process(command_line, -1, -1, -1);
            } else {
                char cwd[BUF_SIZE];
                get_cwd_with_alias_home(cwd);
                
                if (!command_line->bg) {
                    int status = 0;

                    while (waitpid(pid, &status, 0) < 0) {
                        /* waiting */
                    }
                } else {
                    free_cmd_ln = false;
                    /* push a job to job list */
                    append_job_list(&job_list, pid, command_line, cwd);
                }
            }
        }

        if (free_cmd_ln) {
            free_command_line(command_line);
            free(command_line);
        }
    }
}


/******************************************************************************
 * Print prompt
 *****************************************************************************/
void print_prompt()
{
    char user[BUF_SIZE];
    char hostname[BUF_SIZE];
    char cwd[BUF_SIZE];

    /* get username */
    get_username(user);
    /* get hostname */
    gethostname(hostname, sizeof(hostname));
    /* get current working directory */
    get_cwd_with_alias_home(cwd);

    printf("%s@%s:%s$ ", user, hostname, cwd);
}


/******************************************************************************
 * Entrance: main
 *****************************************************************************/
int main(int argc, char* argv[])
{
    FILE* file;
    ShellMode sh_mode;
    char* input_line = NULL;
    size_t input_line_len = 0;
    ssize_t read;

    /* init job list */
    init_job_list(&job_list);

    if (argc <= 1) {  /* interactive mode */
        /* set mode to interactive */
        sh_mode = interactive;
        /* receive input from stdin */
        file = stdin;
    } else {  /* non-interactive mode */
        char* filename;

        /* set mode to non-interactive */
        sh_mode = noninteractive;
        
        /* open input file */
        filename = argv[1];
        file = fopen(filename, "r");

        /* failed to open file */
        if (file == NULL) {
            /* not found message */
            fprintf(stderr, "%s: %s: No such file or directory\n", PROGRAM_NAME, filename);
            /* close file */
            fclose(file);

            exit(EXIT_FAILURE);
        }
    }

    do {
        if (input_line != NULL) {
            /* handle input line */
            handle_line(input_line);
        }

        /* print prompt in interactive mode */
        if (sh_mode == interactive) {
            print_prompt();
        }
    } while ((read = getline(&input_line, &input_line_len, file)) != -1);


    /* close input file in noninteractive mode */
    if (sh_mode == noninteractive) {
        free(input_line);
        fclose(file);
    }

    return 0;
}

#endif