// Student Results Register: demo program for Tasks A1 and A2.
// 
#include <iostream>
#include <stdexcept>
#include <string>
#include "StudentResult.h"
#include "ResultsArray.h"
#include "SortSearch.h"

using Register = ResultsArray<StudentResult>;

static void banner(const std::string& title) { std::cout << "\n=== " << title << " ===\n"; }

static void pauseHere() {
    std::cout << "\n[Press Enter for the next section]";
    std::string dummy;
    std::getline(std::cin, dummy);
}

static void show(const Register& reg) {
    for (int i = 0; i < reg.size(); ++i) {
        std::cout << "  [" << i << "] ";
        printStudentResult(reg.at(i));
    }
}

static void info(const Register& reg) {
    std::cout << "  size = " << reg.size() << ", capacity = " << reg.capacity() << "\n";
}

int main() {
    try {
        Register reg;

        // ---------------- A1: load ----------------
        banner("A1: load the sample records from students.txt");
        int loaded = reg.loadFromFile("students.txt", parseStudentResult);
        std::cout << "  loaded " << loaded << " records\n";
        info(reg);
        show(reg);
        pauseHere();

        // ---------------- A1: operations ----------------
        banner("A1: add, insert, update and remove");
        reg.add(StudentResult{"VU-BCS2507-0013", "Tumusiime Ivan", "BCS2204", 67});
        std::cout << "  add          -> ";   info(reg);
        reg.insertAt(0, StudentResult{"VU-BCS2507-0000", "Front Student", "BCS2204", 50});
        std::cout << "  insertAt(0)  -> first record: ";  printStudentResult(reg.at(0));
        StudentResult changed = reg.at(1);
        changed.marks = 99;
        reg.update(1, changed);
        std::cout << "  update(1)    -> ";  printStudentResult(reg.at(1));
        reg.removeAt(0);
        std::cout << "  removeAt(0)  -> first record: ";  printStudentResult(reg.at(0));
        reg.removeAt(reg.size() - 1);
        std::cout << "  removeAt(last) -> ";  info(reg);
        reg.update(0, StudentResult{"VU-BCS2507-0001", "Nakato Sarah", "BCS2204", 78.5});   // undo the demo change
        pauseHere();

        // ---------------- A1: safe rejection ----------------
        banner("A1: invalid operations are rejected safely (no crash)");
        try { reg.removeAt(999); }  catch (const std::out_of_range& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        try { reg.removeAt(-1); }   catch (const std::out_of_range& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        try { reg.insertAt(500, StudentResult{}); } catch (const std::out_of_range& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        try { reg.at(500); }        catch (const std::out_of_range& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        try { reg.update(500, StudentResult{}); }   catch (const std::out_of_range& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        try { Register other; other.loadFromFile("no_such_file.txt", parseStudentResult); }
        catch (const std::runtime_error& e) { std::cout << "  rejected: " << e.what() << "\n"; }
        std::cout << "  the register is still intact: ";  info(reg);

        banner("A1: the copy constructor makes a DEEP copy");
        Register copy(reg);
        StudentResult edited = copy.at(0);
        edited.name = "EDITED IN THE COPY";
        copy.update(0, edited);
        std::cout << "  original first record: ";  printStudentResult(reg.at(0));
        std::cout << "  copy first record:     ";  printStudentResult(copy.at(0));
        pauseHere();

        // ---------------- A2: sorting ----------------
        banner(std::string("A2: sort by NAME with my O(n log n) sort: ") + NLOGN_NAME);
        nlognSort(reg.raw(), reg.size(), ByName{});
        show(reg);
        pauseHere();

        banner("A2: insertion sort by MARKS (high to low). Stable: equal marks stay in name order");
        insertionSort(reg.raw(), reg.size(), ByMarksDesc{});
        show(reg);
        pauseHere();

        // ---------------- A2: binary search ----------------
        banner("A2: sort by regNo, then iterative binary search");
        nlognSort(reg.raw(), reg.size(), ByRegNo{});
        show(reg);
        std::string present = "VU-BCS2507-0007";
        std::string absent  = "VU-BCS2507-9999";
        int pos = binarySearch(reg.raw(), reg.size(), present, RegNoVsKey{});
        std::cout << "  search " << present << " -> ";
        if (pos >= 0) { std::cout << "found at index " << pos << ": "; printStudentResult(reg.at(pos)); }
        else std::cout << "absent\n";
        pos = binarySearch(reg.raw(), reg.size(), absent, RegNoVsKey{});
        std::cout << "  search " << absent << " -> " << (pos >= 0 ? "found" : "absent (expected)") << "\n";
        pauseHere();

        // ---------------- A1: save ----------------
        banner("A1: save the updated records back to a file");
        reg.saveToFile("students_out.txt", formatStudentResult);
        Register check;
        int again = check.loadFromFile("students_out.txt", parseStudentResult);
        std::cout << "  saved " << reg.size() << " records to students_out.txt; reloaded " << again << "\n";
    } catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
