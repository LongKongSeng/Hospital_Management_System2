#ifndef DATABASE_H
#define DATABASE_H

// --- FIX: Correct MySQL Includes & Warning Suppression ---
#pragma warning(push)
#pragma warning(disable : 26495) // Suppress false positive warning from MySQL library
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#pragma warning(pop)
// ---------------------------------------------------------

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

    // --- NEW HELPER: Check Active Status ---
    bool isPatientActive(const string& patientId);
};

#endif
