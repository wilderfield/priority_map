#include <iostream>
#include <vector>

#include "priority_map.tpp"

using namespace leetcode;

int main() {
    priority_map<int, int> pm;
    std::vector<int> nums = {5, 3, 5, 7, 9, 3};

    for (auto num : nums) {
        ++pm[num];
    }

    --pm[5];

    for (auto it = pm.begin(); it != pm.end(); ++it) {
        std::cout << "Key: " << it->first << ", Val: " << it->second << std::endl;
    }

    std::cout << "Reverse iteration:" << std::endl;
    for (auto rit = pm.rbegin(); rit != pm.rend(); ++rit) {
        std::cout << "Key: " << rit->first << ", Val: " << rit->second << std::endl;
    }

    return 0;
}

