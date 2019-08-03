/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sstream>

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
    CROW_ROUTE(app, "/tasks")([&](const crow::request& req) {
        ostringstream ss;
        ss << "SELECT * FROM tasks";

        vector<int> values;

        if (req.url_params.get("starred") != nullptr || req.url_params.get("completed") != nullptr) {
            ss << " WHERE true";
        }

        if (req.url_params.get("starred") != nullptr) {
            string starred = req.url_params.get("starred");
            int starred_value = -1;

            if (strcmp(starred.c_str(), "true") == 0) {
                starred_value = 1;
            } else if (strcmp(starred.c_str(), "false") == 0) {
                starred_value = 0;
            }

            ss << " AND starred = ?";
            values.push_back(starred_value);
        }

        if (req.url_params.get("completed") != nullptr) {
            string completed = req.url_params.get("completed");
            int completed_value = -1;

            if (strcmp(completed.c_str(), "true") == 0) {
                completed_value = 1;
            } else if (strcmp(completed.c_str(), "false") == 0) {
                completed_value = 0;
            }

            ss << " AND completed = ?";
            values.push_back(completed_value);
        }

        if (req.url_params.get("isTrashed") != nullptr) {
            string completed = req.url_params.get("isTrashed");
            int isTrashed_value = -1;

            if (strcmp(completed.c_str(), "true") == 0) {
                isTrashed_value = 1;
            } else if (strcmp(completed.c_str(), "false") == 0) {
                isTrashed_value = 0;
            }

            ss << " AND isTrashed = ?";
            values.push_back(isTrashed_value);
        }

        prep_stmt = con->prepareStatement(ss.str());

        for (size_t i = 1; i < values.size() + 1; i++) {
            prep_stmt->setInt(i, values[i - 1]);
        }

        res = prep_stmt->executeQuery();
        delete prep_stmt;

        crow::json::wvalue result;
        int count = 0;

        //fill vector 
        while (res->next()) {
            result[count]["name"] = res->getString("name");
            result[count]["id"] = res->getInt("id");
            result[count]["due_date"] = res->getString("due_date");
            result[count]["starred"] = res->getBoolean("starred");
            result[count]["completed"] = res->getBoolean("completed");
            result[count]["isTrashed"] = res->getBoolean("isTrashed");
            result[count]["notes"] = res->getString("notes");
            count++;    
        }

        delete res;

        crow::response r = crow::response(result);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
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
            x["completed"] = res->getBoolean("completed");
            x["isTrashed"] = res->getBoolean("isTrashed");            
            x["notes"] = res->getString("notes");
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

        crow::response r = crow::response(204);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
    });

    CROW_ROUTE(app, "/tasks/<int>").methods("OPTIONS"_method)([&](int taskId) {
        crow::response r = crow::response(204);
        r.add_header("Access-Control-Allow-Origin", "*");
        r.add_header("Access-Control-Allow-Methods", "PUT,DELETE");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    });

    //Update a task's name, due_date, starred, completed, isTrashed, and notes corresponding to the task ID 
    //PUT method is to insert and update
    CROW_ROUTE(app, "/tasks/<int>").methods("PUT"_method)([&](const crow::request& req, int taskId) {
        //selects the unique task to be updated 
        prep_stmt = con->prepareStatement("select * from tasks where id = ?");
        prep_stmt->setInt(1, taskId);
        res = prep_stmt->executeQuery();
        delete prep_stmt;

        //error message 
        if (!res->next()) {
            return crow::response(404); //404 means something cannot be found 
        }

        auto body = crow::json::load(req.body); //imagine body as a map of <key, value>

        bool starred = res->getBoolean("starred"); //original value of starred 
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

        bool completed = res->getBoolean("completed");
        if (body.count("completed") != 0) {
            completed = body["completed"].b();
        }

        bool isTrashed = res->getBoolean("isTrashed");
        if (body.count("isTrashed") != 0) {
            isTrashed = body["isTrashed"].b();
        }


        string notes = res->getString("notes");
        if (body.count("notes") != 0) {
            notes = body["notes"].s();
        }


        prep_stmt = con->prepareStatement("UPDATE tasks SET starred = ?, name = ?, due_date = ?, completed = ?, isTrashed = ?, notes = ? WHERE id = ?");
        prep_stmt->setBoolean(1, starred);
        prep_stmt->setString(2, name);
        prep_stmt->setString(3, due_date);
        prep_stmt->setBoolean(4, completed);
        prep_stmt->setBoolean(5, isTrashed);
        prep_stmt->setString(6, notes);
        prep_stmt->setInt(7, taskId);
        prep_stmt->execute();
        delete prep_stmt;


        //returning the task that was updated 
        crow::json::wvalue x; //json object 
        x["name"] = name;
        x["id"] = taskId;
        x["due_date"] = due_date;
        x["starred"] = starred;
        x["completed"] = completed;
        x["isTrashed"] = isTrashed;
        x["notes"] = notes;
        
        crow::response r = crow::response(x);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
    });

    CROW_ROUTE(app, "/tasks").methods("OPTIONS"_method)([&]() {
        crow::response r = crow::response(204);
        r.add_header("Access-Control-Allow-Origin", "*");
        r.add_header("Access-Control-Allow-Methods", "POST");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    });

    //Create a new task //POST is to insert 
    CROW_ROUTE(app, "/tasks").methods("POST"_method)([&](const crow::request& req) {
        //create a body to insert into the table
        auto body = crow::json::load(req.body);

        string name = "";
        if (body.count("name") != 0) {
            name = body["name"].s(); //.s() method changes the output of body["name"] into string 
        }

        string due_date = "current_timestamp";
        if (body.count("due_date") != 0) {
            due_date = body["due_date"].s();
        }

        bool starred = false;
         if (body.count("starred") != 0) {
            starred = body["starred"].b(); //.b() method changes the output of body["starred"] into boolean
        }

        bool completed = false;
        if (body.count("completed") != 0) {
            completed = body["completed"].b();
        }
        
        bool isTrashed = false;
        if (body.count("isTrashed") != 0) {
            isTrashed = body["isTrashed"].b();
        }

        string notes = "";
        if (body.count("notes") != 0) {
            notes = body["notes"].s(); 
        }


        prep_stmt = con->prepareStatement("INSERT INTO tasks (name, due_date, starred, completed, isTrashed, notes) VALUES (?, ?, ?, ?, ?, ?)");
        prep_stmt->setString(1, name);
        prep_stmt->setString(2, due_date);
        prep_stmt->setBoolean(3, starred);
        prep_stmt->setBoolean(4, completed);
        prep_stmt->setBoolean(5, isTrashed);
        prep_stmt->setString(6, notes);
        res = prep_stmt->executeQuery();
        delete prep_stmt;

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT @@identity AS id");
        res->next();


        crow::json::wvalue x; //json object 
        x["id"] = res->getInt("id");
        x["name"] = name;
        x["due_date"] = due_date;
        x["starred"] = starred;
        x["completed"] = completed;
        x["isTrashed"] = isTrashed;
        x["notes"] = notes;
    
        delete res;
        
        crow::response r = crow::response(x);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
    });
    
    app.port(18080).multithreaded().run();

     delete con;
}