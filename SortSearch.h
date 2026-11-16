#ifndef SORT_SEARCH_H
#define SORT_SEARCH_H

#include <random>
#include <utility>

// Tasks A2: sorting and searching templates. Every sort takes a comparator:
// less(x, y) must return true when x should come BEFORE y.

// ===================== INSERTION SORT (stable) =====================
// Loop invariant: at the start of each pass i, a[0..i-1] holds the first i
// original elements in sorted order.
// Stable: the test "less(key, a[j])" is strict, so equal keys never pass each other.
template <typename T, typename Less>
void insertionSort(T* a, int n, Less less) {
    for (int i = 1; i < n; ++i) {
        T key = a[i];                         // element to insert
        int j = i - 1;
        while (j >= 0 && less(key, a[j])) {   // shift bigger elements one place right
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;                       // drop the key into its slot
    }
}

// ===================== QUICKSORT, RANDOM PIVOT (not stable) =====================
// Last digit of the registration number is ODD, so quicksort with a random pivot
// is the required sort. Expected O(n log n), worst case O(n^2) (very unlikely).
inline std::mt19937& randomEngine() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

template <typename T, typename Less>
int partitionRandom(T* a, int lo, int hi, Less less) {
    std::uniform_int_distribution<int> pick(lo, hi);
    std::swap(a[pick(randomEngine())], a[hi]);   // random pivot goes to the end
    T pivot = a[hi];
    int i = lo - 1;                              // end of the "smaller than pivot" part
    for (int j = lo; j < hi; ++j) {
        if (less(a[j], pivot)) { ++i; std::swap(a[i], a[j]); }
    }
    std::swap(a[i + 1], a[hi]);                  // pivot into its final place
    return i + 1;
}

template <typename T, typename Less>
void quickSortRange(T* a, int lo, int hi, Less less) {
    if (lo >= hi) return;
    int p = partitionRandom(a, lo, hi, less);
    quickSortRange(a, lo, p - 1, less);
    quickSortRange(a, p + 1, hi, less);
}

template <typename T, typename Less>
void quickSort(T* a, int n, Less less) {
    if (n < 2) return;
    quickSortRange(a, 0, n - 1, less);
}

// The rest of the program calls this one name for "my O(n log n) sort".
template <typename T, typename Less>
void nlognSort(T* a, int n, Less less) { quickSort(a, n, less); }

inline const char* NLOGN_NAME = "quicksort (random pivot)";
inline const bool  NLOGN_STABLE = false;

// ===================== ITERATIVE BINARY SEARCH =====================
// The array must already be sorted by the same key.
// Returns the index of the FIRST record equal to key, or -1 if the key is absent.
// less must work both ways: less(record, key) and less(key, record).
template <typename T, typename Key, typename Less>
int binarySearch(const T* a, int n, const Key& key, Less less) {
    int lo = 0, hi = n;                          // search window is [lo, hi)
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (less(a[mid], key)) lo = mid + 1;     // key is to the right of mid
        else                   hi = mid;         // key is at mid or to the left
    }
    if (lo < n && !less(key, a[lo])) return lo;  // a[lo] equals key
    return -1;
}

#endif
