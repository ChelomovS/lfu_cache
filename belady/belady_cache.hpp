#ifndef BELADY_CACHE_HPP
#define BELADY_CACHE_HPP

#include <vector>
#include <iterator>
#include <unordered_map>
#include <list>
#include <map>

namespace belady_cache {

template <typename Value, typename KeyT = int>
struct belady_cache_t {
    long long capacity_;
    long long current_size_ = 0;

    struct node_t {
        KeyT key_;
        Value value_;
    };

    using position = size_t;

    std::unordered_map<KeyT, std::list<position>> positions_;
    std::map<position, node_t> nodes_;

    belady_cache_t(long long size_of_cache, std::vector<int>& elements): capacity_(size_of_cache) {
        position pos = 0;

        for (auto element : elements) {
            auto pos_iterator = positions_.find(element);
            if (pos_iterator == positions_.end())
                positions_[element] = {};
            else
                pos_iterator->second.push_back(pos); 
            ++pos;                
        }
    }

    bool full() const {
        return current_size_ == capacity_;
    }

    bool find_in_cache(KeyT key) const {
        auto list_for_search = positions_.find(key)->second;
        auto list_iterator = list_for_search.front();
        return nodes_.find(list_iterator) == nodes_.end() ? false : true;
    }
    
    template <typename F> void push_page_into_cache(KeyT key, F slow_get_page) {
        auto& list_for_search = positions_.find(key)->second; 

        if (list_for_search.empty()) {
            positions_.erase(key);
            return;
        }

        auto& new_elem_nearest_position = list_for_search.front();

        if (full()) {
            auto farthest_elem = std::prev(nodes_.end());
            if (new_elem_nearest_position > farthest_elem->first) {
                list_for_search.pop_front();
                return;
            }

            positions_[farthest_elem->second.key_].pop_front();
            nodes_.erase(farthest_elem);
            --current_size_;
        }

        nodes_.emplace(new_elem_nearest_position, node_t {key, slow_get_page(key)});
        ++current_size_;
    }

    void cache_update(KeyT key) {
        auto& target_list = positions_.find(key)->second;
        auto nearest_position = target_list.front();

        auto node_iterator = nodes_.find(nearest_position);

        target_list.pop_front();
 
        if (target_list.empty()) {
            nodes_.erase(node_iterator);
            positions_.erase(key);
            --current_size_;
            return;
        }

        auto next_pos = target_list.front();
        nodes_.emplace(next_pos, node_iterator->second);
        nodes_.erase(node_iterator);
    }

    template <typename F> bool lookup_update(KeyT key, F slow_get_page) {
        if (capacity_ == 0) return false;
        
        if (!find_in_cache(key)) {
            push_page_into_cache(key, slow_get_page);
            return false;
        }
        else {
            cache_update(key);
            return true;
        }
    }
};

}; // namespace belady_cache

#endif // BELADY_CACHE_HPP