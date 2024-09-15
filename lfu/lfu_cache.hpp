#ifndef LFU_CACHE_HPP
#define LFU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <iterator>
#include <cassert>

namespace caches {

template <typename Value, typename KeyT>
struct lfu_cache_t {
    size_t capacity_;
    size_t hits_ = 0;

    using list_iterator = typename std::list<std::pair<KeyT, Value>>::iterator;
    using frequency = size_t;
    using cache_list = typename std::list<std::pair<KeyT, Value>>;

    std::unordered_map<KeyT, frequency>       frequency_;
    std::unordered_map<KeyT, list_iterator>   nodes_;
    std::unordered_map<frequency, cache_list> lists_;

    frequency min_frequency_ = 1;

    lfu_cache_t(size_t capacity): capacity_(capacity) {};

    bool full() const {
        return nodes_.size() == capacity_;
    }

    size_t get_hits() const {
        return hits_;
    }

    void delete_element() {
        cache_list& min_freq_list = lists_.at(min_frequency_);
        assert(!min_freq_list.empty());

        list_iterator last_used_element_it = std::prev(min_freq_list.end());
        assert(last_used_element_it != min_freq_list.end());

        KeyT& key_for_delete = last_used_element_it->first;
        frequency_.erase(key_for_delete);
        nodes_.erase(key_for_delete);
        min_freq_list.pop_back();
    }

    void insert_element(KeyT key, Value value) {
        min_frequency_ = 1;
        frequency_.emplace(key, min_frequency_);

        cache_list new_list{};
        lists_.emplace(min_frequency_, new_list);

        cache_list& min_freq_list = lists_.at(min_frequency_);
        min_freq_list.push_front(std::make_pair(key, value));
        list_iterator it = min_freq_list.begin();
        nodes_.emplace(key, it);
    }
 
    void update_element(KeyT key) {
        list_iterator iterator = nodes_.at(key);
        frequency element_frequency = frequency_.at(key);
        cache_list& elem_old_freq_list = lists_.at(element_frequency);
        assert(iterator != elem_old_freq_list.end());

        KeyT key_for_emplace = iterator->first;
        Value value_for_emplace = iterator->second;

        frequency_.erase(key);
        elem_old_freq_list.erase(iterator);
        nodes_.erase(key);

        if ((lists_.at(element_frequency).empty()) && 
                                        (element_frequency == min_frequency_))
            ++min_frequency_;

        ++element_frequency;

        cache_list new_list{};
        lists_.emplace(element_frequency, new_list);

        frequency_.emplace(key_for_emplace, element_frequency);
        cache_list& elem_new_freq_list = lists_.at(element_frequency);
        elem_new_freq_list.emplace_front(key_for_emplace, value_for_emplace);
        nodes_.emplace(key_for_emplace, elem_new_freq_list.begin());
    }

    template <typename F> Value lookup_update(KeyT key, F slow_get_page) {
        if (!nodes_.contains(key)) {
            if (full())
                delete_element();

            Value value = slow_get_page(key);

            insert_element(key, value);
            return value;
        }

        update_element(key);
        ++hits_;

        list_iterator node_with_value = nodes_.at(key);
        frequency elem_freq = frequency_.at(key);
        const cache_list& list_with_elem = lists_.at(elem_freq);
        assert(node_with_value != list_with_elem.end());
        return node_with_value->second;
    }
};

} // namespace caches

#endif // LFU_CACHE_HPP