#include "AdminModule.h"
#include "Reports.h" 
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <cctype>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <iomanip>

AdminModule::AdminModule(Database* database) : db(database) {}

void AdminModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Pharmacy",
            "Prescription Reports",
            "Add Patient",
            "Process Payment",
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
        case 3: processPayment(); break;
        case 4: patientReceipt(); break;
        case 5: return;
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

// ---------------------------------------------------------
// ADD PATIENT
// ---------------------------------------------------------
void AdminModule::addPatient() {
    system("cls");
    displayTableHeader("ADD PATIENT");

    auto redrawForm = [&](const string& name, const string& gender, const string& dob, const string& ic, const string& contact, const string& blood, const string& emergency) {
        system("cls");
        displayTableHeader("ADD PATIENT");
        if (!name.empty()) cout << "Enter Patient Name: " << name << endl;
        if (!gender.empty()) cout << "Enter Gender (Male/Female): " << gender << endl;
        if (!dob.empty()) cout << "Enter DOB (YYYY-MM-DD): " << dob << endl;
        if (!ic.empty()) cout << "Enter IC Number: " << ic << endl;
        if (!contact.empty()) cout << "Enter Contact: " << contact << endl;
        if (!blood.empty()) cout << "Enter Blood Type: " << blood << endl;
        if (!emergency.empty()) cout << "Enter Emergency Contact: " << emergency << endl;
        };

    string fullName = getStringInput("Enter Patient Name: ");
    if (fullName.empty()) return;

    string gender = getStringInput("Enter Gender (Male/Female): ");

    string dob;
    while (true) {
        dob = getStringInput("Enter DOB (YYYY-MM-DD): ");
        if (dob.empty()) {
            cout << "\n[ERROR] Date of Birth cannot be empty!" << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, "", "", "", "", "");
        }
        else if (validateDateFormat(dob)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD (e.g., 1990-05-20)." << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, "", "", "", "", "");
        }
    }

    string icNumber;
    while (true) {
        icNumber = getStringInput("Enter IC Number (12 digits): ");
        if (validateICNumber(icNumber)) {
            break;
        }
        else {
            cout << "\n[ERROR] IC Number must be exactly 12 digits." << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, dob, "", "", "", "");
        }
    }

    string contact;
    while (true) {
        contact = getStringInput("Enter Contact (10-11 digits): ");
        if (validatePhoneNumber(contact)) {
            break;
        }
        else {
            cout << "\n[ERROR] Contact number must be 10-11 digits." << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, dob, icNumber, "", "", "");
        }
    }

    string blood;
    while (true) {
        blood = getStringInput("Enter Blood Type (A/B/AB/O +/-): ");
        if (validateBloodType(blood)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid Blood Type! Use A+, A-, B+, B-, AB+, AB-, O+, O-." << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, dob, icNumber, contact, "", "");
        }
    }

    string emergency;
    while (true) {
        emergency = getStringInput("Enter Emergency Contact (10-11 digits): ");
        if (validatePhoneNumber(emergency)) {
            break;
        }
        else {
            cout << "\n[ERROR] Emergency contact must be 10-11 digits." << endl;
            pressEnterToContinue();
            redrawForm(fullName, gender, dob, icNumber, contact, blood, "");
        }
    }

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

