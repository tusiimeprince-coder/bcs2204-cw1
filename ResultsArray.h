#ifndef RESULTS_ARRAY_H
#define RESULTS_ARRAY_H

#include <fstream>
#include <stdexcept>
#include <string>

// Task A1: dynamic array with capacity doubling. No STL containers are used.
// Invalid operations are rejected safely by throwing std::out_of_range
// (or std::runtime_error for file problems) instead of crashing.
template <typename T>
class ResultsArray {
private:
    T*  data_;       // block of records on the heap
    int count_;      // how many records are stored now
    int capacity_;   // how many records fit before the block must grow

    // Allocate a block of newCapacity, copy the records over, free the old block.
    void resize(int newCapacity) {
        T* block = new T[newCapacity];
        for (int i = 0; i < count_; ++i) block[i] = data_[i];
        delete[] data_;
        data_ = block;
        capacity_ = newCapacity;
    }

    // Capacity doubling: 0 -> 1 -> 2 -> 4 -> 8 -> 16 ...
    void grow() { resize(capacity_ == 0 ? 1 : capacity_ * 2); }

    void checkIndex(int index, const char* where) const {
        if (index < 0 || index >= count_)
            throw std::out_of_range(std::string(where) + ": index out of range");
    }

public:
    ResultsArray() : data_(nullptr), count_(0), capacity_(0) {}

    // Deep copy constructor: a new block and a copy of every record.
    ResultsArray(const ResultsArray& other)
        : data_(nullptr), count_(other.count_), capacity_(other.capacity_) {
        if (capacity_ > 0) {
            data_ = new T[capacity_];
            for (int i = 0; i < count_; ++i) data_[i] = other.data_[i];
        }
    }

    // Copy assignment: build the copy first, then swap (safe for a = a).
    ResultsArray& operator=(const ResultsArray& other) {
        if (this != &other) {
            ResultsArray temp(other);
            T* d = data_; data_ = temp.data_; temp.data_ = d;
            int c = count_; count_ = temp.count_; temp.count_ = c;
            int k = capacity_; capacity_ = temp.capacity_; temp.capacity_ = k;
        }
        return *this;
    }

    // Destructor: frees the block, so there is no memory leak.
    ~ResultsArray() { delete[] data_; }

    // ---- Information (all O(1)) ----
    int  size() const { return count_; }
    int  capacity() const { return capacity_; }
    bool empty() const { return count_ == 0; }

    // ---- Operations ----
    // Add at the end. Doubles the capacity first if the array is full.
    void add(const T& record) {
        if (count_ == capacity_) grow();
        data_[count_++] = record;
    }

    // Insert at position 0..size(). Later records shift right.
    void insertAt(int index, const T& record) {
        if (index < 0 || index > count_)
            throw std::out_of_range("insertAt: index out of range");
        if (count_ == capacity_) grow();
        for (int i = count_; i > index; --i) data_[i] = data_[i - 1];
        data_[index] = record;
        ++count_;
    }

    // Replace the record at an existing position.
    void update(int index, const T& record) {
        checkIndex(index, "update");
        data_[index] = record;
    }

    // Remove the record at an existing position. Later records shift left.
    // If the array becomes less than a quarter full, its capacity is halved.
    void removeAt(int index) {
        checkIndex(index, "removeAt");
        for (int i = index; i < count_ - 1; ++i) data_[i] = data_[i + 1];
        --count_;
        if (capacity_ > 4 && count_ < capacity_ / 4) resize(capacity_ / 2);
    }

    T& at(int index) { checkIndex(index, "at"); return data_[index]; }
    const T& at(int index) const { checkIndex(index, "at"); return data_[index]; }

    void clear() { count_ = 0; }

    // Linear search: index of the first record where pred(record) is true, or -1.
    template <typename Pred>
    int findIndex(Pred pred) const {
        for (int i = 0; i < count_; ++i)
            if (pred(data_[i])) return i;
        return -1;
    }

    // ---- File handling ----
    // parse(line, record) turns one text line into a record (false = bad line).
    // Bad lines are skipped. Returns how many records were loaded.
    template <typename Parse>
    int loadFromFile(const std::string& filename, Parse parse) {
        std::ifstream in(filename);
        if (!in) throw std::runtime_error("cannot open input file: " + filename);
        int loaded = 0;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            T record;
            if (parse(line, record)) { add(record); ++loaded; }
        }
        return loaded;
    }

    // format(record) turns one record into a text line.
    template <typename Format>
    void saveToFile(const std::string& filename, Format format) const {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("cannot open output file: " + filename);
        for (int i = 0; i < count_; ++i) out << format(data_[i]) << "\n";
    }

    // Raw pointer, used by the sorting and searching templates.
    T* raw() { return data_; }
    const T* raw() const { return data_; }
};

#endif
