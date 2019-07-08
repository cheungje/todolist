/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

/* MySQL Connector/C++ specific headers */
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
using namespace std;


int main() {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    sql::PreparedStatement *prep_stmt;

    // Create a connection 
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "netfab@6dpmCcAA");
    // Connect to the database 
    con->setSchema("todolist"); 
    
    string instruction;

    do {
        //Display the tasks list 
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM tasks ORDER BY starred desc");

        while (res->next()) {
            //Access column data by alias or column name 
            cout << "[ ] ";
            cout << res->getString("name");
            if (res->getBoolean("starred")) {
                cout << " *";
            }
            cout << endl;
        }
        cout << endl;

        delete res;

        cout << "Hello Jennifer, what would you like to do? " << endl;
        //Display menu 
        cout << "[A]dd task" << endl;
        cout << "[S]tar a task" << endl;
        cout << "[D]elete a task" << endl;
        cout << "Type STOP to exit at any point to exit the program" << endl << endl;

        getline(cin, instruction);

        // Add the task 
        if (instruction == "A")
        {
            prep_stmt = con->prepareStatement("INSERT INTO tasks (name, starred) VALUES (?, ?)");
            string taskToInsert;
            string isStarred;
            bool starredBool = false;
            cout << "Enter the task that you want to insert: ";
            getline(cin, taskToInsert);
            prep_stmt->setString(1, taskToInsert);
            cout << "Enter \"true\" if the task is to be starred. Enter \"false\" if the task is not THAT important: ";
            getline(cin, isStarred);
            if (isStarred == "true") {
                starredBool = true;
            }
            prep_stmt->setBoolean(2, starredBool);
            prep_stmt->execute();
            delete prep_stmt;
        }
        
        // Update (Star the task) 
        else if (instruction == "S") 
        {
            prep_stmt = con->prepareStatement("UPDATE tasks SET starred = true WHERE name COLLATE utf8mb4_unicode_ci LIKE ?");
            string taskToStar;
            cout << "Enter the task that you want to star: ";
            getline(cin, taskToStar);
            prep_stmt->setString(1, taskToStar);
            prep_stmt->execute();
            delete prep_stmt;
        }

        // Delete the task 
        else if (instruction == "D")
        {
            prep_stmt = con->prepareStatement("DELETE from tasks where name COLLATE utf8mb4_unicode_ci LIKE ?");
            //case insensitivity 
            cout << "Enter the task that you want to delete: ";
            string taskToDelete;
            getline(cin, taskToDelete);
            prep_stmt->setString(1, taskToDelete);
            prep_stmt->execute();
            delete prep_stmt;
        } 

    } while (instruction != "STOP");

    delete stmt;
    delete con;
}