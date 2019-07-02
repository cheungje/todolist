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
    // string get_name() {return name;}
    // Date get_duedate() {return dueDate;}
    // bool get_starred() {return starred;}
    // bool get_done() {return done;}
    // string get_notes() {return notes;}

    // void set_name(string newName) {name = newName;}
    // void set_duedate(Date newDate) {dueDate = newDate;}
    // void set_starred(bool isStarred) {starred = isStarred;}
    // void set_done(bool isDone) {done = isDone;}
    // void set_notes(string newNotes) {notes = newNotes;}

    
        string name;
        Date dueDate;
        bool starred = false;
        bool done;
        string notes;
};
