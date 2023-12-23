
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <iterator>

namespace bryan {

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
        std::unordered_set<KeyType> keys;
    };

    // List to maintain nodes sorted by value.
    std::list<Node> nodeList_;

    // Map from keys to their corresponding node iterator in nodeList.
    std::unordered_map<KeyType, typename std::list<Node>::iterator, Hash> keyToNode_;

    // Increment the value associated with a key.
    void increment(KeyType key);

    // Decrement the value associated with a key.
    void decrement(KeyType key);

    // Get the value associated with a key.
    ValType getVal(const KeyType& key) const;

public:

    size_t size() const;
    bool empty() const;

    std::pair<KeyType, ValType> top() const;

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
void priority_map<KeyType, ValType, Compare, Hash>::increment(KeyType key) {

   auto nodeIt = keyToNode_[key];

   ValType oldVal = nodeIt->val;

   // Increment the value
   ValType newVal = oldVal + 1;

   Compare comp;

   auto nextNodeIt = comp(oldVal, newVal) ? std::next(nodeIt) : std::prev(nodeIt);

   // Remove the key from the current node
   nodeIt->keys.erase(key);

   // Check if we need to create a new node or use the next node
   if (nextNodeIt == nodeList_.end() || nextNodeIt->val != newVal) {
       // Insert a new node with the incremented value
       nextNodeIt = nodeList_.insert(nextNodeIt, {newVal, {}});
   }

   // Add the key to the next node
   nextNodeIt->keys.insert(key);

   // Update the map
   keyToNode_[key] = nextNodeIt;

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
    auto nodeIt = keyToNode_.find(key);
    if (nodeIt == keyToNode_.end()) {
        // Handle error or create a new entry with the lowest value
        return; // Or throw an exception, or handle as per the requirement
    }

    auto current_nodeIt = nodeIt->second;

    ValType oldVal = current_nodeIt->val;
    // Decrement the value
    ValType newVal = oldVal - 1;

    auto prevNodeIt = nodeList_.end();

    Compare comp;

    if (current_nodeIt != nodeList_.begin()) {
       if (comp(oldVal, newVal)) {
           prevNodeIt = std::prev(current_nodeIt);
       }
       else {
           prevNodeIt = std::next(current_nodeIt);
       }
    }

    // Remove the key from the current node
    current_nodeIt->keys.erase(key);

    // Remove the current mapping
    keyToNode_.erase(key);

    // Don't reinsert key if we have decremented it to zero
    if (newVal > 0) {

        // Check if we need to create a new node or use the previous node
        if (prevNodeIt == nodeList_.end() || prevNodeIt->val != newVal) {
            // Insert a new node with the decremented value before the current
            prevNodeIt = nodeList_.insert(current_nodeIt, {newVal, {}});
        }

        // Add the key to the previous node
        prevNodeIt->keys.insert(key);

        // Update the map
        keyToNode_[key] = prevNodeIt;
    }

    // Remove the old node if it's empty
    if (current_nodeIt->keys.empty()) {
        nodeList_.erase(current_nodeIt);
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
        nodeList_.push_front({ValType{}, {key}});
        keyToNode_[key] = nodeList_.begin();
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
priority_map<KeyType, ValType, Compare, Hash>::Proxy::operator ValType() const {
    return pm->getVal(key);
}

} // namespace
