/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "taskClass.h"

/* MySQL Connector/C++ specific headers */
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

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

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        sql::PreparedStatement *prep_stmt;
        
        /* Create a connection */
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "netfab@6dpmCcAA");
        /* Connect to the database */
        con->setSchema("todolist"); 

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM tasks");
        cout << res->getString("name");

        while (res->next()) {
            //Access column data by alias or column name 
            cout << "[ ] ";
            cout << res->getString("name");
            if (res->getBoolean("starred") ) {
                cout << " *";
            }
            cout << endl;
        } 

        delete res;
        delete stmt;
        delete con;

        } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
            << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
}

    while (instruction != "STOP");
}