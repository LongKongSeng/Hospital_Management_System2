#ifndef NURSE_MODULE_H
#define NURSE_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

class NurseModule {
private:
    Database* db;
    string currentNurseId;  // Stores formatted_id (N001, N002, etc.)

    // Search Helpers
    string searchPatientId();
    string searchDoctorId();

public:
    NurseModule(Database* database, const string& nurseId);
    void showMenu();

    // Core Functions
    void viewPatientRecord();
    void generateNextAppointment();

    // UI Helpers
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPatientRecordTable(sql::ResultSet* res);
    void displayAppointmentTable(sql::ResultSet* res);
};

#endif
