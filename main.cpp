#include <iostream>
#include <vector>
#include "taskClass.h"
using namespace std;

int main(){
    
    string name;
    cout << "Please enter your name: ";
    getline(cin, name);
    cout << "Hello " << name << ", what would you like to do? " << endl;

    vector<Task> taskList;
    string instruction;

    do {
        //Display menu 
        cout << "[A]dd task" << endl;
        cout << "[S]tar a task" << endl;
        cout << "[D]elete a task" << endl;
        cout << "Type STOP to exit at any point to exit the program" << endl << endl;

        getline(cin, instruction);

        //Add the task 
        if (instruction == "A")
        {
            cout << "Add task: ";
            string userInput;
            getline(cin, userInput);
            Task task;
            task.name = userInput;
            taskList.push_back(task);
        }
        
        //Go through taskList and star the item by appending it to the ending
        else if (instruction == "S") 
        {
            cout << "Enter the task that you want to star: ";
            string taskToStar; 
            getline(cin, taskToStar);
            for (int i = 0; i < taskList.size(); ++i)
            {
                if (taskList[i].name == taskToStar)
                    taskList[i].starred = true;
            }
        }

        else if (instruction == "D")
        {
            cout << "Enter the task that you want to delete: ";
            string taskToDelete;
            getline(cin, taskToDelete);
            for (int i = 0; i < taskList.size(); ++i)
            {
                if (taskList[i].name == taskToDelete)
                {
                    taskList[i].name = "none";
                }
            }
        }

        //Display task list 
        cout << "Here is a recap of your tasks: " << endl;
        for (int i = 0; i < taskList.size(); ++i)
        {
            if (taskList[i].starred) 
                cout << "[ ] " << taskList[i].name << " * " << endl;
            else 
                cout << "[ ] " << taskList[i].name << endl;
        }
        cout << endl;
    }

    while (instruction != "STOP");
}