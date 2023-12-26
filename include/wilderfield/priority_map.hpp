
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
    // Node structure representing a value and the set of keys associated with this value.
    struct Node {
        ValType val;
        std::unordered_set<KeyType, Hash> keys;
    };

    // List to maintain nodes sorted by value.
    std::list<Node> nodeList_;

    // Map from keys to their corresponding node iterator in nodeList.
    std::unordered_map<KeyType, typename std::list<Node>::iterator, Hash> keyToNode_;

    // Increment the value associated with a key.
    void increment(KeyType key);

    // Decrement the value associated with a key.
    void decrement(KeyType key);

    // Create a new key : val mapping
    void emplace(KeyType key, ValType val);

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
void priority_map<KeyType, ValType, Compare, Hash>::increment(KeyType key) {

    auto nodeIt = keyToNode_[key];

    // Remove the key from the current node
    nodeIt->keys.erase(key);

    // Remove the current mapping
    keyToNode_.erase(key);

    ValType oldVal = nodeIt->val;

    // Increment the value
    ValType newVal = oldVal + 1;

    Compare comp; // Does this have performance hit? Maybe optimize.

    // This will evaluate true it std::less is used
    // If std::less is used we mimic a minheap
    // The key with the lowest val will be at the head of the linked list
    // We insert nodes towards the tail of the LL
    if (comp(oldVal, newVal)) {


        auto nextNodeIt = std::next(nodeIt);

        // Check if we need to create a new node or use the next node
        if (nextNodeIt == nodeList_.end() || nextNodeIt->val != newVal) {
            // Insert a new node with the incremented value
            nextNodeIt = nodeList_.insert(nextNodeIt, {newVal, {}});
        }

        // Add the key to the next node
        nextNodeIt->keys.insert(key);

        // Update the map
        keyToNode_[key] = nextNodeIt;

    }
    // std::greater is used
    // We mimic a maxheap
    // The key with the greatest val will be at the head of the linked list
    // We insert nodes towards the head of the LL
    else {
        auto prevNodeIt = (nodeIt != nodeList_.begin()) ? std::prev(nodeIt) : nodeList_.end();

        // Check if we need to create a new node or use the next node
        if (prevNodeIt == nodeList_.end() || prevNodeIt->val != newVal) {
            // Insert a new node with the incremented value
            prevNodeIt = nodeList_.insert(nodeIt, {newVal, {}});
        }

        // Add the key to the previous node
        prevNodeIt->keys.insert(key);

        // Update the map
        keyToNode_[key] = prevNodeIt;

    }

    // Remove the old node if it's empty
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
void priority_map<KeyType, ValType, Compare, Hash>::decrement(KeyType key) {

    auto nodeIt = keyToNode_[key];

    // Remove the key from the current node
    nodeIt->keys.erase(key);

    // Remove the current mapping
    keyToNode_.erase(key);

    ValType oldVal = nodeIt->val;

    // Decrement the value
    ValType newVal = oldVal - 1;

    Compare comp; // Does this have performance hit? Maybe optimize.

    // This will evaluate true if std::greater is used
    if (comp(oldVal, newVal)) {

        auto nextNodeIt = std::next(nodeIt);

        // Check if we need to create a new node or use the next node
        if (nextNodeIt == nodeList_.end() || nextNodeIt->val != newVal) {
            // Insert a new node with the incremented value
            nextNodeIt = nodeList_.insert(nextNodeIt, {newVal, {}});
        }

        // Add the key to the next node
        nextNodeIt->keys.insert(key);

        // Update the map
        keyToNode_[key] = nextNodeIt;

    }
    // std::greater is used
    // We mimic a maxheap
    // The key with the greatest val will be at the head of the linked list
    // We insert nodes towards the head of the LL
    else {
        auto prevNodeIt = (nodeIt != nodeList_.begin()) ? std::prev(nodeIt) : nodeList_.end();

        // Check if we need to create a new node or use the next node
        if (prevNodeIt == nodeList_.end() || prevNodeIt->val != newVal) {
            // Insert a new node with the incremented value
            prevNodeIt = nodeList_.insert(nodeIt, {newVal, {}});
        }

        // Add the key to the previous node
        prevNodeIt->keys.insert(key);

        // Update the map
        keyToNode_[key] = prevNodeIt;

    }

    // Remove the old node if it's empty
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
void priority_map<KeyType, ValType, Compare, Hash>::emplace(KeyType key, ValType val) {

    auto nodeIt = keyToNode_[key];

    // Remove the key from the current node
    nodeIt->keys.erase(key);

    // Remove the current mapping
    keyToNode_.erase(key);

    ValType oldVal = nodeIt->val;

    ValType newVal = val;

    Compare comp; // Does this have performance hit? Maybe optimize.

    // True if Compare is std::less
    if (comp(0,1)) {
            auto insertionPoint = std::find_if(nodeList_.begin(), nodeList_.end(),
            [&](const auto& node) {
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
    else {
            auto insertionPoint = std::find_if(nodeList_.begin(), nodeList_.end(),
            [&](const auto& node) {
                return node.val <= newVal;
            });

            if (insertionPoint != nodeList_.end() && insertionPoint->val == newVal) {
                insertionPoint->keys.insert(key);
            }
            else {
                insertionPoint = nodeList_.insert(insertionPoint, {newVal, {key}});
            }

            keyToNode_[key] = insertionPoint;

    }

    // Remove the old node if it's empty
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

        Compare comp;

        // True if Compare is std::less
        if (comp(0,1)) {

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
    pm->increment(key);
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
    pm->decrement(key);
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
    pm->emplace(key, val);
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
