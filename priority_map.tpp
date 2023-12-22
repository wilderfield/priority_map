
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <iterator>


namespace leetcode {

template<typename KeyType, typename ValType, typename Compare = std::less<ValType>>
class priority_map {

static_assert(std::is_arithmetic<ValType>::value, "ValType must be a numeric type.");

private:
    // Node structure representing a value and the set of keys associated with this value.
    struct Node {
        ValType val;
        std::unordered_set<KeyType> keys;
    };

    // List to maintain nodes sorted by value.
    std::list<Node> nodeList;

    // Map from keys to their corresponding node iterator in nodeList.
    std::unordered_map<KeyType, typename std::list<Node>::iterator> keyToNode;

    // Increment the value associated with a key.
    void increment(KeyType key);
    
    // Decrement the value associated with a key.
    void decrement(KeyType key);

public:
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
    };

    // Iterator class to iterate through the priority_map.
    class Iterator {
    private:
        typedef typename std::list<Node>::iterator NodeListIterator;
        typedef typename std::unordered_set<KeyType>::iterator KeySetIterator;

        NodeListIterator nodeListIt, endIt;
        KeySetIterator keySetIt;
        std::pair<KeyType, ValType> currentPair;

    public:
        Iterator(NodeListIterator nodeListIt, KeySetIterator keySetIt, NodeListIterator endIt)
            : nodeListIt(nodeListIt), keySetIt(keySetIt), endIt(endIt) {
            if (nodeListIt != endIt) {
                currentPair = {*keySetIt, nodeListIt->val};
            }
        }

        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        const std::pair<KeyType, ValType>& operator*() const;
        const std::pair<KeyType, ValType>* operator->() const;
    };

    // ReverseIterator class for reverse iteration.
    class ReverseIterator {
        typedef typename std::list<Node>::reverse_iterator NodeListReverseIterator;
        typedef typename std::unordered_set<KeyType>::iterator KeySetReverseIterator;

        NodeListReverseIterator nodeListIt, rendIt;
        KeySetReverseIterator keySetIt;
        std::pair<KeyType, ValType> currentPair;

    public:
        ReverseIterator(NodeListReverseIterator nodeListIt, KeySetReverseIterator keySetIt, NodeListReverseIterator rendIt)
            : nodeListIt(nodeListIt), keySetIt(keySetIt), rendIt(rendIt) {
            if (nodeListIt != rendIt) {
                currentPair = {*keySetIt, nodeListIt->val};
            }
        }

        ReverseIterator& operator++();
        bool operator==(const ReverseIterator& other) const;
        bool operator!=(const ReverseIterator& other) const;
        const std::pair<KeyType, ValType>& operator*() const;
        const std::pair<KeyType, ValType>* operator->() const;
    };

    Proxy operator[](const KeyType& key);
    Iterator begin();
    Iterator end();
    ReverseIterator rbegin();
    ReverseIterator rend();
};

// Implementation of priority_map methods
template<typename KeyType, typename ValType, typename Compare>
void priority_map<KeyType, ValType, Compare>::increment(KeyType key) {
   
   auto nodeIt = keyToNode[key];
   auto nextNodeIt = std::next(nodeIt);

   // Increment the value
   ValType newVal = nodeIt->val + 1;

   // Remove the key from the current node
   nodeIt->keys.erase(key);

   // Check if we need to create a new node or use the next node
   if (nextNodeIt == nodeList.end() || nextNodeIt->val != newVal) {
       // Insert a new node with the incremented value
       nextNodeIt = nodeList.insert(nextNodeIt, {newVal, {}});
   }

   // Add the key to the next node
   nextNodeIt->keys.insert(key);

   // Update the map
   keyToNode[key] = nextNodeIt;

   // Remove the old node if it's empty
   if (nodeIt->keys.empty()) {
       nodeList.erase(nodeIt);
   }
}

