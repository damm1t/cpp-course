#include "fixed_vector.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <ctime>
#include <vector>

int main2() {
    uint64_t fixed_vector_clocks = 0;
    uint64_t vector_clocks = 0;
    uint64_t vector_reserved_clocks = 0;
    const size_t N = 10000000;
    time_t now = clock();
    fixed_vector<int, N> a;
    for (int i = 0; i < N; i++) {
        a.push_back(i);
        assert(a[i] == i);
        a[i]++;
        assert(a[i] == i + 1);
    }
    for (int i = 0; i < N; i++)
        a.pop_back();
    time_t res = clock();
    fixed_vector_clocks += (res - now);
    now = clock();
    std::vector<int> b;
    for (int i = 0; i < N; i++)
        b.push_back(i);
    for (int i = 0; i < N; i++)
        b.pop_back();
    res = clock();
    vector_clocks += res - now;
    now = clock();
    std::vector<int> c;
    c.reserve(N);
    for (int i = 0; i < N; i++)
        c.push_back(i);
    for (int i = 0; i < N; i++)
        c.pop_back();
    res = clock();
    vector_reserved_clocks += res - now;
    std::cout << "Perfomance benchmark\nfixed_capacity_vector: "
              << (long double)fixed_vector_clocks / CLOCKS_PER_SEC
              << " sec\nstl vector: " << (long double)vector_clocks / CLOCKS_PER_SEC
              << " sec\nstl vector with reserved capacity "
              << (long double)vector_reserved_clocks / CLOCKS_PER_SEC << " sec" << std::endl;
    return 0;
}