#ifndef DOCTOR_MODULE_H
#define DOCTOR_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

class DoctorModule {
private:
    Database* db;
    string currentDoctorId;  // Stores formatted_id (D001, D002, etc.)

    // Internal Helper for Searching
    string searchPatientId();

public:
    DoctorModule(Database* database, const string& doctorId);
    void showMenu();

    // Core Functions
    void viewPatientRecord();
    void viewAppointments();
    void makeDiagnosis();
    void editPatientMedicalRecord();

    // UI Helpers
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPatientRecordTable(sql::ResultSet* res);
    void displayMedicalRecordTable(sql::ResultSet* res);
};

#endif
