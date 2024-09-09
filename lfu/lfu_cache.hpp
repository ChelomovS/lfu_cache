#ifndef LFU_CACHE_HPP
#define LFU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <iterator>
#include <assert.h>

namespace caches {

template <typename Value, typename KeyT = int>
struct lfu_cache_t {
    long long capacity_;
    long long current_size_ = 0;
    size_t hits_ = 0;

    using list_iterator = typename std::list<std::pair<KeyT, Value> >::iterator;
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

        if (lists_.count(min_frequency_) == 0) {
            lists_[min_frequency_] = {};
        }
        cache_list& min_freq_list = lists_.at(min_frequency_);
        min_freq_list.emplace_front(key, value);
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

        if (lists_.count(element_frequency) == 0) {
            lists_[element_frequency] = {};
        }

        frequency_.emplace(key, element_frequency);
        cache_list& elem_freq_list = lists_.at(element_frequency);
        elem_freq_list.emplace_front(key_for_emplace, value_for_emplace);
        nodes_.emplace(key, elem_freq_list.begin());
    }

    template <typename F> Value lookup_update(KeyT key, F slow_get_page) {
        if (nodes_.count(key) == 0) {
            if (full()) 
                delete_element();

            Value value = slow_get_page(key);

            insert_element(key, value);
            return value;
        }

        update_element(key); 
        ++hits_;
        frequency  elem_freq       = frequency_.at(key);        
        cache_list list_for_search = lists_.at(elem_freq);
        Value      value = list_for_search.begin()->second;
        return value;
    }
};

} // namespace caches

#endif // LFU_CACHE_HPP