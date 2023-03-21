//
// Created by Wu Mianzhi on 2023/3/9.
//

#include "shell.h"

static struct command commands_buffer[MAX_COMMAND]; // the bound element is set argc == 0
static char line_buffer[MAX_LENGTH];

#ifdef SIGTTIN
int jobcontrol = 1;
#else
int jobcontrol = 0;
#endif

int run_shell(void)
{
    initialize_jobs();
    initialize_list();
    if (!jobcontrol)
        printf("no job control in this platform\n");
    while (1) {
        printf("tsh>");
        if (!fgets(line_buffer, MAX_LENGTH, stdin))
            app_error("error when read from stdin");
        /* drop \n */
        char * ch = strchr(line_buffer, '\n');
        if (ch) *ch = '\0';

        add(line_buffer);
        eval(line_buffer);
    }
    return 0;
}


int parse(const char * cmdline)
{
    /* save a local copy of cmdline */
    static char line[MAX_LENGTH];
    strcpy(line, cmdline);

    /* index of the location each rough command start in local copy */
    static char * rough_command[MAX_COMMAND];
    int rc_index;
    //int rc_end; // end of valid rough command buffer

    /* command object currently work on */
    struct command * cmd = commands_buffer;


    /* should command (group) runs background? */
    int st = 0;
    char * ch;
    ch = strchr(line, '&');
    if (ch) {
        st = 1;
        *ch = 0;
    }

    /* split cmdline by delim '|' */
    rough_command[0] = line;
    for (rc_index = 0; (ch = strchr(rough_command[rc_index], '|')); rc_index++)
    {
        *ch = 0;
        rough_command[rc_index + 1] = ch + 1;
    }
    rough_command[rc_index + 1] = 0;

    /* parse rough command to command object */
    for (rc_index = 0; rough_command[rc_index]; rc_index++, cmd++)
    {
        /* set file descriptor */
        set_fd(rough_command[rc_index], cmd);

        /* set argv and argc */
        set_argv(rough_command[rc_index], cmd);
    }

    /* create n-1 pipes and set fd */
    cmd = commands_buffer + 1; // rest working command
    for (rc_index = 1; rough_command[rc_index]; rc_index++, cmd++)
    {
        int p[2];
        Pipe(p);
        (cmd-1)->fd[2] = p[1];
        cmd->fd[0] = p[0];
    }

    cmd->argc = 0;

    return st;
}

void set_fd(char * cmdline, struct command * command)
{
    static char pattern[4][3] = {"<", "2>", ">>", ">"};
    static enum io_mode mode[4] = {in, out, append, out};
    static int default_io[4] = {0, 2, 1, 1};

    static char name_buffer[MAX_FN_LENGTH];

    char* op;

    for (int i = 0; i < 4; i++)
    {
        /* is redirection operator found? */
        op = strstr(cmdline, pattern[i]);

        if (op)
        {
            /* set temp terminated character '<' */
            /* skip operator */
            for (int j = 0; j < strlen(pattern[i]); j++)
                *(op++) = '<';

            /* ignore space */
            while (*op == ' ')
                op++;

            /* copy file name */
            int j;
            for (j = 0; *op != ' ' && *op != 0; j++, op++)
            {
                name_buffer[j] = *op;
            }
            name_buffer[j] = 0;

            /* open file */
            command->fd[i == 3 ? 2 : i] = Open(name_buffer, mode[i]); // if search '>', replace its index in command.fd

            /* skip search ">" */
            if (i == 2)
                break;
        }
        else
        {
            command->fd[i == 3 ? 2 : i] = default_io[i];
        }
    }
}

void set_argv(char * cmdline, struct command * command)
{
    char* terminate;
    char* buf = cmdline; // working character

    /* set terminate character*/
    terminate = strchr(cmdline, '<');
    if (terminate)
        *terminate = '\0';

    /* init */
    command->argc = 0;

    while (*buf)
    {
        /* skip space */
        while (*buf == ' ')
            buf++;

        /* terminate when reach the argument limit */
        if (command->argc == MAX_ARGS)
            break;

        /* set substring */
        command->argv[command->argc] = buf;
        command->argc++;
        /* skip until next delim or the end */
        while (*buf != ' ' && *buf != '\0')
            buf++;

        /* leave loop if reach the end of string */
        if (*buf == 0)
            break;
        *buf = '\0';
        buf++;
    }
    command->argv[command->argc] = 0;
}

