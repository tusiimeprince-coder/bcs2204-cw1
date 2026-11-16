// Automatic test table for Tasks A1 and A2 (this is your A3 evidence).
// It prints the table and also saves it as test_results.csv (open in Excel).
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "StudentResult.h"
#include "ResultsArray.h"
#include "SortSearch.h"

using Register = ResultsArray<StudentResult>;

static int failures = 0;
static std::ofstream csv("test_results.csv");

static std::string quoted(const std::string& s) { return "\"" + s + "\""; }

// One row of the test table: ID | operation | expected | actual | pass/fail
static void check(const std::string& id, const std::string& operation,
                  const std::string& expected, const std::string& actual) {
    bool pass = (expected == actual);
    if (!pass) ++failures;
    std::cout << id << " | " << operation << " | " << expected << " | " << actual
              << " | " << (pass ? "PASS" : "FAIL") << "\n";
    csv << quoted(id) << "," << quoted(operation) << "," << quoted(expected) << ","
        << quoted(actual) << "," << (pass ? "PASS" : "FAIL") << "\n";
}

static std::string yn(bool b) { return b ? "true" : "false"; }

// Runs an operation and reports "ok" or the kind of exception it threw.
template <typename F>
static std::string outcome(F f) {
    try { f(); return "ok"; }
    catch (const std::out_of_range&) { return "out_of_range"; }
    catch (const std::runtime_error&) { return "runtime_error"; }
    catch (...) { return "other"; }
}

static StudentResult make(const std::string& reg, const std::string& name, double marks) {
    StudentResult s;
    s.regNo = reg; s.name = name; s.course = "BCS2204"; s.marks = marks;
    return s;
}

template <typename Less>
static bool isSorted(const Register& r, Less less) {
    for (int i = 1; i < r.size(); ++i)
        if (less(r.at(i), r.at(i - 1))) return false;
    return true;
}

// All regNo values in order as one text, e.g. "A,B,C".
static std::string order(const Register& r) {
    std::string out;
    for (int i = 0; i < r.size(); ++i) { if (i) out += ","; out += r.at(i).regNo; }
    return out;
}

