#include <cstddef>
#include <iostream>
#include "set.h"
using namespace algo;
int main()
{
	persistent_set<int> s;
	auto x = s.insert(1);
	s.insert(2);

	auto s2 = s;
	s.insert(5);
	auto it = s.find(2);
	auto first = ++it;
	auto second = --it;
	auto y = s.begin();
	auto z = s.end();
	/*std::cout << *s.find(1) << std::endl;
	system("pause");*/

	persistent_set<int> it_t;
	it_t.insert(4);
	it_t.insert(2);
	it_t.insert(3);
	it_t.insert(1);
	it_t.insert(5);
	auto itt = it_t.find(3);
	auto itt2 = ++itt;
	it_t.erase(itt);
	bool ok = (it_t.find(3) != it_t.end()) && (it_t.find(4) == it_t.end());
	it_t.insert(4);
	bool new_ok = it_t.find(4) != it_t.end();
	return 0;
}