/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

/* Crow */
#include "crow_all.h"

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

    // Create web server
    crow::SimpleApp app;


    //Displays the entire tasks table 
    CROW_ROUTE(app, "/tasks")([&]() {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM tasks ORDER BY starred desc");

        crow::json::wvalue result;
        int count = 0;

        //fill vector 
        while (res->next()) {
            result[count]["name"] = res->getString("name");
            result[count]["id"] = res->getInt("id");
            result[count]["due_date"] = res->getString("due_date");
            result[count]["starred"] = res->getString("starred");
            count++;    
        }

        delete res;

        return crow::response(result);
    });

    //Displays the task corresponding to the task number the user entered 
    CROW_ROUTE(app, "/tasks/<int>")([&](int taskId) {
        prep_stmt = con->prepareStatement("select * from tasks where id = ?");
        prep_stmt->setInt(1, taskId);
        res = prep_stmt->executeQuery();
        delete prep_stmt;

        if (res->next()) {
            crow::json::wvalue x;
            x["name"] = res->getString("name");
            x["id"] = res->getInt("id");
            x["due_date"] = res->getString("due_date");
            x["starred"] = res->getBoolean("starred");
            return crow::response(x);
        } else {
            return crow::response(404);
        }
    });

    //Delete the task corresponding to the task number the user entered 
    CROW_ROUTE(app, "/tasks/<int>").methods("DELETE"_method)([&](int taskId) {
        prep_stmt = con->prepareStatement("DELETE from tasks where id = ?");
        prep_stmt->setInt(1, taskId);
        prep_stmt->execute();
        delete prep_stmt;

        return crow::response(204);
    });

    //Update a task to be starred corresponding to the task ID 
    CROW_ROUTE(app, "/tasks/<int>").methods("PUT"_method)([&](const crow::request& req, int taskId) {
        prep_stmt = con->prepareStatement("select * from tasks where id = ?");
        prep_stmt->setInt(1, taskId);
        res = prep_stmt->executeQuery();
        delete prep_stmt;

        //error message 
        if (!res->next()) {
            return crow::response(404); //404 means something cannot be found 
        }

        bool starred = res->getBoolean("starred"); //original value of starred 

        auto body = crow::json::load(req.body); //imagine body as a map of <key, value>

        if (body.count("starred") != 0) {
            starred = body["starred"].b(); //.b() method changes the output of body["starred"] into boolean
            // if (body["starred"] == true) {
            //     starred = true;
            // } else {
            //     starred = false;
            // }
        }

        string name = res->getString("name"); //original value of name
        if (body.count("name") != 0) {
            name = body["name"].s(); //.s() method changes the output of body["name"] into string 
        }

        string due_date = res->getString("due_date");
        if (body.count("due_date") != 0) {
            due_date = body["due_date"].s();
        }


        prep_stmt = con->prepareStatement("UPDATE tasks SET starred = ?, name = ?, due_date = ? WHERE id = ?");
        prep_stmt->setBoolean(1, starred);
        prep_stmt->setString(2, name);
        prep_stmt->setString(3, due_date);
        prep_stmt->setInt(4, taskId);
        prep_stmt->execute();


        //returning the task that was updated 
        crow::json::wvalue x; //json object 
        x["name"] = name;
        x["id"] = taskId;
        x["due_date"] = due_date;
        x["starred"] = starred;
        return crow::response(x);
    });

    app.port(18080).multithreaded().run();
    
    // string instruction;

    // do {
    //     //Display the tasks list 
    //     stmt = con->createStatement();
    //     res = stmt->executeQuery("SELECT * FROM tasks ORDER BY starred desc");

    //     while (res->next()) {
    //         //Access column data by alias or column name 
    //         cout << "[ ] ";
    //         cout << res->getString("name");
    //         if (res->getBoolean("starred")) {
    //             cout << " *";
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;

    //     delete res;

    //     cout << "Hello Jennifer, what would you like to do? " << endl;
    //     //Display menu 
    //     cout << "[A]dd task" << endl;
    //     cout << "[S]tar a task" << endl;
    //     cout << "[D]elete a task" << endl;
    //     cout << "Type STOP to exit at any point to exit the program" << endl << endl;

    //     getline(cin, instruction);

    //     // Add the task 
    //     if (instruction == "A")
    //     {
    //         prep_stmt = con->prepareStatement("INSERT INTO tasks (name, starred) VALUES (?, ?)");
    //         string taskToInsert;
    //         string isStarred;
    //         bool starredBool = false;
    //         cout << "Enter the task that you want to insert: ";
    //         getline(cin, taskToInsert);
    //         prep_stmt->setString(1, taskToInsert);
    //         cout << "Enter \"true\" if the task is to be starred. Enter \"false\" if the task is not THAT important: ";
    //         getline(cin, isStarred);
    //         if (isStarred == "true") {
    //             starredBool = true;
    //         }
    //         prep_stmt->setBoolean(2, starredBool);
    //         prep_stmt->execute();
    //         delete prep_stmt;
    //     }
        
    //     // Update (Star the task) 
    //     else if (instruction == "S") 
    //     {
    //         prep_stmt = con->prepareStatement("UPDATE tasks SET starred = true WHERE name COLLATE utf8mb4_unicode_ci LIKE ?");
    //         string taskToStar;
    //         cout << "Enter the task that you want to star: ";
    //         getline(cin, taskToStar);
    //         prep_stmt->setString(1, taskToStar);
    //         prep_stmt->execute();
    //         delete prep_stmt;
    //     }

    //     // Delete the task 
    //     else if (instruction == "D")
    //     {
    //         prep_stmt = con->prepareStatement("DELETE from tasks where name COLLATE utf8mb4_unicode_ci LIKE ?");
    //         //case insensitivity 
    //         cout << "Enter the task that you want to delete: ";
    //         string taskToDelete;
    //         getline(cin, taskToDelete);
    //         prep_stmt->setString(1, taskToDelete);
    //         prep_stmt->execute();
    //         delete prep_stmt;
    //     } 

    // } while (instruction != "STOP");

    //delete stmt;
     delete con;
     
}