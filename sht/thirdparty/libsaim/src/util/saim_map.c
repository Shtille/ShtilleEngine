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

#include "saim_map.h"
#include "saim_memory.h"

#include <stdio.h>

saim_map * saim_map_create(int  (*compare_func)(const void*, const void*),
               void (*key_destroy_func)(void*),
               void (*value_destroy_func)(void*))
{
  saim_map * map;
  saim_map_node* temp;

  map = (saim_map *) SAIM_MALLOC(sizeof(saim_map));
  map->compare_func = compare_func;
  map->key_destroy_func = key_destroy_func;
  map->value_destroy_func = value_destroy_func;

  /*  see the comment in the saim_map structure in saim_map.h */
  /*  for information on nil and root */
  temp = map->nil = (saim_map_node *) SAIM_MALLOC(sizeof(saim_map_node));
  temp->parent = temp->left = temp->right = temp;
  temp->red = 0;
  temp->key = 0;
  temp->value = 0;
  temp = map->root = (saim_map_node *) SAIM_MALLOC(sizeof(saim_map_node));
  temp->parent = temp->left = temp->right = map->nil;
  temp->key = 0;
  temp->value = 0;
  temp->red = 0;
  return map;
}

static void left_rotate(saim_map * tree, saim_map_node * x)
{
  saim_map_node * y;
  saim_map_node * nil = tree->nil;

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

static void right_rotate(saim_map * tree, saim_map_node * y)
{
  saim_map_node * x;
  saim_map_node * nil = tree->nil;

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

static void insert_help(saim_map * tree, saim_map_node * z)
{
  saim_map_node * x;
  saim_map_node * y;
  saim_map_node * nil = tree->nil;
  
  z->left = z->right = nil;
  y = tree->root;
  x = tree->root->left;
  while( x != nil) {
    y = x;
    if (1 == tree->compare_func(x->key, z->key)) { /* x.key > z.key */
      x = x->left;
    } else { /* x.key <= z.key */
      x = x->right;
    }
  }
  z->parent = y;
  if ( (y == tree->root) ||
       (1 == tree->compare_func(y->key, z->key))) { /* y.key > z.key */
    y->left = z;
  } else {
    y->right = z;
  }
}

saim_map_node * saim_map_insert(saim_map * tree, void * key, void * value)
{
  saim_map_node * y;
  saim_map_node * x;
  saim_map_node * new_node;

  x = (saim_map_node *) SAIM_MALLOC(sizeof(saim_map_node));
  x->key = key;
  x->value = value;

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
  return new_node;
}
  
saim_map_node * saim_map_tree_successor(saim_map * tree, saim_map_node * x)
{ 
  saim_map_node * y;
  saim_map_node * nil = tree->nil;
  saim_map_node * root = tree->root;

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
    if (y == root) return nil;
    return y;
  }
}

static void destroy_helper(saim_map * tree, saim_map_node * x)
{
  saim_map_node * nil = tree->nil;
  if (x != nil) {
    destroy_helper(tree, x->left);
    destroy_helper(tree, x->right);
    tree->value_destroy_func(x->value);
    tree->key_destroy_func(x->key);
    SAIM_FREE(x);
  }
}

void saim_map_destroy(saim_map * tree)
{
  destroy_helper(tree, tree->root->left);
  SAIM_FREE(tree->root);
  SAIM_FREE(tree->nil);
  SAIM_FREE(tree);
}

void saim_map_clear(saim_map * tree)
{
  destroy_helper(tree, tree->root->left);
  tree->root->left = tree->nil;
}
  
saim_map_node * saim_map_search(saim_map * tree, void * key)
{
  saim_map_node * x = tree->root->left;
  saim_map_node * nil = tree->nil;
  int compVal;
  if (x == nil) return nil;
  compVal = tree->compare_func(x->key, key);
  while(0 != compVal) {/*assignemnt*/
    if (1 == compVal) { /* x->key > key */
      x = x->left;
    } else {
      x = x->right;
    }
    if ( x == nil) return nil;
    compVal = tree->compare_func(x->key, key);
  }
  return x;
}

/* Performs rotations and changes colors to restore red-black properties after a node is deleted */
static void delete_fix_up(saim_map * tree, saim_map_node * x)
{
  saim_map_node * root = tree->root->left;
  saim_map_node * w;

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

void saim_map_delete(saim_map * tree, saim_map_node * z)
{
  saim_map_node * y;
  saim_map_node * x;
  saim_map_node * nil = tree->nil;
  saim_map_node * root = tree->root;

  y= ((z->left == nil) || (z->right == nil)) ? z : saim_map_tree_successor(tree, z);
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
  
    tree->value_destroy_func(z->value);
    tree->key_destroy_func(z->key);
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
    tree->value_destroy_func(y->value);
    tree->key_destroy_func(y->key);
    if (!(y->red)) delete_fix_up(tree, x);
    SAIM_FREE(y);
  }
}

saim_map_node * saim_map_get_first(saim_map * map)
{
  saim_map_node * x = map->root;
  if (x != map->nil)
      while(x->left != map->nil)
          x = x->left;
  return x;
}
saim_map_node * saim_map_get_next(saim_map * map, saim_map_node * prev)
{
    saim_map_node * x = prev;
    saim_map_node * y;

    if (map->nil != (y = x->right)) { /* assignment to y is intentional */
        while (y->left != map->nil) { /* returns the minium of the right subtree of x */
            y = y->left;
        }
        return y;
    } else {
        y = x->parent;
        while(y != map->nil && x == y->right) { /* sentinel used instead of checking for nil */
            x = y;
            y = y->parent;
        }
        if (y == map->root) return map->nil;
        return y;
    }
}