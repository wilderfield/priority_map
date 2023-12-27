#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "wilderfield/priority_map.hpp"

#include <vector>
#include <unordered_map>

#include <iostream>

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

    SECTION("Checking count()") {
        ++pmap[7];
        REQUIRE(pmap.count(7) == 1);
        REQUIRE(pmap.count(8) == 0);
    }

    SECTION("Checking access by operator[]") {
        ++pmap[7];
        ++pmap[7];
        REQUIRE(pmap[7] == 2);
    }

    SECTION("Check default with operator[]") {
        REQUIRE(pmap[7] == 0);
    }

    SECTION("Check assignment") {
        pmap[7] = 456;
        REQUIRE(pmap[7] == 456);
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

    SECTION("Checking pop()") {
        ++pmap[7];
        ++pmap[7];
        ++pmap[7];
        ++pmap[11];
        ++pmap[11];
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 7);
            REQUIRE(maxVal == 3);
            pmap.pop();
        }
        {
            auto [maxKey, maxVal] = pmap.top();
            REQUIRE(maxKey == 11);
            REQUIRE(maxVal == 2);
            pmap.pop();
        }
        REQUIRE(pmap.empty());
    }

    SECTION("Checking frequency map") {

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

    SECTION("Checking khan's algo") {

        wilderfield::priority_map<int, int, std::less<int>> pmap;

        // Test graph
        std::vector<std::vector<int>> graph(6);
        graph[0].push_back(1);
        graph[0].push_back(3);
        graph[2].push_back(0);
        graph[2].push_back(4);
        graph[3].push_back(1);
        graph[4].push_back(3);
        graph[4].push_back(5);
        graph[5].push_back(1);

        // Initialize pmap to have all nodes with ingress 0
        for (int u = 0; u < graph.size(); u++) {
            pmap[u] = 0;
        }

        // Calculate indegrees for each node
        for (int u = 0; u < graph.size(); u++) {
            for (auto v : graph[u]) {
                ++pmap[v];
            }
        }

        std::vector<int> topological;

        // Valid topological sorts for this graph
        std::vector<std::vector<int>> golden = {{2,0,4,3,5,1},{2,4,0,3,5,1},{2,4,5,0,3,1}};

        while (!pmap.empty()) {
            auto [u, minVal] = pmap.top(); pmap.pop();
            REQUIRE(minVal == 0);
            topological.push_back(u);
            for (auto v : graph[u]) {
                --pmap[v];
            }
        }

	bool valid = false;
        for (auto& gr : golden) {
            if (topological == gr) {
                valid |= true;
            }
        }

        REQUIRE(valid);

    }

}

