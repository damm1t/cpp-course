#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include <gtest/gtest.h>

#include "data_ptr.h"
//
//TEST(correctness, two_plus_two)
//{
//	EXPECT_EQ(big_integer(2) + big_integer(2), big_integer(4));
//	EXPECT_EQ(big_integer(2) + 2, 4); // implicit converion from int must work
//	EXPECT_EQ(2 + big_integer(2), 4);
//}