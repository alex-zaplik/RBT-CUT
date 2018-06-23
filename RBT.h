#pragma once

#include <cmath>

#include "Nodes.h"

// TODO: Test the double red fixup method
// TODO: Test the double black fixup method
// TODO: Bug in splitting and/or gf_pointer (gets deleted)

template<typename T>
class RBT
{
private:
	Node<T>* root;
	unsigned int H;
	unsigned int size;
	BucketNode<T>* gf_pointer;

	void update_h()
	{
		H = static_cast<int>(std::ceil(4.32 * std::log(size + 2)));

		if (H < 7) H = 7;
	}

	Color get_color(ColoredNode* n)
	{
		if (!n)
			return Color::BLACK;
		return n->color;
	}

	void set_color(ColoredNode* n, Color color)
	{
		if (n)
			n->color = color;
	}

	void move_middle_ptr(BucketNode<T>* bucket_pointer)
	{
		if (
			bucket_pointer->up_size > bucket_pointer->down_size &&
			bucket_pointer->up_size - bucket_pointer->down_size > 1 &&
			bucket_pointer->middle->prev != nullptr
			)
		{
			bucket_pointer->middle = bucket_pointer->middle->prev;
			bucket_pointer->up_size--;
			bucket_pointer->down_size++;
		}
		else if (
			bucket_pointer->down_size > bucket_pointer->up_size &&
			bucket_pointer->down_size - bucket_pointer->up_size > 1 &&
			bucket_pointer->middle->next != nullptr
			)
		{
			bucket_pointer->middle = bucket_pointer->middle->next;
			bucket_pointer->down_size--;
			bucket_pointer->up_size++;
		}
	}

	void left_rotate(Node<T>* node)
	{
		Node<T>* right_node = node->right;
		node->right = right_node->left;

		if (right_node->left != nullptr)
		{
			right_node->left->parent = node;
		}
		right_node->parent = node->parent;

		if (node == root)
		{
			root = right_node;
		}
		else if (node == node->parent->left)
		{
			node->parent->left = right_node;
		}
		else
		{
			node->parent->right = right_node;
		}

		right_node->left = node;
		node->parent = right_node;
	}

	void right_rotate(Node<T>* node)
	{
		Node<T>* left_node = node->left;
		node->left = left_node->right;

		if (left_node->right != nullptr)
		{
			left_node->right->parent = node;
		}
		left_node->parent = node->parent;

		if (node == root)
		{
			root = left_node;
		}
		else if (node == node->parent->left)
		{
			node->parent->left = left_node;
		}
		else
		{
			node->parent->right = left_node;
		}

		left_node->right = node;
		node->parent = left_node;
	}

	Node<T>* double_red_fixup(Node<T>* fix_pointer)
	{
		Node<T>* N = fix_pointer;
		Node<T>* P = fix_pointer->parent;
		Node<T>* G = fix_pointer->parent->parent;

		if (P == G->left)
		{
			Node<T>* U = G->right;

			if (get_color(U) == Color::RED)
			{
				if (get_color(G) == Color::DOUBLE_BLACK)
				{
					set_color(G, Color::BLACK);
				}
				else
				{
					set_color(G, Color::RED);
				}

				set_color(P, Color::BLACK);
				set_color(U, Color::BLACK);

				return G;
			}
			else
			{
				if (N == P->right)
				{
					left_rotate(P);
					P = N;
					N = P->left;
				}

				right_rotate(G);

				if (get_color(G) == Color::DOUBLE_BLACK)
				{
					set_color(P, Color::DOUBLE_BLACK);
					set_color(G, Color::RED);
				}
				else
				{
					Color t = get_color(P);
					set_color(P, get_color(G));
					set_color(G, t);
				}

				return P;
			}
		}
		else
		{
			Node<T>* U = G->left;

			if (get_color(U) == Color::RED)
			{
				if (get_color(G) == Color::DOUBLE_BLACK)
				{
					set_color(G, Color::BLACK);
				}
				else
				{
					set_color(G, Color::RED);
				}

				set_color(P, Color::BLACK);
				set_color(U, Color::BLACK);

				return G;
			}
			else
			{
				if (N == P->left)
				{
					right_rotate(P);
					P = N;
					N = P->right;
				}

				left_rotate(G);

				if (get_color(G) == Color::DOUBLE_BLACK)
				{
					set_color(P, Color::DOUBLE_BLACK);
					set_color(G, Color::RED);
				}
				else
				{
					Color t = get_color(P);
					set_color(P, get_color(G));
					set_color(G, t);
				}

				return P;
			}
		}
	}

