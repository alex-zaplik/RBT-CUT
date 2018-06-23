#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

#include "Nodes.h"
#include "RBT.h"

std::vector<int> prepare_test_data(int min, int max, bool shuffle)
{
	std::vector<int> nums;
	for (int i = min; i <= max; i++) nums.push_back(i);
	if (shuffle) std::random_shuffle(nums.begin(), nums.end());

	return nums;
}

int main()
{
	RBT<int> rbt;

	std::vector<int> nums = prepare_test_data(1, 20, true);

	for (int num : nums)
	{
		rbt.ins_query(num);
		std::cout << rbt << "\n";
	}
	std::cout << rbt << "\n";

	std::cin.get();
}