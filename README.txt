Student Results Register - BCS 2204 Group Coursework 1 (Scenario 6)

FILES
  StudentResult.h     record struct, file parse/format helpers, comparators
  ResultsArray.h      class template: dynamic array with capacity doubling (Task A1)
  SortSearch.h        insertion sort, my O(n log n) sort, iterative binary search (Task A2)
  students.txt        sample data (12 records): regNo,name,course,marks
  main.cpp            demo program (Tasks A1 and A2)
  tests.cpp           automatic test table (Task A3); writes test_results.csv
  timing.cpp          timing experiment (Task A4); writes timing.csv

COMPILE AND RUN (Ubuntu / WSL, g++ with C++17)
  Demo, with the memory checker:
    g++ -std=c++17 -Wall -g -fsanitize=address main.cpp -o main
    ./main
  Tests, with the memory checker:
    g++ -std=c++17 -Wall -g -fsanitize=address tests.cpp -o tests
    ./tests
  Timing (WITHOUT the sanitizer, with -O2, so the times are fair; takes about 2 minutes):
    g++ -std=c++17 -Wall -O2 timing.cpp -o timing
    ./timing

students.txt must be in the same folder as the programs.
