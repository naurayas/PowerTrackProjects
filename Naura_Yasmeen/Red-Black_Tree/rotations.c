#include "tree.h"

// function for left rotation
void left_rotate(tree_t **root, tree_t *x)
{
    // store the right node
    tree_t *y = x->right;

    // move its y's left child to right child of x
    x->right = y->left;
    // update y's left child parent
    if(y->left != NIL)
    {
        y->left->parent = x;
    }

    // update y's parent
    y->parent = x->parent;

    // if x was root make y as root
    if(x->parent == NIL)
    {
        *root = y;
    }
    // if x was left child
    else if(x == x->parent->left)
    {
        x->parent->left = y;
    }
    // x was right child
    else
    {
        x->parent->right = y;
    }

    // update x and y
    y->left = x;
    x->parent = y;
}

// function for right rotation
void right_rotate(tree_t **root, tree_t *x)
{
    // store left node
    tree_t *y = x->left;

    // move y's right child to x's left child
    x->left = y->right;
    // update its parent
    if(y->right != NIL)
    {
        y->right->parent = x;
    }

    // update y's parent
    y->parent = x->parent;

    // check if x was root
    if(x->parent == NIL)
    {
        *root = y;
    }
    // x was left child
    else if(x == x->parent->left)
    {
        x->parent->left = y;
    }
    // x was right child
    else
    {
        x->parent->right = y;
    }

    // update y and x
    y->right = x;
    x->parent = y;
}