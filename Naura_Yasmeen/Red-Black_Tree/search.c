#include "tree.h"

// function to search an element
int search_node(tree_t *root, data_t item)
{
    while(root != NIL)
    {
        if(item == root->data)
        {
            return SUCCESS;
        }
        else if(item < root->data)
        {
            root = root->left;
        }
        else
        {
            root = root->right;
        }
    }
    return FAILURE;
}

// function to find the minimum element
int find_min(tree_t *root, data_t *min)
{
    while(root != NIL)
    {
        if(root->left == NIL)
        {
            *min = root->data;
            return SUCCESS;
        }
        
        root = root->left;
    }
    return FAILURE;
}

// function to find the maximum element
int find_max(tree_t *root, data_t *max)
{
    while(root != NIL)
    {
        if(root->right == NIL)
        {
            *max = root->data;
            return SUCCESS;
        }
        
        root = root->right;
    }
    return FAILURE;
}