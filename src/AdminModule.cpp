#include "AdminModule.h"
#include "Reports.h" 
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <cctype>
#include <string>
#include <set>
#include <algorithm>

AdminModule::AdminModule(Database* database) : db(database) {}

void AdminModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Pharmacy",
            "Prescription Reports",
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

    string gender = getStringInput("Enter Gender (Male/Female): ");

    string dob;
    while (true) {
        dob = getStringInput("Enter DOB (YYYY-MM-DD): ");
        if (dob.empty()) {
            cout << "\n[ERROR] Date of Birth cannot be empty!" << endl;
            pressEnterToContinue();
            system("cls");
            displayTableHeader("ADD PATIENT");
            cout << "Enter Patient Name: " << fullName << endl;
            cout << "Enter Gender (Male/Female): " << gender << endl;
        }
        else if (validateDateFormat(dob)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD (e.g., 1990-05-20)." << endl;
            pressEnterToContinue();
            system("cls");
            displayTableHeader("ADD PATIENT");
            cout << "Enter Patient Name: " << fullName << endl;
            cout << "Enter Gender (Male/Female): " << gender << endl;
        }
    }

    string icNumber = getStringInput("Enter IC Number: ");
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

// ---------------------------------------------------------
// UPDATED RECEIPT LOGIC: Unified Date-Based Table
// ---------------------------------------------------------
void AdminModule::calculatePatientReceipt(const string& patientId) {
    // 1. Fetch Patient Details
    string patientQuery = "SELECT full_name, ic_number, contact_number FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* patientRes = db->executeSelect(patientQuery);

    if (!patientRes || !patientRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (patientRes) delete patientRes;
        pressEnterToContinue();
        return;
    }

    string patientName = patientRes->getString("full_name");
    string icNumber = patientRes->getString("ic_number");
    string contact = patientRes->getString("contact_number");
    delete patientRes;

    system("cls");
    displayTableHeader("OFFICIAL RECEIPT");

    cout << "\nPATIENT DETAILS:" << endl;
    cout << "----------------" << endl;
    cout << "Name:    " << patientName << endl;
    cout << "ID:      " << patientId << endl;
    cout << "IC No:   " << icNumber << endl;
    cout << "Contact: " << contact << endl;
    cout << endl;

    // Header with aligned columns
    cout << "+------------+----------------------+----------------------+------------+------------+------------+------------+" << endl;
    cout << "| Date       | Description          | Medication           | Consult.   | Treatment  | Med. Cost  | Total      |" << endl;
    cout << "+------------+----------------------+----------------------+------------+------------+------------+------------+" << endl;

    double grandTotal = 0.0;

    // 2. Get all distinct dates involved (Treatment OR Medical Record)
    string dateQuery = "SELECT DISTINCT d.date_val FROM ("
        "SELECT treatment_date as date_val FROM treatment WHERE patient_id = '" + patientId + "' "
        "UNION "
        "SELECT date_of_record as date_val FROM medical_record WHERE patient_id = '" + patientId + "') AS d "
        "ORDER BY d.date_val DESC";

    sql::ResultSet* dateRes = db->executeSelect(dateQuery);

    if (dateRes) {
        while (dateRes->next()) {
            string date = dateRes->getString("date_val");
            double dailyConsult = 0.0;
            double dailyTreatment = 0.0;
            double dailyMeds = 0.0;
            string description = "";
            string medications = "";

            // A. Fetch Treatment Data for this date
            string treatQuery = "SELECT dressing_applied, consultation_fee, treatment_fee FROM treatment "
                "WHERE patient_id = '" + patientId + "' AND treatment_date = '" + date + "'";
            sql::ResultSet* treatRes = db->executeSelect(treatQuery);
            if (treatRes) {
                while (treatRes->next()) {
                    dailyConsult += treatRes->getDouble("consultation_fee");
                    dailyTreatment += treatRes->getDouble("treatment_fee");
                    string dressing = treatRes->isNull("dressing_applied") ? "" : treatRes->getString("dressing_applied");
                    if (!dressing.empty() && dressing != "None") {
                        if (!description.empty()) description += ", ";
                        description += dressing;
                    }
                }
                delete treatRes;
            }

            // B. Fetch Medical Record (Diagnosis & Prescription) for this date
            string medQuery = "SELECT d.disease, ph.medicine_name, ph.unit_price "
                "FROM medical_record mr "
                "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
                "LEFT JOIN prescription pr ON d.prescription_id = pr.formatted_id "
                "LEFT JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
                "WHERE mr.patient_id = '" + patientId + "' AND mr.date_of_record = '" + date + "'";

            sql::ResultSet* medRes = db->executeSelect(medQuery);
            if (medRes) {
                while (medRes->next()) {
                    string disease = medRes->isNull("disease") ? "" : medRes->getString("disease");
                    string medName = medRes->isNull("medicine_name") ? "" : medRes->getString("medicine_name");
                    double price = medRes->isNull("unit_price") ? 0.0 : medRes->getDouble("unit_price");

                    dailyMeds += price;

                    if (!disease.empty() && description.find(disease) == string::npos) {
                        if (!description.empty()) description += "/";
                        description += disease;
                    }
                    if (!medName.empty()) {
                        if (!medications.empty()) medications += ", ";
                        medications += medName;
                    }
                }
                delete medRes;
            }

            // Defaults if empty
            if (description.empty()) description = "Checkup";
            if (medications.empty()) medications = "None";

            // Formatting Truncation
            if (description.length() > 20) description = description.substr(0, 17) + "...";
            if (medications.length() > 20) medications = medications.substr(0, 17) + "...";

            double dailyTotal = dailyConsult + dailyTreatment + dailyMeds;
            grandTotal += dailyTotal;

            cout << "| " << left << setw(11) << date
                << "| " << left << setw(21) << description
                << "| " << left << setw(21) << medications
                << "| " << right << setw(10) << fixed << setprecision(2) << dailyConsult
                << " | " << right << setw(10) << fixed << setprecision(2) << dailyTreatment
                << " | " << right << setw(10) << fixed << setprecision(2) << dailyMeds
                << " | " << right << setw(10) << fixed << setprecision(2) << dailyTotal << " |" << endl;
        }
        delete dateRes;
    }
    else {
        cout << "| " << left << setw(106) << "No records found." << " |" << endl;
    }

    cout << "+------------+----------------------+----------------------+------------+------------+------------+------------+" << endl;

    // Grand Total
    cout << "|                                                                                         TOTAL | "
        << right << setw(10) << fixed << setprecision(2) << grandTotal << " |" << endl;
    cout << "+------------------------------------------------------------------------------------------------------+" << endl;

    pressEnterToContinue();
}

void AdminModule::displayReceipt(const string& patientId, double totalAmount) {
    // Deprecated
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

bool AdminModule::validateDateFormat(const string& s) {
    if (s.length() != 10) return false;
    if (s[4] != '-' || s[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(s[i])) return false;
    }
    int m = stoi(s.substr(5, 2));
    int d = stoi(s.substr(8, 2));
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;
    return true;
}

bool AdminModule::validateBloodType(const string& s) { return !s.empty(); }
bool AdminModule::validateICNumber(const string& s) { return s.length() == 12; }
string AdminModule::validateAndCorrectGender(string& s) { return s; }
