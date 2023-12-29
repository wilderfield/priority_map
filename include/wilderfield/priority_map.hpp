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

    struct Node {
        ValType val; ///< The priority value.
        std::unordered_set<KeyType, Hash> keys; ///< Set of keys associated with this value.
    };

    std::list<Node> nodeList_; ///< List to maintain nodes sorted by value.

    // Map from keys to their corresponding node iterator in nodeList.
    std::unordered_map<KeyType, typename std::list<Node>::iterator, Hash> keyToNode_; ///< Map from keys to their corresponding node iterator in nodeList.

    // Private member functions

    // Insert new key
    void insert(const KeyType& key, const ValType& newVal);

    // Update Key with Val
    // This function can be used for increment, decrement, or assigning a new val
    void update(const KeyType& key, const ValType& newVal);

    // Get the value associated with a key.
    ValType getVal(const KeyType& key) const;

public:

    size_t size() const { return keyToNode_.size(); } ///< Returns the number of unique keys in the priority map.
    
    bool empty() const { return keyToNode_.empty(); } ///< Checks whether the priority map is empty.
    
    size_t count(const KeyType& key) const { return keyToNode_.count(key); } ///< Returns the count of a particular key in the map.
    
    std::pair<KeyType, ValType> top() const; ///< Returns the top element (key-value pair) in the priority map.
    
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
    if (keyToNode_.find(key) != keyToNode_.end()) {
        auto oldIt = keyToNode_[key];
        oldIt->keys.erase(key);
        if (oldIt->keys.empty()) {
            nodeList_.erase(oldIt);
        }
    }
    return keyToNode_.erase(key);
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
std::pair<KeyType, ValType> priority_map<KeyType, ValType, Compare, Hash>::top() const {
    if (nodeList_.empty()) {
        throw std::out_of_range("Can't access top on an empty priority_map.");
    }
    const auto& node = nodeList_.front();
    if (node.keys.empty()) {
        throw std::logic_error("Inconsistent state: Node with no keys.");
    }
    // Return a pair consisting of one of the keys and the value.
    return {*(node.keys.begin()), node.val};
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::pop() {
    if (nodeList_.empty()) {
        throw std::out_of_range("Can't pop from empty priority_map.");
    }

    auto nodeIt = nodeList_.begin();
    if (nodeIt->keys.empty()) {
        throw std::logic_error("Inconsistent state: Node with no keys.");
    }
    KeyType key = *(nodeIt->keys.begin());

    nodeIt->keys.erase(key);

    keyToNode_.erase(key);

    // Remove node if it's empty
    if (nodeIt->keys.empty()) {
        nodeList_.erase(nodeIt);
    }
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::insert(const KeyType& key, const ValType& newVal) {

    if (keyToNode_.find(key) == keyToNode_.end()) {

        // True if minHeap
        if (comp_(0, 1)) {

            // Linear search towards end
            auto insertionPoint = std::find_if(nodeList_.begin(), nodeList_.end(),
            [&](const auto& node) {
                return node.val >= 0;
            });

            if (insertionPoint != nodeList_.end() && insertionPoint->val == 0) {
               insertionPoint->keys.insert(key);
            }
            else {
                insertionPoint = nodeList_.insert(insertionPoint, {0, {key}});
            }

            keyToNode_[key] = insertionPoint;

        }
        // maxHeap
        else {

            // Linear search towards begin
            auto insertionPoint = std::find_if(nodeList_.rbegin(), nodeList_.rend(),
            [&](const auto& node) {
                return node.val >= 0;
            });

            if (insertionPoint != nodeList_.rend() && insertionPoint->val == 0) {
                insertionPoint->keys.insert(key);
                keyToNode_[key] = std::next(insertionPoint).base();
            }
            else {
                auto newIt = nodeList_.insert(insertionPoint.base(), {0, {key}});
                keyToNode_[key] = newIt;
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

    auto oldIt = keyToNode_[key];

    // Save Old Value
    ValType oldVal = oldIt->val;

    if (oldVal == newVal) return;

    // Remove the key from the old association
    oldIt->keys.erase(key);
    keyToNode_.erase(key);

    // True if minHeap and oldVal < newVal or if maxHeap and oldVal > newVal
    if (comp_(oldVal, newVal)) {

        // Linear search towards end
        auto insertionPoint = std::find_if(oldIt, nodeList_.end(),
        [&](const auto& node) {
            if (comp_(1,0)) {
                return node.val <= newVal;
            }
            return node.val >= newVal;
        });

        if (insertionPoint != nodeList_.end() && insertionPoint->val == newVal) {
           insertionPoint->keys.insert(key);
        }
        else {
            insertionPoint = nodeList_.insert(insertionPoint, {newVal, {key}});
        }

        keyToNode_[key] = insertionPoint;

    }
    // minHeap and oldVal > newVal or maxHeap and oldVal < newVal
    else {
        // Need to search in reverse
        typename std::list<Node>::reverse_iterator oldRit(oldIt); // oldRit will point one element closer to begin

        // Linear search towards begin
        auto insertionPoint = std::find_if(oldRit, nodeList_.rend(),
        [&](const auto& node) {
            if (comp_(0,1)) {
                return node.val <= newVal;
            }
            return node.val >= newVal;
        });

        if (insertionPoint != nodeList_.rend() && insertionPoint->val == newVal) {
            insertionPoint->keys.insert(key);
            keyToNode_[key] = std::next(insertionPoint).base();
        }
        else {
            auto newIt = nodeList_.insert(insertionPoint.base(), {newVal, {key}});
            keyToNode_[key] = newIt;
        }
    }

    // Remove the old node if it's empty
    if (oldIt->keys.empty()) {
        nodeList_.erase(oldIt);
    }

}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
ValType priority_map<KeyType, ValType, Compare, Hash>::getVal(const KeyType& key) const {
    auto it = keyToNode_.find(key); // Try to find the key in the map.
    if (it != keyToNode_.end()) {
        return it->second->val;
    }
    // If not found, handle the missing key.
    throw std::out_of_range("Key not found in priority_map.");
    return ValType{};
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy priority_map<KeyType, ValType, Compare, Hash>::operator[](const KeyType& key) {

    // If the key doesn't exist, create a new node with value 0
    if (keyToNode_.find(key) == keyToNode_.end()) {
        insert(key, 0);
    }
    return Proxy(this, key);
}

} // namespace

#endif // WILDERFIELD_PRIORITY_MAP_HPP