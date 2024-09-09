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
    size_t hits_ = 0;

    using position = size_t;

    std::unordered_map<KeyT, std::list<position>> positions_;
    std::map<position, std::pair<KeyT, Value>>    nodes_;

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

    size_t get_hits() const {
        return hits_;
    }

    bool full() const {
        return current_size_ == capacity_;
    }

    bool find_in_cache(KeyT key) const {
        auto list_for_search = positions_.find(key)->second;
        auto list_iterator   = list_for_search.front();
        return nodes_.find(list_iterator) == nodes_.end() ? false : true;
    }
    
    void push_page_into_cache(KeyT key, Value value) {
        auto& elem_pos_list = positions_.find(key)->second; 

        if (elem_pos_list.empty()) {
            positions_.erase(key);
            return;
        }

        auto& new_elem_nearest_position = elem_pos_list.front();

        if (full()) {
            auto farthest_elem = std::prev(nodes_.end());
            if (new_elem_nearest_position > farthest_elem->first) {
                elem_pos_list.pop_front();
                return;
            }

            positions_.at(farthest_elem->second.first).pop_front();
            nodes_.erase(farthest_elem);
            --current_size_;
        }
        
        nodes_.emplace(new_elem_nearest_position, std::make_pair(key, value));
        ++current_size_;
    }

    void cache_update(KeyT key) {
        auto& target_list      = positions_.find(key)->second;
        auto  nearest_position = target_list.front();
        auto  node_iterator    = nodes_.find(nearest_position);

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

    template <typename F> Value lookup_update(KeyT key, F slow_get_page) {     
        if (!find_in_cache(key)) {
            Value value = slow_get_page(key);
            push_page_into_cache(key, value);
            return value;
        }

        auto list_for_search = positions_.at(key);
        position pos = list_for_search.front();
        Value value = nodes_.at(pos).second;
        cache_update(key);
        ++hits_;
        return value;
    }
};

} // namespace belady_cache

#endif // BELADY_CACHE_HPP