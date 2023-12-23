#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"
#include "bryan/priority_map.hpp" // Your priority_map implementation

TEST_CASE("PriorityMap operations are tested", "[priority_map]") {

    // Instantiate your priority_map
    bryan::priority_map<int, int> pmap;

    SECTION("Checking empty()") {
        REQUIRE(pmap.empty()); // Check map is initially empty
        ++pmap[7]; // Increment value for key 7
        REQUIRE(!pmap.empty()); // Check map is not empty
    }

    SECTION("Checking size()") {
        ++pmap[7]; // Increment value for key 7
        REQUIRE(pmap.size() == 1); // Check size is 1 after increment
    }
    
    SECTION("Checking access by operator[]") {
        ++pmap[7]; // Increment value for key 7
        ++pmap[7]; // Increment value for key 7
        REQUIRE(pmap[7] == 2); // Check associated value for 7 is 2
    }
    
    SECTION("Checking top()") {
        ++pmap[7]; // Increment value for key 7
        ++pmap[7]; // Increment value for key 7
        ++pmap[7]; // Increment value for key 7
        ++pmap[11]; // Increment value for key 11
        ++pmap[11]; // Increment value for key 11
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 7); // Check maximum key is 7
            REQUIRE(maxVal == 3); // Check associated value for 7 is 3
        }
        //--pmap[7]; // Decrement value for key 7
        //--pmap[7]; // Decrement value for key 7
        //{
        //    auto [maxKey, maxVal] = pmap.top();
        //    REQUIRE(maxKey == 11); // Check maximum key is 11
        //    REQUIRE(maxVal == 2); // Check associated value for 11 is 2
        //}
    }

    // ... Additional test cases for increment, decrement, and other functionalities
}

