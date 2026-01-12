#ifndef REPORTS_H
#define REPORTS_H

#include "Database.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

class Reports {
private:
    Database* db;
    void displayTableHeader(const string& title);
    void pressEnterToContinue();

public:
    Reports(Database* database);
    void showMenu();

    // Prescription Reports Only
    void generatePrescriptionReport();
    void generateMonthlyPrescriptionReport();
    void generateYearlyPrescriptionReport();
    void displayPrescriptionTrendChart();
};

#endif