int main() {
    std::cout << "ID | Operation | Expected | Actual | Result\n";
    csv << "Test ID,Input / operation,Expected result,Actual result,Pass/Fail\n";

    // ============ A1: the data structure ============
    Register r;
    check("T01", "New register: size() and empty()", "0 true", std::to_string(r.size()) + " " + yn(r.empty()));
    check("T02", "New register: capacity()", "0", std::to_string(r.capacity()));
    check("T03", "removeAt(0) on an empty register", "out_of_range", outcome([&] { r.removeAt(0); }));
    check("T04", "at(0) on an empty register", "out_of_range", outcome([&] { r.at(0); }));
    check("T05", "findIndex on an empty register", "-1", std::to_string(r.findIndex([](const StudentResult&) { return true; })));

    r.add(make("R1", "One", 50));
    check("T06", "add 1 record (one-record case): size() and capacity()", "1 1", std::to_string(r.size()) + " " + std::to_string(r.capacity()));
    r.add(make("R2", "Two", 60));
    check("T07", "add 2nd record: capacity doubles", "2", std::to_string(r.capacity()));
    r.add(make("R3", "Three", 70));
    check("T08", "add 3rd record: capacity doubles again", "4", std::to_string(r.capacity()));
    for (int i = 4; i <= 9; ++i) r.add(make("R" + std::to_string(i), "N", 40 + i));
    check("T09", "add up to 9 records: size() and capacity()", "9 16", std::to_string(r.size()) + " " + std::to_string(r.capacity()));

    Register s;
    s.add(make("R1", "One", 50)); s.add(make("R2", "Two", 60)); s.add(make("R3", "Three", 70));
    s.insertAt(0, make("R0", "Zero", 40));
    check("T10", "insertAt(0): first record", "R0", s.at(0).regNo);
    s.insertAt(s.size(), make("R9", "Nine", 90));
    check("T11", "insertAt(size()): last record", "R9", s.at(s.size() - 1).regNo);
    check("T12", "insertAt(size()+1) beyond the end", "out_of_range", outcome([&] { s.insertAt(s.size() + 1, make("X", "X", 1)); }));
    check("T13", "insertAt(-1)", "out_of_range", outcome([&] { s.insertAt(-1, make("X", "X", 1)); }));
    s.update(1, make("R1B", "Changed", 55));
    check("T14", "update(1): record replaced", "R1B", s.at(1).regNo);
    check("T15", "update(99) invalid index", "out_of_range", outcome([&] { s.update(99, make("X", "X", 1)); }));

    s.removeAt(0);
    check("T16", "removeAt(0) (first record): new first record", "R1B", s.at(0).regNo);
    s.removeAt(s.size() - 1);
    check("T17", "removeAt(last) (last record): new last record", "R3", s.at(s.size() - 1).regNo);
    check("T18", "removeAt(size()) invalid index", "out_of_range", outcome([&] { s.removeAt(s.size()); }));
    check("T19", "removeAt(-1) invalid index", "out_of_range", outcome([&] { s.removeAt(-1); }));

    Register one;
    one.add(make("ONLY", "Only", 50));
    one.removeAt(0);
    check("T20", "remove the only record: size() and empty()", "0 true", std::to_string(one.size()) + " " + yn(one.empty()));

    Register big;
    for (int i = 0; i < 17; ++i) big.add(make("B" + std::to_string(i), "B", 50));
    std::string before = std::to_string(big.capacity());
    while (big.size() > 7) big.removeAt(big.size() - 1);
    check("T21", "17 adds then remove down to 7: capacity 32 then halves", "32 16", before + " " + std::to_string(big.capacity()));

    Register copy(s);
    copy.update(0, make("CHANGED", "Changed", 1));
    check("T22", "Copy constructor is deep: original unchanged", "R1B", s.at(0).regNo);
    Register assigned;
    assigned = s;
    assigned.update(0, make("CHANGED", "Changed", 1));
    check("T23", "Copy assignment is deep: original unchanged", "R1B", s.at(0).regNo);
    Register& self = s;
    s = self;
    check("T24", "Self-assignment is safe: size unchanged", "3", std::to_string(s.size()));

    Register file;
    check("T25", "loadFromFile students.txt: records loaded", "12", std::to_string(file.loadFromFile("students.txt", parseStudentResult)));
    check("T26", "loadFromFile with a missing file", "runtime_error", outcome([&] { Register x; x.loadFromFile("no_such_file.txt", parseStudentResult); }));
    {
        std::ofstream bad("test_bad_lines.txt");
        bad << "VU-1,Good One,BCS2204,70\n" << "this line is not valid\n" << "VU-2,Bad Marks,BCS2204,abc\n" << "VU-3,Out Of Range,BCS2204,150\n" << "\n" << "VU-4,Good Two,BCS2204,80\n";
    }
    Register badFile;
    check("T27", "loadFromFile with 4 bad lines and 2 good lines: loaded", "2", std::to_string(badFile.loadFromFile("test_bad_lines.txt", parseStudentResult)));
    file.saveToFile("test_out.txt", formatStudentResult);
    Register reloaded;
    reloaded.loadFromFile("test_out.txt", parseStudentResult);
    check("T28", "saveToFile then reload: same size, first and last regNo", "12 VU-BCS2507-0001 VU-BCS2507-0012",
          std::to_string(reloaded.size()) + " " + reloaded.at(0).regNo + " " + reloaded.at(reloaded.size() - 1).regNo);
    check("T29", "findIndex finds the first match (marks == 75)", "6", std::to_string(file.findIndex([](const StudentResult& x) { return x.marks == 75; })));
    check("T30", "findIndex for an absent record", "-1", std::to_string(file.findIndex([](const StudentResult& x) { return x.marks == 12345; })));

    // ============ A2: sorting ============
    Register empty;
    insertionSort(empty.raw(), empty.size(), ByMarksDesc{});
    nlognSort(empty.raw(), empty.size(), ByMarksDesc{});
    check("T31", "Both sorts on an empty register (no crash)", "0", std::to_string(empty.size()));

    Register single;
    single.add(make("A", "A", 5));
    insertionSort(single.raw(), single.size(), ByMarksDesc{});
    nlognSort(single.raw(), single.size(), ByMarksDesc{});
    check("T32", "Both sorts on one record", "A", single.at(0).regNo);

    Register sortedIn, reversedIn;
    for (int i = 1; i <= 6; ++i) sortedIn.add(make("S" + std::to_string(i), "s", 100 - i * 10));   // already high to low
    for (int i = 1; i <= 6; ++i) reversedIn.add(make("S" + std::to_string(i), "s", i * 10));       // low to high = reversed
    Register a = sortedIn, b = sortedIn;
    insertionSort(a.raw(), a.size(), ByMarksDesc{});
    nlognSort(b.raw(), b.size(), ByMarksDesc{});
    check("T33", "Already sorted input: insertion sort (order unchanged)", "true", yn(order(a) == order(sortedIn)));
    check("T34", std::string("Already sorted input: ") + NLOGN_NAME + " (order unchanged)", "true", yn(isSorted(b, ByMarksDesc{})));
    a = reversedIn; b = reversedIn;
    insertionSort(a.raw(), a.size(), ByMarksDesc{});
    nlognSort(b.raw(), b.size(), ByMarksDesc{});
    check("T35", "Reversed input: insertion sort", "true", yn(isSorted(a, ByMarksDesc{})));
    check("T36", std::string("Reversed input: ") + NLOGN_NAME, "true", yn(isSorted(b, ByMarksDesc{})));
    a = file; b = file;
    insertionSort(a.raw(), a.size(), ByMarksDesc{});
    nlognSort(b.raw(), b.size(), ByMarksDesc{});
    check("T37", "12 sample records by marks: insertion sort", "true", yn(isSorted(a, ByMarksDesc{})));
    check("T38", std::string("12 sample records by marks: ") + NLOGN_NAME, "true", yn(isSorted(b, ByMarksDesc{})));

    // Duplicate keys in the sample file: 91 marks = 0003 then 0008; 75 marks = 0007 then 0012.
    check("T39", "Duplicate marks (91): insertion sort keeps the original order", "VU-BCS2507-0003 VU-BCS2507-0008",
          a.at(0).regNo + " " + a.at(1).regNo);
    if (NLOGN_STABLE)
        check("T40", std::string("Duplicate marks (91): ") + NLOGN_NAME + " keeps the original order", "VU-BCS2507-0003 VU-BCS2507-0008",
              b.at(0).regNo + " " + b.at(1).regNo);
    else
        std::cout << "INFO | " << NLOGN_NAME << " is NOT stable, so the order of equal marks is not tested. Here: "
                  << b.at(0).regNo << " " << b.at(1).regNo << "\n";

    Register byReg = file;
    nlognSort(byReg.raw(), byReg.size(), ByRegNo{});
    check("T41", "Sort by regNo (comparator on another field)", "true", yn(isSorted(byReg, ByRegNo{})));

    // ============ A2: binary search ============
    auto find = [&](const Register& reg, const std::string& key) {
        return std::to_string(binarySearch(reg.raw(), reg.size(), key, RegNoVsKey{}));
    };
    check("T42", "Binary search: present key VU-BCS2507-0007", "6", find(byReg, "VU-BCS2507-0007"));
    check("T43", "Binary search: first record", "0", find(byReg, "VU-BCS2507-0001"));
    check("T44", "Binary search: last record", "11", find(byReg, "VU-BCS2507-0012"));
    check("T45", "Binary search: absent key (too large)", "-1", find(byReg, "VU-BCS2507-9999"));
    check("T46", "Binary search: absent key (too small)", "-1", find(byReg, "AAA"));
    check("T47", "Binary search on an empty register", "-1", find(empty, "X"));
    check("T48", "Binary search: one record, present", "0", find(single, "A"));
    check("T49", "Binary search: one record, absent", "-1", find(single, "B"));
    Register dup;
    dup.add(make("D1", "a", 10)); dup.add(make("D2", "b", 20)); dup.add(make("D2", "c", 20)); dup.add(make("D3", "d", 30));
    check("T50", "Binary search: duplicate key D2 returns the FIRST match", "1", find(dup, "D2"));

    std::cout << "\nFailures: " << failures << "\n";
    return failures == 0 ? 0 : 1;
}
