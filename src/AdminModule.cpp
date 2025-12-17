#include "AdminModule.h"
#include "ColorUtils.h"

AdminModule::AdminModule(Database* database) : db(database) {}

void AdminModule::showMenu() {
    int choice;
    do {
        system("cls");
        displayTableHeader("ADMIN MODULE");
        
        // Green color theme for admin menu
        ColorUtils::setColor(GREEN);
        cout << "\n+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(WHITE);
        cout << "|  ";
        ColorUtils::printColored("1. View Pharmacy", GREEN);
        ColorUtils::setColor(WHITE);
        cout << "                     |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("2. Generate Hospital Report", GREEN);
        ColorUtils::setColor(WHITE);
        cout << "          |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("3. Add Patient", GREEN);
        ColorUtils::setColor(WHITE);
        cout << "                       |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("4. Patient Receipt", GREEN);
        ColorUtils::setColor(WHITE);
        cout << "                   |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("5. Return to Main Menu", YELLOW);
        ColorUtils::setColor(WHITE);
        cout << "               |" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(GREEN);
        cout << "+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::printColored("\nEnter your choice: ", CYAN);
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            viewPharmacy();
            break;
        case 2:
            generateHospitalReport();
            break;
        case 3:
            addPatient();
            break;
        case 4:
            patientReceipt();
            break;
        case 5:
            return;
        default:
            cout << "\n❌ Invalid choice! Please try again." << endl;
            pressEnterToContinue();
        }
    } while (choice != 5);
}

