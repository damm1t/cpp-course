#include "fixed_vector.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <ctime>
#include <vector>

int main() {
    //test1
    std::cout << "Test1: ";

    const size_t N = 1000;
    fixed_vector<int, N> a;
    for(int i = 0; i < N; ++i){
        a.push_back(i);
        assert(a[i] == i);
    }
    std::cout << "OK\n";
    //test2
    std::cout << "Test2: ";

    fixed_vector<int, N> b = a;
    for(int i = 0; i < N; ++i){
        assert(b[i] == i);
    }
    std::cout << "OK\n";

    //test3
    std::cout << "Test3: ";
    int id = 0;
    for(auto it = b.begin(); it != b.end(); ++it){
        assert(*it == id++);
    }
    std::cout << "OK\n";

    //test4
    std::cout << "Test4: ";
    assert(*a.rend() == 0);
    std::cout << "OK\n";

    //test5
    std::cout << "Test5: ";
    assert(a.back() == N - 1);
    std::cout << "OK\n";
    return 0;
}