	Node<T>* double_black_fixup(Node<T>* fix_pointer)
	{
		Node<T>* N = fix_pointer;
		Node<T>* P = fix_pointer->parent;

		if (N == P->left)
		{
			Node<T>* S = P->right;

			while (get_color(S) == Color::RED || get_color(S) == Color::DOUBLE_BLACK)
			{
				if (get_color(S) == Color::RED)
				{
					left_rotate(P);
					if (get_color(P) == Color::BLACK)
					{
						set_color(S, Color::BLACK);
						set_color(P, Color::RED);
					}
					S = P->right;
				}
				else if (get_color(S) == Color::DOUBLE_BLACK)
				{
					set_color(S, Color::BLACK);
					set_color(N, Color::BLACK);
					if (get_color(P) == Color::RED)
					{
						set_color(P, Color::BLACK);
					}
					else if (P != root)
					{
						set_color(P, Color::DOUBLE_BLACK);
					}

					return P;
				}
			}

			if (get_color(S) == Color::BLACK && get_color(S->left) == Color::BLACK && get_color(S->right) == Color::BLACK)
			{
				set_color(S, Color::RED);
				if (get_color(P) == RED)
				{
					set_color(P, BLACK);
				}
				else if (get_color(P) == Color::BLACK and P != root)
				{
					set_color(P, Color::DOUBLE_BLACK);
				}

				return P;
			}
			else
			{
				if (get_color(S->left) == Color::RED && get_color(S->right) == Color::BLACK)
				{
					right_rotate(S);
					set_color(S, get_color(S->parent));
					set_color(S->parent, Color::BLACK);
					S = S->parent;
				}
				if (get_color(S->right) == Color::RED)
				{
					left_rotate(P);
					set_color(S, get_color(P));
					set_color(P, Color::BLACK);
					set_color(S->right, Color::BLACK);
					set_color(N, Color::BLACK);
				}

				return root;
			}
		}
		else
		{
			Node<T>* S = P->left;

			while (get_color(S) == Color::RED || get_color(S) == Color::DOUBLE_BLACK)
			{
				if (get_color(S) == Color::RED)
				{
					right_rotate(P);
					if (get_color(P) == Color::BLACK)
					{
						set_color(S, Color::BLACK);
						set_color(P, Color::RED);
					}
					S = P->left;
				}
				else if (get_color(S) == Color::DOUBLE_BLACK)
				{
					set_color(S, Color::BLACK);
					set_color(N, Color::BLACK);
					if (get_color(P) == Color::RED)
					{
						set_color(P, Color::BLACK);
					}
					else if (P != root)
					{
						set_color(P, Color::DOUBLE_BLACK);
					}

					return P;
				}
			}

			if (get_color(S) == Color::BLACK && get_color(S->left) == Color::BLACK && get_color(S->right) == Color::BLACK)
			{
				set_color(S, Color::RED);
				if (get_color(P) == RED)
				{
					set_color(P, BLACK);
				}
				else if (get_color(P) == Color::BLACK and P != root)
				{
					set_color(P, Color::DOUBLE_BLACK);
				}

				return P;
			}
			else
			{
				if (get_color(S->right) == Color::RED && get_color(S->left) == Color::BLACK)
				{
					left_rotate(S);
					set_color(S, get_color(S->parent));
					set_color(S->parent, Color::BLACK);
					S = S->parent;
				}
				if (get_color(S->left) == Color::RED)
				{
					right_rotate(P);
					set_color(S, get_color(P));
					set_color(P, Color::BLACK);
					set_color(S->left, Color::BLACK);
					set_color(N, Color::BLACK);
				}

				return root;
			}
		}
	}

