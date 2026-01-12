#ifndef LOGIN_H
#define LOGIN_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class Login {
private:
    Database* db;
    string currentUserId;
    string currentUserType;
    string currentUsername;
    string currentRole;

    // Helper for password masking
    string getMaskedInput();

public:
    Login(Database* database);
    bool authenticate(const string& username, const string& password);
    void showLoginMenu();
    string getCurrentUserId();
    string getCurrentUserType();
    string getCurrentUsername();
    string getCurrentRole();
    void logout();
    void displayTableHeader(const string& title);
};

#endif