template<typename KeyType, typename ValType, typename Compare>
void priority_map<KeyType, ValType, Compare>::decrement(KeyType key) {
    auto nodeIt = keyToNode.find(key);
    if (nodeIt == keyToNode.end()) {
        // Handle error or create a new entry with the lowest value
        return; // Or throw an exception, or handle as per the requirement
    }

    auto current_nodeIt = nodeIt->second;
    auto prevNodeIt = (current_nodeIt != nodeList.begin()) ? std::prev(current_nodeIt) : nodeList.end();

    // Decrement the value
    ValType newVal = current_nodeIt->val - 1;

    // Remove the key from the current node
    current_nodeIt->keys.erase(key);

    // Don't reinsert key if we have decremented it to zero
    if (newVal > 0) {

        // Check if we need to create a new node or use the previous node
        if (prevNodeIt == nodeList.end() || prevNodeIt->val != newVal) {
            // Insert a new node with the decremented value before the current
            prevNodeIt = nodeList.insert(current_nodeIt, {newVal, {}});
        }

        // Add the key to the previous node
        prevNodeIt->keys.insert(key);

        // Update the map
        keyToNode[key] = prevNodeIt;
    }

    // Remove the old node if it's empty
    if (current_nodeIt->keys.empty()) {
        nodeList.erase(current_nodeIt);
    }
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Proxy priority_map<KeyType, ValType, Compare>::operator[](const KeyType& key) {
    if (keyToNode.find(key) == keyToNode.end()) {
        // If the key doesn't exist, create a new node with value 0
        nodeList.push_front({ValType{}, {key}});
        keyToNode[key] = nodeList.begin();
    }
    return Proxy(this, key);
}

// Implementations of Proxy methods
template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Proxy& priority_map<KeyType, ValType, Compare>::Proxy::operator++() {
    pm->increment(key);
    return *this;
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Proxy priority_map<KeyType, ValType, Compare>::Proxy::operator++(int) {
    Proxy temp = *this;
    ++(*this);
    return temp;
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Proxy& priority_map<KeyType, ValType, Compare>::Proxy::operator--() {
    pm->decrement(key);
    return *this;
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Proxy priority_map<KeyType, ValType, Compare>::Proxy::operator--(int) {
    Proxy temp = *this;
    --(*this);
    return temp;
}
    
// Implementations of Iterator methods
template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Iterator& priority_map<KeyType, ValType, Compare>::Iterator::operator++() {
    if (nodeListIt != endIt) {
        ++keySetIt;
        if (keySetIt == nodeListIt->keys.end()) {
            ++nodeListIt;
            if (nodeListIt != endIt) {
                keySetIt = nodeListIt->keys.begin();
            }
        }
        // Update Current Pair
        if (nodeListIt != endIt) {
            currentPair = {*keySetIt, nodeListIt->val};
        }
    }
    return *this;
}

template<typename KeyType, typename ValType, typename Compare>
bool priority_map<KeyType, ValType, Compare>::Iterator::operator!=(const Iterator& other) const {
    return nodeListIt != other.nodeListIt || (nodeListIt != endIt && keySetIt != other.keySetIt);
}

template<typename KeyType, typename ValType, typename Compare>
bool priority_map<KeyType, ValType, Compare>::Iterator::operator==(const Iterator& other) const {
    return nodeListIt == other.nodeListIt && (nodeListIt == endIt || keySetIt == other.keySetIt);
}

template<typename KeyType, typename ValType, typename Compare>
const std::pair<KeyType, ValType>& priority_map<KeyType, ValType, Compare>::Iterator::operator*() const {
    return currentPair;
}

template<typename KeyType, typename ValType, typename Compare>
const std::pair<KeyType, ValType>* priority_map<KeyType, ValType, Compare>::Iterator::operator->() const {
    return &currentPair;
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Iterator priority_map<KeyType, ValType, Compare>::begin() {
    if (nodeList.empty()) {
        return end();
    }
    return Iterator(nodeList.begin(), nodeList.begin()->keys.begin(), nodeList.end());
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::Iterator priority_map<KeyType, ValType, Compare>::end() {
    return Iterator(nodeList.end(), {}, nodeList.end());
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::ReverseIterator& priority_map<KeyType, ValType, Compare>::ReverseIterator::operator++() {
    if (nodeListIt != rendIt) {
        ++keySetIt;
        if (keySetIt == nodeListIt->keys.end()) {
            ++nodeListIt;
            if (nodeListIt != rendIt) {
                keySetIt = nodeListIt->keys.begin();
            }
        }
        // Update Current Pair
        if (nodeListIt != rendIt) {
            currentPair = {*keySetIt, nodeListIt->val};
        }
    }
    return *this;
}

template<typename KeyType, typename ValType, typename Compare>
bool priority_map<KeyType, ValType, Compare>::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return nodeListIt != other.nodeListIt || (nodeListIt != rendIt && keySetIt != other.keySetIt);
}

template<typename KeyType, typename ValType, typename Compare>
bool priority_map<KeyType, ValType, Compare>::ReverseIterator::operator==(const ReverseIterator& other) const {
    return nodeListIt == other.nodeListIt && (nodeListIt == rendIt || keySetIt == other.keySetIt);
}

template<typename KeyType, typename ValType, typename Compare>
const std::pair<KeyType, ValType>& priority_map<KeyType, ValType, Compare>::ReverseIterator::operator*() const {
    return currentPair;
}

template<typename KeyType, typename ValType, typename Compare>
const std::pair<KeyType, ValType>* priority_map<KeyType, ValType, Compare>::ReverseIterator::operator->() const {
    return &currentPair;
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::ReverseIterator priority_map<KeyType, ValType, Compare>::rbegin() {
    if (nodeList.empty()) {
        return rend();
    }
    auto lastNodeIt = nodeList.rbegin();
    return ReverseIterator(lastNodeIt, lastNodeIt->keys.begin(), nodeList.rend());
}

template<typename KeyType, typename ValType, typename Compare>
typename priority_map<KeyType, ValType, Compare>::ReverseIterator priority_map<KeyType, ValType, Compare>::rend() {
    return ReverseIterator(nodeList.rend(), {}, nodeList.rend());
}

} // namespace