	Node<T>* fixup(Node<T>* fix_pointer)
	{
		if (fix_pointer == root)
		{
			if (get_color(root) == Color::RED)
			{
				set_color(root, Color::BLACK);
			}

			return root;
		}

		if (get_color(fix_pointer) == Color::RED && get_color(fix_pointer->parent) == Color::RED)
		{
			fix_pointer = double_red_fixup(fix_pointer);
		}
		else if (get_color(fix_pointer) == Color::DOUBLE_BLACK)
		{
			fix_pointer = double_black_fixup(fix_pointer);
		}
		else
		{
			fix_pointer = root;
		}

		return fix_pointer;
	}

	void underflow_fixup_borrow(BucketNode<T>* bucket_pointer, BucketNode<T>* sibling)
	{
		if (bucket_pointer == bucket_pointer->parent->left)
		{
			bucket_pointer->last->next = sibling->first;

			sibling->first = sibling->first->next;
			sibling->first->prev->prev = bucket_pointer->last;
			sibling->first->prev = nullptr;

			bucket_pointer->last = bucket_pointer->last->next;
			bucket_pointer->last->next = nullptr;

			bucket_pointer->parent->data = bucket_pointer->last->data;

			bucket_pointer->down_size++;
			if (sibling->up_size > 0) sibling->up_size--;
		}
		else
		{
			bucket_pointer->first->prev = sibling->last;

			sibling->last = sibling->last->prev;
			sibling->last->next->next = bucket_pointer->first;
			sibling->last->next = nullptr;

			bucket_pointer->first = bucket_pointer->first->prev;
			bucket_pointer->first->prev = nullptr;

			bucket_pointer->parent->data = sibling->first->data;

			bucket_pointer->up_size++;
			if (sibling->down_size > 0) sibling->down_size--;
		}

		move_middle_ptr(bucket_pointer);

		if (sibling->fix_pointer != root)
		{
			sibling->fix_pointer = fixup(sibling->fix_pointer);
		}
		if (sibling->fix_pointer != root)
		{
			sibling->fix_pointer = fixup(sibling->fix_pointer);
		}
	}

	void underflow_fixup_merge(BucketNode<T>* bucket_pointer, BucketNode<T>* sibling)
	{
		Color parent_color = get_color(bucket_pointer->parent);

		if (bucket_pointer == bucket_pointer->parent->right)
		{
			std::swap(bucket_pointer, sibling);
		}

		bucket_pointer->last->next = sibling->first;
		sibling->first->prev = bucket_pointer->last;
		bucket_pointer->middle = sibling->first;
		bucket_pointer->last = sibling->last;

		bucket_pointer->up_size = bucket_pointer->size;
		bucket_pointer->down_size = ((sibling->size > 0) ? sibling->size - 1 : 0);

		sibling->first = nullptr;
		sibling->middle = nullptr;
		sibling->last = nullptr;

		if (gf_pointer == sibling)
		{
			gf_pointer = sibling->next_bucket;
		}

		sibling->prev_bucket->next_bucket = sibling->next_bucket;
		sibling->next_bucket->prev_bucket = sibling->prev_bucket;

		delete sibling;

		if (bucket_pointer->parent->parent)
		{
			if (bucket_pointer->parent == bucket_pointer->parent->parent->left)
			{
				bucket_pointer->parent->parent->left = bucket_pointer;
			}
			else
			{
				bucket_pointer->parent->parent->right = bucket_pointer;
			}
		}

		if (bucket_pointer->parent == root)
		{
			root = bucket_pointer;
			delete bucket_pointer->parent;
			bucket_pointer->parent = nullptr;
		}
		else
		{
			Node<T>* t = bucket_pointer->parent;
			bucket_pointer->parent = bucket_pointer->parent->parent;
			delete t;
		}

		if (parent_color == Color::BLACK)
		{
			set_color(bucket_pointer, Color::DOUBLE_BLACK);
		}

		bucket_pointer->fix_pointer = bucket_pointer;

		size--;
		update_h();
		
		bucket_pointer->next_bucket = gf_pointer;
		bucket_pointer->prev_bucket = gf_pointer->prev_bucket;

		gf_pointer->prev_bucket->next_bucket = bucket_pointer;
		gf_pointer->prev_bucket = bucket_pointer;
	}

