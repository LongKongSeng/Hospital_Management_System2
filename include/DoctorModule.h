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
    string currentDoctorId;  // Now stores formatted_id (D001, D002, etc.)

public:
    DoctorModule(Database* database, const string& doctorId);  // Takes formatted_id string
    void showMenu(); // Main doctor menu from flowchart
    
    // Doctor Module Functions (from flowchart)
    void viewPatientRecord();
    void viewAppointments();
    void makeDiagnosis();
    void editPatientMedicalRecord();
    
    // Helper Functions
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPatientRecordTable(sql::ResultSet* res);
    void displayMedicalRecordTable(sql::ResultSet* res);
};

#endif





