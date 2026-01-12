#ifndef ADMIN_MODULE_H
#define ADMIN_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

class AdminModule {
private:
    Database* db;

public:
    AdminModule(Database* database);
    void showMenu(); // Main admin menu from flowchart
    
    // Admin Module Functions (from flowchart)
    void viewPharmacy();
    void addPatient();
    void patientReceipt();
    
    // Pharmacy View Functions
    void displayPharmacyList();
    void filterPharmacyByCategory();

    
    // Patient Receipt Functions
    void calculatePatientReceipt(const string& patientId);
    void displayReceipt(const string& patientId, double totalAmount);
    
    // Helper Functions
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPharmacyTable(sql::ResultSet* res);
    
    // Validation Functions
    bool validatePhoneNumber(const string& phoneNumber);
    bool validateDateFormat(const string& date);
    bool validateBloodType(const string& bloodType);
    bool validateICNumber(const string& icNumber);
    string validateAndCorrectGender(string& gender);
};

#endif
