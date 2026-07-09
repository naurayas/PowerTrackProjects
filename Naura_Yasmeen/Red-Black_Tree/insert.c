#include "tree.h"

// function to create a node
tree_t *create_node(int data)
{
    tree_t *new = malloc(sizeof(tree_t));
    if (new == NULL)
        return NULL;

    new->data = data;
    new->color = RED;
    new->left = NIL;
    new->right = NIL;
    new->parent = NIL;

    return new;
}

// function to insert a new node
int insert_node(tree_t **root, data_t item)
{
    tree_t *new = create_node(item);
    if(new == NULL)
        return FAILURE;
    
    // empty tree
    if(*root == NIL)
    {
        // root node is always BLACK
        new->color = BLACK;
        *root = new;
        
        return SUCCESS;
    }

    // tree not empty
    tree_t *current = *root;
    tree_t *parent = NIL;

    while(current != NIL)
    {
        parent = current;

        if(item < current->data)
        {
            current = current->left;
        }
        else if(item > current->data)
        {
            current = current->right;
        }
        // duplicate node
        else
        {
            free(new);
            return FAILURE;
        }
    }

    // attach new node
    new->parent = parent;
    if(item < parent->data)
    {
        parent->left = new;
    }
    else
    {
        parent->right = new;
    }

    // fixup the tree
    insert_fixup(root, new);

    return SUCCESS;
}
