#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include <iomanip>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "Monthly Prescription Report",
            "Yearly Prescription Report",
            "Back to Admin Menu"
        };

        choice = MenuNavigator::showMenu(menuOptions, "PRESCRIPTION REPORTS", true);

        if (choice == -1) return;

        switch (choice) {
        case 0: generateMonthlyReport(); break;
        case 1: generateYearlyReport(); break;
        case 2: return;
        }
    } while (true);
}

void Reports::generateMonthlyReport() {
    system("cls");
    displayTableHeader("MONTHLY PRESCRIPTION REPORT");

    // 1. DATA QUERY
    string query = "SELECT "
        "DATE_FORMAT(mr.date_of_record, '%Y') as year_str, "
        "DATE_FORMAT(mr.date_of_record, '%M') as month_str, "
        "DATE_FORMAT(mr.date_of_record, '%m') as month_num, "
        "ph.medicine_name, "
        "COUNT(*) as usage_count "
        "FROM medical_record mr "
        "JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
        "JOIN prescription pr ON d.prescription_id = pr.formatted_id "
        "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
        "GROUP BY year_str, month_str, month_num, ph.medicine_name "
        "ORDER BY year_str DESC, month_num ASC, ph.medicine_name ASC";

    sql::ResultSet* res = db->executeSelect(query);

    // Data structures for the Graph
    map<string, int> graphData;
    vector<string> graphOrder;

    if (res && res->rowsCount() > 0) {
        // --- DISPLAY TABLE ---
        string prevYear = "";
        string prevMonth = "";

        // Column Width Configuration
        // Month: 15 chars | Med Name: 30 chars | Count: 12 chars
        // Dashes: 15+2=17 | 30+2=32 | 12+2=14
        const string TABLE_BORDER = "+-----------------+--------------------------------+--------------+";

        while (res->next()) {
            string currentYear = res->getString("year_str");
            string currentMonth = res->getString("month_str");
            string medName = res->getString("medicine_name");
            int count = res->getInt("usage_count");

            // --- GRAPH AGGREGATION LOGIC ---
            string shortMonth = currentMonth.substr(0, 3);
            string graphKey = shortMonth + " " + currentYear;

            if (find(graphOrder.begin(), graphOrder.end(), graphKey) == graphOrder.end()) {
                graphOrder.push_back(graphKey);
            }
            graphData[graphKey] += count;

            // --- YEAR HEADER LOGIC ---
            if (currentYear != prevYear) {
                if (!prevYear.empty()) {
                    ColorUtils::setColor(LIGHT_CYAN);
                    cout << TABLE_BORDER << endl;
                    ColorUtils::resetColor();
                    cout << endl;
                }

                ColorUtils::setColor(LIGHT_CYAN);
                cout << "  YEAR: " << currentYear << endl;
                cout << TABLE_BORDER << endl;
                cout << "| " << left << setw(15) << "Month"
                    << " | " << left << setw(30) << "Medication Name"
                    << " | " << right << setw(12) << "Total Count" << " |" << endl;
                cout << TABLE_BORDER << endl;
                ColorUtils::resetColor();

                prevYear = currentYear;
                prevMonth = "";
            }

            // --- MONTH ROW LOGIC ---
            string displayMonth;
            if (currentMonth != prevMonth) {
                if (!prevMonth.empty()) {
                    // Optional: Separator between months, or just keep it clean
                    // cout << "|                 +--------------------------------+--------------+" << endl;
                }
                displayMonth = currentMonth;
            }
            else {
                displayMonth = "";
            }

            // STRICT TRUNCATION to prevent table breaking
            if (displayMonth.length() > 15) displayMonth = displayMonth.substr(0, 15);
            if (medName.length() > 30) medName = medName.substr(0, 27) + "...";

            cout << "| " << left << setw(15) << displayMonth
                << " | " << left << setw(30) << medName
                << " | " << right << setw(12) << count << " |" << endl;

            prevMonth = currentMonth;
        }

        ColorUtils::setColor(LIGHT_CYAN);
        cout << TABLE_BORDER << endl;
        ColorUtils::resetColor();

        delete res;

        // --- DISPLAY GRAPH ---
        cout << "\n" << endl;
        cout << "  TOTAL PRESCRIPTIONS PER MONTH (GRAPH)" << endl;
        cout << "  -------------------------------------" << endl;

        int maxVal = 0;
        for (const auto& key : graphOrder) {
            if (graphData[key] > maxVal) maxVal = graphData[key];
        }

        for (const auto& key : graphOrder) {
            int val = graphData[key];
            int barLength = (maxVal > 0) ? (val * 40 / maxVal) : 0;

            cout << "  " << left << setw(15) << key << " | ";
            ColorUtils::setColor(LIGHT_CYAN);
            for (int b = 0; b < barLength; b++) cout << "█";
            if (barLength == 0 && val > 0) cout << "▏";
            ColorUtils::resetColor();
            cout << " " << val << endl;
        }
        cout << endl;

    }
    else {
        cout << "\n[INFO] No prescription records found." << endl;
        if (res) delete res;
    }

    pressEnterToContinue();
}

