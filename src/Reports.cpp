#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        // CLEANED MENU: Removed separate Trend Chart, integrated into reports below
        vector<string> menuOptions = {
            "Prescription Report (All)",
            "Monthly Prescription Report", // Now includes Graph
            "Yearly Prescription Report",  // Now includes Graph
            "Back to Admin Menu"
        };

        choice = MenuNavigator::showMenu(menuOptions, "PRESCRIPTION REPORTS", true);

        if (choice == -1) return;

        switch (choice) {
        case 0: generatePrescriptionReport(); break;
        case 1: generateMonthlyPrescriptionReport(); break;
        case 2: generateYearlyPrescriptionReport(); break;
        case 3: return;
        }
    } while (true);
}

// ---------------------------------------------------------
// HELPER: Get Month Name
// ---------------------------------------------------------
string Reports::getMonthName(int monthNumber) {
    const string months[] = {
        "Invalid", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    if (monthNumber >= 1 && monthNumber <= 12) {
        return months[monthNumber];
    }
    return "Unknown";
}

// ---------------------------------------------------------
// HELPER: Display Horizontal Bar Chart (Integrated)
// ---------------------------------------------------------
void Reports::displayIntegratedChart(const vector<pair<string, int>>& data, const string& labelHeader) {
    if (data.empty()) return;

    // Find max value for scaling
    int maxVal = 0;
    for (const auto& item : data) {
        if (item.second > maxVal) maxVal = item.second;
    }

    cout << "\n   " << labelHeader << " TREND ANALYSIS" << endl;
    cout << "   " << string(labelHeader.length() + 15, '-') << endl;

    for (const auto& item : data) {
        // Calculate bar length (max 50 chars)
        int barLength = (maxVal > 0) ? (int)((double)item.second / maxVal * 50.0) : 0;

        cout << "   " << left << setw(15) << item.first << " |";
        ColorUtils::setColor(LIGHT_CYAN);
        for (int i = 0; i < barLength; i++) cout << "█";
        ColorUtils::resetColor();
        cout << " " << item.second << endl;
    }
    cout << endl;
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
// MONTHLY REPORT (Updated with Graph & Month Names)
// ---------------------------------------------------------
void Reports::generateMonthlyPrescriptionReport() {
    system("cls");
    displayTableHeader("MONTHLY PRESCRIPTION REPORT");

    try {
        // Part 1: Detailed Table
        string query = "SELECT DATE_FORMAT(pr.date, '%Y-%m') as month_str, "
            "ph.medicine_name, "
            "COUNT(pr.formatted_id) as total_count "
            "FROM prescription pr "
            "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
            "GROUP BY month_str, ph.medicine_name "
            "ORDER BY month_str DESC, total_count DESC";

        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            cout << "\n+---------------------+----------------------+-------------+" << endl;
            cout << "| Month               | Medication Name      | Total Count |" << endl;
            cout << "+---------------------+----------------------+-------------+" << endl;
            while (res->next()) {
                // Parse "2024-01" -> "January 2024"
                string monthStr = res->getString("month_str");
                string year = monthStr.substr(0, 4);
                int monthNum = stoi(monthStr.substr(5, 2));
                string niceDate = getMonthName(monthNum) + " " + year;

                cout << "| " << setw(19) << left << niceDate
                    << "| " << setw(20) << res->getString("medicine_name").substr(0, 19)
                    << "| " << setw(11) << right << res->getInt("total_count") << " |" << endl;
            }
            cout << "+---------------------+----------------------+-------------+" << endl;
            delete res;
        }
        else {
            cout << "\nNo prescription records found!" << endl;
            pressEnterToContinue();
            return;
        }

        // Part 2: Integrated Summary Graph (Grouped ONLY by Month)
        string graphQuery = "SELECT DATE_FORMAT(date, '%Y-%m') as month_str, COUNT(*) as count "
            "FROM prescription GROUP BY month_str ORDER BY month_str DESC LIMIT 12";

        sql::ResultSet* graphRes = db->executeSelect(graphQuery);
        vector<pair<string, int>> graphData;

        if (graphRes) {
            while (graphRes->next()) {
                string monthStr = graphRes->getString("month_str");
                string year = monthStr.substr(0, 4);
                int monthNum = stoi(monthStr.substr(5, 2));
                string label = getMonthName(monthNum) + " " + year; // e.g. "January 2024"

                graphData.push_back({ label, graphRes->getInt("count") });
            }
            delete graphRes;

            // Display the graph below the table
            cout << "\n" << string(66, '=') << endl;
            displayIntegratedChart(graphData, "MONTHLY");
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// YEARLY REPORT (Updated with Graph)
// ---------------------------------------------------------
void Reports::generateYearlyPrescriptionReport() {
    system("cls");
    displayTableHeader("YEARLY PRESCRIPTION REPORT");

    try {
        // Part 1: Detailed Table
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

        // Part 2: Integrated Summary Graph (Grouped ONLY by Year)
        string graphQuery = "SELECT DATE_FORMAT(date, '%Y') as year_str, COUNT(*) as count "
            "FROM prescription GROUP BY year_str ORDER BY year_str DESC LIMIT 5";

        sql::ResultSet* graphRes = db->executeSelect(graphQuery);
        vector<pair<string, int>> graphData;

        if (graphRes) {
            while (graphRes->next()) {
                graphData.push_back({ graphRes->getString("year_str"), graphRes->getInt("count") });
            }
            delete graphRes;

            // Display the graph below the table
            cout << "\n" << string(46, '=') << endl;
            displayIntegratedChart(graphData, "YEARLY");
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
