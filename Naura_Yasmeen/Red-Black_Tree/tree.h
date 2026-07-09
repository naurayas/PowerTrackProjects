#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS     0
#define FAILURE     -1

typedef int data_t;

// node color
typedef enum
{
    RED,
    BLACK
}color_t;

// node structure
typedef struct node
{
    int data;
    color_t color;
    struct node *left, *right, *parent;
}tree_t;

extern tree_t *NIL;

int insert_node(tree_t **root, data_t item);
int delete_node(tree_t **root, data_t item);
int search_node(tree_t *root, data_t item);

int find_min(tree_t *root, data_t *min);
int find_max(tree_t *root, data_t *max);

int delete_min(tree_t **root);
int delete_max(tree_t **root);

void inorder(tree_t *root);
void preorder(tree_t *root);
void postorder(tree_t *root);
void display_color(tree_t *root, int space);

void left_rotate(tree_t **root, tree_t *x);
void right_rotate(tree_t **root, tree_t *x);

tree_t *create_node(int data);
void insert_fixup(tree_t **root, tree_t *new);
tree_t *min_node(tree_t *root);
tree_t *max_node(tree_t *root);
void delete_fixup(tree_t **root, tree_t *x);


#endif