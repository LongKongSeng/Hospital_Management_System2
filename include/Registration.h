#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

class Registration {
private:
    Database* db;

public:
    Registration(Database* database);
    void showMenu();
    void registerPatient();
    void registerStaff();
    void registerAdmin();
    bool validateEmail(const string& email);
    bool validatePhone(const string& phone);
    bool usernameExists(const string& username, const string& table);
    bool emailExists(const string& email, const string& table);
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
};

#endif

