// sht_map.h	-- by Vladimir Sviridov <v.shtille@gmail.com> 29 July 2014

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#ifndef __SHT_MAP_H__
#define __SHT_MAP_H__

#include "sht_pair.h"
#include <stack>

#pragma pack(push, _CRT_PACKING)

namespace sht {

	template <typename _Kty, typename _Ty>
	class map {
	public:
		typedef _Kty key_type;
		typedef _Ty mapped_type;
		typedef _Ty referent_type;	// retained
		typedef size_t size_type;
		typedef Pair<_Kty, _Ty> value_type;

	protected:
		struct Node {
			value_type value;
			bool red;
			Node* left;
			Node* right;
			Node* parent;

			Node() : value(), red(false), left(NULL), right(NULL), parent(NULL) {}
			Node(const key_type& key) : value(key), red(false), left(NULL), right(NULL), parent(NULL) {}
			Node(const key_type& key, Node* par) : value(key), red(true), left(NULL), right(NULL), parent(par) {}

			inline key_type& get_key() {
				return value.first;
			}

			Node* get_next(Node* root) {
				Node* x = this;
				Node* y;

				if (NULL != (y = x->right)) { /* assignment to y is intentional */
					while (y->left != NULL) { /* returns the minium of the right subtree of x */
						y = y->left;
					}
					return y;
				} else {
					y = x->parent;
					while(y != NULL && x == y->right) { /* sentinel used instead of checking for nil */
						x = y;
						y = y->parent;
					}
					//if (y == root) return NULL;
					return y;
				}
			}
		};

