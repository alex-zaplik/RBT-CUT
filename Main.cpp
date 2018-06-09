#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

#include "Nodes.h"

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