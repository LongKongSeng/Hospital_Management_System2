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

// Helper to draw a graph for a specific dataset
void drawGraph(const map<string, int>& data, const vector<string>& order, const string& title, int colorCode) {
    cout << "\n  " << title << endl;
    cout << "  -------------------------------------" << endl;

    int maxVal = 0;
    for (const auto& key : order) {
        if (data.at(key) > maxVal) maxVal = data.at(key);
    }

    for (const auto& key : order) {
        int val = data.at(key);
        int barLength = (maxVal > 0) ? (val * 40 / maxVal) : 0;

        cout << "  " << left << setw(15) << key << " | ";
        ColorUtils::setColor(colorCode);
        for (int b = 0; b < barLength; b++) cout << "█";
        if (barLength == 0 && val > 0) cout << "▏";
        ColorUtils::resetColor();
        cout << " " << val << endl;
    }
    cout << endl;
}

void Reports::generateMonthlyReport() {
    system("cls");
    displayTableHeader("MONTHLY PRESCRIPTION REPORT");

    string query = "SELECT "
        "DATE_FORMAT(mr.date_of_record, '%Y') as year_str, "
        "DATE_FORMAT(mr.date_of_record, '%M') as month_str, "
        "DATE_FORMAT(mr.date_of_record, '%m') as month_num, "
        "ph.medicine_name, "
        "COUNT(*) as usage_count "
        "FROM medical_record mr "
        "JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
        "JOIN prescription pr ON pr.diagnosis_id = d.formatted_id "
        "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
        "GROUP BY year_str, month_str, month_num, ph.medicine_name "
        "ORDER BY year_str DESC, month_num ASC, ph.medicine_name ASC";

    sql::ResultSet* res = db->executeSelect(query);

    if (res && res->rowsCount() > 0) {

        string prevYear = "";
        const string TABLE_BORDER = "+-----------------+--------------------------------+--------------+";

        // Temporary storage for the current year's graph data
        map<string, int> currentYearGraphData;
        vector<string> currentYearGraphOrder;

        while (res->next()) {
            string currentYear = res->getString("year_str");
            string currentMonth = res->getString("month_str");
            string medName = res->getString("medicine_name");
            int count = res->getInt("usage_count");

            // --- YEAR CHANGE LOGIC ---
            if (currentYear != prevYear) {
                // If we finished a previous year, print its graph NOW
                if (!prevYear.empty()) {
                    ColorUtils::setColor(LIGHT_CYAN);
                    cout << TABLE_BORDER << endl;
                    ColorUtils::resetColor();

                    // Draw Graph for the PREVIOUS year immediately
                    drawGraph(currentYearGraphData, currentYearGraphOrder,
                        "TOTAL PRESCRIPTIONS FOR " + prevYear, LIGHT_CYAN);

                    // Clear graph data for the new year
                    currentYearGraphData.clear();
                    currentYearGraphOrder.clear();
                }

                // Print Header for NEW Year
                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\n YEAR: " << currentYear << endl;
                cout << TABLE_BORDER << endl;
                cout << "| " << left << setw(15) << "Month"
                    << " | " << left << setw(30) << "Medication Name"
                    << " | " << right << setw(12) << "Total Count" << " |" << endl;
                cout << TABLE_BORDER << endl;
                ColorUtils::resetColor();

                prevYear = currentYear;
            }

            // --- GRAPH DATA ACCUMULATION ---
            string shortMonth = currentMonth.substr(0, 3);
            // Key is just the month name since the graph is specific to this year
            if (find(currentYearGraphOrder.begin(), currentYearGraphOrder.end(), shortMonth) == currentYearGraphOrder.end()) {
                currentYearGraphOrder.push_back(shortMonth);
            }
            currentYearGraphData[shortMonth] += count;

            // --- PRINT ROW ---
            if (medName.length() > 30) medName = medName.substr(0, 27) + "...";
            if (currentMonth.length() > 15) currentMonth = currentMonth.substr(0, 15);

            cout << "| " << left << setw(15) << currentMonth
                << " | " << left << setw(30) << medName
                << " | " << right << setw(12) << count << " |" << endl;
        }

        // Print final footer and graph for the very last year loop
        ColorUtils::setColor(LIGHT_CYAN);
        cout << TABLE_BORDER << endl;
        ColorUtils::resetColor();

        drawGraph(currentYearGraphData, currentYearGraphOrder,
            "TOTAL PRESCRIPTIONS FOR " + prevYear, LIGHT_CYAN);

        delete res;
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
        "JOIN prescription pr ON pr.diagnosis_id = d.formatted_id "
        "JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
        "GROUP BY year_str, ph.medicine_name "
        "ORDER BY year_str DESC, total_count DESC";

    sql::ResultSet* res = db->executeSelect(query);

    map<string, int> graphData;
    vector<string> graphOrder;

    if (res && res->rowsCount() > 0) {

        const string TABLE_BORDER = "+------------+--------------------------------+--------------+";

        ColorUtils::setColor(LIGHT_GREEN);
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

            // Graph Logic
            if (find(graphOrder.begin(), graphOrder.end(), currentYear) == graphOrder.end()) {
                graphOrder.push_back(currentYear);
            }
            graphData[currentYear] += count;

            string displayYear = (currentYear != prevYear) ? currentYear : "";

            if (medName.length() > 30) medName = medName.substr(0, 27) + "...";

            cout << "| " << left << setw(10) << displayYear
                << " | " << left << setw(30) << medName
                << " | " << right << setw(12) << count << " |" << endl;

            prevYear = currentYear;
        }

        ColorUtils::setColor(LIGHT_GREEN);
        cout << TABLE_BORDER << endl;
        ColorUtils::resetColor();
        delete res;

        // Display Graph
        drawGraph(graphData, graphOrder, "TOTAL PRESCRIPTIONS PER YEAR", LIGHT_GREEN);

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
