//
// Created by Wu Mianzhi on 2023/3/9.
//

/* The shell uses a list to save a copy of inputs it received, with the copy is allocated on the heap.
 * To save memory, the list has a max count of element, defined by MAX_NODE. Since the max length of each command(input)
 * is restricted, the max memory used by list is MAX_NODE * MAX_LENGTH bytes.
 * Shell should initialize the list when it starts, and explicitly free it when exits. The built-in command history do
 * not change the content of list, it's up to shell who manage it by methods defined here.
 */
#ifndef UNTITLED1_HISTORY_H
#define UNTITLED1_HISTORY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_NODE 64

struct input_node
{
    struct input_node * next;
    struct input_node * prev;
    // null-terminated string allocated on the heap
    char* content;
};

static struct input_node * history_list_beg;
static struct input_node * history_list_end;
static int node_count;

int initialize_list();

static void remove_node();

int add(char*);

int free_list();

int history(int);
#endif //UNTITLED1_HISTORY_H
