# RED-BLACK TREE
A Red-Black Tree is a self-balancing Binary Search Tree that maintains balance using node colors and rotations. This projects implements insertion, deletion, searching, traversals and balancing operations while preserving the Red-Black tree properties.

## Features
- Insert nodes into tree
- Delete any node
- Search for a node
- Find minimum and maximum node
- Delete minumum and maximum node
- Inorder, Preorder and Postorder traversals
- Display tree with node colors
- Automatic tree balancing

## Working
The Project inserts and deletes nodes following the Binary Search Tree rules. It is then balanced using recoloring and left/right rotations on the tree.

It also performs search, traversals and displays using the same Binary Search Tree methods.

## Technologies Used
- C programming
- Data Structures
- Red-Black Tree
- Dynamic Memory Allocation

## Project Structure
```bash
.
├── delete.c
├── display.c
├── insert.c
├── main.c
├── makefile
├── rotations.c
├── search.c
├── tree.h
└── utilities.c
```

## Build Instructions
Compile the project:
```bash
make
```
Clean the object files and executable:
```bash
make clean
```

## Usage
Run the executable:
```bash
./rbtree
```
The project supports the following operations given in the menu:
1. Insert element
2. Delete element
3. Search element
4. Find minimum
5. Delete minimum
6. Find maximum
7. Delete maximum
8. Display (inorder)
9. Display (postorder)
10. Display (preorder)
11. Display tree with colors
12. Exit
