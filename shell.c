//
// Created by Wu Mianzhi on 2023/3/9.
//

#include "shell.h"

int run_shell(void)
{
    while (fgets(line_buffer, MAX_LENGTH, 0)) {
        add(line_buffer);
        parse(line_buffer);
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
    int rc_end; // end of valid rough command buffer

    /* command object currently work on */
    struct command * cmd = commands_buffer;


    /* should command (group) runs background? */
    enum cgs st = fg;
    char * ch;
    ch = strchr(line, '&');
    if (ch) {
        st = bg;
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


#ifdef SHELL_DEBUG
void check_buffer()
{
    for (int i = 0; commands_buffer[i].argc; i++)
    {
        printf("argc:%d \n", commands_buffer[i].argc);
    }
}
#endif