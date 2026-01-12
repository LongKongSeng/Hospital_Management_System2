#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        // CLEANED MENU: Only Prescription Reports remain
        vector<string> menuOptions = {
            "Prescription Report (All)",
            "Monthly Prescription Report",
            "Yearly Prescription Report",
            "Prescription Trend Chart",
            "Back to Admin Menu"
        };

        choice = MenuNavigator::showMenu(menuOptions, "PRESCRIPTION REPORTS", true);

        if (choice == -1) return;

        switch (choice) {
        case 0: generatePrescriptionReport(); break;
        case 1: generateMonthlyPrescriptionReport(); break;
        case 2: generateYearlyPrescriptionReport(); break;
        case 3: displayPrescriptionTrendChart(); break;
        case 4: return;
        }
    } while (true);
}

// ---------------------------------------------------------
// PRESCRIPTION REPORT (ALL)
// ---------------------------------------------------------
void Reports::generatePrescriptionReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PRESCRIPTION REPORT");

    try {
        string query = "SELECT pr.formatted_id, pt.full_name as patient_name, "
            "ph.medicine_name, pr.dosage, pr.duration_of_meds, "
            "pr.date "
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
            cout << "\nNo prescription records found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// MONTHLY REPORT
// ---------------------------------------------------------
void Reports::generateMonthlyPrescriptionReport() {
    system("cls");
    displayTableHeader("MONTHLY PRESCRIPTION REPORT");

    try {
        string query = "SELECT DATE_FORMAT(pr.date, '%Y-%m') as month_str, "
            "ph.medicine_name, "
            "COUNT(pr.formatted_id) as total_count "
            "FROM prescription pr "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "GROUP BY month_str, ph.medicine_name "
            "ORDER BY month_str DESC, total_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-----------+----------------------+-------------+" << endl;
            cout << "| Month     | Medication Name      | Total Count |" << endl;
            cout << "+-----------+----------------------+-------------+" << endl;
            while (res->next()) {
                cout << "| " << setw(9) << left << res->getString("month_str")
                    << "| " << setw(20) << res->getString("medicine_name").substr(0, 19)
                    << "| " << setw(11) << right << res->getInt("total_count") << " |" << endl;
            }
            cout << "+-----------+----------------------+-------------+" << endl;
            delete res;
        }
        else {
            cout << "\nNo prescription records found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// YEARLY REPORT (REVENUE REMOVED)
// ---------------------------------------------------------
void Reports::generateYearlyPrescriptionReport() {
    system("cls");
    displayTableHeader("YEARLY PRESCRIPTION REPORT");

    try {
        // MODIFIED: Removed revenue calculations
        string query = "SELECT DATE_FORMAT(pr.date, '%Y') as year_str, "
            "ph.category_of_meds, "
            "COUNT(pr.formatted_id) as total_count "
            "FROM prescription pr "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "GROUP BY year_str, ph.category_of_meds "
            "ORDER BY year_str DESC, total_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+-------+----------------------+-------------+" << endl;
            cout << "| Year  | Medication Category  | Total Count |" << endl;
            cout << "+-------+----------------------+-------------+" << endl;
            while (res->next()) {
                cout << "| " << setw(5) << left << res->getString("year_str")
                    << "| " << setw(20) << res->getString("category_of_meds")
                    << "| " << setw(11) << right << res->getInt("total_count") << " |" << endl;
            }
            cout << "+-------+----------------------+-------------+" << endl;
            delete res;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// TREND CHART
// ---------------------------------------------------------
void displayVerticalBarChart(const vector<pair<string, int>>& data) {
    if (data.empty()) return;
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
        string query = "SELECT DATE_FORMAT(date, '%Y-%m') as month, COUNT(*) as count FROM prescription GROUP BY month ORDER BY month DESC LIMIT 6";
        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            vector<pair<string, int>> data;
            while (res->next()) {
                data.insert(data.begin(), { res->getString("month"), res->getInt("count") });
            }
            delete res;
            if (!data.empty()) {
                displayVerticalBarChart(data);
            }
            else {
                cout << "\nNo data available." << endl;
            }
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
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
