/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "saim_set.h"
#include "saim_memory.h"

#include <stdio.h>

#define SAIM_SET_NULL set->nil

saim_set * saim_set_create(int  (*compare_func)(const void*,const void*),
			      		   void (*destroy_func)(void*))
{
  saim_set * set;
  saim_set_node* temp;

  set = (saim_set *) SAIM_MALLOC(sizeof(saim_set));
  set->compare_func = compare_func;
  set->destroy_func = destroy_func;
  set->size = 0;

  /*  see the comment in the saim_set structure in saim_set.h */
  /*  for information on nil and root */
  temp = set->nil = (saim_set_node *) SAIM_MALLOC(sizeof(saim_set_node));
  temp->parent = temp->left = temp->right = temp;
  temp->red = 0;
  temp->data = 0;
  temp = set->root = (saim_set_node *) SAIM_MALLOC(sizeof(saim_set_node));
  temp->parent = temp->left = temp->right = set->nil;
  temp->data = 0;
  temp->red = 0;
  return set;
}

static void left_rotate(saim_set * tree, saim_set_node * x)
{
  saim_set_node * y;
  saim_set_node * nil = tree->nil;

  y = x->right;
  x->right = y->left;

  if (y->left != nil) y->left->parent = x; /* used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */
  
  y->parent = x->parent;

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  if( x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void right_rotate(saim_set * tree, saim_set_node * y)
{
  saim_set_node * x;
  saim_set_node * nil = tree->nil;

  x = y->left;
  y->left = x->right;

  if (nil != x->right)  x->right->parent = y; /*used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  x->parent = y->parent;
  if( y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

static void insert_help(saim_set * tree, saim_set_node * z)
{
  saim_set_node * x;
  saim_set_node * y;
  saim_set_node * nil = tree->nil;
  
  z->left = z->right = nil;
  y = tree->root;
  x = tree->root->left;
  while( x != nil) {
    y = x;
    if (1 == tree->compare_func(x->data, z->data)) { /* x.data > z.data */
      x = x->left;
    } else { /* x.data <= z.data */
      x = x->right;
    }
  }
  z->parent = y;
  if ( (y == tree->root) ||
       (1 == tree->compare_func(y->data, z->data))) { /* y.data > z.data */
    y->left = z;
  } else {
    y->right = z;
  }
}

saim_set_node * saim_set_insert(saim_set * tree, void * data)
{
  saim_set_node * y;
  saim_set_node * x;
  saim_set_node * new_node;

  x = (saim_set_node *) SAIM_MALLOC(sizeof(saim_set_node));
  x->data = data;

  insert_help(tree, x);
  new_node = x;
  x->red = 1;
  while (x->parent->red)
  { /* use sentinel instead of checking for root */
    if (x->parent == x->parent->parent->left)
    {
      y = x->parent->parent->right;
      if (y->red)
      {
		x->parent->red = 0;
		y->red = 0;
		x->parent->parent->red = 1;
		x = x->parent->parent;
	  }
	  else
	  {
		if (x == x->parent->right)
		{
		  x = x->parent;
		  left_rotate(tree, x);
		}
		x->parent->red = 0;
		x->parent->parent->red = 1;
		right_rotate(tree, x->parent->parent);
	  } 
	}
	else
	{ /* case for x->parent == x->parent->parent->right */
		y = x->parent->parent->left;
	    if (y->red)
	    {
			x->parent->red = 0;
			y->red=0;
			x->parent->parent->red=1;
			x=x->parent->parent;
	    }
	    else
	    {
			if (x == x->parent->left)
			{
		  		x = x->parent;
		  		right_rotate(tree,x);
			}
			x->parent->red = 0;
			x->parent->parent->red = 1;
			left_rotate(tree, x->parent->parent);
      	}
    }
  }
  tree->root->left->red = 0;
  ++tree->size;
  return new_node;
}
  
saim_set_node * saim_set_tree_successor(saim_set * set, saim_set_node * x)
{ 
  saim_set_node * y;
  saim_set_node * nil = set->nil;
  saim_set_node * root = set->root;

  if (nil != (y = x->right)) { /* assignment to y is intentional */
    while(y->left != nil) { /* returns the minium of the right subtree of x */
      y = y->left;
    }
    return y;
  } else {
    y = x->parent;
    while(x == y->right) { /* sentinel used instead of checking for nil */
      x = y;
      y = y->parent;
    }
    if (y == root) return SAIM_SET_NULL;
    return y;
  }
}

static void destroy_helper(saim_set * tree, saim_set_node * x)
{
  saim_set_node * nil = tree->nil;
  if (x != nil) {
    destroy_helper(tree, x->left);
    destroy_helper(tree, x->right);
    tree->destroy_func(x->data);
    SAIM_FREE(x);
  }
}

void saim_set_destroy(saim_set * tree)
{
  destroy_helper(tree, tree->root->left);
  SAIM_FREE(tree->root);
  SAIM_FREE(tree->nil);
  SAIM_FREE(tree);
}

void saim_set_clear(saim_set * tree)
{
  destroy_helper(tree, tree->root->left);
  tree->root->left = tree->nil;
  tree->size = 0;
}
  
