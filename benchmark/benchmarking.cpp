#include <benchmark/benchmark.h>
#include "wilderfield/priority_map.hpp"  // Include your wilderfield::priority_map implementation

#include <list>
#include <unordered_map>

static void BM_InsertZeroRef(benchmark::State& state) {

    std::list<int> pmap;
    std::unordered_map<int, std::list<int>::iterator> nmap;

    for (auto _ : state) {
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            pmap.insert(pmap.begin(), 0); // Insert elements into the list
            nmap[i] = pmap.begin();
        }

        // Clear the map for the next iteration
        state.PauseTiming();
        nmap.clear();
        pmap.clear();
        state.ResumeTiming();
    }
}

BENCHMARK(BM_InsertZeroRef)->Range(8, 8<<10);

static void BM_InsertZero(benchmark::State& state) {
    wilderfield::priority_map<int, int> pmap;

    for (auto _ : state) {
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            pmap[i] = 0; // Insert elements into the map
        }

        // Clear the map for the next iteration
        state.PauseTiming();
        while(!pmap.empty()) pmap.pop();
        state.ResumeTiming();
    }
}

BENCHMARK(BM_InsertZero)->Range(8, 8<<10);

static void BM_Index(benchmark::State& state) {
    wilderfield::priority_map<int, int> pmap;

    for (int i = 0; i < state.range(0); ++i) {
        pmap[i] = i; // Insert elements into the map
    }

    for (auto _ : state) {
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            auto val = pmap[i]; // Read priorities
        }
    }
}

BENCHMARK(BM_Index)->Range(8, 8<<10);

static void BM_Increment(benchmark::State& state) {
    wilderfield::priority_map<int, int> pmap;

    for (auto _ : state) {
        state.PauseTiming();
        for (int i = 0; i < state.range(0); ++i) {
            pmap[i] = 0; // Insert elements into the map
        }
        state.ResumeTiming();
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            ++pmap[i]; // Increment priorities
        }

        // Clear the map for the next iteration
        state.PauseTiming();
        while(!pmap.empty()) pmap.pop();
        state.ResumeTiming();
    }
}

BENCHMARK(BM_Increment)->Range(8, 8<<10);

static void BM_Decrement(benchmark::State& state) {
    wilderfield::priority_map<int, int> pmap;

    for (auto _ : state) {
        state.PauseTiming();
        for (int i = 0; i < state.range(0); ++i) {
            pmap[i] = 0; // Insert elements into the map
        }
        state.ResumeTiming();
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            --pmap[i]; // Decrement priorities
        }

        // Clear the map for the next iteration
        state.PauseTiming();
        while(!pmap.empty()) pmap.pop();
        state.ResumeTiming();
    }
}

BENCHMARK(BM_Decrement)->Range(8, 8<<10);

static void BM_TopPop(benchmark::State& state) {
    wilderfield::priority_map<int, int> pmap;

    for (auto _ : state) {
        state.PauseTiming();
        for (int i = 0; i < state.range(0); ++i) {
            pmap[i] = i; // Insert elements into the map
        }
        state.ResumeTiming();
        // This code gets timed
        for (int i = 0; i < state.range(0); ++i) {
            auto [maxKey, maxVal] = pmap.top();pmap.pop();
        }
    }
}

BENCHMARK(BM_TopPop)->Range(8, 8<<10);


BENCHMARK_MAIN();

