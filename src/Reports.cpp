#include "Reports.h"
#include "ColorUtils.h"

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        system("cls");
        displayTableHeader("REPORTS & ANALYTICS MODULE");
        
        // Bright White/Yellow color theme for reports menu
        ColorUtils::setColor(WHITE);
        cout << "\n+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(WHITE);
        cout << "|  ";
        ColorUtils::printColored("COMPLEX CALCULATIONS:", YELLOW);
        ColorUtils::setColor(WHITE);
        cout << "                  |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("1. Total Inventory Value", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "              |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("2. Category Statistics", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "               |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("3. Monthly Sales Analysis", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "            |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("4. Profit Margin Analysis", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "             |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("5. Patient Statistics", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                 |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("6. Prescription Statistics", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "            |" << endl;
        
        cout << "|                                        |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("REPORT GENERATION:", YELLOW);
        ColorUtils::setColor(WHITE);
        cout << "                   |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("7. Inventory Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                  |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("8. Patient Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                    |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("9. Prescription Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("10. Financial Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                  |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("11. Low Stock Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "                 |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("12. Expiring Items Report", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "            |" << endl;
        
        cout << "|                                        |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("CHARTS & GRAPHS:", YELLOW);
        ColorUtils::setColor(WHITE);
        cout << "                     |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("13. Inventory Value Chart", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "            |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("14. Category Distribution Chart", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "      |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("15. Monthly Transaction Chart", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "        |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("16. Patient Status Chart", WHITE);
        ColorUtils::setColor(WHITE);
        cout << "             |" << endl;
        
        cout << "|                                        |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("0. Back to Main Menu", RED);
        ColorUtils::setColor(WHITE);
        cout << "                 |" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(WHITE);
        cout << "+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::printColored("\nEnter your choice: ", CYAN);
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: calculateTotalInventoryValue(); break;
        case 2: calculateCategoryStatistics(); break;
        case 3: calculateMonthlySales(); break;
        case 4: calculateProfitMargins(); break;
        case 5: calculatePatientStatistics(); break;
        case 6: calculatePrescriptionStatistics(); break;
        case 7: generateInventoryReport(); break;
        case 8: generatePatientReport(); break;
        case 9: generatePrescriptionReport(); break;
        case 10: generateFinancialReport(); break;
        case 11: generateLowStockReport(); break;
        case 12: generateExpiringItemsReport(); break;
        case 13: displayInventoryValueChart(); break;
        case 14: displayCategoryDistributionChart(); break;
        case 15: displayMonthlyTransactionChart(); break;
        case 16: displayPatientStatusChart(); break;
        case 0: return;
        default:
            cout << "\n❌ Invalid choice! Please try again." << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

// Complex Calculation: Total Inventory Value (Aggregation)
void Reports::calculateTotalInventoryValue() {
    system("cls");
    displayTableHeader("TOTAL INVENTORY VALUE CALCULATION");
    
    try {
        // Using SUM aggregation
        string query = "SELECT SUM(quantity * unit_price) as total_value, "
                      "SUM(quantity) as total_quantity, "
                      "COUNT(*) as item_count, "
                      "AVG(unit_price) as avg_price "
                      "FROM inventory";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res && res->next()) {
            double totalValue = res->getDouble("total_value");
            int totalQuantity = res->getInt("total_quantity");
            int itemCount = res->getInt("item_count");
            double avgPrice = res->getDouble("avg_price");
            
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|              INVENTORY VALUE CALCULATION RESULTS                 |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            cout << "| Total Inventory Value:    RM " << setw(40) << right << fixed << setprecision(2) << totalValue << "  |" << endl;
            cout << "| Total Quantity:           " << setw(45) << right << totalQuantity << "  |" << endl;
            cout << "| Number of Items:          " << setw(45) << right << itemCount << "  |" << endl;
            cout << "| Average Unit Price:       RM " << setw(40) << right << fixed << setprecision(2) << avgPrice << "  |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            
            delete res;
        } else {
            cout << "\n⚠️  No inventory data found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Complex Calculation: Category Statistics (Grouping & Aggregation)
void Reports::calculateCategoryStatistics() {
    system("cls");
    displayTableHeader("CATEGORY STATISTICS (GROUP BY & AGGREGATION)");
    
    try {
        // Using GROUP BY with multiple aggregations
        string query = "SELECT category, "
                      "COUNT(*) as item_count, "
                      "SUM(quantity) as total_quantity, "
                      "SUM(quantity * unit_price) as category_value, "
                      "AVG(unit_price) as avg_price, "
                      "MIN(quantity) as min_quantity, "
                      "MAX(quantity) as max_quantity "
                      "FROM inventory "
                      "GROUP BY category "
                      "ORDER BY category_value DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+--------------+-------------+-----------------+------------------+-------------+-------------+-------------+" << endl;
            cout << "| Category     | Item Count  | Total Quantity  | Category Value   | Avg Price   | Min Qty     | Max Qty     |" << endl;
            cout << "+--------------┼-------------┼-----------------┼------------------┼-------------┼-------------┼-------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(12) << left << res->getString("category")
                     << "| " << setw(11) << right << res->getInt("item_count")
                     << "| " << setw(15) << right << res->getInt("total_quantity")
                     << "| RM " << setw(12) << right << fixed << setprecision(2) << res->getDouble("category_value")
                     << "| RM " << setw(9) << right << fixed << setprecision(2) << res->getDouble("avg_price")
                     << "| " << setw(11) << right << res->getInt("min_quantity")
                     << "| " << setw(11) << right << res->getInt("max_quantity") << "|" << endl;
            }
            
            cout << "+--------------+-------------+-----------------+------------------+-------------+-------------+-------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Complex Calculation: Monthly Sales (Grouping with Date Functions)
void Reports::calculateMonthlySales() {
    system("cls");
    displayTableHeader("MONTHLY SALES ANALYSIS (GROUP BY & DATE FUNCTIONS)");
    
    try {
        // Using GROUP BY with DATE_FORMAT and aggregations
        string query = "SELECT DATE_FORMAT(transaction_date, '%Y-%m') as month, "
                      "DATE_FORMAT(transaction_date, '%M %Y') as month_name, "
                      "transaction_type, "
                      "SUM(quantity_change) as total_change, "
                      "COUNT(*) as transaction_count, "
                      "SUM(CASE WHEN transaction_type = 'Purchase' THEN quantity_change ELSE 0 END) as purchases, "
                      "SUM(CASE WHEN transaction_type = 'Usage' THEN ABS(quantity_change) ELSE 0 END) as usage "
                      "FROM inventory_transaction "
                      "GROUP BY DATE_FORMAT(transaction_date, '%Y-%m'), transaction_type "
                      "ORDER BY month DESC, transaction_type";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-----------+------------------+------------------+---------------+------------------+--------------+--------------+" << endl;
            cout << "| Month     | Month Name       | Transaction Type | Total Change  | Transaction Count| Purchases    | Usage         |" << endl;
            cout << "+-----------┼------------------┼------------------┼---------------┼------------------┼--------------┼--------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(9) << res->getString("month")
                     << "| " << setw(16) << res->getString("month_name")
                     << "| " << setw(16) << res->getString("transaction_type")
                     << "| " << setw(13) << res->getInt("total_change")
                     << "| " << setw(16) << res->getInt("transaction_count")
                     << "| " << setw(12) << res->getInt("purchases")
                     << "| " << setw(12) << res->getInt("usage") << "|" << endl;
            }
            
            cout << "+-----------+------------------+------------------+---------------+------------------+--------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Complex Calculation: Profit Margins (Subquery & Calculations)
void Reports::calculateProfitMargins() {
    system("cls");
    displayTableHeader("PROFIT MARGIN ANALYSIS (SUBQUERY & CALCULATIONS)");
    
    try {
        // Using subquery to calculate profit margins
        string query = "SELECT i.item_name, i.category, i.unit_price, "
                      "i.quantity, (i.quantity * i.unit_price) as total_value, "
                      "(SELECT COALESCE(SUM(CASE WHEN it.transaction_type = 'Purchase' THEN it.quantity_change * i.unit_price ELSE 0 END), 0) "
                      " FROM inventory_transaction it WHERE it.inventory_id = i.inventory_id) as purchase_cost, "
                      "(SELECT COALESCE(SUM(CASE WHEN it.transaction_type = 'Usage' THEN ABS(it.quantity_change) * i.unit_price ELSE 0 END), 0) "
                      " FROM inventory_transaction it WHERE it.inventory_id = i.inventory_id) as usage_value, "
                      "((i.quantity * i.unit_price) - "
                      "(SELECT COALESCE(SUM(CASE WHEN it.transaction_type = 'Purchase' THEN it.quantity_change * i.unit_price ELSE 0 END), 0) "
                      " FROM inventory_transaction it WHERE it.inventory_id = i.inventory_id)) as profit_margin "
                      "FROM inventory i "
                      "ORDER BY profit_margin DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+----------------------+--------------+-------------+----------+--------------+--------------+--------------+--------------+" << endl;
            cout << "| Item Name            | Category     | Unit Price  | Quantity | Total Value  | Purchase Cost| Usage Value  | Profit Margin|" << endl;
            cout << "+----------------------┼--------------┼-------------┼----------┼--------------┼--------------┼--------------┼--------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(20) << left << res->getString("item_name")
                     << "| " << setw(12) << res->getString("category")
                     << "| RM " << setw(9) << right << fixed << setprecision(2) << res->getDouble("unit_price")
                     << "| " << setw(8) << right << res->getInt("quantity")
                     << "| RM " << setw(12) << right << fixed << setprecision(2) << res->getDouble("total_value")
                     << "| RM " << setw(12) << right << fixed << setprecision(2) << res->getDouble("purchase_cost")
                     << "| RM " << setw(12) << right << fixed << setprecision(2) << res->getDouble("usage_value")
                     << "| RM " << setw(12) << right << fixed << setprecision(2) << res->getDouble("profit_margin") << "|" << endl;
            }
            
            cout << "+----------------------+--------------+-------------+----------+--------------+--------------+--------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Complex Calculation: Patient Statistics (Joins & Aggregations)
void Reports::calculatePatientStatistics() {
    system("cls");
    displayTableHeader("PATIENT STATISTICS (JOINS & AGGREGATIONS)");
    
    try {
        // Using JOIN with aggregations
        string query = "SELECT p.status, "
                      "COUNT(DISTINCT p.patient_id) as patient_count, "
                      "COUNT(DISTINCT pr.prescription_id) as total_prescriptions, "
                      "COUNT(DISTINCT r.report_id) as total_reports, "
                      "AVG(DATEDIFF(CURDATE(), p.registration_date)) as avg_days_registered "
                      "FROM patient p "
                      "LEFT JOIN prescription pr ON p.patient_id = pr.patient_id "
                      "LEFT JOIN patient_report r ON p.patient_id = r.patient_id "
                      "GROUP BY p.status "
                      "ORDER BY patient_count DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+----------------------+---------------+----------------------+--------------+----------------------+" << endl;
            cout << "| Status               | Patient Count | Total Prescriptions | Total Reports| Avg Days Registered |" << endl;
            cout << "+----------------------┼---------------┼----------------------┼--------------┼----------------------+" << endl;
            
            while (res->next()) {
                double avgDays = res->isNull("avg_days_registered") ? 0 : res->getDouble("avg_days_registered");
                cout << "| " << setw(20) << left << res->getString("status")
                     << "| " << setw(13) << right << res->getInt("patient_count")
                     << "| " << setw(20) << right << res->getInt("total_prescriptions")
                     << "| " << setw(12) << right << res->getInt("total_reports")
                     << "| " << setw(20) << right << fixed << setprecision(1) << avgDays << "|" << endl;
            }
            
            cout << "+----------------------+---------------+----------------------+--------------+----------------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Complex Calculation: Prescription Statistics (Unary Operations & Aggregations)
void Reports::calculatePrescriptionStatistics() {
    system("cls");
    displayTableHeader("PRESCRIPTION STATISTICS (UNARY OPERATIONS & AGGREGATIONS)");
    
    try {
        // Using unary operations (DATEDIFF) with aggregations
        string query = "SELECT pr.status, "
                      "COUNT(*) as prescription_count, "
                      "AVG(pr.duration) as avg_duration, "
                      "SUM(pr.duration) as total_duration, "
                      "MIN(DATEDIFF(CURDATE(), pr.prescribed_date)) as min_days_old, "
                      "MAX(DATEDIFF(CURDATE(), pr.prescribed_date)) as max_days_old, "
                      "AVG(DATEDIFF(CURDATE(), pr.prescribed_date)) as avg_days_old "
                      "FROM prescription pr "
                      "GROUP BY pr.status "
                      "ORDER BY prescription_count DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+----------------------+----------------------+---------------+-----------------+--------------+--------------+--------------+" << endl;
            cout << "| Status               | Prescription Count   | Avg Duration  | Total Duration  | Min Days Old | Max Days Old | Avg Days Old |" << endl;
            cout << "+----------------------┼----------------------┼---------------┼-----------------┼--------------┼--------------┼--------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(20) << left << res->getString("status")
                     << "| " << setw(20) << right << res->getInt("prescription_count")
                     << "| " << setw(13) << right << fixed << setprecision(1) << res->getDouble("avg_duration")
                     << "| " << setw(15) << right << res->getInt("total_duration")
                     << "| " << setw(12) << right << res->getInt("min_days_old")
                     << "| " << setw(12) << right << res->getInt("max_days_old")
                     << "| " << setw(12) << right << fixed << setprecision(1) << res->getDouble("avg_days_old") << "|" << endl;
            }
            
            cout << "+----------------------+----------------------+---------------+-----------------+--------------+--------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Comprehensive Inventory Report
void Reports::generateInventoryReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE INVENTORY REPORT");
    
    try {
        string query = "SELECT inventory_id, item_name, category, quantity, unit_price, "
                      "(quantity * unit_price) as total_value, supplier, expiry_date, "
                      "reorder_level, "
                      "CASE WHEN quantity <= reorder_level THEN 'LOW' ELSE 'OK' END as stock_status "
                      "FROM inventory "
                      "ORDER BY total_value DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-------------+----------------------+--------------+----------+-------------+------------------+--------------+--------------+" << endl;
            cout << "| Inventory ID| Item Name            | Category     | Quantity | Unit Price  | Total Value      | Stock Status | Supplier     |" << endl;
            cout << "+-------------┼----------------------┼--------------┼----------┼-------------┼------------------┼--------------┼--------------+" << endl;
            
            while (res->next()) {
                string expiry = res->isNull("expiry_date") ? "N/A" : res->getString("expiry_date");
                cout << "| " << setw(11) << res->getInt("inventory_id")
                     << "| " << setw(20) << left << res->getString("item_name")
                     << "| " << setw(12) << res->getString("category")
                     << "| " << setw(8) << right << res->getInt("quantity")
                     << "| RM " << setw(9) << right << fixed << setprecision(2) << res->getDouble("unit_price")
                     << "| RM " << setw(14) << right << fixed << setprecision(2) << res->getDouble("total_value")
                     << "| " << setw(12) << res->getString("stock_status")
                     << "| " << setw(16) << res->getString("supplier") << "|" << endl;
            }
            
            cout << "+-------------+----------------------+--------------+----------+-------------+------------------+--------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Patient Report
void Reports::generatePatientReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PATIENT REPORT");
    
    try {
        string query = "SELECT p.patient_id, p.full_name, p.email, p.phone, p.status, "
                      "p.registration_date, "
                      "COUNT(DISTINCT pr.prescription_id) as prescription_count, "
                      "COUNT(DISTINCT r.report_id) as report_count "
                      "FROM patient p "
                      "LEFT JOIN prescription pr ON p.patient_id = pr.patient_id "
                      "LEFT JOIN patient_report r ON p.patient_id = r.patient_id "
                      "GROUP BY p.patient_id, p.full_name, p.email, p.phone, p.status, p.registration_date "
                      "ORDER BY p.patient_id";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-------------+----------------------+----------------------+--------------+--------------+----------------------+----------------------+--------------+" << endl;
            cout << "| Patient ID  | Full Name            | Email                | Phone        | Status       | Registration Date    | Prescription Count   | Report Count |" << endl;
            cout << "+-------------┼----------------------┼----------------------┼--------------┼--------------┼----------------------┼----------------------┼--------------+" << endl;
            
            while (res->next()) {
                string regDate = res->isNull("registration_date") ? "N/A" : res->getString("registration_date").substr(0, 10);
                cout << "| " << setw(11) << res->getInt("patient_id")
                     << "| " << setw(20) << left << res->getString("full_name")
                     << "| " << setw(20) << res->getString("email")
                     << "| " << setw(12) << res->getString("phone")
                     << "| " << setw(12) << res->getString("status")
                     << "| " << setw(20) << regDate
                     << "| " << setw(20) << right << res->getInt("prescription_count")
                     << "| " << setw(12) << right << res->getInt("report_count") << "|" << endl;
            }
            
            cout << "+-------------+----------------------+----------------------+--------------+--------------+----------------------+----------------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Prescription Report
void Reports::generatePrescriptionReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PRESCRIPTION REPORT");
    
    try {
        string query = "SELECT pr.prescription_id, pt.full_name as patient_name, "
                      "pr.medication_name, pr.dosage, pr.frequency, pr.duration, "
                      "pr.status, pr.prescribed_date, "
                      "DATEDIFF(CURDATE(), pr.prescribed_date) as days_since_prescribed "
                      "FROM prescription pr "
                      "JOIN patient pt ON pr.patient_id = pt.patient_id "
                      "ORDER BY pr.prescribed_date DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-----------------+----------------------+----------------------+-----------+-----------+----------+--------------+----------------------+----------------------+" << endl;
            cout << "| Prescription ID | Patient Name          | Medication           | Dosage    | Frequency | Duration | Status       | Prescribed Date       | Days Since Prescribed|" << endl;
            cout << "+-----------------┼----------------------┼----------------------┼-----------┼-----------┼----------┼--------------┼----------------------┼----------------------+" << endl;
            
            while (res->next()) {
                string presDate = res->getString("prescribed_date").substr(0, 10);
                cout << "| " << setw(15) << res->getInt("prescription_id")
                     << "| " << setw(20) << left << res->getString("patient_name")
                     << "| " << setw(20) << res->getString("medication_name")
                     << "| " << setw(9) << res->getString("dosage")
                     << "| " << setw(9) << res->getString("frequency")
                     << "| " << setw(8) << res->getInt("duration")
                     << "| " << setw(12) << res->getString("status")
                     << "| " << setw(20) << presDate
                     << "| " << setw(20) << right << res->getInt("days_since_prescribed") << "|" << endl;
            }
            
            cout << "+-----------------+----------------------+----------------------+-----------+-----------+----------+--------------+----------------------+----------------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Financial Report
void Reports::generateFinancialReport() {
    system("cls");
    displayTableHeader("FINANCIAL REPORT");
    
    try {
        // Summary of financial data
        string query1 = "SELECT SUM(quantity * unit_price) as total_inventory_value FROM inventory";
        sql::ResultSet* res1 = db->executeSelect(query1);
        
        double totalInventoryValue = 0;
        if (res1 && res1->next()) {
            totalInventoryValue = res1->getDouble("total_inventory_value");
        }
        if (res1) delete res1;
        
        string query2 = "SELECT SUM(CASE WHEN transaction_type = 'Purchase' THEN quantity_change * "
                        "(SELECT unit_price FROM inventory WHERE inventory_id = inventory_transaction.inventory_id) ELSE 0 END) as total_purchases "
                        "FROM inventory_transaction";
        sql::ResultSet* res2 = db->executeSelect(query2);
        
        double totalPurchases = 0;
        if (res2 && res2->next()) {
            totalPurchases = res2->getDouble("total_purchases");
        }
        if (res2) delete res2;
        
        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    FINANCIAL SUMMARY                           |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Total Inventory Value:    RM " << setw(40) << right << fixed << setprecision(2) << totalInventoryValue << "  |" << endl;
        cout << "| Total Purchase Cost:      RM " << setw(40) << right << fixed << setprecision(2) << totalPurchases << "  |" << endl;
        cout << "| Estimated Profit Margin:  RM " << setw(40) << right << fixed << setprecision(2) << (totalInventoryValue - totalPurchases) << "  |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Low Stock Report
void Reports::generateLowStockReport() {
    system("cls");
    displayTableHeader("LOW STOCK ALERT REPORT");
    
    try {
        string query = "SELECT inventory_id, item_name, category, quantity, reorder_level, "
                      "unit_price, (quantity * unit_price) as current_value, "
                      "(reorder_level - quantity) as units_needed "
                      "FROM inventory "
                      "WHERE quantity <= reorder_level "
                      "ORDER BY (reorder_level - quantity) DESC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            if (res->rowsCount() == 0) {
                cout << "\n✅ All items are above reorder level!" << endl;
            } else {
                cout << "\n+-------------+----------------------+--------------+----------+--------------+-------------+------------------+-----------------+" << endl;
                cout << "| Inventory ID| Item Name            | Category     | Quantity | Reorder Level | Unit Price  | Current Value    | Units Needed    |" << endl;
                cout << "+-------------┼----------------------┼--------------┼----------┼--------------┼-------------┼------------------┼-----------------+" << endl;
                
                while (res->next()) {
                    cout << "| " << setw(11) << res->getInt("inventory_id")
                         << "| " << setw(20) << left << res->getString("item_name")
                         << "| " << setw(12) << res->getString("category")
                         << "| " << setw(8) << right << res->getInt("quantity")
                         << "| " << setw(12) << right << res->getInt("reorder_level")
                         << "| RM " << setw(9) << right << fixed << setprecision(2) << res->getDouble("unit_price")
                         << "| RM " << setw(14) << right << fixed << setprecision(2) << res->getDouble("current_value")
                         << "| " << setw(15) << right << res->getInt("units_needed") << "|" << endl;
                }
                
                cout << "+-------------+----------------------+--------------+----------+--------------+-------------+------------------+-----------------+" << endl;
            }
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Expiring Items Report (Unary Operation: DATEDIFF)
void Reports::generateExpiringItemsReport() {
    system("cls");
    displayTableHeader("EXPIRING ITEMS REPORT (Next 30 Days)");
    
    try {
        // Using DATEDIFF unary operation
        string query = "SELECT inventory_id, item_name, category, quantity, expiry_date, "
                      "DATEDIFF(expiry_date, CURDATE()) as days_until_expiry, "
                      "unit_price, (quantity * unit_price) as total_value "
                      "FROM inventory "
                      "WHERE expiry_date IS NOT NULL "
                      "AND expiry_date BETWEEN CURDATE() AND DATE_ADD(CURDATE(), INTERVAL 30 DAY) "
                      "ORDER BY expiry_date ASC";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            if (res->rowsCount() == 0) {
                cout << "\n✅ No items expiring in the next 30 days!" << endl;
            } else {
                cout << "\n+-------------+----------------------+--------------+----------+--------------+----------------------+-------------+------------------+" << endl;
                cout << "| Inventory ID| Item Name            | Category     | Quantity | Expiry Date  | Days Until Expiry    | Unit Price  | Total Value      |" << endl;
                cout << "+-------------┼----------------------┼--------------┼----------┼--------------┼----------------------┼-------------┼------------------+" << endl;
                
                while (res->next()) {
                    int daysLeft = res->getInt("days_until_expiry");
                    string urgency = (daysLeft <= 7) ? "⚠️ URGENT" : (daysLeft <= 14) ? "⚠️ SOON" : "";
                    
                    cout << "| " << setw(11) << res->getInt("inventory_id")
                         << "| " << setw(20) << left << res->getString("item_name")
                         << "| " << setw(12) << res->getString("category")
                         << "| " << setw(8) << right << res->getInt("quantity")
                         << "| " << setw(12) << res->getString("expiry_date")
                         << "| " << setw(20) << right << daysLeft << " " << urgency
                         << "| RM " << setw(9) << right << fixed << setprecision(2) << res->getDouble("unit_price")
                         << "| RM " << setw(14) << right << fixed << setprecision(2) << res->getDouble("total_value") << "|" << endl;
                }
                
                cout << "+-------------+----------------------+--------------+----------+--------------+----------------------+-------------+------------------+" << endl;
            }
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Chart: Inventory Value Chart (Horizontal Bar)
void Reports::displayInventoryValueChart() {
    system("cls");
    displayTableHeader("INVENTORY VALUE CHART (Text-based Bar Chart)");
    
    try {
        string query = "SELECT item_name, (quantity * unit_price) as value FROM inventory ORDER BY value DESC LIMIT 10";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            double maxValue = 0;
            vector<pair<string, double>> items;
            
            while (res->next()) {
                string name = res->getString("item_name");
                double value = res->getDouble("value");
                items.push_back({name, value});
                if (value > maxValue) maxValue = value;
            }
            delete res;
            
            if (maxValue > 0) {
                cout << "\n";
                for (const auto& item : items) {
                    displayHorizontalBar(item.first, item.second, maxValue, 50);
                }
            } else {
                cout << "\n⚠️  No inventory data available for chart!" << endl;
            }
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Chart: Category Distribution Chart
void Reports::displayCategoryDistributionChart() {
    system("cls");
    displayTableHeader("CATEGORY DISTRIBUTION CHART");
    
    try {
        string query = "SELECT category, SUM(quantity * unit_price) as category_value FROM inventory GROUP BY category ORDER BY category_value DESC";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            double maxValue = 0;
            vector<pair<string, double>> categories;
            
            while (res->next()) {
                string category = res->getString("category");
                double value = res->getDouble("category_value");
                categories.push_back({category, value});
                if (value > maxValue) maxValue = value;
            }
            delete res;
            
            if (maxValue > 0) {
                cout << "\n";
                for (const auto& cat : categories) {
                    displayHorizontalBar(cat.first, cat.second, maxValue, 50);
                }
            }
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Chart: Monthly Transaction Chart
void Reports::displayMonthlyTransactionChart() {
    system("cls");
    displayTableHeader("MONTHLY TRANSACTION CHART");
    
    try {
        string query = "SELECT DATE_FORMAT(transaction_date, '%Y-%m') as month, "
                      "SUM(CASE WHEN transaction_type = 'Purchase' THEN quantity_change ELSE 0 END) as purchases, "
                      "SUM(CASE WHEN transaction_type = 'Usage' THEN ABS(quantity_change) ELSE 0 END) as usage "
                      "FROM inventory_transaction "
                      "GROUP BY DATE_FORMAT(transaction_date, '%Y-%m') "
                      "ORDER BY month DESC LIMIT 6";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            vector<tuple<string, int, int>> months;
            int maxValue = 0;
            
            while (res->next()) {
                string month = res->getString("month");
                int purchases = res->getInt("purchases");
                int usage = res->getInt("usage");
                months.push_back({month, purchases, usage});
                if (purchases > maxValue) maxValue = purchases;
                if (usage > maxValue) maxValue = usage;
            }
            delete res;
            
            if (maxValue > 0) {
                cout << "\nMonthly Transactions (Last 6 Months):\n" << endl;
                for (const auto& m : months) {
                    cout << get<0>(m) << ":" << endl;
                    cout << "  Purchases: ";
                    displayHorizontalBar("", get<1>(m), maxValue, 30);
                    cout << "  Usage:     ";
                    displayHorizontalBar("", get<2>(m), maxValue, 30);
                    cout << endl;
                }
            }
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Chart: Patient Status Chart
void Reports::displayPatientStatusChart() {
    system("cls");
    displayTableHeader("PATIENT STATUS DISTRIBUTION CHART");
    
    try {
        string query = "SELECT status, COUNT(*) as count FROM patient GROUP BY status ORDER BY count DESC";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            int maxCount = 0;
            vector<pair<string, int>> statuses;
            
            while (res->next()) {
                string status = res->getString("status");
                int count = res->getInt("count");
                statuses.push_back({status, count});
                if (count > maxCount) maxCount = count;
            }
            delete res;
            
            if (maxCount > 0) {
                cout << "\n";
                for (const auto& s : statuses) {
                    displayHorizontalBar(s.first, s.second, maxCount, 50);
                }
            }
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Helper: Display Horizontal Bar
void Reports::displayHorizontalBar(const string& label, double value, double maxValue, int width) {
    int barLength = (int)((value / maxValue) * width);
    cout << left << setw(25) << label.substr(0, 24) << " |";
    for (int i = 0; i < barLength; i++) {
        cout << "█";
    }
    cout << " " << fixed << setprecision(2) << value << endl;
}

// Helper: Display Vertical Bar (for future use)
void Reports::displayVerticalBar(const string& label, double value, double maxValue, int height) {
    int barHeight = (int)((value / maxValue) * height);
    cout << label << ":" << endl;
    for (int i = height; i >= 1; i--) {
        if (i <= barHeight) {
            cout << "  █" << endl;
        } else {
            cout << "   " << endl;
        }
    }
    cout << "  " << fixed << setprecision(2) << value << endl;
}

void Reports::displayTableHeader(const string& title) {
    // Bright White/Yellow theme header
    ColorUtils::setColor(WHITE);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title
    ColorUtils::setColor(WHITE);
    cout << "|";
    int padding = (60 - title.length()) / 2;
    for (int i = 0; i < padding; i++) cout << " ";
    ColorUtils::printColoredBG(title, BLACK, YELLOW);
    for (int i = 0; i < (60 - title.length() - padding); i++) cout << " ";
    ColorUtils::setColor(WHITE);
    cout << "|" << endl;
    
    ColorUtils::setColor(WHITE);
    cout << "|" << setw(60) << "" << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    ColorUtils::resetColor();
}

void Reports::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

