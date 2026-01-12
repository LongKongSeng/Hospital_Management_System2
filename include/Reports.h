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

    // New Helpers
    string getMonthName(int monthNumber);
    void displayIntegratedChart(const vector<pair<string, int>>& data, const string& labelHeader);

public:
    Reports(Database* database);
    void showMenu();

    // Prescription Reports Only (Trend Chart integrated into these)
    void generatePrescriptionReport();
    void generateMonthlyPrescriptionReport();
    void generateYearlyPrescriptionReport();
};

#endif