	void underflow_fixup(BucketNode<T>* bucket_pointer)
	{
		if (bucket_pointer == root)
		{
			return;
		}

		while (bucket_pointer->fix_pointer != root)
		{
			bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);
		}

		if (bucket_pointer == bucket_pointer->parent->left && get_color(bucket_pointer->parent->right) == Color::RED)
		{
			left_rotate(bucket_pointer->parent);
			set_color(bucket_pointer->parent->parent, get_color(bucket_pointer->parent));
			set_color(bucket_pointer->parent, Color::RED);
		}
		else if (get_color(bucket_pointer->parent->left) == Color::RED)
		{
			right_rotate(bucket_pointer->parent);
			set_color(bucket_pointer->parent->parent, get_color(bucket_pointer->parent));
			set_color(bucket_pointer->parent, Color::RED);
		}

		BucketNode<T>* sibling;
		if (bucket_pointer == bucket_pointer->parent->left)
		{
			sibling = static_cast<BucketNode<T>*>(bucket_pointer->parent->right);
		}
		else
		{
			sibling = static_cast<BucketNode<T>*>(bucket_pointer->parent->left);
		}

		if (2 * sibling->size > H + 6)
		{
			underflow_fixup_borrow(bucket_pointer, sibling);
		}
		else
		{
			underflow_fixup_merge(bucket_pointer, sibling);
		}
	}

	void overflow_fixup(BucketNode<T>* bucket_pointer)
	{
		while (bucket_pointer->fix_pointer != root)
		{
			bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);
		}

		InnerNode<T>* n = new InnerNode<T>(bucket_pointer->middle->data, Color::RED);
		n->parent = bucket_pointer->parent;

		if (bucket_pointer->parent)
		{
			if (bucket_pointer == bucket_pointer->parent->left)
			{
				bucket_pointer->parent->left = n;
			}
			else
			{
				bucket_pointer->parent->right = n;
			}
		}
		else
		{
			root = n;
		}

		BucketNode<T>* l = new BucketNode<T>(
			bucket_pointer->first, bucket_pointer->first, bucket_pointer->middle,
			bucket_pointer->up_size + 1, 0, bucket_pointer->up_size
			);

		l->parent = n;
		l->fix_pointer = n;
		n->left = l;

		BucketNode<T>* r = new BucketNode<T>(
			bucket_pointer->middle->next, bucket_pointer->middle->next, bucket_pointer->last,
			bucket_pointer->down_size, 0, ((bucket_pointer->down_size > 0) ? bucket_pointer->down_size - 1 : 0)
			);
		r->parent = n;
		r->fix_pointer = n;
		n->right = r;

		if (l->last && l->last->next) l->last->next->prev = nullptr;
		if (l->last) l->last->next = nullptr;

		if (r->first && r->first->prev) r->first->prev->next = nullptr;
		if (r->first) r->first->prev = nullptr;

		bucket_pointer->first = nullptr;
		bucket_pointer->middle = nullptr;
		bucket_pointer->last = nullptr;

		if (bucket_pointer->prev_bucket != bucket_pointer && bucket_pointer->next_bucket != bucket_pointer)
		{
			if (gf_pointer == bucket_pointer)
			{
				gf_pointer = bucket_pointer->next_bucket;
			}

			bucket_pointer->prev_bucket->next_bucket = bucket_pointer->next_bucket;
			bucket_pointer->next_bucket->prev_bucket = bucket_pointer->prev_bucket;

			l->next_bucket = r;
			r->prev_bucket = l;
			l->prev_bucket = gf_pointer->prev_bucket;
			r->next_bucket = gf_pointer;

			gf_pointer->prev_bucket->next_bucket = l;
			gf_pointer->prev_bucket = r;
		}
		else
		{
			l->next_bucket = r;
			l->prev_bucket = r;
			r->next_bucket = l;
			r->prev_bucket = l;

			gf_pointer = r;
		}

		delete bucket_pointer;

		size++;
		update_h();

		l->fix_pointer = fixup(l->fix_pointer);
	}

	void global_bucket_fixup(BucketNode<T>* bucket_pointer)
	{
		if (bucket_pointer != root && bucket_pointer->size < H / 2 + 3)
		{
			underflow_fixup(bucket_pointer);
		}
		else if (bucket_pointer->size > 2 * H - 10)
		{
			overflow_fixup(bucket_pointer);
		}
	}

	void global_fixup()
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			for (unsigned int j = 0; j < 11; j++)
			{
				gf_pointer->fix_pointer = fixup(gf_pointer->fix_pointer);
			}
			global_bucket_fixup(gf_pointer);
			gf_pointer = gf_pointer->next_bucket;
		}
	}

	void insert_node(BucketNode<T>* bucket_pointer, Node<T>* x_pointer, unsigned int in_bucket_pos, T data)
	{
		Node<T>* n = new Node<T>(data);

		if (x_pointer == nullptr)
		{
			if (bucket_pointer->first == nullptr)
			{
				bucket_pointer->first = n;

				if (bucket_pointer->middle == nullptr)
				{
					bucket_pointer->middle = n;
				}

				if (bucket_pointer->last == nullptr)
				{
					bucket_pointer->last = n;
				}
			}
			else
			{
				bucket_pointer->first->prev = n;
				n->next = bucket_pointer->first;

				if (bucket_pointer->first == bucket_pointer->middle && bucket_pointer->middle == bucket_pointer->last)
				{
					bucket_pointer->middle = n;
					bucket_pointer->down_size++;
				}
				else
				{
					bucket_pointer->up_size++;
				}

				bucket_pointer->first = n;
			}
		}
		else
		{
			if (x_pointer == bucket_pointer->last)
			{
				if (bucket_pointer->first == bucket_pointer->last)
				{
					bucket_pointer->middle = n;
					bucket_pointer->up_size++;
				}
				else
				{
					bucket_pointer->down_size++;
				}

				bucket_pointer->last = n;
			}
			else if (in_bucket_pos < bucket_pointer->up_size)
			{
				bucket_pointer->up_size++;
			}
			else
			{
				if (x_pointer == bucket_pointer->first && bucket_pointer->first == bucket_pointer->middle)
				{
					bucket_pointer->middle = n;
					bucket_pointer->up_size++;
				}
				else
				{
					bucket_pointer->down_size++;
				}
			}

			n->next = x_pointer->next;
			n->prev = x_pointer;

			if (x_pointer->next != nullptr)
			{
				x_pointer->next->prev = n;
			}
			x_pointer->next = n;
		}

		bucket_pointer->size++;
		move_middle_ptr(bucket_pointer);
	}

	void delete_node(BucketNode<T>* bucket_pointer, Node<T>* x_pointer, unsigned int in_bucket_pos)
	{
		if (x_pointer == bucket_pointer->first)
		{
			if (bucket_pointer->first == bucket_pointer->middle)
			{
				bucket_pointer->middle = x_pointer->next;

				if (bucket_pointer->down_size > 0)
				{
					bucket_pointer->down_size--;
				}
			}

			bucket_pointer->first = x_pointer->next;

			if (bucket_pointer->first == nullptr)
			{
				bucket_pointer->last = nullptr;
			}
			if (x_pointer->next != nullptr)
			{
				x_pointer->next->prev = nullptr;
			}
			if (bucket_pointer->up_size > 0)
			{
				bucket_pointer->up_size--;
			}
		}
		else if (x_pointer == bucket_pointer->last)
		{
			if (bucket_pointer->last == bucket_pointer->middle)
			{
				bucket_pointer->middle = x_pointer->prev;

				if (bucket_pointer->up_size > 0)
				{
					bucket_pointer->up_size--;
				}
			}

			bucket_pointer->last = x_pointer->prev;

			if (x_pointer->prev != nullptr)
			{
				x_pointer->prev->next = nullptr;
			}
			if (bucket_pointer->down_size > 0)
			{
				bucket_pointer->down_size--;
			}
		}
		else
		{
			if (x_pointer == bucket_pointer->middle)
			{
				bucket_pointer->middle = bucket_pointer->middle->prev;
				bucket_pointer->up_size--;
			}
			else
			{
				if (in_bucket_pos < bucket_pointer->up_size)
				{
					bucket_pointer->up_size--;
				}
				else
				{
					bucket_pointer->down_size--;
				}
			}

			if (x_pointer->prev) x_pointer->prev->next = x_pointer->next;
			if (x_pointer->next) x_pointer->next->prev = x_pointer->prev;
		}

		bucket_pointer->size--;
		move_middle_ptr(bucket_pointer);
	}

	void bucket_find(BucketNode<T>* bucket_pointer, Node<T>* &x_pointer, unsigned int &in_bucket_pos, T data)
	{
		Node<T>* dn = bucket_pointer->first;
		in_bucket_pos = 0;

		if (dn && dn->data > data)
		{
			x_pointer = nullptr;
			return;
		}

		while (dn != nullptr)
		{
			if (!dn->next || dn->next->data > data)
			{
				break;
			}

			in_bucket_pos++;
			dn = dn->next;
		}

		x_pointer = (dn) ? dn : nullptr;
	}

