#include <iostream>
#include <string>
using namespace std;

class Date {
public:
    int month;
    int day;
    int year;
};


class Task {
public:
    string name;
    Date dueDate;
    bool starred = false;
    bool done;
    string notes;
};
