#ifndef DOCTOR_MODULE_H
#define DOCTOR_MODULE_H

#include "Database.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class DoctorModule {
private:
    Database* db;
    string currentDoctorId;

    string searchPatientId();

public:
    DoctorModule(Database* database, const string& doctorId);
    void showMenu();

    void viewPatientRecord();
    void viewAppointments();
    void makeDiagnosis();
    void editPatientMedicalRecord();

    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    void displayMedicalRecordTable(sql::ResultSet* res);
    void displayPatientRecordTable(sql::ResultSet* res);
};

#endif
