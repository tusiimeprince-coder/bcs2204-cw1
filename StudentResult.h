#ifndef STUDENT_RESULT_H
#define STUDENT_RESULT_H

#include <iostream>
#include <sstream>
#include <string>

// One record in the Student Results Register (Task A1: our own struct type).
struct StudentResult {
    std::string regNo;      // unique key, e.g. VU-BCS2507-0001
    std::string name;       // student full name
    std::string course;     // course code, e.g. BCS2204
    double      marks = 0;  // final mark, 0 to 100
};

// Turns one text line "regNo,name,course,marks" into a StudentResult.
// Returns false if the line is badly formed, so the caller can skip it.
inline bool parseStudentResult(std::string line, StudentResult& out) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
    std::istringstream ss(line);
    std::string regNo, name, course, marksText;
    if (!std::getline(ss, regNo, ',') || !std::getline(ss, name, ',') ||
        !std::getline(ss, course, ',') || !std::getline(ss, marksText)) {
        return false;
    }
    double marks = 0;
    try {
        std::size_t used = 0;
        marks = std::stod(marksText, &used);
        if (used != marksText.size()) return false;   // e.g. "78abc" is rejected
    } catch (...) {
        return false;                                 // not a number at all
    }
    if (regNo.empty() || marks < 0 || marks > 100) return false;
    out.regNo = regNo;
    out.name = name;
    out.course = course;
    out.marks = marks;
    return true;
}

// Turns a StudentResult back into one text line for saving.
inline std::string formatStudentResult(const StudentResult& s) {
    std::ostringstream os;
    os << s.regNo << ',' << s.name << ',' << s.course << ',' << s.marks;
    return os.str();
}

inline void printStudentResult(const StudentResult& s) {
    std::cout << s.regNo << " | " << s.name << " | " << s.course << " | " << s.marks << "\n";
}

// ---------- Comparators: less(x, y) is true when x must come BEFORE y ----------
struct ByMarksDesc {   // highest marks first (ranking)
    bool operator()(const StudentResult& x, const StudentResult& y) const { return x.marks > y.marks; }
};
struct ByRegNo {       // registration number A to Z
    bool operator()(const StudentResult& x, const StudentResult& y) const { return x.regNo < y.regNo; }
};
struct ByName {        // name A to Z
    bool operator()(const StudentResult& x, const StudentResult& y) const { return x.name < y.name; }
};

// Comparator for binary search by regNo. It must work in BOTH directions:
// (record, key) and (key, record).
struct RegNoVsKey {
    bool operator()(const StudentResult& r, const std::string& key) const { return r.regNo < key; }
    bool operator()(const std::string& key, const StudentResult& r) const { return key < r.regNo; }
};

#endif
