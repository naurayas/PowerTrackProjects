/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 01/06/2026
   End Date     : 04/06/2026
   Project Name : Red-Black Tree
*/
#include "tree.h"

tree_t sentinel = {0, BLACK, NULL, NULL, NULL};

tree_t *NIL = &sentinel;

int main()
{
    NIL->left = NIL;
    NIL->right = NIL;
    NIL->parent = NIL;

    tree_t *root = NIL;
    
    int choice, data, ret;
    int min, max;

    while(1)
    {
        printf("\n-----------------------------------------\n");
        printf("             RED-BLACK TREE              \n");
        printf("-----------------------------------------\n");
        printf("1. Insert element\n");
        printf("2. Delete element\n");
        printf("3. Search element\n");
        printf("4. Find minimum\n");
        printf("5. Delete minimum\n");
        printf("6. Find maximum\n");
        printf("7. Delete maximum\n");
        printf("8. Display (inorder)\n");
        printf("9. Display (postorder)\n");
        printf("10. Display (preorder)\n");
        printf("11. Display tree with colors\n");
        printf("12. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                printf("Enter data to insert: ");
                scanf("%d", &data);

                ret = insert_node(&root, data);
                if (ret == SUCCESS)
                    printf("Insertion successful\n");
                else
                    printf("Insertion failed\n");

                break;
            case 2:
                printf("Enter data to delete: ");
                scanf("%d", &data);

                ret = delete_node(&root, data);
                if (ret == SUCCESS)
                    printf("Deletion successful\n");
                else
                    printf("Deletion failed\n");

                break;
            case 3:
                printf("Enter data to search: ");
                scanf("%d", &data);

                ret = search_node(root, data);
                if (ret == SUCCESS)
                    printf("Data found\n");
                else
                    printf("Data not found\n");

                break;
            case 4:
                ret = find_min(root, &min);
                if(ret == SUCCESS)
                    printf("Minimum element = %d\n", min);
                else
                    printf("Tree is empty\n");
                
                break;
            case 5:
                ret = delete_min(&root);
                if(ret == SUCCESS)
                    printf("Minimum node deleted\n");
                else
                    printf("Deletion failed\n");

                break;
            case 6:
                ret = find_max(root, &max);
                if(ret == SUCCESS)
                    printf("Maximum element = %d\n", max);
                else
                    printf("Tree is empty\n");
                
                break;
            case 7:
                ret = delete_max(&root);
                if(ret == SUCCESS)
                    printf("Maximum node deleted\n");
                else
                    printf("Deletion failed\n");

                break;
            case 8:
                printf("Inorder traversal : ");
                inorder(root);
                printf("\n");
                break;
            case 9:
                printf("Postorder traversal : ");
                postorder(root);
                printf("\n");
                break;
            case 10:
                printf("Preorder Traversal : ");
                preorder(root);
                printf("\n");
                break;
            case 11:
                printf("Tree with colors : ");
                display_color(root, 0);
                printf("\n");
                break;
            case 12:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }
    return 0;
}