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
    void showMenu();
    void managePatients();
    void manageInventory();
    void viewInventoryReports();
    void createPatient();
    void readPatients();
    void updatePatient();
    void deletePatient();
    void createInventory();
    void readInventory();
    void updateInventory();
    void deleteInventory();
    void updateInventoryQuantity();
    void displayPatientTable(sql::ResultSet* res);
    void displayInventoryTable(sql::ResultSet* res);
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
};

#endif