// TODO: simplify
int builtin_command(struct command * cmd, int bg)
{
    // builtin cmd list
    static char builtin_cmd[7][8] = {
        [BUILTIN_CD]   = "cd",
        [BUILTIN_PWD]  = "pwd", 
        [BUILTIN_HIST] = "history", 
        [BUILTIN_EXIT] = "exit", 
        [BUILTIN_BG]   = "bg", 
        [BUILTIN_FG]   = "fg", 
        [BUILTIN_JOBS] = "jobs",
        };
    // default fd without redirection
    static const int fd_default[3] = {0, 2, 1};
    // copy of stdin, stderr and stdout
    static int stds_copy[3];
    char** argv = cmd->argv;
    int argc = cmd->argc;
    int res = 1;
    pid_t pid;

    if (bg)
    {
        if ((pid = Fork()))
        {
            return 1; // parent process return
        }
        else //child process set fd
        {
            for (int fd_index = 0; fd_index < 3; fd_index++)
            {
                if (cmd->fd[fd_index] != fd_default[fd_index])
                {
                    Close(fd_default[fd_index]); //close default fd
                    Dup(cmd->fd[fd_index]);
                    //Close(fd_default[fd_index]); //close opened fd;
                }
            }
        }
    }
    else
    {
        for (int fd_index = 0; fd_index < 3; fd_index++)
        {
            if (cmd->fd[fd_index] != fd_default[fd_index])
            {
                stds_copy[fd_index] = Dup(fd_default[fd_index]);
                Close(fd_default[fd_index]); //close default fd
                Dup(cmd->fd[fd_index]);
            }
            else
                stds_copy[fd_index] = fd_default[fd_index];
        }
    }

    int cmd_index;
    for (cmd_index = 0; cmd_index < 7; cmd_index++) // 7 builtin cmd implyed
    {
        if (strcmp(argv[0], builtin_cmd[cmd_index]) == 0)
        {
            break;
        }
    }
    switch (cmd_index)
    {
    case BUILTIN_CD:
        cd(argv);
        break;
    case BUILTIN_PWD:
        pwd();
        break;
    case BUILTIN_EXIT:
        builtin_exit();
        break;
    case BUILTIN_HIST:
        history(argc, argv);
        break;
    case BUILTIN_JOBS:
        list_jobs();
        break;
    default:
        res = 0;
        break;
    }

    if (bg)
        exit(0); //exit child process
    
    /* reset file descriptor */
    for (int fd_index = 0; fd_index < 3; fd_index++)
    {
        // is stdstream edited? 
        if (stds_copy[fd_index] != fd_default[fd_index])
        {
            Close(fd_default[fd_index]); //close default fd
            Dup(stds_copy[fd_index]);
            Close(stds_copy[fd_index]); //close opened fd;
        }
    }
    return res; //should not reached when bg is set
}

static void closefd()
{
    struct command * cmd; //current working
    for (cmd = commands_buffer; cmd->argc; cmd++)
    {
            for (int fd_index = 0; fd_index < 3; fd_index++)
            {
                if (cmd->fd[fd_index] != 0 && 
                    cmd->fd[fd_index] != 1 &&
                    cmd->fd[fd_index] != 2)
                {
                    Close(cmd->fd[fd_index]);
                }
            }
    }
}

void eval(const char * cmdline)
{

    int bg;
    bg = parse(cmdline);

    #ifdef SHELL_DEBUG
    check_buffer();
    #endif

    // default fd without redirection
    static const int fd_default[3] = {0, 2, 1};

    struct command * cmd; // current working cmd
    for (cmd = commands_buffer; cmd->argc; cmd++)
    {
        pid_t pid;
        if (!builtin_command(cmd, bg))
        {
            if ((pid = Fork()) == 0)
            {
                /* set file descriptor for command */
                for (int fd_index = 0; fd_index < 3; fd_index++)
                {
                    if (cmd->fd[fd_index] != fd_default[fd_index])
                    {
                        Close(fd_default[fd_index]); //close default fd
                        Dup(cmd->fd[fd_index]);
                    }
                    else if (bg)
                    {
                        Open("/dev/null", in);
                        Close(fd_default[fd_index]);
                        Dup(cmd->fd[fd_index]);
                    }
                }
                closefd(); // close unused fd
                // let minix manage child process
                if (bg) // set background handler and fd
                {
                    signal(SIGCHLD, SIG_IGN);
                    setpgid(0, 0);
                    /* block termianl in and out */
                }
                else // set foreground group
                {
                    setpgid(getpid(), getppid());
                }
                if (Execve(cmd->argv[0], cmd->argv, environ) < 0)
                {
                    printf("%s: Command not found.\n", cmd->argv[0]);
                    exit(0);
                }
            }
            add_job(pid, bg+1, line_buffer);
            if (!bg) // set foreground group
            {
                setpgid(pid, getpid());
            }
        }
    }
    closefd(); // close unused fd
    if (!bg)
        wait_fg();

}

void wait_fg()
{
    pid_t pid;
    while (maxjid())
    {
        if (jobcontrol) pid = waitpid(-getpid(), NULL, 0);
        else pid = waitpid(-1, NULL, 0);
        del_job(pid);
    }
}

#ifdef SHELL_DEBUG
void check_buffer()
{
    for (int i = 0; commands_buffer[i].argc; i++)
    {
        printf("cmd: %s argc:%d \n",commands_buffer[i].argv[0], commands_buffer[i].argc);
        printf("stdin:%d, stdout:%d, stderr:%d\n", commands_buffer[i].fd[0], commands_buffer[i].fd[2], commands_buffer[i].fd[1]);
    }
}
#endif