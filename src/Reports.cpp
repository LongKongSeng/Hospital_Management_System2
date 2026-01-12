#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "Total Inventory Value (N/A)", // Marked N/A due to missing table
            "Category Statistics (N/A)",
            "Monthly Sales Analysis (N/A)",
            "Profit Margin Analysis (N/A)",
            "Patient Statistics",
            "Prescription Statistics",
            "Inventory Report (N/A)",
            "Patient Report",
            "Prescription Report",
            "Financial Report (Partial)",
            "Low Stock Report (N/A)",
            "Expiring Items Report (N/A)",
            "Inventory Value Chart (N/A)",
            "Category Distribution Chart (N/A)",
            "Monthly Transaction Chart (N/A)",
            "Patient Status Chart",
            "Monthly Prescription Report",
            "Yearly Prescription Report",
            "Prescription Trend Chart",
            "Back to Main Menu"
        };

        choice = MenuNavigator::showMenu(menuOptions, "REPORTS & ANALYTICS MODULE", true);

        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0: calculateTotalInventoryValue(); break;
        case 1: calculateCategoryStatistics(); break;
        case 2: calculateMonthlySales(); break;
        case 3: calculateProfitMargins(); break;
        case 4: calculatePatientStatistics(); break;
        case 5: calculatePrescriptionStatistics(); break;
        case 6: generateInventoryReport(); break;
        case 7: generatePatientReport(); break;
        case 8: generatePrescriptionReport(); break;
        case 9: generateFinancialReport(); break;
        case 10: generateLowStockReport(); break;
        case 11: generateExpiringItemsReport(); break;
        case 12: displayInventoryValueChart(); break;
        case 13: displayCategoryDistributionChart(); break;
        case 14: displayMonthlyTransactionChart(); break;
        case 15: displayPatientStatusChart(); break;
        case 16: generateMonthlyPrescriptionReport(); break;
        case 17: generateYearlyPrescriptionReport(); break;
        case 18: displayPrescriptionTrendChart(); break;
        case 19: return;
        default:
            cout << "\n[ERROR] Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 19);
}

// ---------------------------------------------------------
// INVENTORY FUNCTIONS (DISABLED - Table 'inventory' missing)
// ---------------------------------------------------------
void Reports::calculateTotalInventoryValue() {
    system("cls");
    displayTableHeader("TOTAL INVENTORY VALUE");
    cout << "\n⚠️  Error: Table 'inventory' does not exist in the database schema." << endl;
    cout << "    This feature is currently unavailable." << endl;
    pressEnterToContinue();
}

void Reports::calculateCategoryStatistics() {
    system("cls");
    displayTableHeader("CATEGORY STATISTICS");
    cout << "\n⚠️  Error: Table 'inventory' does not exist in the database schema." << endl;
    pressEnterToContinue();
}

void Reports::calculateMonthlySales() {
    system("cls");
    displayTableHeader("MONTHLY SALES ANALYSIS");
    cout << "\n⚠️  Error: Table 'inventory_transaction' does not exist in the database schema." << endl;
    pressEnterToContinue();
}

void Reports::calculateProfitMargins() {
    system("cls");
    displayTableHeader("PROFIT MARGIN ANALYSIS");
    cout << "\n⚠️  Error: Table 'inventory' does not exist in the database schema." << endl;
    pressEnterToContinue();
}

