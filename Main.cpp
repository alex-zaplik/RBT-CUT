#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

#include "Nodes.h"

// TODO: Test the underflow fixup method
// TODO: Test the overflow fixup method

static Node* root;
static unsigned int H;
static BucketNode<int>* gf_pointer;

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

template<typename T>
void move_middle_ptr(BucketNode<T>* bucket_pointer)
{
	if (
		bucket_pointer->up_size > bucket_pointer->down_size &&
		bucket_pointer->up_size - bucket_pointer->down_size > 1 &&
		bucket_pointer->middle->prev != nullptr
		)
	{
		bucket_pointer->middle = static_cast<DataNode<T>*>(bucket_pointer->middle->prev);
		bucket_pointer->up_size--;
		bucket_pointer->down_size++;
	}
	else if (
		bucket_pointer->down_size > bucket_pointer->up_size &&
		bucket_pointer->down_size - bucket_pointer->up_size > 1 &&
		bucket_pointer->middle->next != nullptr
		)
	{
		bucket_pointer->middle = static_cast<DataNode<T>*>(bucket_pointer->middle->next);
		bucket_pointer->down_size--;
		bucket_pointer->up_size++;
	}
}

void left_rotate(Node* node)
{
	Node* right_node = node->right;
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

void right_rotate(Node* node)
{
	Node* left_node = node->left;
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

// TODO: Implement the double red fixup method
Node* double_red_fixup(Node* fix_pointer)
{
	return fix_pointer;
}

// TODO: Implement the double black fixup method
Node* double_black_fixup(Node* fix_pointer)
{
	return fix_pointer;
}

template<typename T>
Node* fixup(Node* fix_pointer)
{
	Node* cn = static_cast<InnerNode<T>*>(fix_pointer);
	if (!cn) cn = static_cast<BucketNode<T>*>(fix_pointer);

	if (get_color(cn) == Color::RED && get_color(cn->parent) == Color::RED)
	{
		fix_pointer = double_red_fixup(fix_pointer);
	}
	else if (get_color(cn) == Color::DOUBLE_BLACK)
	{
		fix_pointer = double_black_fixup(fix_pointer);
	}
	else
	{
		fix_pointer = root;
	}

	return fix_pointer;
}

template<typename T>
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

template<typename T>
void underflow_fixup_merge(BucketNode<T>* bucket_pointer, BucketNode<T>* sibling)
{
	Color parent_color = get_color(bucket_pointer->parent);

	if (bucket_pointer == bucket_pointer->parent->left)
	{
		bucket_pointer->middle = sibling->first;
		bucket_pointer->last->next = sibling->first;
		sibling->first->prev = bucket_pointer->last;
		bucket_pointer->last = sibling->last;
	}
	else
	{
		bucket_pointer->middle = bucket_pointer->first;
		sibling->last->next = bucket_pointer->first;
		bucket_pointer->first->prev = sibling->last;
		bucket_pointer->first = sibling->first;
	}

	sibling->first = nullptr;
	sibling->middle = nullptr;
	sibling->last = nullptr;

	sibling->prev_bucket->next_bucket = sibling->next_bucket;
	sibling->next_bucket->prev_bucket = sibling->prev_bucket;

	if (bucket_pointer->parent = bucket_pointer->parent->parent->left)
	{
		bucket_pointer->parent->parent->left = bucket_pointer;
	}
	else
	{
		bucket_pointer->parent->parent->right = bucket_pointer;
	}

	bucket_pointer->parent = bucket_pointer->parent->parent;

    if (parent_color = Color::BLACK)
	{
		set_color(bucket_pointer, Color::DOUBLE_BLACK);
	}
        
	bucket_pointer->fix_pointer = bucket_pointer;

	bucket_pointer->next_bucket = gf_pointer;
	bucket_pointer->prev_bucket = gf_pointer->prev_bucket;
	
	gf_pointer->prev_bucket->next_bucket = bucket_pointer;
	gf_pointer->prev_bucket = bucket_pointer;
}

template<typename T>
void underflow_fixup(BucketNode<T>* bucket_pointer)
{
	while (bucket_pointer->fix_pointer != root)
	{
		bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);
	}

	if (bucket_pointer == bucket_pointer->parent->left && get_color(bucket_pointer->parent->right) == Color::RED)
	{
		left_rotate(bucket_pointer->parent);
	}
	else if (get_color(bucket_pointer->parent->left) == Color::RED)
	{
		right_rotate(bucket_pointer->parent);
	}

	BucketNode* sibling;
	if (bucket_pointer == bucket_pointer->parent->left)
	{
		sibling = bucket_pointer->parent->right;
	}
	else
	{
		sibling = bucket_pointer->parent->left;
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

template<typename T>
void overflow_fixup(BucketNode<T>* bucket_pointer)
{
	while (bucket_pointer->fix_pointer != root)
	{
		bucket_pointer->fix_pointer = fixup(bucket_pointer->fix_pointer);
	}

	InnerNode<T>* n = new InnerNode<T>(bucket_pointer->middle->data, Color::RED);
	n->parent = bucket_pointer->parent;

	if (bucket_pointer == bucket_pointer->parent->left)
	{
		bucket_pointer->parent->left = n;
	}
	else
	{
		bucket_pointer->parent->right = n;
	}

	BucketNode<T>* l = new BucketNode<T>(bucket_pointer->start, bucket_pointer->start, bucket_pointer->middle->prev);
	l->parent = n;
	l->fix_pointer = n;

	BucketNode<T>* r = new BucketNode<T>(bucket_pointer->middle, bucket_pointer->middle, bucket_pointer->last);
	r->parent = n;
	r->fix_pointer = n;

	n->left = l;
	n->right = r;

	bucket_pointer->first = nullptr;
	bucket_pointer->middle = nullptr;
	bucket_pointer->last = nullptr;

	bucket_pointer->prev_bucket->next_bucket = bucket_pointer->next_bucket;
	bucket_pointer->next_bucket->prevBucket = bucket_pointer->prev_bucket;

	delete bucket_pointer;

	l->next_bucket = r;
	r->prev_bucket = l;
	l->prev_bucket = gf_pointer->prev_bucket;
	r->next_bucket = gf_pointer;

	gf_pointer->prev_bucket->next_bucket = l;
	gf_pointer->prev_bucket = r;

	l->fix_pointer = fixup(l->fix_pointer);
}

template<typename T>
void insert_node(BucketNode<T>* bucket_pointer, DataNode<T>* x_pointer, unsigned int in_bucket_pos, T data)
{
	DataNode<T>* n = new DataNode<T>(data);

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

template<typename T>
void delete_node(BucketNode<T>* bucket_pointer, DataNode<T>* x_pointer, unsigned int in_bucket_pos)
{
	if (x_pointer == bucket_pointer->first)
	{
		if (bucket_pointer->first == bucket_pointer->middle)
		{
			bucket_pointer->middle = static_cast<DataNode<T>*>(x_pointer->next);

			if (bucket_pointer->down_size > 0)
			{
				bucket_pointer->down_size--;
			}
		}

		bucket_pointer->first = static_cast<DataNode<T>*>(x_pointer->next);

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
			bucket_pointer->middle = static_cast<DataNode<T>*>(x_pointer->prev);

			if (bucket_pointer->up_size > 0)
			{
				bucket_pointer->up_size--;
			}
		}

		bucket_pointer->last = static_cast<DataNode<T>*>(x_pointer->prev);

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
			bucket_pointer->middle = static_cast<DataNode<T>*>(bucket_pointer->middle->prev);
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

template<typename T>
void bucket_find(BucketNode<T>* bucket_pointer, DataNode<T>* &x_pointer, unsigned int &in_bucket_pos, T data)
{
	DataNode<T>* dn = bucket_pointer->first;
	in_bucket_pos = 0;

	if (dn && dn->data > data)
	{
		x_pointer = nullptr;
		return;
	}

	while (dn != nullptr)
	{
		if (!dn->next || static_cast<DataNode<T>*>(dn->next)->data > data)
		{
			break;
		}

		in_bucket_pos++;
		dn = static_cast<DataNode<T>*>(dn->next);
	}

	x_pointer = (dn) ? static_cast<DataNode<T>*>(dn) : nullptr;
}

int main()
{
	BucketNode<int>* bn = new BucketNode<int>();

	std::vector<int> nums(25);
	for (unsigned int i = 0; i < nums.size(); i++) nums[i] = nums.size() - i;
	std::random_shuffle(nums.begin(), nums.end());

	for (unsigned int i = 0; i < nums.size(); i++)
	{
		DataNode<int>* xp;
		unsigned int ibp;
		bucket_find(bn, xp, ibp, nums[i]);

		insert_node<int>(bn, xp, ibp, nums[i]);

		std::ostringstream ss;
		ss << "[" << bn->up_size << ", " << bn->down_size << "]:";
		std::cout << std::left << std::setw(15) << ss.str() << *bn << "\n";
	}

	std::random_shuffle(nums.begin(), nums.end());

	for (unsigned int i = 0; i < nums.size(); i++)
	{
		DataNode<int>* xp;
		unsigned int ibp;
		bucket_find(bn, xp, ibp, nums[i]);

		delete_node<int>(bn, xp, ibp);

		std::ostringstream ss;
		ss << "[" << bn->up_size << ", " << bn->down_size << "]:";
		std::cout << std::left << std::setw(15) << ss.str() << *bn << "\n";
	}

	std::cin.get();
}