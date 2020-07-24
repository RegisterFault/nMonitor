#ifndef LIST_H
#define LIST_H

struct node {
        struct node *next;
        long int data;
};

struct node *init_node(void);
struct node *last_elem(struct node *);
void add_node(struct node **, long int );
int count_elems(struct node *);
struct node *nth_elem(struct node *, int);
struct node *free_top(struct node *);
void free_to_nth(struct node **, int );
void free_list(struct node *);
void trunc_list(struct node **, int);
#endif
