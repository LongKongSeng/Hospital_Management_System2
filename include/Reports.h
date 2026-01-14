#ifndef REPORTS_H
#define REPORTS_H

#include "Database.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

class Reports {
private:
    Database* db;

public:
    Reports(Database* database);
    void showMenu();

    // Core Report Functions
    void generateMonthlyReport();
    void generateYearlyReport();

    // Helper Functions
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
};

#endif
