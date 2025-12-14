#ifndef STAFF_MODULE_H
#define STAFF_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

class StaffModule {
private:
    Database* db;
    int currentStaffId;

public:
    StaffModule(Database* database, int staffId);
    void showMenu();
    void viewPatientStatus();
    void updatePatientStatus();
    void updatePrescription();
    void deleteFinishedPrescriptions();
    void updatePatientReport();
    void displayPatientTable(sql::ResultSet* res);
    void displayPrescriptionTable(sql::ResultSet* res);
    void displayReportTable(sql::ResultSet* res);
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
};

#endif

