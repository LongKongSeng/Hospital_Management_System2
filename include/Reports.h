#ifndef REPORTS_H
#define REPORTS_H

#include "Database.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

class Reports {
private:
    Database* db;

public:
    Reports(Database* database);
    void showMenu();
    
    // Complex Calculations
    void calculateTotalInventoryValue();
    void calculateCategoryStatistics();
    void calculateMonthlySales();
    void calculateProfitMargins();
    void calculatePatientStatistics();
    void calculatePrescriptionStatistics();
    
    // Report Generation
    void generateInventoryReport();
    void generatePatientReport();
    void generatePrescriptionReport();
    void generateFinancialReport();
    void generateLowStockReport();
    void generateExpiringItemsReport();
    
    // Text-based Charts and Graphs
    void displayInventoryValueChart();
    void displayCategoryDistributionChart();
    void displayMonthlyTransactionChart();
    void displayPatientStatusChart();
    
    // Helper Functions
    void displayTableHeader(const string& title);
    void pressEnterToContinue();
    void displayHorizontalBar(const string& label, double value, double maxValue, int width = 50);
    void displayVerticalBar(const string& label, double value, double maxValue, int height = 20);
};

#endif

