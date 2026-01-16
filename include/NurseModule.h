#ifndef NURSE_MODULE_H
#define NURSE_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

// --- FIX: Correct MySQL Includes & Warning Suppression ---
#pragma warning(push)
#pragma warning(disable : 26495) // Suppress false positive warning from MySQL library
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#pragma warning(pop)
// ---------------------------------------------------------

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

    // --- NEW FUNCTIONS ADDED HERE ---
    void updatePatientStatus();      // Feature 1: Active/Inactive
    void updateAppointmentStatus();  // Feature 2: Scheduled/Completed
    // --------------------------------

    // UI Helpers
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayPatientRecordTable(sql::ResultSet* res);
    void displayAppointmentTable(sql::ResultSet* res);
};

#endif
