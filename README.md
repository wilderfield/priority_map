\mainpage
# wilderfield::priority_map
The `priority_map` class is designed to manage a set of elements, each with a priority,  
allowing for efficient retrieval, updating, and tracking of elements based on their priority.  
  
It is designed to provide constant time access to the element with the highest priority,  
as well as constant time priority increment and decrement.  
  
Setting priority for a given element directly is O(n).  
  
The underlying implementation is a doubly linked list backed by a hashmap.  
  
The source code for this project is available on GitHub: [wilderfield/priority_map](https://github.com/wilderfield/priority_map)

# usage
Here is a basic example of how to use the `priority_map` class:
```cpp
#include "wilderfield/priority_map.hpp"
#include <iostream>

int main() {

  // Define a priority_map instance
  wilderfield::priority_map<int, int> pmap;

  // Increment priority of key 7
  ++pmap[7];

  // Retrieve and check the top element
  auto [key, value] = pmap.top();
  std::cout << "Top element key: " << key << " with priority: " << value << std::endl;

  return 0;
}
```

# Example Application

Below is an example application illustrating topological sort with Khan's algorithm and a priority map:

```cpp
#include "wilderfield/priority_map.hpp"
#include <vector>
#include <cassert>

std::vector<int> topSort(std::vector<std::vector<int>>& graph) {
 
   std::vector<int> result;
 
   // Construct priority map with lower values as highest priority
   wilderfield::priority_map<int, int, std::less<int>> pmap;
 
   // Initialize pmap to have all nodes with indegree 0
   for (int u = 0; u < graph.size(); u++) {
     pmap[u] = 0;
   }
 
   // Calculate indegrees for each node
   for (int u = 0; u < graph.size(); u++) {
     for (auto v : graph[u]) {
         ++pmap[v];
     }
   }
 
   // Iterate over nodes by smallest indegree
   while (!pmap.empty()) {
     auto [u, minVal] = pmap.top(); pmap.pop();
     assert(minVal == 0); // If not the graph has a cycle
     result.push_back(u);
     for (auto v : graph[u]) { // Decrease indegrees after removing node u
       --pmap[v];
     }
   }
 
   return result;
}
```

# build and test
```
mkdir build
cd build
cmake ..
cmake —-build . —-config Release
./tests/priority_map_test -s
```

# documentation 
For more information and advanced usage, refer to the respective class and method documentation.  
  
The documentation for this project is available on GitHub: [wilderfield/priority_map documentation](https://wilderfield.github.io/priority_map)
