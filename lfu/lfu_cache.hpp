#ifndef LFU_CACHE_HPP
#define LFU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <iterator>

namespace caches {

template <typename Value, typename KeyT = int>
struct lfu_cache_t {
    long long capacity_;
    long long current_size_ = 0;
    size_t hits_ = 0;

    using list_iterator = typename std::list<std::pair<KeyT, Value>>::iterator;
    using frequency = size_t;
    using cache_list = typename std::list<std::pair<KeyT, Value>>;

    std::unordered_map<KeyT, frequency>       frequency_;
    std::unordered_map<KeyT, list_iterator>   nodes_;
    std::unordered_map<frequency, cache_list> lists_;

    frequency min_frequency_ = 1;

    lfu_cache_t(long long capacity): capacity_(capacity) {};

    bool full() const {
        return current_size_ == capacity_;
    }

    size_t get_hits() const {
        return hits_;
    }

    void delete_element() {
        cache_list& min_freq_list = lists_.at(min_frequency_);
        list_iterator last_used_element = std::prev(min_freq_list.end());
        KeyT key_for_delete = last_used_element->first;

        frequency_.erase(key_for_delete);
        min_freq_list.pop_back();
        nodes_.erase(key_for_delete);
        --current_size_;
    }

    void insert_element(KeyT key, Value value) {
        min_frequency_ = 1;
        frequency_.emplace(key, min_frequency_);

        if (lists_.find(min_frequency_) == lists_.end())
            lists_[min_frequency_] = {};

        cache_list& min_freq_list = lists_.at(min_frequency_);
        min_freq_list.push_front(std::make_pair(key, value));
        list_iterator it = min_freq_list.begin();
        nodes_.emplace(key, it);
        ++current_size_;
    }
 
    void update_element(KeyT key) {
        list_iterator iterator      = nodes_.at(key);
        KeyT key_for_emplace        = iterator->first;
        Value value_for_emplace     = iterator->second;
        frequency element_frequency = frequency_.at(key);

        frequency_.erase(key);
        lists_.at(element_frequency).erase(iterator);
        nodes_.erase(key);

        if (lists_.at(element_frequency).size() == 0) 
        {
            if (element_frequency == min_frequency_)
                ++min_frequency_;
        }

        ++element_frequency;

        if (lists_.find(element_frequency) == lists_.end()) {
            lists_[element_frequency] = {};
        }

        frequency_.emplace(key, element_frequency);
        cache_list& elem_freq_list = lists_.at(element_frequency);
        elem_freq_list.emplace_front(key_for_emplace, value_for_emplace);
        nodes_.emplace(key, elem_freq_list.begin());
    }

    template <typename F> Value lookup_update(KeyT key, F slow_get_page) {
        if (nodes_.find(key) == nodes_.end()) {
            if (full())
                delete_element();

            Value value = slow_get_page(key);

            insert_element(key, value);
            return value;
        }

        update_element(key);
        ++hits_;
  
        list_iterator node_with_value = nodes_.at(key);
        return node_with_value->second;
    }
};

} // namespace caches

#endif // LFU_CACHE_HPP