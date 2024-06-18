/**
 * @file priority_map.hpp
 * @brief Priority Map Template Class Definition
 *
 * Defines a templated priority map class that allows for efficient retrieval,
 * update, and tracking of keys by their associated priority values.
 */

#ifndef WILDERFIELD_PRIORITY_MAP_HPP
#define WILDERFIELD_PRIORITY_MAP_HPP

#include <unordered_map>
#include <list>
#include <unordered_set>
#include <iterator>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

namespace wilderfield {

/**
 * @brief Priority map class
 *
 * Implements a priority map where each key is associated with a priority value.
 * The map maintains the keys in sorted order based on their priority, allowing
 * for efficient retrieval and modification of priorities.
 *
 * @tparam KeyType The type of the keys.
 * @tparam ValType The type of the values (priorities), must be numeric.
 * @tparam Compare Comparison class used to maintain the ordering of values.
 * @tparam Hash Hashing class used for keys.
 */
template<
    typename KeyType,
    typename ValType,
    typename Compare = std::greater<ValType>,
    typename Hash = std::hash<KeyType>
>
class priority_map final {

static_assert(std::is_arithmetic<ValType>::value, "ValType must be a numeric type.");

private:
    Compare comp_;

    std::list<ValType> vals_; ///< List to maintain sorted values.

    std::unordered_map<KeyType, typename std::list<ValType>::iterator, Hash> keys_; ///< Map from keys to their corresponding list iterator in vals_.

    std::unordered_map<ValType, std::unordered_set<KeyType>> valToKeys_; ///< Map from vals to their corresponding keys

    // Private member functions

    // Insert new key
    void insert(const KeyType& key, const ValType& newVal);

    // Update Key with Val
    // This function can be used for increment, decrement, or assigning a new val
    void update(const KeyType& key, const ValType& newVal);

    // Get the value associated with a key.
    ValType getVal(const KeyType& key) const { return *(keys_.at(key)); }

public:

    size_t size() const { return keys_.size(); } ///< Returns the number of unique keys in the priority map.

    bool empty() const { return keys_.empty(); } ///< Checks whether the priority map is empty.

    size_t count(const KeyType& key) const { return keys_.count(key); } ///< Returns the count of a particular key in the map.

    std::pair<KeyType, ValType> top() const; ///< Returns the top element (key-value pair) in the priority map.
    
    /**
     * Retrieves the priority value associated with a given key.
     * @param key The key for which to retrieve the priority.
     * @return The priority associated with the key.
     * @throws std::out_of_range If the key does not exist in the map.
    */
    const ValType& at(const KeyType& key) const{
        auto it = keys_.find(key);  // Check if key exists
        if (it == keys_.end()) {
            throw std::out_of_range("Key not found in priority_map");
        }
        return *(it->second);  // Dereference the iterator to the list to return the value

    }
    size_t erase(const KeyType& key); ///< Erases key from the priority map. Returns the number of elements removed (0 or 1).

    void pop(); ///< Removes the top element from the priority map.

    class Proxy;
    Proxy operator[](const KeyType& key);

    // Proxy class to handle the increment operation.
    class Proxy {
    private:
        priority_map* pm;
        KeyType key;

    public:
        Proxy(priority_map* pm, const KeyType& key) : pm(pm), key(key) {}

        Proxy& operator++() {
            pm->update(key, pm->getVal(key)+1);
            return *this;

        }

        Proxy operator++(int) {
            Proxy temp = *this;
            ++(*this);
            return temp;
        }

        Proxy& operator--() {
            pm->update(key, pm->getVal(key)-1);
            return *this;
        }

        Proxy operator--(int) {
            Proxy temp = *this;
            --(*this);
            return temp;
        }
       	
        void operator=(const ValType& val) {pm->update(key, val);}