public:
	RBT() noexcept
	{
		BucketNode<T>* bn = new BucketNode<T>();
		bn->next_bucket = bn;
		bn->prev_bucket = bn;
		bn->fix_pointer = bn;

		gf_pointer = bn;
		root = bn;
		size = 0;
		H = 0;
		update_h();
	}

	Node<T>* get_root()
	{
		return root;
	}

	void ins(BucketNode<T>* bucket_pointer, Node<T>* x_pointer, unsigned int in_bucket_pos, T data)
	{
		insert_node(bucket_pointer, x_pointer, in_bucket_pos, data);
		bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);

		if (bucket_pointer->size > 2 * H - 10)
		{
			overflow_fixup(bucket_pointer);
		}

		global_fixup();
	}

	void del(BucketNode<T>* bucket_pointer, Node<T>* x_pointer, unsigned int in_bucket_pos)
	{
		delete_node(bucket_pointer, x_pointer, in_bucket_pos);
		bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);
		bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);

		if (bucket_pointer->size < H / 2 + 3)
		{
			underflow_fixup(bucket_pointer);
		}

		global_fixup();
	}

	void query(T data, BucketNode<T>* &bucket_pointer, Node<T>* &x_pointer, unsigned int &in_bucket_pos)
	{
		x_pointer = root;

		if (!x_pointer)
		{
			bucket_pointer = nullptr;
			x_pointer = nullptr;
			in_bucket_pos = 0;
			 
			return;
		}

		while (x_pointer->left || x_pointer->right)
		{
			if (data > x_pointer->data)
			{
				x_pointer = x_pointer->right;
			}
			else
			{
				x_pointer = x_pointer->left;
			}
		}

		if (x_pointer->left == nullptr && x_pointer->right == nullptr)
		{
			bucket_pointer = static_cast<BucketNode<T>*>(x_pointer);
			bucket_find(bucket_pointer, x_pointer, in_bucket_pos, data);
		}
	}

	void ins_query(T data)
	{
		BucketNode<int>* bp;
		Node<int>* xp;
		unsigned int ibp;

		query(data, bp, xp, ibp);
		ins(bp, xp, ibp, data);
	}

	void del_query(T data)
	{
		BucketNode<int>* bp;
		Node<int>* xp;
		unsigned int ibp;

		query(data, bp, xp, ibp);
		del(bp, xp, ibp);
	}

	static void print(std::ostream& stream, Node<T>* start, unsigned int tab_count)
	{
		if (!start)
		{
			return;
		}

		for (unsigned int t = 0; t < tab_count; t++)
		{
			stream << "   ";
		}

		BucketNode<T>* b = dynamic_cast<BucketNode<T>*>(start);

		stream << (start->color == Color::RED ? "R" : (start->color == Color::BLACK ? "B" : "D")) << " ";
		if (b)
		{
			stream << "[" << b->up_size << ", " << b->down_size << "]: " << *b << "\n";
		}
		else
		{
			stream << start->data << ": \n";
		}

		print(stream, start->left, tab_count + 1);
		print(stream, start->right, tab_count + 1);
	}

	friend std::ostream& operator<< (std::ostream& stream, const RBT<T> &rbt) {
		stream << "Size: " << rbt.size << ", H: " << rbt.H << "\n";

		print(stream, rbt.root, 0);
		return stream;
	}
};