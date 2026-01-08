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
    string currentNurseId;  // Now stores formatted_id (N001, N002, etc.)

public:
    NurseModule(Database* database, const string& nurseId);  // Takes formatted_id string
    void showMenu(); // Main nurse menu from flowchart
    
    // Nurse Module Functions (from flowchart)
    void viewPatientRecord();
    void generateNextAppointment();
    
    // Helper Functions
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPatientRecordTable(sql::ResultSet* res);
    void displayAppointmentTable(sql::ResultSet* res);
};

#endif










