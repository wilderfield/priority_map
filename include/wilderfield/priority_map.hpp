
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <iterator>

namespace wilderfield {

template<
    typename KeyType,
    typename ValType,
    typename Compare = std::greater<ValType>,
    typename Hash = std::hash<KeyType>
>
class priority_map {

static_assert(std::is_arithmetic<ValType>::value, "ValType must be a numeric type.");

private:
    Compare comp_;

    // Node structure representing a value and the set of keys associated with this value.
    struct Node {
        ValType val;
        std::unordered_set<KeyType, Hash> keys;
    };

    // List to maintain nodes sorted by value.
    std::list<Node> nodeList_;

    // Map from keys to their corresponding node iterator in nodeList.
    std::unordered_map<KeyType, typename std::list<Node>::iterator, Hash> keyToNode_;

    // Update Key with Val
    // This function can be used for increment, decrement, or assigning a new val
    void update(KeyType key, ValType newVal);

    // Get the value associated with a key.
    ValType getVal(const KeyType& key) const;

public:

    size_t size() const;
    bool empty() const;

    std::pair<KeyType, ValType> top() const;

    void pop();

    class Proxy;
    Proxy operator[](const KeyType& key);

    // Proxy class to handle the increment operation.
    class Proxy {
    private:
        priority_map* pm;
        KeyType key;

    public:
        Proxy(priority_map* pm, const KeyType& key) : pm(pm), key(key) {}

        Proxy& operator++();
        Proxy operator++(int);
        Proxy& operator--();
        Proxy operator--(int);
        void operator=(const ValType& val);

	operator ValType() const; // Implicit conversion for access to val
    };

};

// Implementation of priority_map methods

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
size_t priority_map<KeyType, ValType, Compare, Hash>::size() const {
  return keyToNode_.size();
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
bool priority_map<KeyType, ValType, Compare, Hash>::empty() const {
  return keyToNode_.empty();
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
void priority_map<KeyType, ValType, Compare, Hash>::update(KeyType key, ValType newVal) {

    auto oldIt = keyToNode_[key];

    // Save Old Value
    ValType oldVal = oldIt->val;

    if (oldVal == newVal) return;

    // Remove the key from the old association
    oldIt->keys.erase(key);
    keyToNode_.erase(key);

    // Scenarios:
    // MinHeap std::less:
    //     oldVal < newVal, search from nodeIt towards end for newVal < node.val
    //     oldVal > newVal, search from nodeIt towards begin for newVal > node.val

    // MaxHeap std::greater:
    //     oldVal > newVal, search from nodeIt towards end for newVal > node.val
    //     olVal < newVal, search from nodeIt towards begin for newVal < node.val

    // True if minHeap and oldVal < newVal or if maxHeap and oldVal > newVal
    if (comp_(oldVal, newVal)) {

        // Linear search towards end
        auto insertionPoint = std::find_if(oldIt, nodeList_.end(),
        [&](const auto& node) {
            if (comp_(1,0)) {
                return newVal > node.val;
            }
            return newVal < node.val;
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
                return newVal > node.val;
            }
            return newVal < node.val;
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
        // If found, return the associated value.
        return it->second->val; // Assuming 'second' is an iterator to the list of Nodes.
    } else {
        // If not found, handle the missing key. Options include:
        // Option 1: Throw an exception.
        throw std::out_of_range("Key not found in priority_map.");

        // Option 2: Return a default value for ValType.
        // return ValType{};
    }
}


template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy priority_map<KeyType, ValType, Compare, Hash>::operator[](const KeyType& key) {
    if (keyToNode_.find(key) == keyToNode_.end()) {
        // If the key doesn't exist, create a new node with value 0

        // True if Compare is std::less
        if (comp_(0,1)) {

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
        else {

            /*

            // Max heap, insert 0
            auto insertionPoint = std::find_if(nodeList_.rbegin(), nodeList_.rend(), [](const auto& node) {
                return node.val >= 0;
            });

            if (insertionPoint != nodeList_.rend()) {
                if (insertionPoint->val == 0) {
                    // Found existing zero node
                    insertionPoint->keys.insert(key);
                    keyToNode_[key] = insertionPoint.base();
                } else {
                    // Found insertion point for new node
                    auto newNodeIt = nodeList_.insert(std::next(std::prev(insertionPoint).base()), {0, {key}});
                    keyToNode_[key] = newNodeIt;
                }
            } else {
                // Couldn't find any node with val >= 0, insert at beginning
                auto newNodeIt = nodeList_.insert(nodeList_.begin(), {0, {key}});
                keyToNode_[key] = newNodeIt;
            }

            */



            auto insertionPoint = std::find_if(nodeList_.rbegin(), nodeList_.rend(),
            [&](const auto& node) {
                return node.val >= 0;
            });

            if (insertionPoint != nodeList_.rend() && insertionPoint->val == 0) {
                insertionPoint->keys.insert(key);
                keyToNode_[key] = insertionPoint.base();
            }
            else if (insertionPoint == nodeList_.rend()) {
                auto newNodeIt = nodeList_.insert(nodeList_.begin(), {0, {key}});
                keyToNode_[key] = newNodeIt;

            }
            //else if (insertionPoint == nodeList_.rbegin()) {
            //    auto newNodeIt = nodeList_.insert(nodeList_.end(), {0, {key}});
            //    keyToNode_[key] = newNodeIt;
            //}
            else {
                auto newNodeIt = nodeList_.insert(insertionPoint.base(), {0, {key}});
                keyToNode_[key] = newNodeIt;
            }


        }

    }
    return Proxy(this, key);
}

// Implementations of Proxy methods
template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy& priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator++() {
    pm->update(key,(*pm)[key]+1);
    return *this;
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator++(int) {
    Proxy temp = *this;
    ++(*this);
    return temp;
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy& priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator--() {
    //pm->decrement(key);
    pm->update(key, (*pm)[key]-1);
    return *this;
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
typename priority_map<KeyType, ValType, Compare, Hash>::Proxy priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator--(int) {
    Proxy temp = *this;
    --(*this);
    return temp;
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
void priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator=(const ValType& val) {
    pm->update(key, val);
}

template<
    typename KeyType,
    typename ValType,
    typename Compare,
    typename Hash
>
priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator ValType() const {
    return pm->getVal(key);
}

} // namespace