        operator ValType() const {return pm->getVal(key);}
    };

};

// Out-of-line implementation of priority_map methods

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
size_t priority_map<KeyType, ValType, Compare, Hash>::erase(const KeyType& key) {
    if (keys_.find(key) != keys_.end()) {
        auto oldIt = keys_[key];
        valToKeys_[*oldIt].erase(key);
        if (valToKeys_[*oldIt].empty()) {
            valToKeys_.erase(*oldIt); // For now avoid memory bloat
            vals_.erase(oldIt);
        }
    }
    return keys_.erase(key);
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
std::pair<KeyType, ValType> priority_map<KeyType, ValType, Compare, Hash>::top() const {
    if (vals_.empty()) {
        throw std::out_of_range("Can't access top on an empty priority_map.");
    }
    const auto val = vals_.front();
    if (valToKeys_.at(val).empty()) {
        throw std::logic_error("Inconsistent state: Val with no keys.");
    }
    // Return a pair consisting of one of the keys and the value.
    return {*(valToKeys_.at(val).begin()), val};
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::pop() {
    if (vals_.empty()) {
        throw std::out_of_range("Can't pop from empty priority_map.");
    }

    auto oldIt = vals_.begin();
    if (valToKeys_[*oldIt].empty()) {
        throw std::logic_error("Inconsistent state: Val with no keys.");
    }
    KeyType key = *(valToKeys_[*oldIt].begin());

    valToKeys_[*oldIt].erase(key);
    keys_.erase(key);

    // Remove node if it's empty
    if (valToKeys_[*oldIt].empty()) {
        valToKeys_.erase(*oldIt); // For now avoid memory bloat
        vals_.erase(oldIt);
    }
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::insert(const KeyType& key, const ValType& newVal) {

    if (keys_.find(key) == keys_.end()) {

        valToKeys_[0].insert(key);

        // True if minHeap
        if (comp_(0, 1)) {

            // Linear search towards end
            auto insertionPoint = std::find_if(vals_.begin(), vals_.end(),
            [&](const auto val) {
                return val >= 0;
            });

            if (insertionPoint == vals_.end() || (*insertionPoint) != 0) {
                insertionPoint = vals_.insert(insertionPoint, 0);
            }

            keys_[key] = insertionPoint;

        }
        // maxHeap
        else {

            // Linear search towards begin
            auto insertionPoint = std::find_if(vals_.rbegin(), vals_.rend(),
            [&](const auto val) {
                return val >= 0;
            });

            if (insertionPoint == vals_.rend() || (*insertionPoint) != 0) {
                auto newIt = vals_.insert(insertionPoint.base(), 0);
                keys_[key] = newIt;
            }
            else {
                keys_[key] = std::next(insertionPoint).base();
            }
        }

    }

    update(key, newVal);

}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::update(const KeyType& key, const ValType& newVal) {

    auto oldIt = keys_[key];

    // Save Old Value
    ValType oldVal = *oldIt;

    if (oldVal == newVal) return;

    // Remove the key from the old association
    keys_.erase(key);
    valToKeys_[oldVal].erase(key);

    // Make new association
    valToKeys_[newVal].insert(key);

    // True if minHeap and oldVal < newVal or if maxHeap and oldVal > newVal
    if (comp_(oldVal, newVal)) {

        // Linear search towards end
        auto insertionPoint = std::find_if(oldIt, vals_.end(),
        [&](const auto val) {
            if (comp_(1,0)) {
                return val <= newVal;
            }
            return val >= newVal;
        });

        if (insertionPoint == vals_.end() || (*insertionPoint) != newVal) {
            insertionPoint = vals_.insert(insertionPoint, newVal);
        }

        keys_[key] = insertionPoint;

    }
    // minHeap and oldVal > newVal or maxHeap and oldVal < newVal
    else {
        // Need to search in reverse
        typename std::list<ValType>::reverse_iterator oldRit(oldIt); // oldRit will point one element closer to begin

        // Linear search towards begin
        auto insertionPoint = std::find_if(oldRit, vals_.rend(),
        [&](const auto val) {
            if (comp_(0,1)) {
                return val <= newVal;
            }
            return val >= newVal;
        });

        if (insertionPoint == vals_.rend() || (*insertionPoint) != newVal) {
            auto newIt = vals_.insert(insertionPoint.base(), newVal);
            keys_[key] = newIt;
            keys_[key] = std::next(insertionPoint).base();
        }
        else {
            keys_[key] = std::next(insertionPoint).base();
        }
    }

    // Remove the old node if it's empty
    if (valToKeys_[oldVal].empty()) {
        valToKeys_.erase(*oldIt); // For now avoid memory bloat
        vals_.erase(oldIt);
    }

}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy priority_map<KeyType, ValType, Compare, Hash>::operator[](const KeyType& key) {

    // If the key doesn't exist, create a new node with value 0
    if (keys_.find(key) == keys_.end()) {
        insert(key, 0);
    }
    return Proxy(this, key);
}

} // namespace

#endif // WILDERFIELD_PRIORITY_MAP_HPP