// ---------------------------------------------------------
// SEARCH PATIENT HELPER
// ---------------------------------------------------------
void AdminModule::patientReceipt() {
    system("cls");
    displayTableHeader("SEARCH PATIENT RECEIPT");

    string input = getStringInput("Enter Patient Name, IC Number, or ID: ");
    if (input.empty()) return;

    string query = "SELECT formatted_id, full_name, ic_number, status FROM patient WHERE "
        "full_name LIKE '%" + input + "%' OR "
        "ic_number LIKE '%" + input + "%' OR "
        "formatted_id = '" + input + "'";

    sql::ResultSet* res = db->executeSelect(query);

    if (!res || res->rowsCount() == 0) {
        cout << "\n[INFO] No patients found matching \"" << input << "\"." << endl;
        if (res) delete res;
        pressEnterToContinue();
        return;
    }

    string selectedId = "";

    if (res->rowsCount() == 1) {
        res->next();
        selectedId = res->getString("formatted_id");
    }
    else {
        cout << "\n[INFO] Multiple patients found. Please select one:\n" << endl;
        struct PatientOption { string id; string name; string ic; };
        vector<PatientOption> options;

        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+-----+-------------+-----------------------------+------------------+" << endl;
        cout << "| No. | ID          | Name                        | IC Number        |" << endl;
        cout << "+-----+-------------+-----------------------------+------------------+" << endl;
        ColorUtils::resetColor();

        int index = 1;
        while (res->next()) {
            string pId = res->getString("formatted_id");
            string pName = res->getString("full_name");
            string pIc = res->getString("ic_number");
            options.push_back({ pId, pName, pIc });

            string displayName = (pName.length() > 27) ? pName.substr(0, 24) + "..." : pName;

            cout << "| " << left << setw(4) << index
                << "| " << left << setw(12) << pId
                << "| " << left << setw(28) << displayName
                << "| " << left << setw(17) << pIc << "|" << endl;
            index++;
        }
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+-----+-------------+-----------------------------+------------------+" << endl;
        ColorUtils::resetColor();

        int selection = getIntInput("\nEnter the number of the patient: ");

        if (selection > 0 && selection <= (int)options.size()) {
            selectedId = options[selection - 1].id;
        }
        else {
            cout << "\n[ERROR] Invalid selection." << endl;
            delete res;
            pressEnterToContinue();
            return;
        }
    }
    delete res;

    if (!selectedId.empty()) {
        calculatePatientReceipt(selectedId);
    }
}

// ---------------------------------------------------------
// RECEIPT GENERATOR (FIXED ALIGNMENT)
// ---------------------------------------------------------
void AdminModule::calculatePatientReceipt(const string& patientId) {
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

    // FIX: Last column border changed from 12 to 13 dashes to fit 13 chars of content
    const string TABLE_BORDER = "+------------+----------------------+----------------------+-------------+-------------+-------------+-------------+-------------+";

    ColorUtils::setColor(LIGHT_CYAN);
    cout << TABLE_BORDER << endl;
    cout << "| " << left << setw(10) << "Date"
        << "| " << left << setw(20) << "Description"
        << "| " << left << setw(20) << "Medication"
        << "| " << left << setw(11) << "Consult."
        << "| " << left << setw(11) << "Treatment"
        << "| " << left << setw(11) << "Med. Cost"
        << "| " << left << setw(11) << "Total"
        << "| " << left << setw(11) << "Status" << " |" << endl; // Matched border 13 (2 + 11 = 13)
    cout << TABLE_BORDER << endl;
    ColorUtils::resetColor();

    double grandTotal = 0.0;

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
            string paymentStatus = "Pending";

            string treatQuery = "SELECT t.formatted_id, t.dressing_applied, t.consultation_fee, t.treatment_fee, f.payment_status "
                "FROM treatment t "
                "LEFT JOIN finance f ON t.formatted_id = f.treatment_id "
                "WHERE t.patient_id = '" + patientId + "' AND t.treatment_date = '" + date + "'";

            sql::ResultSet* treatRes = db->executeSelect(treatQuery);
            if (treatRes) {
                while (treatRes->next()) {
                    dailyConsult += treatRes->getDouble("consultation_fee");
                    dailyTreatment += treatRes->getDouble("treatment_fee");
                    string dressing = treatRes->isNull("dressing_applied") ? "" : treatRes->getString("dressing_applied");

                    if (!treatRes->isNull("payment_status")) {
                        string status = treatRes->getString("payment_status");
                        if (status == "Paid") paymentStatus = "Paid";
                    }

                    if (!dressing.empty() && dressing != "None") {
                        if (!description.empty()) description += ", ";
                        description += dressing;
                    }
                }
                delete treatRes;
            }

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

            if (description.empty()) description = "Checkup";
            if (medications.empty()) medications = "None";

            if (description.length() > 20) description = description.substr(0, 17) + "...";
            if (medications.length() > 20) medications = medications.substr(0, 17) + "...";

            double dailyTotal = dailyConsult + dailyTreatment + dailyMeds;
            grandTotal += dailyTotal;

            string statusDisplay = (paymentStatus == "Paid") ? "PAID" : "PENDING";

            // Row output aligned with headers
            // Last column: | (2) + setw(10) + | (1) = 13. Fits 13-dash border.
            cout << "| " << left << setw(10) << date
                << "| " << left << setw(20) << description
                << "| " << left << setw(20) << medications
                << "| " << right << setw(11) << fixed << setprecision(2) << dailyConsult
                << "| " << right << setw(11) << fixed << setprecision(2) << dailyTreatment
                << "| " << right << setw(11) << fixed << setprecision(2) << dailyMeds
                << "| " << right << setw(11) << fixed << setprecision(2) << dailyTotal
                << "| " << left << setw(10) << statusDisplay << " |" << endl; // | + 10 + space + | = 13
        }
        delete dateRes;
    }
    else {
        cout << "| " << left << setw(126) << "No records found." << "|" << endl;
    }

    ColorUtils::setColor(LIGHT_CYAN);
    cout << TABLE_BORDER << endl;

    // Grand Total Row - Merged first 6 columns
    // Math: 12+1+22+1+22+1+13+1+13+1+13 = 99 + 1(start pipe) = 100.
    // Start | + 98 spaces = 100 chars
    cout << "| " << right << setw(98) << "TOTAL BILL "
        << "| " << right << setw(11) << fixed << setprecision(2) << grandTotal
        << "| " << setw(11) << " " << " |" << endl; // Matches last col width 13

    cout << TABLE_BORDER << endl;
    ColorUtils::resetColor();

    pressEnterToContinue();
}

