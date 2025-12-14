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
    int currentUserId;
    string currentUserType;
    string currentUsername;

public:
    Login(Database* database);
    bool authenticate(const string& username, const string& password, const string& userType);
    void showLoginMenu();
    int getCurrentUserId();
    string getCurrentUserType();
    string getCurrentUsername();
    void logout();
    void displayTableHeader(const string& title);
};

#endif