		void rotate_left(Node *x) {
			Node* y = x->right;
			x->right = y->left;

			if (y->left != NULL) y->left->parent = x;
  
			if (y != NULL) y->parent = x->parent;
			if (x->parent) {
				if (x == x->parent->left)
					x->parent->left = y;
				else
					x->parent->right = y;
			}
			else
				root = y;
			y->left = x;
			if (x != NULL) x->parent = y;
		}
		void rotate_right(Node *y) {
			Node* x = y->left;
			y->left = x->right;

			if (NULL != x->right)  x->right->parent = y;

			if (x != NULL) x->parent = y->parent;
			if (y->parent) {
				if( y == y->parent->left)
					y->parent->left = x;
				else
					y->parent->right = x;
			}
			else
				root = x;
			x->right = y;
			if (y != NULL) y->parent = x;
		}
		// maintain tree balance after inserting node x
		void insert_fixup(Node *x) {
			/* check Red-Black properties */
			while (x != root && x->parent->red == true) {
				/* we have a violation */
				if (x->parent == x->parent->parent->left) {
					Node *y = x->parent->parent->right;
					if (y != NULL && y->red == true) {

						/* uncle is RED */
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					} else {

						/* uncle is BLACK */
						if (x == x->parent->right) {
							/* make x a left child */
							x = x->parent;
							rotate_left(x);
						}

						/* recolor and rotate */
						x->parent->red = false;
						x->parent->parent->red = true;
						rotate_right(x->parent->parent);
					}
				} else {

					/* mirror image of above code */
					Node *y = x->parent->parent->left;
					if (y != NULL && y->red == true) {

						/* uncle is RED */
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					} else {

						/* uncle is BLACK */
						if (x == x->parent->left) {
							x = x->parent;
							rotate_right(x);
						}
						x->parent->red = false;
						x->parent->parent->red = true;
						rotate_left(x->parent->parent);
					}
				}
			}
			root->red = false;
		}
		// allocate node for data and insert in tree
		Node* insert(const key_type& key) {
			Node *current, *parent, *x;

			/* find where node belongs */
			current = root;
			parent = 0;
			while (current != NULL) {
				if (key == current->get_key()) return (current);
				parent = current;
				current = (key < current->get_key()) ?
					current->left : current->right;
			}

			/* setup new node */
			x = new Node(key, parent);

			/* insert node in tree */
			if(parent) {
				if (key < parent->get_key())
					parent->left = x;
				else
					parent->right = x;
			} else {
				root = x;
			}

			insert_fixup(x);

			++tree_size;
			return x;
		}
		// maintain tree balance after deleting node x
		void delete_fix_up(Node *x) {
			while (x != root && x->red == false) {
				if (x == x->parent->left) {
					Node *w = x->parent->right;
					if (w->red == true) {
						w->red = false;
						x->parent->red = true;
						rotate_left(x->parent);
						w = x->parent->right;
					}
					if (w->left->red == false && w->right->red == false) {
						w->red = true;
						x = x->parent;
					} else {
						if (w->right->red == false) {
							w->left->red = false;
							w->red = true;
							rotate_right(w);
							w = x->parent->right;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->right->red = false;
						rotate_left(x->parent);
						x = root;
					}
				} else {
					Node *w = x->parent->left;
					if (w->red == true) {
						w->red = false;
						x->parent->red = true;
						rotate_right(x->parent);
						w = x->parent->left;
					}
					if (w->right->red == false && w->left->red == false) {
						w->red = true;
						x = x->parent;
					} else {
						if (w->left->red == false) {
							w->right->red = false;
							w->red = true;
							rotate_left(w);
							w = x->parent->left;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->left->red = false;
						rotate_right(x->parent);
						x = root;
					}
				}
			}
			x->red = false;
		}
		// delete node z from tree
		void delete_node(Node* z) {
			Node *x, *y;

			if (z == NULL) return;

			if (z->left == NULL || z->right == NULL) {
				/* y has a NIL node as a child */
				y = z;
			} else {
				/* find tree successor with a NIL node as a child */
				y = z->right;
				while (y->left != NULL) y = y->left;
			}

			/* x is y's only child */
			if (y->left != NULL)
				x = y->left;
			else
				x = y->right;

			/* remove y from the parent chain */
			x->parent = y->parent;
			if (y->parent)
				if (y == y->parent->left)
					y->parent->left = x;
				else
					y->parent->right = x;
			else
				root = x;

			if (y != z) z->value = y->value;

			if (y->red == false)
				delete_fix_up(x);

			delete y;
			--tree_size;
		}
		Node* search(const key_type& key) {
			Node *current = root;
			while (current != NULL)
				if (key == current->get_key())
					return current;
				else
					current = (key < current->get_key()) ?
						current->left : current->right;
			return NULL;
		}
		Node* first() {
			Node* x = root;
			while(x && x->left) {
				x = x->left;
			}
			return x;
		}
		Node* last() {
			Node* x = root;
			while(x && x->right) {
				x = x->right;
			}
			return x;
		}
	public:
		map() : tree_size(0), root(NULL) {
		}
		~map() {
			Node * x = root;
			std::stack<Node *> stuff_to_free;

			if (x != NULL) {
				if (x->left != NULL) {
					stuff_to_free.push(x->left);
				}
				if (x->right != NULL) {
					stuff_to_free.push(x->right);
				}
				delete x;
				while( ! stuff_to_free.empty() ) {
					x = stuff_to_free.top();
					stuff_to_free.pop();
					if (x->left != NULL) {
						stuff_to_free.push(x->left);
					}
					if (x->right != NULL) {
						stuff_to_free.push(x->right);
					}
					delete x;
				}
			}
		}

		void clear() {
			Node * x = root;
			std::stack<Node *> stuff_to_free;

			if (x != NULL) {
				if (x->left != NULL) {
					stuff_to_free.push(x->left);
				}
				if (x->right != NULL) {
					stuff_to_free.push(x->right);
				}
				delete x;
				while( ! stuff_to_free.empty() ) {
					x = stuff_to_free.top();
					stuff_to_free.pop();
					if (x->left != NULL) {
						stuff_to_free.push(x->left);
					}
					if (x->right != NULL) {
						stuff_to_free.push(x->right);
					}
					delete x;
				}
			}
			root = NULL;
			tree_size = 0;
		}

		bool empty() {
			return tree_size == 0;
		}
		size_type size() {
			return tree_size;
		}

		class iterator {
		private:
			Node* node;
			Node* saved_root;
		public:
			explicit iterator(Node* n, Node* r) : node(n), saved_root(r) {}
			iterator(const iterator& it) : node(it.node), saved_root(it.saved_root) {}
			void operator = (const iterator& it) {
				node = it.node;
				saved_root = it.saved_root;
			}
			void operator = (Node* n) {
				node = n;
			}
			void operator ++() { // prefix increment
				node = node->get_next(saved_root);
			}
			void operator ++(int) { // postfix increment
				node = node->get_next(saved_root);
			}
			bool operator == (const iterator& it) {
				return node == it.node;
			}
			bool operator != (const iterator& it) {
				return node != it.node;
			}
			value_type& operator *() {
				return node->value;
			}
			value_type* operator ->() {
				return &node->value;
			}
		};

		iterator begin() {
			return iterator(first(), root);
		}
		iterator end() {
			return iterator(NULL, root);
		}
		iterator find(const key_type& key) {
			return iterator(search(key), root);
		}
		void erase(const iterator& pos) {
			delete_node(pos.node);
		}
		size_type count(const key_type& key) {
			size_type n = 0;
			Node* x = first();
			while (NULL != x) {
				if (key == x->get_key()) ++n;
				x = x->get_next(root);
			}
			return n;
		}

		mapped_type& operator[] (const key_type& key) {
			Node* n = search(key);
			if (NULL == n) {
				n = insert(key);
			}
			return n->value.second;
		}

	protected:
		size_type tree_size;
		Node* root;
	};

} // namespace sht

#pragma pack(pop)

#endif