#include <iostream>
#include <vector>

#include "belady_cache.hpp"

int slow_get_page_int(int key) {
    return key;
}

int main () {
    long long cache_capacity  = 0;
    long long number_of_elems = 0;
    size_t hits = 0;

    std::cin >> cache_capacity;
    if ((!std::cin.good()) || (cache_capacity < 0)) {
        std::cout << "Error input" << std::endl;
        return 1;
    }
    
    std::cin >> number_of_elems;
    if ((!std::cin.good()) || (number_of_elems < 0)) {
        std::cout << "Error input" << std::endl;
        return 1;
    }

    std::vector<int> elements;

    for (long long i = 0; i < number_of_elems; ++i) {
        int key = 0;
        std::cin >> key;
        if ((key < 0)) {
            std::cout << "Error input" << std::endl;
            return 1;
        } 
        elements.push_back(key);
    }

    belady_cache::belady_cache_t<int> cache (cache_capacity, elements);
    for (long long i = 0; i < number_of_elems; ++i) {
        if (cache.lookup_update(elements[(size_t)i], slow_get_page_int)) ++hits;
    }
    std::cout << hits << std::endl;
    return 0;
}