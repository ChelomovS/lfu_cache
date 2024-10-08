#ifndef BELADY_CACHE_HPP
#define BELADY_CACHE_HPP

#include <vector>
#include <iterator>
#include <unordered_map>
#include <list>
#include <map>

namespace belady_cache {

template <typename Value, typename KeyT>
struct belady_cache_t {
    size_t capacity_;
    size_t hits_ = 0;
    size_t current_size_;

    using position = size_t;

    std::unordered_map<KeyT, std::list<position>> positions_;
    std::map<position, std::pair<KeyT, Value>>    nodes_;

    using positions_iter = 
                typename std::unordered_map<KeyT, std::list<position>>::iterator;
    using positions_list = typename std::list<position>;
    using pos_list_iterator = typename std::list<position>::iterator;
    using nodes_iter = typename std::map<position, std::pair<KeyT, Value>>::iterator;

    template <typename Iterator>
    belady_cache_t(size_t size_of_cache, Iterator begin, Iterator end):
                                                capacity_(size_of_cache) {
        position pos = 0;
        KeyT element = 0;

        for (Iterator i = begin; i < end; i++) {
            element = *i;
            if (!positions_.contains(element)) {
                positions_list new_list{};
                positions_.emplace(element, new_list);
            }
            else {
                positions_list& list_for_push = positions_.at(element);
                list_for_push.push_back(pos);
            }
            ++pos;
        }
    }

    size_t get_hits() const {
        return hits_;
    }

    bool full() const {
        return current_size_ == capacity_;
    }

    bool find_in_cache(const KeyT& key) const {
        const positions_list& list_for_search = positions_.at(key);
        position front_elem_pos = list_for_search.front();
        return nodes_.contains(front_elem_pos);
    }

    void push_page_into_cache(const KeyT& key, const Value& value) {
        positions_iter key_it = positions_.find(key);
        assert(key_it != positions_.end());

        positions_list& elem_pos_list = key_it->second; 

        if (elem_pos_list.empty()) {
            positions_.erase(key);
            return;
        }

        position new_elem_nearest_position = elem_pos_list.front();

        if (full()) {
            assert(!nodes_.empty());
            nodes_iter farthest_elem_it = std::prev(nodes_.end());
            assert(farthest_elem_it != nodes_.end());
            
            if (new_elem_nearest_position > farthest_elem_it->first) {
                elem_pos_list.pop_front();
                return;
            }

            KeyT& key_of_farhest_elem = farthest_elem_it->second.first;

            positions_.at(key_of_farhest_elem).pop_front();
            nodes_.erase(farthest_elem_it);
            --current_size_;
        }

        nodes_.emplace(new_elem_nearest_position, std::make_pair(key, value));
        ++current_size_;
    }

    void cache_update(const KeyT& key) {
        positions_iter key_it = positions_.find(key);
        assert(key_it != positions_.end());

        positions_list& target_list = key_it->second;
        position nearest_position = target_list.front();
        nodes_iter node_iterator = nodes_.find(nearest_position);
        assert(node_iterator != nodes_.end());

        target_list.pop_front();

        if (target_list.empty()) {
            positions_.erase(key);            
            nodes_.erase(node_iterator);
            --current_size_;
        }
        else {
            position next_pos = target_list.front();
            nodes_.emplace(next_pos, node_iterator->second);
            nodes_.erase(node_iterator);
        }
    }

    template <typename F> Value lookup_update(const KeyT& key, F slow_get_page) {     
        if (!find_in_cache(key)) {
            Value value = slow_get_page(key);
            push_page_into_cache(key, value);
            return value;
        }

        positions_list& list_for_search = positions_.at(key);
        position pos = list_for_search.front();
        Value value = nodes_.at(pos).second;
        cache_update(key);
        ++hits_;
        return value;
    }
};

} // namespace belady_cache

#endif // BELADY_CACHE_HPP