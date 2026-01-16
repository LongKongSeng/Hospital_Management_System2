#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime>
#include <cctype>

using namespace std;

class Registration {
private:
    Database* db;
    // Helper for masking
    string getMaskedInput();

public:
    Registration(Database* database);
    void showPreRegistrationMenu();
    void registerDoctor();
    void registerNurse();
    void registerAdmin();

    bool validatePassword(const string& password1, const string& password2);
    // New validation for complexity
    bool validatePasswordComplexity(const string& password);
    bool isIcDuplicate(const string& ic);
    bool isContactDuplicate(const string& contact);
    bool isUsernameDuplicate(const string& username);

    bool validateContactNumber(const string& contactNumber);
    bool validateFullName(const string& fullName);
    bool validateICNumber(const string& icNumber);
    bool validateEmail(const string& email);
    string validateAndCorrectGender(string& gender);
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    string getStringInput(const string& prompt);
};

#endif