// ---------------------------------------------------------
// PROCESS PAYMENT FUNCTION (FIXED ALIGNMENT)
// ---------------------------------------------------------
void AdminModule::processPayment() {
    system("cls");
    displayTableHeader("PROCESS PAYMENT");

    string input = getStringInput("Enter Patient Name, IC Number, or ID: ");
    if (input.empty()) return;

    string query = "SELECT formatted_id, full_name FROM patient WHERE "
        "full_name LIKE '%" + input + "%' OR "
        "ic_number LIKE '%" + input + "%' OR "
        "formatted_id = '" + input + "'";

    sql::ResultSet* res = db->executeSelect(query);
    if (!res || res->rowsCount() == 0) {
        cout << "\n[INFO] Patient not found." << endl;
        if (res) delete res;
        pressEnterToContinue();
        return;
    }

    string patientId = "";
    string patientName = "";

    if (res->rowsCount() == 1) {
        res->next();
        patientId = res->getString("formatted_id");
        patientName = res->getString("full_name");
    }
    else {
        res->next();
        patientId = res->getString("formatted_id");
        patientName = res->getString("full_name");
        cout << "\n[INFO] Auto-selected: " << patientName << " (" << patientId << ")" << endl;
    }
    delete res;

    string billQuery = "SELECT t.formatted_id, t.treatment_date, t.consultation_fee, t.treatment_fee, d.full_name as doctor_name, "
        "(SELECT COALESCE(SUM(ph.unit_price), 0) "
        " FROM medical_record mr "
        " JOIN diagnosis diag ON mr.diagnosis_id = diag.formatted_id "
        " JOIN prescription pr ON diag.prescription_id = pr.formatted_id "
        " JOIN pharmacy ph ON pr.pharmacy_id = ph.formatted_id "
        " WHERE mr.patient_id = t.patient_id AND mr.date_of_record = t.treatment_date) as med_cost "
        "FROM treatment t "
        "JOIN doctor d ON t.doctor_id = d.formatted_id "
        "LEFT JOIN finance f ON t.formatted_id = f.treatment_id "
        "WHERE t.patient_id = '" + patientId + "' "
        "AND (f.payment_status IS NULL OR f.payment_status != 'Paid') "
        "ORDER BY t.treatment_date DESC";

    sql::ResultSet* billRes = db->executeSelect(billQuery);

    if (!billRes || billRes->rowsCount() == 0) {
        cout << "\n✅ No pending bills found for " << patientName << "." << endl;
        if (billRes) delete billRes;
        pressEnterToContinue();
        return;
    }

    cout << "\nPENDING BILLS FOR: " << patientName << endl;

    // Widths: No(6) | Date(12) | Doctor(22) | Consult(12) | Treat(12) | Meds(12) | Total(14)
    const string BORDER = "+------+------------+----------------------+------------+------------+------------+--------------+";
    ColorUtils::setColor(LIGHT_CYAN);
    cout << BORDER << endl;
    cout << "| No.  | Date       | Doctor Name          | Consult    | Treatment  | Meds       | Total Amount |" << endl;
    cout << BORDER << endl;
    ColorUtils::resetColor();

    struct PendingBill {
        string tId;
        double totalAmount;
    };
    vector<PendingBill> bills;
    int idx = 1;

    while (billRes->next()) {
        string tId = billRes->getString("formatted_id");
        string date = billRes->getString("treatment_date");
        string doc = billRes->getString("doctor_name");
        double consult = billRes->getDouble("consultation_fee");
        double treat = billRes->getDouble("treatment_fee");
        double meds = billRes->getDouble("med_cost");
        double total = consult + treat + meds;

        bills.push_back({ tId, total });

        if (doc.length() > 20) doc = doc.substr(0, 17) + "...";

        cout << "| " << left << setw(4) << idx
            << " | " << left << setw(10) << date
            << " | " << left << setw(20) << doc
            << " | " << right << setw(10) << fixed << setprecision(2) << consult
            << " | " << right << setw(10) << fixed << setprecision(2) << treat
            << " | " << right << setw(10) << fixed << setprecision(2) << meds
            << " | " << right << setw(12) << fixed << setprecision(2) << total << " |" << endl;
        idx++;
    }
    ColorUtils::setColor(LIGHT_CYAN);
    cout << BORDER << endl;
    ColorUtils::resetColor();
    delete billRes;

    int selection = getIntInput("\nEnter Bill No. to pay (0 to cancel): ");

    if (selection > 0 && selection <= (int)bills.size()) {
        PendingBill selected = bills[selection - 1];

        cout << "\nPaying Amount: RM " << fixed << setprecision(2) << selected.totalAmount << endl;
        string confirm = getStringInput("Confirm Payment? (yes/no): ");

        if (confirm == "yes" || confirm == "YES") {
            string payQuery = "INSERT INTO finance (formatted_id, treatment_id, date_of_payment, payment_status) "
                "VALUES (NULL, '" + selected.tId + "', CURDATE(), 'Paid')";

            if (db->executeUpdate(payQuery)) {
                cout << "\n✅ Payment Successful! Receipt updated." << endl;
            }
            else {
                cout << "\n[ERROR] Database error during payment." << endl;
            }
        }
        else {
            cout << "\n[INFO] Payment cancelled." << endl;
        }
    }

    pressEnterToContinue();
}

