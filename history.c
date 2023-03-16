//
// Created by Wu Mianzhi on 2023/3/9.
//

#include "history.h"

int initialize_list()
{
    node_count = 0;
    history_list_beg = malloc(sizeof(struct input_node));
    history_list_end = history_list_beg;
    history_list_beg->next = NULL;
    history_list_beg->prev = NULL;
    history_list_beg->content = NULL;
    return 0;
}

static void remove_node()
{
    struct input_node * end = history_list_end->prev;
    history_list_end->prev = end->prev;
    end->prev->next = history_list_end;
    node_count--;

    free(end->content);
    free(end);
}

int add(char * input)
{
    if (node_count == MAX_NODE)
        remove_node();
    unsigned long long input_len = strlen(input);
    struct input_node * new_node = malloc(sizeof(struct input_node));
    if (!new_node)
        return -1;

    history_list_beg->prev = new_node;
    new_node->prev = NULL;
    new_node->next = history_list_beg;
    history_list_beg = new_node;

    new_node->content = malloc((input_len+1) * sizeof(char)); // null included
    if (!new_node->content)
        return -1;
    strcpy(new_node->content, input);

    node_count++;
    return 0;
}

int free_list()
{
    struct input_node * node = history_list_beg;
    struct input_node * prev = NULL;
    while (node)
    {
        if (node->content)
            free(node->content);
        prev = node;
        node = node->next;
        free(prev);
    }
    return 0;
}

int history(int n)
{
    if (n > node_count)
        return -1;
    int count;
    struct input_node * node = history_list_beg;
    for (count = 1; count <= n; count++)
    {
        printf("%s\n", node->content);
        node = node->next;
    }
    return 0;
}