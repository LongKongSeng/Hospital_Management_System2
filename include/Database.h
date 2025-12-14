#ifndef DATABASE_H
#define DATABASE_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;
using namespace sql;

class Database {
private:
    sql::Driver* driver;
    sql::Connection* con;
    string host;
    string user;
    string password;
    string database;

public:
    Database();
    ~Database();
    bool connect();
    void disconnect();
    sql::Connection* getConnection();
    bool executeQuery(const string& query);
    sql::ResultSet* executeSelect(const string& query);
    bool executeUpdate(const string& query);
    void displayError(const string& operation);
};

#endif

