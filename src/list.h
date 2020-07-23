#ifndef LIST_H
#define LIST_H

struct node {
        struct node *next;
        long int data;
};


struct node *last_elem(struct node *);
void add_node(struct node *, long int );
int count_elems(struct node *);
struct node *nth_elem(struct node *, int);
struct node *free_top(struct node *);
struct node *free_to_nth(struct node *, int );
int get_val_y(long int, long int, long int);
void draw_graph(WINDOW *, struct node **, long int);
#endif
