#include "tree.h"

// inorder traversal function
void inorder(tree_t *root)
{
    if(root == NIL)
        return;

    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

// preorder traversal function
void preorder(tree_t *root)
{
    if(root == NIL)
        return;

    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

// postorder traversal function
void postorder(tree_t *root)
{
    if(root == NIL)
        return;

    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}

// display tree with colors (inorder)
void display_color(tree_t *root, int space)
{
    if(root == NIL)
        return;
    
    space += 5;
    display_color(root->right, space);
    printf("\n");

    for(int i = 5; i < space; i++)
    {
        printf(" ");
    }

    printf("%d(%c)\n", root->data, root->color == RED ? 'R' : 'B');

    display_color(root->left, space);
}