void AdminModule::displayReceipt(const string& patientId, double totalAmount) {
    // Deprecated
}

void AdminModule::displayPharmacyTable(sql::ResultSet* res) {
    ColorUtils::setColor(LIGHT_CYAN);
    cout << "\n+-------------+----------------------+----------------------+-------------+" << endl;
    cout << "| " << left << setw(12) << "Pharmacy ID"
        << "| " << left << setw(21) << "Medicine Name"
        << "| " << left << setw(21) << "Category"
        << "| " << right << setw(12) << "Unit Price" << "|" << endl;
    cout << "+-------------+----------------------+----------------------+-------------+" << endl;
    ColorUtils::resetColor();

    while (res->next()) {
        string id = res->getString("formatted_id");
        string name = res->getString("medicine_name");
        string cat = res->getString("category_of_meds");
        double price = res->getDouble("unit_price");

        if (id.length() > 11) id = id.substr(0, 11);
        if (name.length() > 20) name = name.substr(0, 17) + "...";
        if (cat.length() > 20) cat = cat.substr(0, 17) + "...";

        cout << "| " << left << setw(11) << id
            << " | " << left << setw(20) << name
            << " | " << left << setw(20) << cat
            << " | " << right << setw(10) << fixed << setprecision(2) << price << "  |" << endl;
    }

    ColorUtils::setColor(LIGHT_CYAN);
    cout << "+-------------+----------------------+----------------------+-------------+" << endl;
    ColorUtils::resetColor();
}

void AdminModule::displayTableHeader(const string& title) {
    ColorUtils::setColor(LIGHT_BLUE);
    cout << "+==============================================================================+" << endl;
    int paddingLeft = (78 - title.length()) / 2;
    int paddingRight = 78 - paddingLeft - title.length();

    cout << "|" << string(paddingLeft, ' ') << title << string(paddingRight, ' ') << "|" << endl;
    cout << "+==============================================================================+" << endl;
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
bool AdminModule::validatePhoneNumber(const string& s) {
    if (s.length() < 10 || s.length() > 11) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

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

bool AdminModule::validateBloodType(const string& s) {
    string upperS = s;
    transform(upperS.begin(), upperS.end(), upperS.begin(), ::toupper);
    set<string> validTypes = { "A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-" };
    return validTypes.find(upperS) != validTypes.end();
}

bool AdminModule::validateICNumber(const string& s) {
    if (s.length() != 12) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}


string AdminModule::validateAndCorrectGender(string& s) { return s; }
