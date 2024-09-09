#include <iostream>

#include "lfu_cache.hpp"

int slow_get_page_int(int key) {
    return key;
}

int main() {
    long long capacity = 0;
    long long n = 0;
    size_t hits = 0;

    std::cin >> capacity >> n;

    if ((!std::cin.good()) || (capacity < 0) || (n < 0)) {
        std::cout << "Error input" << std::endl;
        return 1;
    }
    
    caches::lfu_cache_t<int> c{capacity};

    for (long long i = 0; i < n; ++i) {
        int key = 0;
        std::cin >> key;

        if ((!std::cin.good()) || (key < 0)) {
            std::cout << "Error input" << std::endl;
            return 1;
        }

        if (c.lookup_update(key, slow_get_page_int))
            ++hits;
    }   

    std::cout << hits << std::endl;
    return 0;
}