void AdminModule::viewPharmacy() {
    int choice;
    do {
        system("cls");
        displayTableHeader("VIEW PHARMACY");
        cout << "\n+----------------------------------------+" << endl;
        cout << "|  1. Display All Medications            |" << endl;
        cout << "|  2. Filter by Category                |" << endl;
        cout << "|  3. Display Graphical Report           |" << endl;
        cout << "|  0. Back                              |" << endl;
        cout << "+----------------------------------------+" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            displayPharmacyList();
            break;
        case 2:
            filterPharmacyByCategory();
            break;
        case 3:
            displayPharmacyGraphical();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::displayPharmacyList() {
    system("cls");
    displayTableHeader("PHARMACY - ALL MEDICATIONS");

    try {
        string query = "SELECT pharmacy_id, medicine_name, category_of_meds, quantity, unit_price "
                      "FROM pharmacy ORDER BY category_of_meds, medicine_name";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            displayPharmacyTable(res);
            delete res;
        } else {
            cout << "\n❌ Failed to retrieve pharmacy data!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::filterPharmacyByCategory() {
    system("cls");
    displayTableHeader("FILTER PHARMACY BY CATEGORY");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Maximum (DESC by quantity)         |" << endl;
    cout << "|  2. Minimum (ASC by quantity)          |" << endl;
    cout << "|  3. Select Specific Category           |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    string query;
    switch (choice) {
    case 1: {
        query = "SELECT pharmacy_id, medicine_name, category_of_meds, quantity, unit_price "
                "FROM pharmacy ORDER BY quantity DESC, category_of_meds";
        sql::ResultSet* res = db->executeSelect(query);
        if (res) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|          PHARMACY - SORTED BY MAXIMUM QUANTITY (DESC)            |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            displayPharmacyTable(res);
            delete res;
        }
        break;
    }
    case 2: {
        query = "SELECT pharmacy_id, medicine_name, category_of_meds, quantity, unit_price "
                "FROM pharmacy ORDER BY quantity ASC, category_of_meds";
        sql::ResultSet* res = db->executeSelect(query);
        if (res) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|          PHARMACY - SORTED BY MINIMUM QUANTITY (ASC)            |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            displayPharmacyTable(res);
            delete res;
        }
        break;
    }
    case 3: {
        string category = getStringInput("Enter Category name to search: ");
        if (category.empty()) {
            cout << "\n❌ Category name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }
        query = "SELECT pharmacy_id, medicine_name, category_of_meds, quantity, unit_price "
                "FROM pharmacy WHERE category_of_meds LIKE '%" + category + "%' "
                "ORDER BY medicine_name";
        sql::ResultSet* res = db->executeSelect(query);
        if (res) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|          PHARMACY - CATEGORY: " << left << setw(30) << category << "|" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            if (res->rowsCount() == 0) {
                cout << "\n⚠️  No medications found in this category!" << endl;
            } else {
                displayPharmacyTable(res);
            }
            delete res;
        }
        break;
    }
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::displayPharmacyGraphical() {
    system("cls");
    displayTableHeader("PHARMACY GRAPHICAL REPORT");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Monthly Report                     |" << endl;
    cout << "|  2. Yearly Report                     |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        generateMonthlyReport();
        break;
    case 2:
        generateYearlyReport();
        break;
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice! Returning to main menu." << endl;
    }

    pressEnterToContinue();
}

void AdminModule::generateMonthlyReport() {
    system("cls");
    displayTableHeader("MONTHLY MEDICATION REPORT");

    try {
        // Complex query: Join prescription with pharmacy, group by month
        string query = "SELECT DATE_FORMAT(p.date, '%Y-%m') as month, "
                      "ph.category_of_meds, "
                      "COUNT(p.prescription_id) as prescription_count, "
                      "SUM(ph.quantity) as total_quantity_prescribed "
                      "FROM prescription p "
                      "JOIN pharmacy ph ON p.pharmacy_id = ph.pharmacy_id "
                      "WHERE p.date >= DATE_SUB(CURDATE(), INTERVAL 12 MONTH) "
                      "GROUP BY DATE_FORMAT(p.date, '%Y-%m'), ph.category_of_meds "
                      "ORDER BY month DESC, ph.category_of_meds";

        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-----------+----------------------+----------------------+--------------------------+" << endl;
            cout << "| Month     | Category              | Prescription Count   | Total Quantity Prescribed|" << endl;
            cout << "+-----------┼----------------------┼----------------------┼--------------------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(9) << res->getString("month")
                     << "| " << setw(20) << res->getString("category_of_meds")
                     << "| " << setw(20) << res->getInt("prescription_count")
                     << "| " << setw(24) << res->getInt("total_quantity_prescribed") << "|" << endl;
            }
            cout << "+-----------+----------------------+----------------------+--------------------------+" << endl;
            
            // Display graphical representation
            cout << "\n\nGRAPHICAL REPRESENTATION (Monthly Medication Prescribed):\n" << endl;
            res->beforeFirst();
            double maxValue = 0;
            vector<pair<string, pair<string, int>>> data;
            
            while (res->next()) {
                string month = res->getString("month");
                string category = res->getString("category_of_meds");
                int count = res->getInt("prescription_count");
                data.push_back({month, {category, count}});
                if (count > maxValue) maxValue = count;
            }
            delete res;
            
            // Display bar chart
            for (const auto& item : data) {
                int barLength = maxValue > 0 ? (int)((item.second.second / maxValue) * 50) : 0;
                cout << left << setw(10) << item.first << " | " 
                     << setw(20) << item.second.first << " |";
                for (int i = 0; i < barLength; i++) cout << "█";
                cout << " " << item.second.second << " prescriptions" << endl;
            }
        } else {
            cout << "\n⚠️  No data found for monthly report!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::generateYearlyReport() {
    system("cls");
    displayTableHeader("YEARLY MEDICATION REPORT");

    try {
        // Complex query: Join prescription with pharmacy, group by year
        string query = "SELECT YEAR(p.date) as year, "
                      "ph.category_of_meds, "
                      "COUNT(p.prescription_id) as prescription_count, "
                      "SUM(ph.quantity) as total_quantity_prescribed "
                      "FROM prescription p "
                      "JOIN pharmacy ph ON p.pharmacy_id = ph.pharmacy_id "
                      "WHERE p.date >= DATE_SUB(CURDATE(), INTERVAL 5 YEAR) "
                      "GROUP BY YEAR(p.date), ph.category_of_meds "
                      "ORDER BY year DESC, ph.category_of_meds";

        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-------+----------------------+----------------------+--------------------------+" << endl;
            cout << "| Year  | Category              | Prescription Count   | Total Quantity Prescribed|" << endl;
            cout << "+-------┼----------------------┼----------------------┼--------------------------+" << endl;
            
            while (res->next()) {
                cout << "| " << setw(5) << res->getInt("year")
                     << "| " << setw(20) << res->getString("category_of_meds")
                     << "| " << setw(20) << res->getInt("prescription_count")
                     << "| " << setw(24) << res->getInt("total_quantity_prescribed") << "|" << endl;
            }
            cout << "+-------+----------------------+----------------------+--------------------------+" << endl;
            
            // Display graphical representation
            cout << "\n\nGRAPHICAL REPRESENTATION (Annual Medication Prescribed):\n" << endl;
            res->beforeFirst();
            double maxValue = 0;
            vector<pair<int, pair<string, int>>> data;
            
            while (res->next()) {
                int year = res->getInt("year");
                string category = res->getString("category_of_meds");
                int count = res->getInt("prescription_count");
                data.push_back({year, {category, count}});
                if (count > maxValue) maxValue = count;
            }
            delete res;
            
            // Display bar chart
            for (const auto& item : data) {
                int barLength = maxValue > 0 ? (int)((item.second.second / maxValue) * 50) : 0;
                cout << left << setw(6) << item.first << " | " 
                     << setw(20) << item.second.first << " |";
                for (int i = 0; i < barLength; i++) cout << "█";
                cout << " " << item.second.second << " prescriptions" << endl;
            }
        } else {
            cout << "\n⚠️  No data found for yearly report!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::generateHospitalReport() {
    system("cls");
    displayTableHeader("GENERATE HOSPITAL REPORT");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Filter Category                    |" << endl;
    cout << "|  2. Display Graph                      |" << endl;
    cout << "|  3. Exit Report Generation             |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        filterPharmacyByCategory();
        break;
    case 2:
        displayGraphicalReport();
        break;
    case 3:
        cout << "\nExiting Report Generation..." << endl;
        pressEnterToContinue();
        return;
    default:
        cout << "\n❌ Invalid choice! Try again." << endl;
        pressEnterToContinue();
    }
}

void AdminModule::displayGraphicalReport() {
    system("cls");
    displayTableHeader("GRAPHICAL HOSPITAL REPORT");

    // This will show medication prescribed in given timeline
    // Using complex aggregation and grouping
    try {
        string query = "SELECT ph.category_of_meds, "
                      "COUNT(p.prescription_id) as total_prescriptions, "
                      "SUM(ph.quantity) as total_quantity, "
                      "AVG(ph.unit_price) as avg_price "
                      "FROM prescription p "
                      "JOIN pharmacy ph ON p.pharmacy_id = ph.pharmacy_id "
                      "GROUP BY ph.category_of_meds "
                      "ORDER BY total_prescriptions DESC";

        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+----------------------+----------------------+----------------------+------------------+" << endl;
            cout << "| Category             | Total Prescriptions  | Total Quantity       | Average Price    |" << endl;
            cout << "+----------------------┼----------------------┼----------------------┼------------------+" << endl;
            
            double maxPrescriptions = 0;
            vector<pair<string, int>> chartData;
            
            while (res->next()) {
                string category = res->getString("category_of_meds");
                int prescriptions = res->getInt("total_prescriptions");
                chartData.push_back({category, prescriptions});
                if (prescriptions > maxPrescriptions) maxPrescriptions = prescriptions;
                
                cout << "| " << setw(20) << category
                     << "| " << setw(20) << prescriptions
                     << "| " << setw(20) << res->getInt("total_quantity")
                     << "| RM " << setw(13) << fixed << setprecision(2) << res->getDouble("avg_price") << "|" << endl;
            }
            cout << "+----------------------+----------------------+----------------------+------------------+" << endl;
            
            // Display bar chart
            cout << "\n\nGRAPHICAL CHART (Amount of Medication Prescribed):\n" << endl;
            for (const auto& item : chartData) {
                int barLength = maxPrescriptions > 0 ? (int)((item.second / maxPrescriptions) * 50) : 0;
                cout << left << setw(25) << item.first << " |";
                for (int i = 0; i < barLength; i++) cout << "█";
                cout << " " << item.second << " prescriptions" << endl;
            }
            
            delete res;
        } else {
            cout << "\n⚠️  No data found!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::addPatient() {
    system("cls");
    displayTableHeader("ADD PATIENT");

    try {
        // Check if patient already exists
        int patientId = getIntInput("Enter Patient ID (or 0 for new patient): ");
        
        if (patientId > 0) {
            // Check if patient exists
            string checkQuery = "SELECT patient_id, full_name FROM patient WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            
            if (checkRes && checkRes->next()) {
                cout << "\n⚠️  Patient already exists!" << endl;
                cout << "Patient Name: " << checkRes->getString("full_name") << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            if (checkRes) delete checkRes;
        }

        string fullName = getStringInput("Enter Patient Name: ");
        if (fullName.empty()) {
            cout << "\n❌ Patient name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        string gender = getStringInput("Enter Patient Gender (Male/Female/Other): ");
        if (gender.empty()) {
            cout << "\n❌ Gender cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        string dob = getStringInput("Enter Patient Date of Birth (YYYY-MM-DD): ");
        if (dob.empty()) {
            cout << "\n❌ Date of birth cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        string contactNumber = getStringInput("Enter Patient Contact Number: ");
        if (contactNumber.empty()) {
            cout << "\n❌ Contact number cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        string bloodType = getStringInput("Enter Patient Blood Type: ");
        string emergencyContact = getStringInput("Enter Emergency Contact: ");

        string query = "INSERT INTO patient (full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status) "
            "VALUES ('" + fullName + "', '" + gender + "', '" + dob + "', '" + contactNumber + "', '" + bloodType + "', '" + emergencyContact + "', 'Active')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Patient added successfully!" << endl;
            
            // Display patient details
            string getQuery = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, status FROM patient WHERE full_name = '" + fullName + "' ORDER BY patient_id DESC LIMIT 1";
            sql::ResultSet* res = db->executeSelect(getQuery);
            if (res && res->next()) {
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    PATIENT DETAILS                             |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient ID: " << left << setw(44) << res->getInt("patient_id") << "|" << endl;
                cout << "| Full Name: " << left << setw(46) << res->getString("full_name") << "|" << endl;
                cout << "| Gender: " << left << setw(49) << res->getString("gender") << "|" << endl;
                cout << "| Date of Birth: " << left << setw(42) << res->getString("date_of_birth") << "|" << endl;
                cout << "| Contact Number: " << left << setw(41) << res->getString("contact_number") << "|" << endl;
                cout << "| Status: " << left << setw(49) << res->getString("status") << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            }
            if (res) delete res;
        } else {
            cout << "\n❌ Failed to add patient!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::patientReceipt() {
    system("cls");
    displayTableHeader("PATIENT RECEIPT");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    calculatePatientReceipt(patientId);
}

void AdminModule::calculatePatientReceipt(int patientId) {
    try {
        // First, get patient details
        string patientQuery = "SELECT patient_id, full_name, contact_number, status FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n❌ Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }
        
        string patientName = patientRes->getString("full_name");
        delete patientRes;

        // Calculate total treatment done to patient with consultant fee
        // Using SUM aggregation with JOIN
        string query = "SELECT SUM(t.consultation_fee + t.treatment_fee) as total_amount, "
                      "COUNT(t.treatment_id) as treatment_count "
                      "FROM treatment t "
                      "JOIN medical_record mr ON t.doctor_id = mr.doctor_id "
                      "WHERE mr.patient_id = " + to_string(patientId);

        sql::ResultSet* res = db->executeSelect(query);
        
        double totalAmount = 0.0;
        int treatmentCount = 0;
        
        if (res && res->next()) {
            totalAmount = res->getDouble("total_amount");
            treatmentCount = res->getInt("treatment_count");
        }
        if (res) delete res;

        // If no treatments found, set default consultation fee
        if (totalAmount == 0) {
            totalAmount = 50.00; // Default consultation fee
        }

        displayReceipt(patientId, totalAmount);
        
        // Update patient status based on next appointment
        // If next appointment exists -> patient status = Active, else = Inactive
        string appointmentQuery = "SELECT COUNT(*) as count FROM appointment WHERE patient_id = " + to_string(patientId) + " AND appointment_date >= CURDATE()";
        sql::ResultSet* apptRes = db->executeSelect(appointmentQuery);
        
        string newStatus = "Inactive";
        if (apptRes && apptRes->next() && apptRes->getInt("count") > 0) {
            newStatus = "Active";
        }
        if (apptRes) delete apptRes;

        // Update patient status
        string updateQuery = "UPDATE patient SET status = '" + newStatus + "' WHERE patient_id = " + to_string(patientId);
        db->executeUpdate(updateQuery);

    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::displayReceipt(int patientId, double totalAmount) {
    system("cls");
    displayTableHeader("PATIENT RECEIPT");

    try {
        // Get patient details
        string patientQuery = "SELECT patient_id, full_name, contact_number, date_of_birth FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n❌ Patient not found!" << endl;
            if (patientRes) delete patientRes;
            return;
        }

        string patientName = patientRes->getString("full_name");
        string contactNumber = patientRes->getString("contact_number");
        string dob = patientRes->isNull("date_of_birth") ? "N/A" : patientRes->getString("date_of_birth");
        delete patientRes;

        // Get treatment details
        string treatmentQuery = "SELECT t.treatment_id, t.consultation_fee, t.treatment_fee, t.treatment_date, d.full_name as doctor_name "
                              "FROM treatment t "
                              "JOIN medical_record mr ON t.doctor_id = mr.doctor_id "
                              "JOIN doctor d ON t.doctor_id = d.doctor_id "
                              "WHERE mr.patient_id = " + to_string(patientId) + " "
                              "ORDER BY t.treatment_date DESC";

        sql::ResultSet* treatmentRes = db->executeSelect(treatmentQuery);

        // Display receipt
        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    HOSPITAL RECEIPT                            |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient ID: " << left << setw(45) << to_string(patientId) << "|" << endl;
        cout << "| Patient Name: " << left << setw(43) << patientName << "|" << endl;
        cout << "| Contact Number: " << left << setw(42) << contactNumber << "|" << endl;
        cout << "| Date of Birth: " << left << setw(42) << dob << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "|                    TREATMENT DETAILS                           |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;

        if (treatmentRes && treatmentRes->rowsCount() > 0) {
            cout << "| " << left << setw(58) << "Treatment ID | Doctor | Consultation Fee | Treatment Fee | Date" << "|" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            
            while (treatmentRes->next()) {
                cout << "| " << left << setw(5) << treatmentRes->getInt("treatment_id")
                     << " | " << setw(15) << treatmentRes->getString("doctor_name").substr(0, 15)
                     << " | RM " << setw(12) << fixed << setprecision(2) << treatmentRes->getDouble("consultation_fee")
                     << " | RM " << setw(12) << fixed << setprecision(2) << treatmentRes->getDouble("treatment_fee")
                     << " | " << setw(10) << treatmentRes->getString("treatment_date") << "|" << endl;
            }
        } else {
            cout << "| " << left << setw(58) << "No treatments found. Default consultation fee applied." << "|" << endl;
        }
        if (treatmentRes) delete treatmentRes;

        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Total Amount: " << left << setw(43) << ("RM " + to_string(totalAmount)) << "|" << endl;
        cout << "| Date: " << left << setw(50) << (string(__DATE__) + " " + string(__TIME__)) << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;

    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }
}

void AdminModule::displayPharmacyTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+----------------------+----------+-------------+" << endl;
    cout << "| Pharmacy ID | Medicine Name        | Category              | Quantity | Unit Price  |" << endl;
    cout << "+-------------┼----------------------┼----------------------┼----------┼-------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << res->getInt("pharmacy_id")
             << "| " << setw(20) << res->getString("medicine_name")
             << "| " << setw(20) << res->getString("category_of_meds")
             << "| " << setw(8) << res->getInt("quantity")
             << "| RM " << setw(9) << fixed << setprecision(2) << res->getDouble("unit_price") << "|" << endl;
    }
    
    cout << "+-------------+----------------------+----------------------+----------+-------------+" << endl;
}

void AdminModule::displayTableHeader(const string& title) {
    // Green theme header
    ColorUtils::setColor(GREEN);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title
    ColorUtils::setColor(WHITE);
    cout << "|";
    int padding = (60 - title.length()) / 2;
    for (int i = 0; i < padding; i++) cout << " ";
    ColorUtils::printColoredBG(title, YELLOW, GREEN);
    for (int i = 0; i < (60 - title.length() - padding); i++) cout << " ";
    ColorUtils::setColor(WHITE);
    cout << "|" << endl;
    
    ColorUtils::setColor(GREEN);
    cout << "|" << setw(60) << "" << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
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
    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

string AdminModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}