saim_set_node * saim_set_search(saim_set * set, const void * data)
{
  saim_set_node * x = set->root->left;
  saim_set_node * nil = set->nil;
  int compVal;
  if (x == nil) return SAIM_SET_NULL;
  compVal = set->compare_func(x->data, data);
  while(0 != compVal) {/*assignemnt*/
    if (1 == compVal) { /* x->data > data */
      x = x->left;
    } else {
      x = x->right;
    }
    if ( x == nil) return SAIM_SET_NULL;
    compVal = set->compare_func(x->data, data);
  }
  return x;
}

/* Performs rotations and changes colors to restore red-black properties after a node is deleted */
static void delete_fix_up(saim_set * tree, saim_set_node * x)
{
  saim_set_node * root = tree->root->left;
  saim_set_node * w;

  while( (!x->red) && (root != x))
  {
    if (x == x->parent->left)
    {
      w = x->parent->right;
      if (w->red)
      {
		w->red = 0;
		x->parent->red = 1;
		left_rotate(tree, x->parent);
		w = x->parent->right;
      }
      if ( (!w->right->red) && (!w->left->red) )
      {
		w->red = 1;
		x = x->parent;
      } else {
		if (!w->right->red) {
	  		w->left->red = 0;
	  		w->red = 1;
	  		right_rotate(tree, w);
	  		w = x->parent->right;
		}
		w->red = x->parent->red;
		x->parent->red = 0;
		w->right->red = 0;
		left_rotate(tree, x->parent);
		x = root; /* this is to exit while loop */
      }
    } else { /* the code below is has left and right switched from above */
      w = x->parent->left;
      if (w->red) {
		w->red = 0;
		x->parent->red = 1;
		right_rotate(tree, x->parent);
		w = x->parent->left;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
		w->red = 1;
		x = x->parent;
      } else {
		if (!w->left->red) {
	  		w->right->red = 0;
	  		w->red = 1;
	  		left_rotate(tree, w);
	  		w = x->parent->left;
		}
		w->red = x->parent->red;
		x->parent->red = 0;
		w->left->red = 0;
		right_rotate(tree,x->parent);
		x = root; /* this is to exit while loop */
      }
    }
  }
  x->red = 0;
}

void saim_set_delete(saim_set * tree, saim_set_node * z)
{
  saim_set_node * y;
  saim_set_node * x;
  saim_set_node * nil = tree->nil;
  saim_set_node * root = tree->root;

  y= ((z->left == nil) || (z->right == nil)) ? z : saim_set_tree_successor(tree, z);
  x= (y->left == nil) ? y->right : y->left;
  if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
    root->left = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }
  if (y != z) { /* y should not be nil in this case */

    /* y is the node to splice out and x is its child */

    if (!(y->red)) delete_fix_up(tree, x);
  
    tree->destroy_func(z->data);
    y->left = z->left;
    y->right = z->right;
    y->parent = z->parent;
    y->red = z->red;
    z->left->parent = z->right->parent = y;
    if (z == z->parent->left) {
      z->parent->left = y; 
    } else {
      z->parent->right = y;
    }
    SAIM_FREE(z);
  } else {
    tree->destroy_func(y->data);
    if (!(y->red)) delete_fix_up(tree, x);
    SAIM_FREE(y);
  }
  --tree->size;
}
void saim_set_cut(saim_set * tree, saim_set_node * z)
{
  saim_set_node * y;
  saim_set_node * x;
  saim_set_node * nil = tree->nil;
  saim_set_node * root = tree->root;

  y= ((z->left == nil) || (z->right == nil)) ? z : saim_set_tree_successor(tree, z);
  x= (y->left == nil) ? y->right : y->left;
  if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
    root->left = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }
  if (y != z) { /* y should not be nil in this case */

    /* y is the node to splice out and x is its child */

    if (!(y->red)) delete_fix_up(tree, x);
  
    y->left = z->left;
    y->right = z->right;
    y->parent = z->parent;
    y->red = z->red;
    z->left->parent = z->right->parent = y;
    if (z == z->parent->left) {
      z->parent->left = y; 
    } else {
      z->parent->right = y;
    }
    SAIM_FREE(z);
  } else {
    if (!(y->red)) delete_fix_up(tree, x);
    SAIM_FREE(y);
  }
  --tree->size;
}

saim_set_node * saim_set_get_first(saim_set * set)
{
  saim_set_node * x = set->root->left;
  if (x != set->nil)
  {
    while(x->left != set->nil)
        x = x->left;
    return x;
  }
  else
    return SAIM_SET_NULL;
}
saim_set_node * saim_set_get_next(saim_set * set, saim_set_node * prev)
{
    saim_set_node * x = prev;
    saim_set_node * y;

    if (set->nil != (y = x->right)) { /* assignment to y is intentional */
        while (y->left != set->nil) { /* returns the minium of the right subtree of x */
            y = y->left;
        }
        return y;
    } else {
        y = x->parent;
        while(y != set->nil && x == y->right) { /* sentinel used instead of checking for nil */
            x = y;
            y = y->parent;
        }
        if (y == set->root) return SAIM_SET_NULL;
        return y;
    }
}
unsigned int saim_set_size(saim_set * set)
{
  return set->size;
}

#undef SAIM_SET_NULL