#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"
#include "bryan/priority_map.hpp" // Your priority_map implementation

TEST_CASE("PriorityMap operations are tested", "[priority_map]") {
    // Instantiate your priority_map
    bryan::priority_map<int, int> pmap;

    // Test case for insertion
    SECTION("Insertion works correctly") {
        pmap[7]++; // Increment value for key 7
        auto val = pmap.begin();
        REQUIRE(val->first == 7);  // Check key is 7
        REQUIRE(val->second == 1); // Check value is 1 after increment
    }
    
    // Test case for insertion and deletion
    SECTION("Insertion and Deletion works correctly") {
        pmap[7]++; // Increment value for key 7
        pmap[7]--; // Increment value for key 7
        auto it = pmap.begin();
        REQUIRE(pmap.begin() == pmap.end());
    }
    
    // Test case for insertion and deletion
    SECTION("Increment thousand times works correctly") {
        for (size_t i = 0; i < 1000; i++) {
            ++pmap[7]; // Increment value for key 7
        }
        auto val = pmap.begin();
        REQUIRE(val->first == 7);  // Check key is 7
        REQUIRE(val->second == 1000); // Check value is 1000 after increment
    }
    
    // Test case for insertion and deletion
    SECTION("Increment thousand times works correctly for two keys") {
        for (size_t i = 0; i < 1000; i++) {
            ++pmap[7]; // Increment value for key 7
        }
        for (size_t i = 0; i < 1000; i++) {
            ++pmap[8]; // Increment value for key 8
        }
        auto val = pmap.begin();
        REQUIRE(val->second == 1000); // Check value is 1000 after increment
        ++val;
        REQUIRE(val->second == 1000); // Check value is 1000 after increment

    }

    // ... Additional test cases for increment, decrement, and other functionalities
}

