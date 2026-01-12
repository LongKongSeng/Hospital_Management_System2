#include "AdminModule.h"
#include "Reports.h" 
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <cctype>

AdminModule::AdminModule(Database* database) : db(database) {}

void AdminModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Pharmacy",
            "Prescription Reports", // Renamed to reflect content
            "Add Patient",
            "Patient Receipt",
            "Logout"
        };

        choice = MenuNavigator::showMenu(menuOptions, "ADMIN DASHBOARD", true);

        if (choice == -1) continue;

        switch (choice) {
        case 0: viewPharmacy(); break;
        case 1:
        {
            // Correctly calls the new Reports module
            Reports reports(db);
            reports.showMenu();
        }
        break;
        case 2: addPatient(); break;
        case 3: patientReceipt(); break;
        case 4: return;
        }
    } while (true);
}

void AdminModule::viewPharmacy() {
    int choice;
    do {
        vector<string> options = {
            "Display All Medications",
            "Filter by Category",
            "Back"
        };

        choice = MenuNavigator::showMenu(options, "VIEW PHARMACY", true);

        if (choice == -1) return;

        switch (choice) {
        case 0: displayPharmacyList(); break;
        case 1: filterPharmacyByCategory(); break;
        case 2: return;
        }
    } while (choice != 2);
}

void AdminModule::displayPharmacyList() {
    system("cls");
    displayTableHeader("PHARMACY - ALL MEDICATIONS");

    try {
        string query = "SELECT formatted_id, medicine_name, category_of_meds, unit_price "
            "FROM pharmacy ORDER BY category_of_meds, medicine_name";
        sql::ResultSet* res = db->executeSelect(query);

        if (res) {
            displayPharmacyTable(res);
            delete res;
        }
        else {
            cout << "\n[ERROR] Failed to retrieve pharmacy data!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void AdminModule::filterPharmacyByCategory() {
    int choice;
    do {
        vector<string> options = {
            "Search Specific Category",
            "Back"
        };

        choice = MenuNavigator::showMenu(options, "FILTER PHARMACY", true);

        if (choice == -1) return;

        switch (choice) {
        case 0: {
            string category = getStringInput("\nEnter Category name to search: ");
            if (category.empty()) {
                cout << "\n[ERROR] Category name cannot be empty!" << endl;
                pressEnterToContinue();
                break;
            }

            string query = "SELECT formatted_id, medicine_name, category_of_meds, unit_price "
                "FROM pharmacy WHERE category_of_meds LIKE '%" + category + "%' "
                "ORDER BY medicine_name";

            sql::ResultSet* res = db->executeSelect(query);
            if (res) {
                cout << "\nResults for category: " << category << endl;
                if (res->rowsCount() == 0) {
                    cout << "⚠️  No medications found in this category!" << endl;
                }
                else {
                    displayPharmacyTable(res);
                }
                delete res;
            }
            pressEnterToContinue();
            break;
        }
        case 1: return;
        }
    } while (choice != 1);
}

void AdminModule::addPatient() {
    system("cls");
    displayTableHeader("ADD PATIENT");

    string fullName = getStringInput("Enter Patient Name: ");
    if (fullName.empty()) return;

    // Add your validation logic back here if needed
    string gender = getStringInput("Enter Gender (Male/Female): ");
    string icNumber = getStringInput("Enter IC Number: ");
    string dob = getStringInput("Enter DOB (YYYY-MM-DD): ");
    string contact = getStringInput("Enter Contact: ");
    string blood = getStringInput("Enter Blood Type: ");
    string emergency = getStringInput("Enter Emergency Contact: ");

    string query = "INSERT INTO patient (formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, ic_number, status) "
        "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + dob + "', '" + contact + "', '" + blood + "', '" + emergency + "', '" + icNumber + "', 'Active')";

    if (db->executeUpdate(query)) {
        cout << "\n✅ Patient added successfully!" << endl;
    }
    else {
        cout << "\n[ERROR] Failed to add patient." << endl;
    }
    pressEnterToContinue();
}

void AdminModule::patientReceipt() {
    system("cls");
    displayTableHeader("PATIENT RECEIPT");
    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) return;
    calculatePatientReceipt(patientId);
}

void AdminModule::calculatePatientReceipt(const string& patientId) {
    string query = "SELECT full_name FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* res = db->executeSelect(query);
    if (!res || !res->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (res) delete res;
        pressEnterToContinue();
        return;
    }
    delete res;

    string countQuery = "SELECT COUNT(*) as visits FROM medical_record WHERE patient_id = '" + patientId + "'";
    sql::ResultSet* countRes = db->executeSelect(countQuery);
    double total = 0;
    if (countRes && countRes->next()) {
        total = countRes->getInt("visits") * 50.00;
    }
    if (countRes) delete countRes;

    displayReceipt(patientId, total);
    pressEnterToContinue();
}

void AdminModule::displayReceipt(const string& patientId, double totalAmount) {
    system("cls");
    displayTableHeader("RECEIPT");
    cout << "Patient ID: " << patientId << endl;
    cout << "Total Outstanding: RM " << fixed << setprecision(2) << totalAmount << endl;
    cout << "--------------------------------" << endl;
}

void AdminModule::displayPharmacyTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+----------------------+-------------+" << endl;
    cout << "| " << left << setw(11) << "Pharmacy ID"
        << "| " << left << setw(20) << "Medicine Name"
        << "| " << left << setw(20) << "Category"
        << "| " << right << setw(11) << "Unit Price" << " |" << endl;
    cout << "+-------------+----------------------+----------------------+-------------+" << endl;
    while (res->next()) {
        cout << "| " << left << setw(11) << res->getString("formatted_id")
            << "| " << left << setw(20) << res->getString("medicine_name").substr(0, 19)
            << "| " << left << setw(20) << res->getString("category_of_meds").substr(0, 19)
            << "| " << right << setw(9) << fixed << setprecision(2) << res->getDouble("unit_price") << " |" << endl;
    }
    cout << "+-------------+----------------------+----------------------+-------------+" << endl;
}

void AdminModule::displayTableHeader(const string& title) {
    ColorUtils::setColor(LIGHT_BLUE);
    cout << string(80, '=') << endl;
    ColorUtils::resetColor();
    MenuNavigator::displayTitle(title, 80);
    ColorUtils::setColor(LIGHT_BLUE);
    cout << string(80, '=') << endl;
    ColorUtils::resetColor();
}

void AdminModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int AdminModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try { return stoi(input); }
    catch (...) { return -1; }
}

string AdminModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

// Helpers
bool AdminModule::validatePhoneNumber(const string& s) { return s.length() >= 10; }
bool AdminModule::validateDateFormat(const string& s) { return s.length() == 10; }
bool AdminModule::validateBloodType(const string& s) { return !s.empty(); }
bool AdminModule::validateICNumber(const string& s) { return s.length() == 12; }
string AdminModule::validateAndCorrectGender(string& s) { return s; }