// ---------------------------------------------------------
// PATIENT STATISTICS (FIXED JOINS)
// ---------------------------------------------------------
void Reports::calculatePatientStatistics() {
    system("cls");
    displayTableHeader("PATIENT STATISTICS");

    try {
        // FIXED: Joined through medical_record and diagnosis to count prescriptions
        string query = "SELECT p.status, "
            "COUNT(DISTINCT p.formatted_id) as patient_count, "
            "COUNT(DISTINCT d.prescription_id) as total_prescriptions, "
            "COUNT(DISTINCT mr.formatted_id) as total_reports, "
            "AVG(DATEDIFF(CURDATE(), p.created_at)) as avg_days_registered "
            "FROM patient p "
            "LEFT JOIN medical_record mr ON p.formatted_id = mr.patient_id "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "GROUP BY p.status "
            "ORDER BY patient_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+----------------------+---------------+----------------------+--------------+----------------------+" << endl;
            cout << "| Status               | Patient Count | Total Prescriptions | Total Reports| Avg Days Registered |" << endl;
            cout << "+----------------------+---------------+----------------------+--------------+----------------------+" << endl;

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
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// PRESCRIPTION STATISTICS (FIXED JOINS)
// ---------------------------------------------------------
void Reports::calculatePrescriptionStatistics() {
    system("cls");
    displayTableHeader("PRESCRIPTION STATISTICS (DOSAGE)");

    try {
        // FIXED: Joined to patient via diagnosis->medical_record
        string query = "SELECT pr.dosage, "
            "COUNT(pr.formatted_id) as dosage_count, "
            "COUNT(DISTINCT mr.patient_id) as unique_patients "
            "FROM prescription pr "
            "LEFT JOIN diagnosis d ON d.prescription_id = pr.formatted_id "
            "LEFT JOIN medical_record mr ON mr.diagnosis_id = d.formatted_id "
            "GROUP BY pr.dosage "
            "ORDER BY dosage_count DESC LIMIT 10";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+----------------------+----------------------+----------------------+" << endl;
            cout << "| Dosage               | Times Prescribed     | Unique Patients      |" << endl;
            cout << "+----------------------+----------------------+----------------------+" << endl;

            while (res->next()) {
                string dosage = res->getString("dosage");
                if (dosage.empty()) dosage = "Unspecified";
                if (dosage.length() > 20) dosage = dosage.substr(0, 17) + "...";

                cout << "| " << setw(20) << left << dosage
                    << "| " << setw(20) << right << res->getInt("dosage_count")
                    << "| " << setw(20) << right << res->getInt("unique_patients") << " |" << endl;
            }
            cout << "+----------------------+----------------------+----------------------+" << endl;
            delete res;
        }
        else {
            cout << "\n⚠️  No prescription data found." << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void Reports::generateInventoryReport() {
    system("cls");
    displayTableHeader("INVENTORY REPORT");
    cout << "\n⚠️  Error: Table 'inventory' does not exist." << endl;
    pressEnterToContinue();
}

// ---------------------------------------------------------
// PATIENT REPORT (FIXED COLUMN NAMES)
// ---------------------------------------------------------
void Reports::generatePatientReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PATIENT REPORT");

    try {
        // FIXED: p.patient_id -> p.formatted_id, fixed join logic
        string query = "SELECT p.formatted_id, p.full_name, p.contact_number, p.status, "
            "p.created_at, "
            "COUNT(DISTINCT d.prescription_id) as prescription_count, "
            "COUNT(DISTINCT mr.formatted_id) as report_count "
            "FROM patient p "
            "LEFT JOIN medical_record mr ON mr.patient_id = p.formatted_id "
            "LEFT JOIN diagnosis d ON d.formatted_id = mr.diagnosis_id "
            "GROUP BY p.formatted_id, p.full_name, p.contact_number, p.status, p.created_at "
            "ORDER BY p.formatted_id";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-------------+----------------------+----------------------+--------------+----------------------+----------------------+--------------+" << endl;
            cout << "| Patient ID  | Full Name            | Contact Number       | Status       | Registration Date    | Prescription Count   | Report Count |" << endl;
            cout << "+-------------+----------------------+----------------------+--------------+----------------------+----------------------+--------------+" << endl;

            while (res->next()) {
                string regDate = res->isNull("created_at") ? "N/A" : res->getString("created_at").substr(0, 10);
                cout << "| " << setw(11) << res->getString("formatted_id")
                    << "| " << setw(20) << left << res->getString("full_name").substr(0, 19)
                    << "| " << setw(20) << res->getString("contact_number")
                    << "| " << setw(12) << res->getString("status")
                    << "| " << setw(20) << regDate
                    << "| " << setw(20) << right << res->getInt("prescription_count")
                    << "| " << setw(12) << right << res->getInt("report_count") << "|" << endl;
            }
            cout << "+-------------+----------------------+----------------------+--------------+----------------------+----------------------+--------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// PRESCRIPTION REPORT (YOUR FIXED VERSION)
// ---------------------------------------------------------
void Reports::generatePrescriptionReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PRESCRIPTION REPORT");

    try {
        string query = "SELECT pr.formatted_id, pt.full_name as patient_name, "
            "ph.medicine_name, pr.dosage, pr.duration_of_meds, "
            "pr.date, "
            "DATEDIFF(CURDATE(), pr.date) as days_since_prescribed "
            "FROM prescription pr "
            "JOIN diagnosis d ON d.prescription_id = pr.formatted_id "
            "JOIN medical_record mr ON mr.diagnosis_id = d.formatted_id "
            "JOIN patient pt ON mr.patient_id = pt.formatted_id "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "ORDER BY pr.date DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+" << endl;
            cout << "| Prescription ID | Patient Name         | Medication           | Dosage    | Duration             | Prescribed Date      |" << endl;
            cout << "+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+" << endl;

            while (res->next()) {
                string presDate = res->getString("date").substr(0, 10);
                cout << "| " << setw(15) << res->getString("formatted_id")
                    << "| " << setw(20) << left << res->getString("patient_name").substr(0, 19)
                    << "| " << setw(20) << res->getString("medicine_name").substr(0, 19)
                    << "| " << setw(9) << res->getString("dosage").substr(0, 8)
                    << "| " << setw(20) << res->getString("duration_of_meds").substr(0, 19)
                    << "| " << setw(20) << presDate << " |" << endl;
            }
            cout << "+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+" << endl;
            delete res;
        }
        else {
            cout << "\n⚠️  No prescription records found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void Reports::generateFinancialReport() {
    system("cls");
    displayTableHeader("FINANCIAL REPORT");
    cout << "\n⚠️  Inventory Data Missing." << endl;
    pressEnterToContinue();
}

void Reports::generateLowStockReport() {
    system("cls");
    displayTableHeader("LOW STOCK ALERT");
    cout << "\n⚠️  Error: Table 'inventory' does not exist." << endl;
    pressEnterToContinue();
}

void Reports::generateExpiringItemsReport() {
    system("cls");
    displayTableHeader("EXPIRING ITEMS");
    cout << "\n⚠️  Error: Table 'inventory' does not exist." << endl;
    pressEnterToContinue();
}

void Reports::displayInventoryValueChart() {
    system("cls");
    displayTableHeader("INVENTORY VALUE CHART");
    cout << "\n⚠️  Error: Table 'inventory' does not exist." << endl;
    pressEnterToContinue();
}

void Reports::displayCategoryDistributionChart() {
    system("cls");
    displayTableHeader("CATEGORY DISTRIBUTION CHART");
    cout << "\n⚠️  Error: Table 'inventory' does not exist." << endl;
    pressEnterToContinue();
}

void Reports::displayMonthlyTransactionChart() {
    system("cls");
    displayTableHeader("MONTHLY TRANSACTION CHART");
    cout << "\n⚠️  Error: Table 'inventory_transaction' does not exist." << endl;
    pressEnterToContinue();
}

void Reports::displayPatientStatusChart() {
    system("cls");
    displayTableHeader("PATIENT STATUS CHART");

    try {
        string query = "SELECT status, COUNT(*) as count FROM patient GROUP BY status ORDER BY count DESC";
        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            int maxCount = 0;
            vector<pair<string, int>> statuses;

            while (res->next()) {
                string status = res->getString("status");
                int count = res->getInt("count");
                statuses.push_back({ status, count });
                if (count > maxCount) maxCount = count;
            }
            delete res;

            if (maxCount > 0) {
                cout << "\n";
                for (const auto& s : statuses) {
                    displayHorizontalBar(s.first, s.second, maxCount, 50);
                }
            }
            else {
                cout << "\nNo patient data found." << endl;
            }
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void Reports::displayHorizontalBar(const string& label, double value, double maxValue, int width) {
    int barLength = (maxValue > 0) ? (int)((value / maxValue) * width) : 0;
    cout << left << setw(25) << label.substr(0, 24) << " |";
    for (int i = 0; i < barLength; i++) {
        cout << "█";
    }
    cout << " " << fixed << setprecision(0) << value << endl;
}

void Reports::displayTableHeader(const string& title) {
    ColorUtils::setColor(BLUE);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();

    ColorUtils::setColor(WHITE);
    cout << "|";
    int padding = static_cast<int>((60 - static_cast<int>(title.length())) / 2);
    for (int i = 0; i < padding; i++) cout << " ";
    ColorUtils::printColoredBG(title, YELLOW, BLUE);
    for (int i = 0; i < static_cast<int>(60 - title.length() - padding); i++) cout << " ";
    ColorUtils::setColor(WHITE);
    cout << "|" << endl;

    ColorUtils::setColor(BLUE);
    cout << "|" << setw(60) << "" << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    ColorUtils::resetColor();
}

void Reports::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

// ---------------------------------------------------------
// MONTHLY PRESCRIPTION REPORT (CORRECTED)
// ---------------------------------------------------------
void Reports::generateMonthlyPrescriptionReport() {
    system("cls");
    displayTableHeader("MONTHLY PRESCRIPTION REPORT");

    try {
        string query = "SELECT DATE_FORMAT(pr.date, '%Y-%m') as month_str, "
            "ph.medicine_name, "
            "doc.full_name as doctor_name, "
            "COUNT(pr.formatted_id) as total_count "
            "FROM prescription pr "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "LEFT JOIN diagnosis d ON d.prescription_id = pr.formatted_id "
            "LEFT JOIN medical_record mr ON mr.diagnosis_id = d.formatted_id "
            "LEFT JOIN doctor doc ON mr.doctor_id = doc.formatted_id "
            "GROUP BY month_str, ph.medicine_name, doc.full_name "
            "ORDER BY month_str DESC, total_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-----------+----------------------+----------------------+-------------+" << endl;
            cout << "| Month     | Medication Name      | Prescribed By (Doc)  | Total Count |" << endl;
            cout << "+-----------+----------------------+----------------------+-------------+" << endl;

            string currentMonth = "";
            while (res->next()) {
                string month = res->getString("month_str");
                string med = res->getString("medicine_name");
                string doc = res->isNull("doctor_name") ? "Unknown" : res->getString("doctor_name");
                int count = res->getInt("total_count");

                if (month != currentMonth && !currentMonth.empty()) {
                    cout << "+-----------+----------------------+----------------------+-------------+" << endl;
                }
                currentMonth = month;

                if (med.length() > 20) med = med.substr(0, 17) + "...";
                if (doc.length() > 20) doc = doc.substr(0, 17) + "...";

                cout << "| " << setw(9) << left << month
                    << "| " << setw(20) << med
                    << "| " << setw(20) << doc
                    << "| " << setw(11) << right << count << " |" << endl;
            }
            cout << "+-----------+----------------------+----------------------+-------------+" << endl;
            delete res;
        }
        else {
            cout << "\n⚠️  No prescription records found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void Reports::generateYearlyPrescriptionReport() {
    system("cls");
    displayTableHeader("YEARLY PRESCRIPTION REPORT");

    try {
        string query = "SELECT DATE_FORMAT(pr.date, '%Y') as year_str, "
            "ph.category_of_meds, "
            "COUNT(pr.formatted_id) as total_count, "
            "SUM(ph.unit_price) as estimated_revenue "
            "FROM prescription pr "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "GROUP BY year_str, ph.category_of_meds "
            "ORDER BY year_str DESC, total_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-------+----------------------+-------------+-------------------+" << endl;
            cout << "| Year  | Medication Category  | Total Count | Est. Revenue (RM) |" << endl;
            cout << "+-------+----------------------+-------------+-------------------+" << endl;

            while (res->next()) {
                cout << "| " << setw(5) << left << res->getString("year_str")
                    << "| " << setw(20) << res->getString("category_of_meds")
                    << "| " << setw(11) << right << res->getInt("total_count")
                    << "| " << setw(17) << right << fixed << setprecision(2) << res->getDouble("estimated_revenue") << " |" << endl;
            }
            cout << "+-------+----------------------+-------------+-------------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void displayVerticalBarChart(const vector<pair<string, int>>& data) {
    if (data.empty()) {
        cout << "\n[No Data]\n";
        return;
    }
    int maxVal = 0;
    for (const auto& d : data) maxVal = max(maxVal, d.second);

    cout << "\n   (Count)\n";
    for (int row = 10; row > 0; row--) {
        int threshold = (maxVal * row) / 10;
        if (threshold == 0 && row == 1 && maxVal > 0) threshold = 1;
        cout << setw(4) << threshold << " |";
        for (const auto& d : data) {
            int barHeight = (maxVal > 0) ? (int)(((double)d.second / maxVal) * 10.0) : 0;
            if (barHeight >= row) cout << "  █  ";
            else                  cout << "     ";
        }
        cout << endl;
    }
    cout << "      +";
    for (size_t i = 0; i < data.size(); i++) cout << "-----";
    cout << endl;
    cout << "       ";
    for (const auto& d : data) {
        string label = (d.first.length() >= 7) ? d.first.substr(5, 2) : "??";
        cout << "  " << label << " ";
    }
    cout << "\n         (Month)" << endl;
}

void Reports::displayPrescriptionTrendChart() {
    system("cls");
    displayTableHeader("PRESCRIPTION TREND CHART");

    try {
        string query = "SELECT DATE_FORMAT(date, '%Y-%m') as month, "
            "COUNT(*) as count "
            "FROM prescription "
            "GROUP BY month "
            "ORDER BY month DESC LIMIT 6";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            vector<pair<string, int>> data;
            while (res->next()) {
                string month = res->getString("month");
                int count = res->getInt("count");
                data.insert(data.begin(), { month, count });
            }
            delete res;
            if (!data.empty()) {
                displayVerticalBarChart(data);
                cout << "\nNote: Labels show the Month number." << endl;
            }
            else {
                cout << "\n⚠️  No data available." << endl;
            }
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}
