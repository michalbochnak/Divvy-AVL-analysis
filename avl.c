/*avl.c*/

//
// AVL Tree ADT implementation file.
//
// <<YOUR NAME>>
// U. of Illinois, Chicago
// CS251, Spring 2017
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "avl.h"


//
// AVLCreate:
//
// Dynamically creates and returns an empty AVL tree.
//
AVL *AVLCreate()
{
  AVL *tree;

  tree = (AVL *)malloc(sizeof(AVL));
  tree->Root = NULL;
  tree->Count = 0;

  return tree;
}


//
// AVLFree:
//
// Frees the memory associated with the tree: the handle and the nodes.
// The provided function pointer is called to free the memory that
// might have been allocated as part of the key or value.
//
void AVLFree(AVL *tree, void(*fp)(AVLKey key, AVLValue value))
{

  printf(">>>>> AVLFree needs to be implemented <<<<<\n");

}


//
// AVLCompareKeys: 
//
// Compares key1 and key2, returning
//   value < 0 if key1 <  key2
//   0         if key1 == key2
//   value > 0 if key1 >  key2
//
int AVLCompareKeys(AVLKey key1, AVLKey key2)
{
  if (key1 < key2)
    return -1;
  else if (key1 == key2)
    return 0;
  else
    return 1;
}


//
// AVLCount:
//
// Returns # of nodes in the tree.
//
int AVLCount(AVL *tree)
{
  return tree->Count;
}


//
// AVLHeight: 
//
// Returns the overall height of the AVL tree.
//
int AVLHeight(AVL *tree)
{
  if (tree->Root == NULL)
    return -1;
  else
    return tree->Root->Height;
}
