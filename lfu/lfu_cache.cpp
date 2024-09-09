#include <iostream>

#include "lfu_cache.hpp"

int slow_get_page_int(int key) {
    return key;
}

int main() {
    long long capacity = 0;
    long long number_of_elems = 0;
    size_t hits = 0;

    std::cin >> capacity >> number_of_elems;

    if ((!std::cin.good()) || (capacity < 0) || (number_of_elems < 0)) {
        std::cout << "Error input" << std::endl;
        return 1;
    }
    
    caches::lfu_cache_t<int> cache{capacity};

    for (long long i = 0; i < number_of_elems; ++i) {
        int key = 0;
        std::cin >> key;

        if ((!std::cin.good()) || (key < 0)) {
            std::cout << "Error input" << std::endl;
            return 1;
        }
        cache.lookup_update(key, slow_get_page_int);
    }   

    std::cout << cache.get_hits() << std::endl;
    return 0;
}