#include "tree.h"

// function to detele an element
int delete_node(tree_t **root, data_t item)
{
    tree_t *current = *root;
    tree_t *parent = NIL;
    color_t original_color;

    // find the node
    while(current != NIL && current->data != item)
    {
        parent = current;

        if(item < current->data)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    // element not found or empty tree
    if(current == NIL)
    {
        return FAILURE;
    }

    // node has 0/1 child
    if(current->left == NIL || current->right == NIL)
    {
        tree_t *child;
        // left child only
        if(current->left != NIL)
        {
            child = current->left;
        }
        // right child only
        else
        {
            child = current->right;
        }
        original_color = current->color;

        // delete the node
        // item is root node
        if(parent == NIL)
        {
            *root = child;
        }
        // item is left child
        else if(current == parent->left)
        {
            parent->left = child;
        }
        // item is right child
        else if(current == parent->right)
        {
            parent->right = child;
        }

        // update parent
        child->parent = parent;

        // fixup only when deleted node is BLACK
        if(original_color == BLACK)
        {
            delete_fixup(root, child);
        }
        
        free(current);

        return SUCCESS;
    }

    // 2 children
    tree_t *next = min_node(current->right);
    current->data = next->data;
    original_color = next->color;

    // delete the minimum node of right subtree 
    tree_t *next_child = next->right;
    next_child->parent = next->parent;

    if(next == next->parent->left)
    {
        next->parent->left = next_child;
    }
    else
    {
        next->parent->right = next_child;
    }

    if(original_color == BLACK)
    {
        delete_fixup(root, next_child);
    }

    free(next);
    return SUCCESS;
}

// function to delete the minimum element
int delete_min(tree_t **root)
{
    // empty tree
    if(*root == NIL)
        return FAILURE;

    tree_t *current = *root;
    tree_t *parent = NIL;
    color_t original_color;

    // find minimum node
    while(current->left != NIL)
    {
        parent = current;
        current = current->left;
    }

    tree_t *child = current->right;
    original_color = current->color;

    // minimum node is root
    if(parent == NIL)
    {
        *root = child;
    }
    else
    {
        parent->left = child;
    }
    
    child->parent = parent;

    if(original_color == BLACK)
    {
        delete_fixup(root, child);
    }

    free(current);
    return SUCCESS;
}

// function to delete the maximum element
int delete_max(tree_t **root)
{
    // empty tree
    if(*root == NIL)
        return FAILURE;

    tree_t *current = *root;
    tree_t *parent = NIL;
    color_t original_color;

    // find maximum node
    while(current->right != NIL)
    {
        parent = current;
        current = current->right;
    }

    tree_t *child = current->left;
    original_color = current->color;

    // maximum node is root
    if(parent == NIL)
    {
        *root = child;
    }
    else
    {
        parent->right = child;
    }

    child->parent = parent;

    if(original_color == BLACK)
    {
        delete_fixup(root, child);
    }

    free(current);
    return SUCCESS;
}
