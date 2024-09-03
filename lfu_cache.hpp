#ifndef LFU_CACHE_HPP
#define LFU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <iterator>
#include <assert.h>

namespace caches {

template <typename Value, typename KeyT = int>
struct lfu_cache_t {
    long long current_size_ = 0;
    long long capacity_;
    using list_iterator = typename std::list<std::pair<KeyT, Value> >::iterator;
    using frequency = size_t;
    using cache_list = typename std::list<std::pair<KeyT, Value>>;
    std::unordered_map<KeyT, frequency> frequency_;
    std::unordered_map<KeyT, list_iterator> nodes_;
    std::unordered_map<frequency, cache_list> lists_;

    frequency min_frequency_ = 1;

    lfu_cache_t(long long capacity): capacity_(capacity) {};

    bool full() const {
        return current_size_ == capacity_;
    }

    void delete_element() {
        list_iterator last_used_element = std::prev(lists_.at(min_frequency_).end());
        KeyT key_for_delete = last_used_element->first;
        frequency_.erase(key_for_delete);
        lists_.at(min_frequency_).pop_back();
        nodes_.erase(key_for_delete);
        --current_size_;
    }

    template <typename F> void insert_element(KeyT key, F slow_get_page) {
        min_frequency_ = 1;
        frequency_.emplace(key, min_frequency_);

        if (lists_.count(min_frequency_) == 0) {
            lists_[min_frequency_] = {};//REVIEW - 
        }

        lists_.at(min_frequency_).emplace_front(key, slow_get_page(key));
        list_iterator node = lists_.at(min_frequency_).begin();                                         //REVIEW - 
        nodes_.emplace(key, node);
        ++current_size_;
    }
 
    void update_element(KeyT key) {
        list_iterator iterator = nodes_.at(key);
        KeyT key_emplace = iterator->first;
        Value value_emplace = iterator->second;
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
        lists_.at(element_frequency).emplace_front(key_emplace, value_emplace);
        nodes_.emplace(key, lists_.at(element_frequency).begin());
    }

    template <typename F> bool lookup_update(KeyT key, F slow_get_page) {
        if (capacity_ == 0) return false;

        if (nodes_.count(key) == 0) {
            if (full()) delete_element();
            insert_element(key, slow_get_page);
            return false;
        }

        update_element(key);
        return true;
    }
};

} // namespace caches

#endif // LFU_CACHE_HPP