void Reports::generateYearlyReport() {
    system("cls");
    displayTableHeader("YEARLY PRESCRIPTION REPORT");

    string query = "SELECT "
        "DATE_FORMAT(mr.date_of_record, '%Y') as year_str, "
        "ph.medicine_name, "
        "COUNT(*) as total_count "
        "FROM medical_record mr "
        "JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
        "JOIN prescription pr ON d.prescription_id = pr.formatted_id "
        "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
        "GROUP BY year_str, ph.medicine_name "
        "ORDER BY year_str DESC, total_count DESC";

    sql::ResultSet* res = db->executeSelect(query);

    map<string, int> graphData;
    vector<string> graphOrder;

    if (res && res->rowsCount() > 0) {

        // Column Width Config
        // Year: 10 chars | Med: 30 chars | Count: 12 chars
        // Dashes: 10+2=12 | 30+2=32 | 12+2=14
        const string TABLE_BORDER = "+------------+--------------------------------+--------------+";

        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n" << TABLE_BORDER << endl;
        cout << "| " << left << setw(10) << "Year"
            << " | " << left << setw(30) << "Medication Name"
            << " | " << right << setw(12) << "Total Count" << " |" << endl;
        cout << TABLE_BORDER << endl;
        ColorUtils::resetColor();

        string prevYear = "";

        while (res->next()) {
            string currentYear = res->getString("year_str");
            string medName = res->getString("medicine_name");
            int count = res->getInt("total_count");

            // --- GRAPH AGGREGATION LOGIC ---
            string graphKey = currentYear;
            if (find(graphOrder.begin(), graphOrder.end(), graphKey) == graphOrder.end()) {
                graphOrder.push_back(graphKey);
            }
            graphData[graphKey] += count;

            // --- YEAR ROW LOGIC ---
            string displayYear;
            if (currentYear != prevYear) {
                if (!prevYear.empty()) {
                    // cout << TABLE_BORDER << endl; // Optional separator
                }
                displayYear = currentYear;
            }
            else {
                displayYear = "";
            }

            // STRICT TRUNCATION
            if (medName.length() > 30) medName = medName.substr(0, 27) + "...";

            cout << "| " << left << setw(10) << displayYear
                << " | " << left << setw(30) << medName
                << " | " << right << setw(12) << count << " |" << endl;

            prevYear = currentYear;
        }

        ColorUtils::setColor(LIGHT_CYAN);
        cout << TABLE_BORDER << endl;
        ColorUtils::resetColor();
        delete res;

        // --- DISPLAY GRAPH ---
        cout << "\n" << endl;
        cout << "  TOTAL PRESCRIPTIONS PER YEAR (GRAPH)" << endl;
        cout << "  ------------------------------------" << endl;

        int maxVal = 0;
        for (const auto& key : graphOrder) {
            if (graphData[key] > maxVal) maxVal = graphData[key];
        }

        for (const auto& key : graphOrder) {
            int val = graphData[key];
            int barLength = (maxVal > 0) ? (val * 40 / maxVal) : 0;

            cout << "  " << left << setw(15) << key << " | ";
            ColorUtils::setColor(LIGHT_GREEN);
            for (int b = 0; b < barLength; b++) cout << "█";
            if (barLength == 0 && val > 0) cout << "▏";
            ColorUtils::resetColor();
            cout << " " << val << endl;
        }
        cout << endl;

    }
    else {
        cout << "\n[INFO] No records found." << endl;
        if (res) delete res;
    }

    pressEnterToContinue();
}

void Reports::displayTableHeader(const string& title) {
    ColorUtils::setColor(LIGHT_BLUE);
    cout << "+==============================================================================+" << endl;

    int paddingLeft = (78 - title.length()) / 2;
    int paddingRight = 78 - paddingLeft - title.length();

    cout << "|" << string(paddingLeft, ' ') << title << string(paddingRight, ' ') << "|" << endl;
    cout << "+==============================================================================+" << endl;
    ColorUtils::resetColor();
}

void Reports::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}
