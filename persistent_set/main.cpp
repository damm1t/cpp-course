#include <cstddef>
#include <iostream>
#include "set.h"
using namespace algo;
int main()
{
	persistent_set<int> s;
	s.insert(1);
	s.insert(2);

	auto s2 = s;
	s.insert(5);
	/*std::cout << *s.find(1) << std::endl;
	system("pause");*/
	return 0;
}