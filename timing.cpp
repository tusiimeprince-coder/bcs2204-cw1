
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include "StudentResult.h"
#include "ResultsArray.h"
#include "SortSearch.h"

using Register = ResultsArray<StudentResult>;

// n random records. Marks are random decimals, so there are almost no equal keys.
static Register randomData(int n, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    Register reg;
    for (int i = 0; i < n; ++i) {
        StudentResult s;
        s.regNo = "R" + std::to_string(i);
        s.name = "Student";
        s.course = "BCS2204";
        s.marks = dist(gen);
        reg.add(s);
    }
    return reg;
}

template <typename SortFn>
static double timeSort(Register reg, SortFn sortFn) {        // takes a COPY on purpose
    auto start = std::chrono::steady_clock::now();
    sortFn(reg.raw(), reg.size());
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

int main() {
    std::mt19937 gen(2204);
    const int repeats = 3;
    std::ofstream csv("timing.csv");
    csv << "n,insertion_ms,insertion_ratio,nlogn_ms,nlogn_ratio\n";

    std::cout << "Sorting by marks. Second sort: " << NLOGN_NAME << "\n";
    std::cout << std::setw(8) << "n" << std::setw(16) << "insertion(ms)" << std::setw(8) << "ratio"
              << std::setw(16) << "n log n (ms)" << std::setw(8) << "ratio" << "\n";

    double prevIns = 0, prevFast = 0;
    for (int n = 1000; n <= 64000; n *= 2) {
        double insTotal = 0, fastTotal = 0;
        for (int r = 0; r < repeats; ++r) {
            Register data = randomData(n, gen);
            insTotal  += timeSort(data, [](StudentResult* a, int m) { insertionSort(a, m, ByMarksDesc{}); });
            fastTotal += timeSort(data, [](StudentResult* a, int m) { nlognSort(a, m, ByMarksDesc{}); });
        }
        double ins = insTotal / repeats, fast = fastTotal / repeats;
        double insRatio  = prevIns  > 0 ? ins  / prevIns  : 0;     // about 4 expected
        double fastRatio = prevFast > 0 ? fast / prevFast : 0;     // a little above 2 expected
        std::cout << std::fixed << std::setprecision(3) << std::setw(8) << n
                  << std::setw(16) << ins << std::setw(8) << insRatio
                  << std::setw(16) << fast << std::setw(8) << fastRatio << "\n";
        csv << n << "," << ins << "," << insRatio << "," << fast << "," << fastRatio << "\n";
        prevIns = ins; prevFast = fast;
    }
    std::cout << "Saved timing.csv\n";
    return 0;
}
