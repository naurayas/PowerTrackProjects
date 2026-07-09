#include "tree.h"

// function to fix the red-black-tree violations
void insert_fixup(tree_t **root, tree_t *new)
{
    tree_t *parent = NIL;
    tree_t *grandparent = NIL;
    tree_t *uncle = NIL;

    // run loop as long as parent is RED
    while(new != *root && new->parent->color == RED)
    {
        parent = new->parent;
        grandparent = parent->parent;

        // parent is left child of grandparent
        if(parent == grandparent->left)
        {
            uncle = grandparent->right;
            // Case 1: uncle is RED
            if(uncle->color == RED)
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;

                new = grandparent;
            }
            // uncle is BLACK, including leaf
            else
            {
                // Case 2: left-right
                if(new == parent->right)
                {
                    left_rotate(root, parent);
                    new = parent;
                    parent = new->parent;
                }

                // Case 3: left-left
                parent->color = BLACK;
                grandparent->color = RED;
                right_rotate(root, grandparent);
                new = parent;
            }
        }
        // parent is right child of grandparent
        else
        {
            uncle = grandparent->left;
            // case 1: uncle is RED
            if(uncle->color == RED)
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                new = grandparent;
            }
            // uncle is BLACK, including leaf
            else
            {
                // case 2: right-left
                if(new == parent->left)
                {
                    right_rotate(root, parent);
                    new = parent;
                    parent = new->parent;
                }
                
                // case 3: right-right
                parent->color = BLACK;
                grandparent->color = RED;
                left_rotate(root, grandparent);
                new = parent;
            }

        }
    }
    // root always BLACK
    (*root)->color = BLACK;
}

// function to find the pointer to the minimum node
tree_t *min_node(tree_t *root)
{
    while(root->left != NIL)
    {
        root = root->left;
    }

    return root;
}

// function to find the pointer to the maximum node
tree_t *max_node(tree_t *root)
{
    while(root->right != NIL)
    {
        root = root->right;
    }

    return root;
}

void delete_fixup(tree_t **root, tree_t *x)
{
    tree_t *sibling = NIL;

    // x is double black
    while(x != *root && x->color == BLACK)
    {
        // x is left child
        if(x == x->parent->left)
        {
            sibling = x->parent->right;

            // case 1: RED sibling
            if(sibling->color == RED)
            {
                // convert to BLACK sibling case2/3/4
                sibling->color = BLACK;
                x->parent->color = RED;
                left_rotate(root, x->parent);
                sibling = x->parent->right;
            }
            // case 2: BLACK sibling with BLACK children
            if(sibling->left->color == BLACK && sibling->right->color == BLACK)
            {
                // remove one BLACK by recoloring S->RED
                if(sibling != NIL)
                    sibling->color = RED;
                // move upward
                x = x->parent;
            }   
            else
            {
                // case 3: BLACK sibling with RED near child
                if(sibling->right->color == BLACK)
                {
                    // convert to case 4
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(root, sibling);
                    sibling = x->parent->right;
                }

                // case 4: BLACK sibling with RED far child (final fix)
                sibling->color = x->parent->color;
                x->parent->color = BLACK;
                sibling->right->color = BLACK;
                left_rotate(root, x->parent);

                x = *root;
            }
        }
        // x is right child (mirror image)
        else
        {
            sibling = x->parent->left;

            // case 1: RED sibling
            if(sibling->color == RED)
            {
                sibling->color = BLACK;
                x->parent->color = RED;
                right_rotate(root, x->parent);
                sibling = x->parent->left;
            }
            
            // case 2: BLACK sibling with BLACK children
            if(sibling->left->color == BLACK && sibling->right->color == BLACK)
            {
                if(sibling != NIL)
                    sibling->color = RED;
                // move upward
                x = x->parent;
            }
            else
            {
                // case 3: BLACK sibling with RED near child
                if(sibling->left->color == BLACK)
                {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(root, sibling);
                    sibling = x->parent->left;
                }

                // case 4: BLACK sibling with RED far child (final fix)
                sibling->color = x->parent->color;
                x->parent->color = BLACK;
                sibling->left->color = BLACK;
                right_rotate(root, x->parent);
                x = *root;
            }
        }
    }

    // remove any remaining double black
    x->color = BLACK;
}