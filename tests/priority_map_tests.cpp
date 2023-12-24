#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "wilderfield/priority_map.hpp"

TEST_CASE("PriorityMap operations are tested", "[priority_map]") {

    wilderfield::priority_map<int, int> pmap;

    SECTION("Checking empty()") {
        REQUIRE(pmap.empty());
        ++pmap[7];
        REQUIRE(!pmap.empty());
    }

    SECTION("Checking size()") {
        ++pmap[7];
        REQUIRE(pmap.size() == 1);
    }

    SECTION("Checking access by operator[]") {
        ++pmap[7];
        ++pmap[7];
        REQUIRE(pmap[7] == 2);
    }

    SECTION("Check default with operator[]") {
        REQUIRE(pmap[7] == 0);
    }

    SECTION("Check decrement") {
        --pmap[7];
        ++pmap[9];
        REQUIRE(pmap[7] == -1);
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 9);
            REQUIRE(maxVal == 1);
        }
    }

    SECTION("Check decrement 2") {
        --pmap[7];
        ++pmap[8];
        ++pmap[9];
        ++pmap[9];
        REQUIRE(pmap[7] == -1);
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 9);
            REQUIRE(maxVal == 2);
        }
    }

    SECTION("Checking top()") {
        ++pmap[7];
        ++pmap[7];
        ++pmap[7];
        ++pmap[11];
        ++pmap[11];
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 7);
            REQUIRE(maxVal == 3);
        }
        --pmap[7];
        --pmap[7];
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 11);
            REQUIRE(maxVal == 2);
        }
    }

    SECTION("Checking common functionality") {

        wilderfield::priority_map<char, int> pmap;
        std::unordered_map<char, int> umap;

        std::string s = "supercalifragilisticexpialidocious";

        for (auto c : s) {
            ++pmap[c];
            ++umap[c];
        }
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(pmap[maxKey] == umap[maxKey]);
            REQUIRE(maxKey == 'i');
            REQUIRE(maxVal == 7);
        }
    }


}

