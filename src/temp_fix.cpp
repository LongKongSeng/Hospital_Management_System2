#include "AdminModule.h"
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
            "Generate Hospital Report",
            "Add Patient",
            "Patient Receipt",
            "Return to Main Menu"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "ADMIN MODULE", true);
        
        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            viewPharmacy();
            break;
        case 1:
            generateHospitalReport();
            break;
        case 2:
            addPatient();
            break;
        case 3:
            patientReceipt();
            break;
        case 4:
            return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 4);
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
            cout << "\n[ERROR] Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::displayPharmacyList() {
    system("cls");
    displayTableHeader("PHARMACY - ALL MEDICATIONS");

    try {
        string query = "SELECT pharmacy_id, formatted_id, medicine_name, category_of_meds, quantity, unit_price "
                      "FROM pharmacy ORDER BY category_of_meds, medicine_name";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            displayPharmacyTable(res);
            delete res;
        } else {
            cout << "\n[ERROR] Failed to retrieve pharmacy data!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        query = "SELECT pharmacy_id, formatted_id, medicine_name, category_of_meds, quantity, unit_price "
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
        query = "SELECT pharmacy_id, formatted_id, medicine_name, category_of_meds, quantity, unit_price "
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
            cout << "\n[ERROR] Category name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }
        query = "SELECT pharmacy_id, formatted_id, medicine_name, category_of_meds, quantity, unit_price "
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
        cout << "\n[ERROR] Invalid choice!" << endl;
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
        cout << "\n[ERROR] Invalid choice! Returning to main menu." << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Invalid choice! Try again." << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::addPatient() {
    system("cls");
    displayTableHeader("ADD PATIENT");

    try {
        string fullName = getStringInput("Enter Patient Name: ");
        if (fullName.empty()) {
            cout << "\n[ERROR] Patient name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        // Gender input with validation (Male/Female only)
        string gender;
        bool validGender = false;
        while (!validGender) {
            gender = getStringInput("Enter Patient Gender (Male/Female or M/F): ");
            if (gender.empty()) {
                cout << "\n[ERROR] Gender cannot be empty! Please try again." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                continue;
            }
            
            // Validate and correct gender
            string correctedGender = validateAndCorrectGender(gender);
            if (correctedGender != "Male" && correctedGender != "Female") {
                cout << "\n[ERROR] Gender must be Male or Female only! Please try again." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                continue;
            }
            gender = correctedGender;
            validGender = true;
        }

        // IC Number input and validation (12 digits)
        string icNumber;
        bool validIC = false;
        while (!validIC) {
            icNumber = getStringInput("Enter Patient IC Number (12 digits): ");
            if (icNumber.empty()) {
                cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                continue;
            }
            
            if (!validateICNumber(icNumber)) {
                cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                continue;
            }
            
            // Check if IC number already exists
            string checkICQuery = "SELECT patient_id, formatted_id, full_name FROM patient WHERE ic_number = '" + icNumber + "'";
            sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
            
            if (checkICRes && checkICRes->next()) {
                cout << "\n⚠️  Patient already exists with this IC number!" << endl;
                cout << "Patient ID: " << getFormattedId(checkICRes, "formatted_id", "patient_id") << endl;
                cout << "Patient Name: " << checkICRes->getString("full_name") << endl;
                if (checkICRes) delete checkICRes;
                pressEnterToContinue();
                return;
            }
            if (checkICRes) delete checkICRes;
            validIC = true;
        }

        // Date of Birth with format validation
        string dob;
        bool validDOB = false;
        while (!validDOB) {
            dob = getStringInput("Enter Patient Date of Birth (YYYY-MM-DD): ");
            if (dob.empty()) {
                cout << "\n[ERROR] Date of birth cannot be empty!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                continue;
            }
            
            if (!validateDateFormat(dob)) {
                cout << "\n[ERROR] Invalid date format! Please use YYYY-MM-DD format (e.g., 1990-01-15)" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                continue;
            }
            validDOB = true;
        }

        // Contact Number with validation (10 or 11 digits)
        string contactNumber;
        bool validContact = false;
        while (!validContact) {
            contactNumber = getStringInput("Enter Patient Contact Number: ");
            if (contactNumber.empty()) {
                cout << "\n[ERROR] Contact number cannot be empty!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                continue;
            }
            
            if (!validatePhoneNumber(contactNumber)) {
                cout << "\n[ERROR] Contact number must be 10 or 11 digits!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                continue;
            }
            validContact = true;
        }

        // Blood Type with validation (only valid types)
        string bloodType;
        bool validBloodType = false;
        while (!validBloodType) {
            bloodType = getStringInput("Enter Patient Blood Type (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
            if (bloodType.empty()) {
                cout << "\n[ERROR] Blood type cannot be empty!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                cout << "Enter Patient Contact Number: " << contactNumber << endl;
                continue;
            }
            
            if (!validateBloodType(bloodType)) {
                cout << "\n[ERROR] Invalid blood type! Please enter one of: A+, A-, B+, B-, AB+, AB-, O+, O-" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                cout << "Enter Patient Contact Number: " << contactNumber << endl;
                continue;
            }
            validBloodType = true;
        }

        // Emergency Contact with validation (10 or 11 digits)
        string emergencyContact;
        bool validEmergency = false;
        while (!validEmergency) {
            emergencyContact = getStringInput("Enter Emergency Contact: ");
            if (emergencyContact.empty()) {
                cout << "\n[ERROR] Emergency contact cannot be empty!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                cout << "Enter Patient Contact Number: " << contactNumber << endl;
                cout << "Enter Patient Blood Type (A+, A-, B+, B-, AB+, AB-, O+, O-): " << bloodType << endl;
                continue;
            }
            
            if (!validatePhoneNumber(emergencyContact)) {
                cout << "\n[ERROR] Emergency contact must be 10 or 11 digits!" << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADD PATIENT");
                cout << "Enter Patient Name: " << fullName << endl;
                cout << "Enter Patient Gender (Male/Female or M/F): " << gender << endl;
                cout << "Enter Patient IC Number (12 digits): " << icNumber << endl;
                cout << "Enter Patient Date of Birth (YYYY-MM-DD): " << dob << endl;
                cout << "Enter Patient Contact Number: " << contactNumber << endl;
                cout << "Enter Patient Blood Type (A+, A-, B+, B-, AB+, AB-, O+, O-): " << bloodType << endl;
                continue;
            }
            validEmergency = true;
        }

        string query = "INSERT INTO patient (full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, ic_number, status) "
            "VALUES ('" + fullName + "', '" + gender + "', '" + dob + "', '" + contactNumber + "', '" + bloodType + "', '" + emergencyContact + "', '" + icNumber + "', 'Active')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Patient added successfully!" << endl;
            
            // Display patient details
            string getQuery = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, status FROM patient WHERE ic_number = '" + icNumber + "' ORDER BY patient_id DESC LIMIT 1";
            sql::ResultSet* res = db->executeSelect(getQuery);
            if (res && res->next()) {
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    PATIENT DETAILS                             |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient ID: " << left << setw(44) << getFormattedId(res, "formatted_id", "patient_id") << "|" << endl;
                cout << "| Full Name: " << left << setw(46) << res->getString("full_name") << "|" << endl;
                cout << "| Gender: " << left << setw(49) << res->getString("gender") << "|" << endl;
                cout << "| Date of Birth: " << left << setw(42) << res->getString("date_of_birth") << "|" << endl;
                cout << "| Contact Number: " << left << setw(41) << res->getString("contact_number") << "|" << endl;
                cout << "| Status: " << left << setw(49) << res->getString("status") << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            }
            if (res) delete res;
        } else {
            cout << "\n[ERROR] Failed to add patient!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::patientReceipt() {
    system("cls");
    displayTableHeader("PATIENT RECEIPT");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
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
            cout << "\n[ERROR] Patient not found!" << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            return;
        }

        string patientName = patientRes->getString("full_name");
        string contactNumber = patientRes->getString("contact_number");
        string dob = patientRes->isNull("date_of_birth") ? "N/A" : patientRes->getString("date_of_birth");
        delete patientRes;

        // Get treatment details
        string treatmentQuery = "SELECT t.treatment_id, t.formatted_id as treatment_formatted_id, t.consultation_fee, t.treatment_fee, t.treatment_date, d.full_name as doctor_name "
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
        string patientFormattedQuery = "SELECT formatted_id FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* patientFormattedRes = db->executeSelect(patientFormattedQuery);
        string patientFormattedId = to_string(patientId);
        if (patientFormattedRes && patientFormattedRes->next()) {
            patientFormattedId = patientFormattedRes->isNull("formatted_id") ? to_string(patientId) : string(patientFormattedRes->getString("formatted_id"));
        }
        if (patientFormattedRes) delete patientFormattedRes;
        cout << "| Patient ID: " << left << setw(45) << patientFormattedId << "|" << endl;
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
                cout << "| " << left << setw(5) << getFormattedId(treatmentRes, "treatment_formatted_id", "treatment_id")
                     << " | " << setw(15) << string(treatmentRes->getString("doctor_name")).substr(0, 15)
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
}

void AdminModule::displayPharmacyTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+----------------------+----------+-------------+" << endl;
    cout << "| Pharmacy ID | Medicine Name        | Category              | Quantity | Unit Price  |" << endl;
    cout << "+-------------┼----------------------┼----------------------┼----------┼-------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << getFormattedId(res, "formatted_id", "pharmacy_id")
             << "| " << setw(20) << res->getString("medicine_name")
             << "| " << setw(20) << res->getString("category_of_meds")
             << "| " << setw(8) << res->getInt("quantity")
             << "| RM " << setw(9) << fixed << setprecision(2) << res->getDouble("unit_price") << "|" << endl;
    }
    
    cout << "+-------------+----------------------+----------------------+----------+-------------+" << endl;
}

void AdminModule::displayTableHeader(const string& title) {
    // Blue theme header matching new GUI style
    const int SEPARATOR_LENGTH = 80;
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Centered title with white text on blue background
    MenuNavigator::displayTitle(title, SEPARATOR_LENGTH);
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
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

bool AdminModule::validatePhoneNumber(const string& phoneNumber) {
    if (phoneNumber.empty()) {
        return false;
    }
    
    // Check if all characters are digits
    for (char c : phoneNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    
    // Check if length is 10 or 11 digits
    if (phoneNumber.length() != 10U && phoneNumber.length() != 11U) {
        return false;
    }
    
    return true;
}

bool AdminModule::validateDateFormat(const string& date) {
    if (date.empty() || date.length() != 10U) {
        return false;
    }
    
    // Check format: YYYY-MM-DD (10 characters)
    // Format: YYYY-MM-DD where YYYY is 4 digits, MM is 2 digits, DD is 2 digits
    if (date[4] != '-' || date[7] != '-') {
        return false;
    }
    
    // Check if all characters except dashes are digits
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7) {
            if (!isdigit(date[i])) {
                return false;
            }
        }
    }
    
    // Basic validation for month (01-12) and day (01-31)
    try {
        string monthStr = date.substr(5, 2);
        string dayStr = date.substr(8, 2);
        
        int month = stoi(monthStr);
        int day = stoi(dayStr);
        
        if (month < 1 || month > 12) {
            return false;
        }
        
        if (day < 1 || day > 31) {
            return false;
        }
    } catch (...) {
        return false;
    }
    
    return true;
}

bool AdminModule::validateBloodType(const string& bloodType) {
    // Valid blood types: A+, A-, B+, B-, AB+, AB-, O+, O-
    vector<string> validTypes = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    
    for (const string& type : validTypes) {
        if (bloodType == type) {
            return true;
        }
    }
    
    return false;
}

bool AdminModule::validateICNumber(const string& icNumber) {
    if (icNumber.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (char c : icNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    // Check if length is exactly 12 digits
    if (icNumber.length() != 12U) {
        return false;
    }
    return true;
}

string AdminModule::validateAndCorrectGender(string& gender) {
    // Convert to lowercase for comparison
    string genderLower = gender;
    for (char& c : genderLower) {
        c = tolower(c);
    }
    
    // Auto-correct common inputs
    if (genderLower == "m" || genderLower == "male") {
        return "Male";
    } else if (genderLower == "f" || genderLower == "female") {
        return "Female";
    } else {
        // Return original if not valid
        return gender;
    }
}

#include "Database.h"

Database::Database() {
    host = "tcp://127.0.0.1:3306";
    user = "root";
    password = "";
    database = "hospital_management_system";
    driver = nullptr;
    con = nullptr;
}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    try {
        driver = get_driver_instance();
        con = driver->connect(host, user, password);
        con->setSchema(database);
        cout << "\n✅ Successfully connected to database: " << database << endl;
        return true;
    }
    catch (sql::SQLException& e) {
        displayError("Connection");
        cout << "SQL Error: " << e.what() << endl;
        cout << "Error Code: " << e.getErrorCode() << endl;
        return false;
    }
    catch (exception& e) {
        displayError("Connection");
        cout << "Error: " << e.what() << endl;
        return false;
    }
}

void Database::disconnect() {
    if (con) {
        delete con;
        con = nullptr;
    }
}

sql::Connection* Database::getConnection() {
    return con;
}

bool Database::executeQuery(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return false;
        }
        sql::Statement* stmt = con->createStatement();
        stmt->execute(query);
        delete stmt;
        return true;
    }
    catch (sql::SQLException& e) {
        displayError("Query Execution");
        cout << "SQL Error: " << e.what() << endl;
        cout << "Query: " << query << endl;
        return false;
    }
}

sql::ResultSet* Database::executeSelect(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return nullptr;
        }
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);
        return res;
    }
    catch (sql::SQLException& e) {
        displayError("Select Query");
        cout << "SQL Error: " << e.what() << endl;
        return nullptr;
    }
}

bool Database::executeUpdate(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return false;
        }
        sql::Statement* stmt = con->createStatement();
        int result = stmt->executeUpdate(query);
        delete stmt;
        return result > 0;
    }
    catch (sql::SQLException& e) {
        displayError("Update Query");
        cout << "SQL Error: " << e.what() << endl;
        return false;
    }
}

void Database::displayError(const string& operation) {
    cout << "\n[ERROR] Error in " << operation << " operation!" << endl;
}


#include "DoctorModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

DoctorModule::DoctorModule(Database* database, const string& doctorId) : db(database), currentDoctorId(doctorId) {}

void DoctorModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Record",
            "View Appointments",
            "Make Diagnosis",
            "Edit Patient Medical Record",
            "Return to Main Menu"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "DOCTOR MODULE", true);
        
        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            viewPatientRecord();
            break;
        case 1:
            viewAppointments();
            break;
        case 2:
            makeDiagnosis();
            break;
        case 3:
            editPatientMedicalRecord();
            break;
        case 4:
            return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 4);
}

void DoctorModule::viewPatientRecord() {
    system("cls");
    displayTableHeader("VIEW PATIENT RECORD");

    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        // Display patient details
        string patientQuery = "SELECT formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status "
                             "FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    PATIENT INFORMATION                         |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient ID: " << left << setw(46) << string(patientRes->getString("formatted_id")) << "|" << endl;
        cout << "| Full Name: " << left << setw(46) << patientRes->getString("full_name") << "|" << endl;
        cout << "| Gender: " << left << setw(49) << patientRes->getString("gender") << "|" << endl;
        cout << "| Date of Birth: " << left << setw(42) << patientRes->getString("date_of_birth") << "|" << endl;
        cout << "| Contact Number: " << left << setw(41) << patientRes->getString("contact_number") << "|" << endl;
        cout << "| Blood Type: " << left << setw(45) << (patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type")) << "|" << endl;
        cout << "| Status: " << left << setw(49) << patientRes->getString("status") << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        delete patientRes;

        // Display medical records
        string recordQuery = "SELECT mr.record_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
                            "FROM medical_record mr "
                            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.diagnosis_id "
                            "WHERE mr.patient_id = " + to_string(patientId) + " "
                            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);
        
        if (recordRes && recordRes->rowsCount() > 0) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|                    MEDICAL RECORDS                             |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            displayMedicalRecordTable(recordRes);
        } else {
            cout << "\n⚠️  No medical records found for this patient." << endl;
        }
        if (recordRes) delete recordRes;

    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::viewAppointments() {
    system("cls");
    displayTableHeader("VIEW APPOINTMENTS");

    try {
        // Get appointments assigned to this doctor
        string appointmentQuery = "SELECT a.formatted_id as appointment_formatted_id, a.appointment_date, a.appointment_time, a.status, "
                                 "p.formatted_id as patient_formatted_id, p.full_name as patient_name, n.full_name as nurse_name "
                                 "FROM appointment a "
                                 "JOIN patient p ON a.patient_id = p.formatted_id "
                                 "JOIN nurse n ON a.nurse_id = n.formatted_id "
                                 "WHERE a.doctor_id = '" + currentDoctorId + "' "
                                 "ORDER BY a.appointment_date DESC, a.appointment_time DESC";

        sql::ResultSet* appointmentRes = db->executeSelect(appointmentQuery);
        
        if (appointmentRes && appointmentRes->rowsCount() > 0) {
            cout << "\n+-----------------+-----------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            cout << "| Appointment ID  | Patient ID| Patient Name          | Nurse Name            | Date         | Time         | Status               |" << endl;
            cout << "+-----------------+-----------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            
            while (appointmentRes->next()) {
                cout << "| " << setw(15) << getFormattedId(appointmentRes, "appointment_formatted_id", "appointment_id")
                     << "| " << setw(9) << getFormattedId(appointmentRes, "patient_formatted_id", "patient_id")
                     << "| " << setw(20) << appointmentRes->getString("patient_name")
                     << "| " << setw(20) << appointmentRes->getString("nurse_name")
                     << "| " << setw(12) << appointmentRes->getString("appointment_date")
                     << "| " << setw(12) << appointmentRes->getString("appointment_time")
                     << "| " << setw(20) << appointmentRes->getString("status") << "|" << endl;
            }
            
            cout << "+-----------------+-----------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            
            // Show appointment count
            string countQuery = "SELECT COUNT(*) as total_count, "
                               "SUM(CASE WHEN status = 'Scheduled' THEN 1 ELSE 0 END) as scheduled_count, "
                               "SUM(CASE WHEN status = 'Completed' THEN 1 ELSE 0 END) as completed_count "
                               "FROM appointment WHERE doctor_id = '" + currentDoctorId + "'";
            
            sql::ResultSet* countRes = db->executeSelect(countQuery);
            if (countRes && countRes->next()) {
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    APPOINTMENT SUMMARY                          |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Total Appointments: " << left << setw(41) << countRes->getInt("total_count") << "|" << endl;
                cout << "| Scheduled: " << left << setw(48) << countRes->getInt("scheduled_count") << "|" << endl;
                cout << "| Completed: " << left << setw(47) << countRes->getInt("completed_count") << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            }
            if (countRes) delete countRes;
        } else {
            cout << "\n⚠️  No appointments found for this doctor." << endl;
        }
        if (appointmentRes) delete appointmentRes;
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::makeDiagnosis() {
    system("cls");
    displayTableHeader("MAKE DIAGNOSIS");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify patient exists
    string checkQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    
    string patientName = checkRes->getString("full_name");
    delete checkRes;

    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|                    PATIENT INFORMATION                         |" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << endl;
    
    string disease = getStringInput("Enter Disease: ");
    string disorder = getStringInput("Enter Disorder: ");
    string durationOfPain = getStringInput("Enter Duration of Pain: ");
    string severity = getStringInput("Enter Severity: ");
    string date = getStringInput("Enter Date (YYYY-MM-DD): ");
    if (date.empty()) {
        date = "CURDATE()";
    }

    // Get or create prescription
    int prescriptionId = -1;
    string prescriptionChoice = getStringInput("Do you want to add prescription? (yes/no): ");
    if (prescriptionChoice == "yes" || prescriptionChoice == "YES") {
        int pharmacyId = getIntInput("Enter Pharmacy ID (Medication ID): ");
        if (pharmacyId > 0) {
            // Validate that pharmacy_id exists
            string validatePharmacyQuery = "SELECT pharmacy_id, formatted_id, medicine_name FROM pharmacy WHERE pharmacy_id = " + to_string(pharmacyId);
            sql::ResultSet* validateRes = db->executeSelect(validatePharmacyQuery);
            
            if (!validateRes || !validateRes->next()) {
                cout << "\n[ERROR] Pharmacy ID " << pharmacyId << " not found! Please enter a valid Pharmacy ID." << endl;
                cout << "Continuing without prescription..." << endl;
                if (validateRes) delete validateRes;
            } else {
                string medicineName = validateRes->getString("medicine_name");
                cout << "Medication: " << medicineName << endl;
                delete validateRes;
                
                string dosage = getStringInput("Enter Dosage: ");
                string durationOfMeds = getStringInput("Enter Duration of Medications: ");
                string instructions = getStringInput("Enter Instructions: ");
                
                string presQuery = "INSERT INTO prescription (pharmacy_id, dosage, duration_of_meds, instructions, date) "
                    "VALUES (" + to_string(pharmacyId) + ", '" + dosage + "', '" + durationOfMeds + "', '" + instructions + "', '" + date + "')";
                
                if (db->executeUpdate(presQuery)) {
                    // Get the prescription_id
                    string getIdQuery = "SELECT prescription_id FROM prescription WHERE pharmacy_id = " + to_string(pharmacyId) + " AND date = '" + date + "' ORDER BY prescription_id DESC LIMIT 1";
                    sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                    if (idRes && idRes->next()) {
                        prescriptionId = idRes->getInt("prescription_id");
                    }
                    if (idRes) delete idRes;
                } else {
                    cout << "\n⚠️  Failed to create prescription. Continuing without prescription..." << endl;
                }
            }
        }
    }

    try {
        // Insert diagnosis
        string diagQuery = "INSERT INTO diagnosis (disease, disorder, duration_of_pain, severity, prescription_id, date) "
            "VALUES ('" + disease + "', '" + disorder + "', '" + durationOfPain + "', '" + severity + "', ";
        
        if (prescriptionId > 0) {
            diagQuery += to_string(prescriptionId) + ", '" + date + "')";
        } else {
            diagQuery += "NULL, '" + date + "')";
        }

        if (db->executeUpdate(diagQuery)) {
            // Get diagnosis_id
            string getIdQuery = "SELECT diagnosis_id FROM diagnosis WHERE disease = '" + disease + "' ORDER BY diagnosis_id DESC LIMIT 1";
            sql::ResultSet* idRes = db->executeSelect(getIdQuery);
            int diagnosisId = -1;
            if (idRes && idRes->next()) {
                diagnosisId = idRes->getInt("diagnosis_id");
            }
            if (idRes) delete idRes;

            if (diagnosisId > 0) {
                // Insert medical record
                string recordQuery = "INSERT INTO medical_record (patient_id, doctor_id, diagnosis_id, date_of_record, notes) "
                    "VALUES (" + to_string(patientId) + ", " + to_string(currentDoctorId) + ", " + to_string(diagnosisId) + ", '" + date + "', 'Diagnosis made by doctor')";

                if (db->executeUpdate(recordQuery)) {
                    cout << "\n✅ Diagnosis made successfully!" << endl;
                    cout << "\n+----------------------------------------------------------------+" << endl;
                    cout << "|                    DIAGNOSIS DETAILS                             |" << endl;
                    cout << "+----------------------------------------------------------------+" << endl;
                    cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
                    cout << "| Disease: " << left << setw(50) << (disease.empty() ? "N/A" : disease) << "|" << endl;
                    cout << "| Disorder: " << left << setw(49) << (disorder.empty() ? "N/A" : disorder) << "|" << endl;
                    cout << "| Duration of Pain: " << left << setw(41) << (durationOfPain.empty() ? "N/A" : durationOfPain) << "|" << endl;
                    cout << "| Severity: " << left << setw(49) << (severity.empty() ? "N/A" : severity) << "|" << endl;
                    cout << "| Date: " << left << setw(52) << (date == "CURDATE()" ? "Today" : date) << "|" << endl;
                    cout << "| Medical Record: " << left << setw(43) << "Created" << "|" << endl;
                    cout << "+----------------------------------------------------------------+" << endl;
                } else {
                    cout << "\n⚠️  Diagnosis created but medical record failed!" << endl;
                }
            } else {
                cout << "\n[ERROR] Failed to retrieve diagnosis ID!" << endl;
            }
        } else {
            cout << "\n[ERROR] Failed to create diagnosis!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::editPatientMedicalRecord() {
    system("cls");
    displayTableHeader("EDIT PATIENT MEDICAL RECORD");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        // Verify patient exists
        string checkPatientQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* checkPatientRes = db->executeSelect(checkPatientQuery);
        
        if (!checkPatientRes || !checkPatientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (checkPatientRes) delete checkPatientRes;
            pressEnterToContinue();
            return;
        }
        string patientName = checkPatientRes->getString("full_name");
        delete checkPatientRes;

        // Display ALL medical records for this patient
        string recordQuery = "SELECT mr.formatted_id as record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
                            "FROM medical_record mr "
                            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
                            "WHERE mr.patient_id = '" + patientId + "' "
                            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);
        
        if (!recordRes) {
            cout << "\n[ERROR] Error retrieving medical records!" << endl;
            pressEnterToContinue();
            return;
        }

        // Check if there are any records by trying to read the first row
        bool hasRecords = recordRes->next();
        
        if (!hasRecords) {
            cout << "\n⚠️  No medical record found for this patient." << endl;
            cout << "Would you like to create a new record? (yes/no): ";
            string createChoice;
            getline(cin, createChoice);
            
            if (createChoice == "yes" || createChoice == "YES") {
                makeDiagnosis();
            }
            delete recordRes;
            return;
        }

        // Re-execute query to display all records (since we moved the cursor)
        delete recordRes;
        recordRes = db->executeSelect(recordQuery);

        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    PATIENT INFORMATION                         |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "\nMedical Record History:" << endl;
        displayMedicalRecordTable(recordRes);
        delete recordRes;

        // Ask user to select which record to edit
        int recordId = getIntInput("\nEnter Record ID to edit (or 0 to cancel): ");
        if (recordId <= 0) {
            cout << "\n[ERROR] Invalid Record ID or operation cancelled!" << endl;
            pressEnterToContinue();
            return;
        }

        // Verify record exists and get diagnosis_id for the selected record
        string getDiagQuery = "SELECT mr.diagnosis_id, d.disease, d.disorder, d.duration_of_pain, d.severity, d.date "
                             "FROM medical_record mr "
                             "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.diagnosis_id "
                             "WHERE mr.record_id = " + to_string(recordId) + " AND mr.patient_id = " + to_string(patientId);
        
        sql::ResultSet* diagRes = db->executeSelect(getDiagQuery);
        
        if (!diagRes || !diagRes->next()) {
            cout << "\n[ERROR] Record ID " << recordId << " not found for this patient!" << endl;
            if (diagRes) delete diagRes;
            pressEnterToContinue();
            return;
        }

        int diagnosisId = diagRes->getInt("diagnosis_id");
        string currentDisease = diagRes->isNull("disease") ? "" : diagRes->getString("disease");
        string currentDisorder = diagRes->isNull("disorder") ? "" : diagRes->getString("disorder");
        string currentDuration = diagRes->isNull("duration_of_pain") ? "" : diagRes->getString("duration_of_pain");
        string currentSeverity = diagRes->isNull("severity") ? "" : diagRes->getString("severity");
        string currentDate = diagRes->isNull("date") ? "" : diagRes->getString("date");
        delete diagRes;

        if (diagnosisId <= 0) {
            cout << "\n[ERROR] No diagnosis found for this record!" << endl;
            pressEnterToContinue();
            return;
        }

        // Display current values and allow editing
        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    CURRENT DIAGNOSIS DETAILS                    |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Disease: " << left << setw(50) << (currentDisease.empty() ? "N/A" : currentDisease) << "|" << endl;
        cout << "| Disorder: " << left << setw(49) << (currentDisorder.empty() ? "N/A" : currentDisorder) << "|" << endl;
        cout << "| Duration of Pain: " << left << setw(41) << (currentDuration.empty() ? "N/A" : currentDuration) << "|" << endl;
        cout << "| Severity: " << left << setw(49) << (currentSeverity.empty() ? "N/A" : currentSeverity) << "|" << endl;
        cout << "| Date: " << left << setw(52) << (currentDate.empty() ? "N/A" : currentDate) << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;

        cout << "\nEnter new diagnosis details (press Enter to keep current value):" << endl;
        string disease = getStringInput("Disease: ");
        string disorder = getStringInput("Disorder: ");
        string durationOfPain = getStringInput("Duration of Pain: ");
        string severity = getStringInput("Severity: ");
        string date = getStringInput("Date (YYYY-MM-DD): ");

        string updateQuery = "UPDATE diagnosis SET ";
        vector<string> updates;
        
        if (!disease.empty()) updates.push_back("disease = '" + disease + "'");
        if (!disorder.empty()) updates.push_back("disorder = '" + disorder + "'");
        if (!durationOfPain.empty()) updates.push_back("duration_of_pain = '" + durationOfPain + "'");
        if (!severity.empty()) updates.push_back("severity = '" + severity + "'");
        if (!date.empty()) updates.push_back("date = '" + date + "'");

        if (!updates.empty()) {
            for (size_t i = 0; i < updates.size(); i++) {
                updateQuery += updates[i];
                if (i < updates.size() - 1) updateQuery += ", ";
            }
            updateQuery += " WHERE diagnosis_id = " + to_string(diagnosisId);

            if (db->executeUpdate(updateQuery)) {
                cout << "\n✅ Medical record updated successfully!" << endl;
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    UPDATED MEDICAL RECORD                        |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
                cout << "| Record ID: " << left << setw(48) << recordId << "|" << endl;
                cout << "| Disease: " << left << setw(50) << (disease.empty() ? "Not changed" : disease) << "|" << endl;
                cout << "| Disorder: " << left << setw(49) << (disorder.empty() ? "Not changed" : disorder) << "|" << endl;
                cout << "| Duration of Pain: " << left << setw(41) << (durationOfPain.empty() ? "Not changed" : durationOfPain) << "|" << endl;
                cout << "| Severity: " << left << setw(49) << (severity.empty() ? "Not changed" : severity) << "|" << endl;
                cout << "| Date: " << left << setw(52) << (date.empty() ? "Not changed" : date) << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            } else {
                cout << "\n[ERROR] Failed to update medical record!" << endl;
            }
        } else {
            cout << "\n[ERROR] No fields to update!" << endl;
        }

    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::displayPatientRecordTable(sql::ResultSet* res) {
    // Implementation for displaying patient records in table format
    cout << "\n+-------------+----------------------+----------+--------------+--------------+" << endl;
    cout << "| Patient ID  | Full Name            | Gender   | Date of Birth| Status       |" << endl;
    cout << "+-------------┼----------------------┼----------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << getFormattedId(res, "patient_formatted_id", "patient_id")
             << "| " << setw(20) << res->getString("full_name")
             << "| " << setw(8) << res->getString("gender")
             << "| " << setw(12) << res->getString("date_of_birth")
             << "| " << setw(12) << res->getString("status") << "|" << endl;
    }
    
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;
}

void DoctorModule::displayMedicalRecordTable(sql::ResultSet* res) {
    cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;
    cout << "| Record ID   | Date         | Disease              | Disorder             | Duration     | Severity     |" << endl;
    cout << "+-------------┼--------------┼----------------------┼----------------------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << getFormattedId(res, "record_formatted_id", "record_id")
             << "| " << setw(12) << res->getString("date_of_record")
             << "| " << setw(20) << (res->isNull("disease") ? "N/A" : res->getString("disease"))
             << "| " << setw(20) << (res->isNull("disorder") ? "N/A" : res->getString("disorder"))
             << "| " << setw(12) << (res->isNull("duration_of_pain") ? "N/A" : res->getString("duration_of_pain"))
             << "| " << setw(12) << (res->isNull("severity") ? "N/A" : res->getString("severity")) << "|" << endl;
    }
    
    cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;
}

void DoctorModule::displayTableHeader(const string& title) {
    // Blue theme header matching new GUI style
    const int SEPARATOR_LENGTH = 80;
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Centered title with white text on blue background
    MenuNavigator::displayTitle(title, SEPARATOR_LENGTH);
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
}

void DoctorModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int DoctorModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

string DoctorModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}




#include "Login.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Login::Login(Database* database) : db(database), currentUserId(""), currentUserType(""), currentUsername(""), currentRole("") {}

bool Login::authenticate(const string& username, const string& password) {
    try {
        // Query the login table which has username, password, and role
        string query = "SELECT formatted_id, username, role, doctor_id, nurse_id, admin_id FROM login "
                      "WHERE username = '" + username + "' AND password = '" + password + "'";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res && res->next()) {
            currentUsername = res->getString("username");
            currentRole = res->getString("role");
            
            // Determine user type and ID based on role (now using formatted_id strings)
            if (currentRole == "Doctor") {
                currentUserType = "doctor";
                currentUserId = string(res->getString("doctor_id"));
            } else if (currentRole == "Nurse") {
                currentUserType = "nurse";
                currentUserId = string(res->getString("nurse_id"));
            } else if (currentRole == "Admin") {
                currentUserType = "admin";
                currentUserId = string(res->getString("admin_id"));
            }
            
            delete res;
            return !currentUserId.empty();
        }
        
        if (res) delete res;
        return false;
    }
    catch (exception& e) {
        cout << "\n[ERROR] Authentication Error: " << e.what() << endl;
        return false;
    }
}

void Login::showLoginMenu() {
    system("cls");
    
    const int SEPARATOR_LENGTH = 80;
    
    // Top separator line
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Display title (centered, in blue box)
    MenuNavigator::displayTitle("LOGIN", SEPARATOR_LENGTH);
    
    // Separator after title
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl << endl;
    
    string username, password;

    // Enter Username
    ColorUtils::setColor(WHITE);
    cout << "Enter Username: ";
    ColorUtils::resetColor();
    getline(cin, username);
    
    if (username.empty()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n[ERROR] Username cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    // Enter Password
    ColorUtils::setColor(WHITE);
    cout << "Enter Password: ";
    ColorUtils::resetColor();
    getline(cin, password);
    
    if (password.empty()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n[ERROR] Password cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    // Bottom separator
    cout << endl;
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;

    if (authenticate(username, password)) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n✅ Login successful! Welcome, " << currentUsername << "!\n";
        ColorUtils::resetColor();
        cout << "Role: ";
        ColorUtils::setColor(LIGHT_CYAN);
        cout << currentRole << endl;
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    } else {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n[ERROR] Invalid username or password!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    }
}

string Login::getCurrentUserId() {
    return currentUserId;
}

string Login::getCurrentUserType() {
    return currentUserType;
}

string Login::getCurrentUsername() {
    return currentUsername;
}

string Login::getCurrentRole() {
    return currentRole;
}

void Login::logout() {
    currentUserId = "";
    currentUserType = "";
    currentUsername = "";
    currentRole = "";
}

void Login::displayTableHeader(const string& title) {
    // Blue theme header matching new GUI style
    const int SEPARATOR_LENGTH = 80;
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Centered title with white text on blue background
    MenuNavigator::displayTitle(title, SEPARATOR_LENGTH);
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
}

#include <iostream>
#include <windows.h>
#include <iomanip>
#include "Database.h"
#include "Registration.h"
#include "Login.h"
#include "AdminModule.h"
#include "DoctorModule.h"
#include "NurseModule.h"
#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

using namespace std;

int main() {
    // Set console to support UTF-8 characters
    SetConsoleOutputCP(65001);
    
    Database db;
    
    if (!db.connect()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n? Failed to connect to database!" << endl;
        ColorUtils::setColor(WHITE);
        cout << "Please ensure:" << endl;
        cout << "1. XAMPP MySQL is running" << endl;
        cout << "2. Database 'hospital_management_system' exists" << endl;
        cout << "3. Run database_schema.sql to create tables" << endl;
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\nPress Enter to exit...";
        ColorUtils::resetColor();
        cin.get();
        return 1;
    }

    Registration registration(&db);
    Login login(&db);
    AdminModule* adminModule = nullptr;
    DoctorModule* doctorModule = nullptr;
    NurseModule* nurseModule = nullptr;
    Reports reports(&db);

    int mainChoice = -1;
    bool loggedIn = false;

    do {
        if (!loggedIn) {
            vector<string> mainMenuOptions = {
                "Registration",
                "Login",
                "Exit"
            };
            
            int selected = MenuNavigator::showMenu(mainMenuOptions, "HOSPITAL MANAGEMENT SYSTEM", false);
            
            if (selected == -1) {
                mainChoice = 2; // Exit
            } else {
                mainChoice = selected;
            }

            switch (mainChoice) {
            case 0: // Registration
                registration.showPreRegistrationMenu();
                break;
            case 1: // Login
                login.showLoginMenu();
                if (!login.getCurrentUserId().empty()) {  // Check if formatted_id string is not empty
                    loggedIn = true;
                }
                break;
            case 2: // Exit
                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\nThank you for using Hospital Management System!" << endl;
                ColorUtils::resetColor();
                break;
            }
        } else {
            // User is logged in - route to appropriate module based on role
            string role = login.getCurrentRole();
            string userId = login.getCurrentUserId();  // Now returns formatted_id string
            
            if (role == "Admin") {
                if (!adminModule) {
                    adminModule = new AdminModule(&db);
                }
                adminModule->showMenu();
                
                // After admin module returns, logout
                login.logout();
                loggedIn = false;
                delete adminModule;
                adminModule = nullptr;
                
            } else if (role == "Doctor") {
                if (!doctorModule) {
                    doctorModule = new DoctorModule(&db, userId);  // userId is now formatted_id string
                }
                doctorModule->showMenu();
                
                // After doctor module returns, logout
                login.logout();
                loggedIn = false;
                delete doctorModule;
                doctorModule = nullptr;
                
            } else if (role == "Nurse") {
                if (!nurseModule) {
                    nurseModule = new NurseModule(&db, userId);  // userId is now formatted_id string
                }
                nurseModule->showMenu();
                
                // After nurse module returns, logout
                login.logout();
                loggedIn = false;
                delete nurseModule;
                nurseModule = nullptr;
            } else {
                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\n? Unknown role! Logging out..." << endl;
                ColorUtils::resetColor();
                login.logout();
                loggedIn = false;
            }
        }
    } while (mainChoice != 2); // Exit is now index 2

    // Cleanup
    if (adminModule) delete adminModule;
    if (doctorModule) delete doctorModule;
    if (nurseModule) delete nurseModule;

    db.disconnect();
    return 0;
}

#include "NurseModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

NurseModule::NurseModule(Database* database, const string& nurseId) : db(database), currentNurseId(nurseId) {}

void NurseModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Record",
            "Generate Next Appointment",
            "Return to Main Menu"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "NURSE MODULE", true);
        
        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            viewPatientRecord();
            break;
        case 1:
            generateNextAppointment();
            break;
        case 2:
            return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 2);
}

void NurseModule::viewPatientRecord() {
    system("cls");
    displayTableHeader("VIEW PATIENT RECORD");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        // Display patient details
        string patientQuery = "SELECT patient_id, formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status "
                             "FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    PATIENT INFORMATION                         |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient ID: " << left << setw(46) << getFormattedId(patientRes, "formatted_id", "patient_id") << "|" << endl;
        cout << "| Full Name: " << left << setw(46) << patientRes->getString("full_name") << "|" << endl;
        cout << "| Gender: " << left << setw(49) << patientRes->getString("gender") << "|" << endl;
        cout << "| Date of Birth: " << left << setw(42) << patientRes->getString("date_of_birth") << "|" << endl;
        cout << "| Contact Number: " << left << setw(41) << patientRes->getString("contact_number") << "|" << endl;
        cout << "| Blood Type: " << left << setw(45) << (patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type")) << "|" << endl;
        cout << "| Emergency Contact: " << left << setw(39) << (patientRes->isNull("emergency_contact") ? "N/A" : patientRes->getString("emergency_contact")) << "|" << endl;
        cout << "| Status: " << left << setw(49) << patientRes->getString("status") << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        delete patientRes;

        // Display medical records (Nurse can view but not edit)
        string recordQuery = "SELECT mr.record_id, mr.formatted_id as record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes, doc.full_name as doctor_name "
                            "FROM medical_record mr "
                            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.diagnosis_id "
                            "LEFT JOIN doctor doc ON mr.doctor_id = doc.doctor_id "
                            "WHERE mr.patient_id = " + to_string(patientId) + " "
                            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);
        
        if (recordRes && recordRes->rowsCount() > 0) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|                    MEDICAL RECORDS                             |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            
            cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            cout << "| Record ID   | Date         | Disease              | Disorder             | Duration     | Severity     | Doctor               |" << endl;
            cout << "+-------------┼--------------┼----------------------┼----------------------┼--------------┼--------------┼----------------------+" << endl;
            
            while (recordRes->next()) {
                cout << "| " << setw(11) << getFormattedId(recordRes, "record_formatted_id", "record_id")
                     << "| " << setw(12) << recordRes->getString("date_of_record")
                     << "| " << setw(20) << (recordRes->isNull("disease") ? "N/A" : recordRes->getString("disease"))
                     << "| " << setw(20) << (recordRes->isNull("disorder") ? "N/A" : recordRes->getString("disorder"))
                     << "| " << setw(12) << (recordRes->isNull("duration_of_pain") ? "N/A" : recordRes->getString("duration_of_pain"))
                     << "| " << setw(12) << (recordRes->isNull("severity") ? "N/A" : recordRes->getString("severity"))
                     << "| " << setw(20) << (recordRes->isNull("doctor_name") ? "N/A" : recordRes->getString("doctor_name")) << "|" << endl;
            }
            cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
        } else {
            cout << "\n⚠️  No medical records found for this patient." << endl;
        }
        if (recordRes) delete recordRes;

        // Display appointments
        string appointmentQuery = "SELECT a.appointment_id, a.formatted_id as appointment_formatted_id, a.appointment_date, a.appointment_time, a.status, p.full_name as patient_name "
                                 "FROM appointment a "
                                 "JOIN patient p ON a.patient_id = p.patient_id "
                                 "WHERE a.patient_id = " + to_string(patientId) + " "
                                 "ORDER BY a.appointment_date DESC";

        sql::ResultSet* appointmentRes = db->executeSelect(appointmentQuery);
        
        if (appointmentRes && appointmentRes->rowsCount() > 0) {
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|                    APPOINTMENTS                                 |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            displayAppointmentTable(appointmentRes);
        } else {
            cout << "\n⚠️  No appointments found for this patient." << endl;
        }
        if (appointmentRes) delete appointmentRes;

    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void NurseModule::generateNextAppointment() {
    system("cls");
    displayTableHeader("GENERATE NEXT APPOINTMENT");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify patient exists
    string checkQuery = "SELECT full_name, status FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    
    string patientName = checkRes->getString("full_name");
    string patientStatus = checkRes->getString("status");
    delete checkRes;

    // Get available doctors
    string doctorQuery = "SELECT doctor_id, formatted_id, full_name, specialization FROM doctor WHERE status = 'Active' ORDER BY doctor_id";
    sql::ResultSet* doctorRes = db->executeSelect(doctorQuery);
    
    if (!doctorRes || doctorRes->rowsCount() == 0) {
        cout << "\n[ERROR] No active doctors available!" << endl;
        if (doctorRes) delete doctorRes;
        pressEnterToContinue();
        return;
    }

    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|                    PATIENT INFORMATION                         |" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << "\nAvailable Doctors:\n" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    cout << "| Doctor ID | Full Name            | Specialization       |" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    while (doctorRes->next()) {
        cout << "| " << setw(9) << getFormattedId(doctorRes, "formatted_id", "doctor_id")
             << "| " << setw(20) << doctorRes->getString("full_name")
             << "| " << setw(20) << doctorRes->getString("specialization") << "|" << endl;
    }
    cout << "+-----------+----------------------+----------------------+" << endl;
    delete doctorRes;

    int doctorId = getIntInput("\nEnter Doctor ID: ");
    if (doctorId <= 0) {
        cout << "\n[ERROR] Invalid Doctor ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify doctor exists
    string verifyDoctorQuery = "SELECT doctor_id FROM doctor WHERE doctor_id = " + to_string(doctorId) + " AND status = 'Active'";
    sql::ResultSet* verifyDoctorRes = db->executeSelect(verifyDoctorQuery);
    if (!verifyDoctorRes || !verifyDoctorRes->next()) {
        cout << "\n[ERROR] Doctor not found or not active!" << endl;
        if (verifyDoctorRes) delete verifyDoctorRes;
        pressEnterToContinue();
        return;
    }
    delete verifyDoctorRes;

    string appointmentDate = getStringInput("Enter Appointment Date (YYYY-MM-DD): ");
    if (appointmentDate.empty()) {
        cout << "\n[ERROR] Appointment date cannot be empty!" << endl;
        pressEnterToContinue();
        return;
    }

    string appointmentTime = getStringInput("Enter Appointment Time (HH:MM:SS): ");
    if (appointmentTime.empty()) {
        appointmentTime = "09:00:00"; // Default time
    }

    try {
        string query = "INSERT INTO appointment (patient_id, nurse_id, doctor_id, appointment_date, appointment_time, status) "
            "VALUES (" + to_string(patientId) + ", " + to_string(currentNurseId) + ", " + to_string(doctorId) + ", '" + appointmentDate + "', '" + appointmentTime + "', 'Scheduled')";

        if (db->executeUpdate(query)) {
            // Update patient status: If next appointment exists -> Active, else Inactive
            string updateQuery = "UPDATE patient SET status = 'Active' WHERE patient_id = " + to_string(patientId);
            db->executeUpdate(updateQuery);
            
            // Get doctor name for display
            string getDoctorQuery = "SELECT full_name FROM doctor WHERE doctor_id = " + to_string(doctorId);
            sql::ResultSet* doctorRes = db->executeSelect(getDoctorQuery);
            string doctorName = "N/A";
            if (doctorRes && doctorRes->next()) {
                doctorName = doctorRes->getString("full_name");
            }
            if (doctorRes) delete doctorRes;
            
            // Show how many times patient has visited
            string visitQuery = "SELECT COUNT(*) as visit_count FROM appointment WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* visitRes = db->executeSelect(visitQuery);
            int visitCount = 0;
            if (visitRes && visitRes->next()) {
                visitCount = visitRes->getInt("visit_count");
            }
            if (visitRes) delete visitRes;
            
            cout << "\n✅ Next appointment generated successfully!" << endl;
            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|                    APPOINTMENT DETAILS                           |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
            cout << "| Doctor Name: " << left << setw(45) << doctorName << "|" << endl;
            cout << "| Appointment Date: " << left << setw(41) << appointmentDate << "|" << endl;
            cout << "| Appointment Time: " << left << setw(41) << appointmentTime << "|" << endl;
            cout << "| Patient Status: " << left << setw(43) << "Active" << "|" << endl;
            cout << "| Total Visits: " << left << setw(45) << visitCount << "|" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
        } else {
            cout << "\n[ERROR] Failed to generate appointment!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void NurseModule::displayPatientRecordTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+----------+--------------+--------------+" << endl;
    cout << "| Patient ID  | Full Name            | Gender   | Date of Birth| Status       |" << endl;
    cout << "+-------------┼----------------------┼----------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << getFormattedId(res, "patient_formatted_id", "patient_id")
             << "| " << setw(20) << res->getString("full_name")
             << "| " << setw(8) << res->getString("gender")
             << "| " << setw(12) << res->getString("date_of_birth")
             << "| " << setw(12) << res->getString("status") << "|" << endl;
    }
    
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;
}

void NurseModule::displayAppointmentTable(sql::ResultSet* res) {
    cout << "\n+-----------------+----------------------+--------------+--------------+--------------+" << endl;
    cout << "| Appointment ID  | Patient Name          | Date         | Time         | Status       |" << endl;
    cout << "+-----------------┼----------------------┼--------------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(15) << getFormattedId(res, "appointment_formatted_id", "appointment_id")
             << "| " << setw(20) << res->getString("patient_name")
             << "| " << setw(12) << res->getString("appointment_date")
             << "| " << setw(12) << res->getString("appointment_time")
             << "| " << setw(12) << res->getString("status") << "|" << endl;
    }
    
    cout << "+-----------------+----------------------+--------------+--------------+--------------+" << endl;
}

void NurseModule::displayTableHeader(const string& title) {
    // Blue theme header matching new GUI style
    const int SEPARATOR_LENGTH = 80;
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Centered title with white text on blue background
    MenuNavigator::displayTitle(title, SEPARATOR_LENGTH);
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
}

void NurseModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int NurseModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

string NurseModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}




#include "Registration.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

Registration::Registration(Database* database) : db(database) {}

void Registration::showPreRegistrationMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "Doctor",
            "Nurse",
            "Admin",
            "Back to Main Menu"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "PRE-REGISTRATION", true);
        
        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            registerDoctor();
            break;
        case 1:
            registerNurse();
            break;
        case 2:
            registerAdmin();
            break;
        case 3:
            return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 3);
}

void Registration::registerDoctor() {
    string fullName, gender, specialization, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("DOCTOR REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Gender
            while (!validInput) {
                cout << "Enter Gender (Male/Female or M/F): ";
                getline(cin, gender);
                if (gender == "0") {
                    return;
                }
                if (gender.empty()) {
                    cout << "\n[ERROR] Gender cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    string correctedGender = validateAndCorrectGender(gender);
                    if (correctedGender != "Male" && correctedGender != "Female") {
                        cout << "\n[ERROR] Gender must be Male or Female only! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                    } else {
                        gender = correctedGender; // Use corrected gender
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                } else {
                    // Check if IC number already exists in doctor table
                    string checkICQuery = "SELECT formatted_id, full_name FROM doctor WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        cout << "\n⚠️  Doctor already exists with this IC number!" << endl;
                        cout << "Doctor ID: " << string(checkICRes->getString("formatted_id")) << endl;
                        cout << "Doctor Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Specialization
            while (!validInput) {
                cout << "Enter Specialization: ";
                getline(cin, specialization);
                if (specialization == "0") {
                    return;
                }
                if (specialization.empty()) {
                    cout << "\n[ERROR] Specialization cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists in login table
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Specialization: " << specialization << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Specialization: " << specialization << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into doctor table (formatted_id is auto-generated by trigger)
            string query = "INSERT INTO doctor (formatted_id, full_name, gender, specialization, contact_number, ic_number, availability, status, role) "
                "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + specialization + "', '" + contactNumber + "', '" + icNumber + "', 'Available', 'Active', 'Doctor')";

            if (db->executeUpdate(query)) {
                // Get the formatted_id that was just inserted
                string getIdQuery = "SELECT formatted_id FROM doctor WHERE full_name = '" + fullName + "' AND contact_number = '" + contactNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string doctorId = "";
                if (idRes && idRes->next()) {
                    doctorId = string(idRes->getString("formatted_id"));
                }
                if (idRes) delete idRes;

                if (!doctorId.empty()) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, doctor_id) "
                        "VALUES (NULL, '" + username + "', '" + password1 + "', 'Doctor', '" + doctorId + "')";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Doctor registered successfully!" << endl;
                        
                        // Display registered doctor in table format
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED DOCTOR DETAILS                  |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Specialization: " << left << setw(41) << specialization << "|" << endl;
                        cout << "| Contact Number: " << left << setw(41) << contactNumber << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Doctor created but login failed! Please contact admin." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve doctor ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register doctor!" << endl;
                pressEnterToContinue();
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }

    pressEnterToContinue();
}

void Registration::registerNurse() {
    string fullName, gender, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("NURSE REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Gender
            while (!validInput) {
                cout << "Enter Gender (Male/Female or M/F): ";
                getline(cin, gender);
                if (gender == "0") {
                    return;
                }
                if (gender.empty()) {
                    cout << "\n[ERROR] Gender cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    string correctedGender = validateAndCorrectGender(gender);
                    if (correctedGender != "Male" && correctedGender != "Female") {
                        cout << "\n[ERROR] Gender must be Male or Female only! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                    } else {
                        gender = correctedGender; // Use corrected gender
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                } else {
                    // Check if IC number already exists in nurse table
                    string checkICQuery = "SELECT formatted_id, full_name FROM nurse WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        cout << "\n⚠️  Nurse already exists with this IC number!" << endl;
                        cout << "Nurse ID: " << string(checkICRes->getString("formatted_id")) << endl;
                        cout << "Nurse Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into nurse table (formatted_id is auto-generated by trigger)
            string query = "INSERT INTO nurse (formatted_id, full_name, gender, contact_number, ic_number, status, role) "
                "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Nurse')";

            if (db->executeUpdate(query)) {
                // Get the formatted_id
                string getIdQuery = "SELECT formatted_id FROM nurse WHERE full_name = '" + fullName + "' AND contact_number = '" + contactNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string nurseId = "";
                if (idRes && idRes->next()) {
                    nurseId = string(idRes->getString("formatted_id"));
                }
                if (idRes) delete idRes;

                if (!nurseId.empty()) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, nurse_id) "
                        "VALUES (NULL, '" + username + "', '" + password1 + "', 'Nurse', '" + nurseId + "')";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Nurse registered successfully!" << endl;
                        
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED NURSE DETAILS                   |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Contact Number: " << left << setw(41) << contactNumber << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Nurse created but login failed! Please contact admin." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve nurse ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register nurse!" << endl;
                pressEnterToContinue();
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }

    pressEnterToContinue();
}

void Registration::registerAdmin() {
    string fullName, email, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("ADMIN REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Email
            while (!validInput) {
                cout << "Enter Email: ";
                getline(cin, email);
                if (email == "0") {
                    return;
                }
                if (email.empty() || email.find('@') == string::npos) {
                    cout << "\n[ERROR] Invalid email format! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                } else {
                    // Check if IC number already exists in admin table
                    string checkICQuery = "SELECT formatted_id, full_name FROM admin WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        cout << "\n⚠️  Admin already exists with this IC number!" << endl;
                        cout << "Admin ID: " << getFormattedId(checkICRes, "formatted_id", "admin_id") << endl;
                        cout << "Admin Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("ADMIN REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Email: " << email << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("ADMIN REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Email: " << email << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into admin table
            string query = "INSERT INTO admin (full_name, email, contact_number, ic_number, status, role) "
                "VALUES ('" + fullName + "', '" + email + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Admin')";

            if (db->executeUpdate(query)) {
                // Get the admin_id
                string getIdQuery = "SELECT admin_id FROM admin WHERE email = '" + email + "' ORDER BY admin_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                int adminId = -1;
                if (idRes && idRes->next()) {
                    adminId = idRes->getInt("admin_id");
                }
                if (idRes) delete idRes;

                if (adminId > 0) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (username, password, role, admin_id) "
                        "VALUES ('" + username + "', '" + password1 + "', 'Admin', " + to_string(adminId) + ")";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Admin registered successfully!" << endl;
                        
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED ADMIN DETAILS                     |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Email: " << left << setw(50) << email << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Admin created but login failed! Please contact system administrator." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve admin ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register admin!" << endl;
                pressEnterToContinue();
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }

    pressEnterToContinue();
}

bool Registration::validatePassword(const string& password1, const string& password2) {
    return password1 == password2;
}

bool Registration::validateContactNumber(const string& contactNumber) {
    if (contactNumber.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (char c : contactNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    // Check if length is 10 or 11 digits
    if (contactNumber.length() != 10U && contactNumber.length() != 11U) {
        return false;
    }
    return true;
}

bool Registration::validateFullName(const string& fullName) {
    if (fullName.empty()) {
        return false;
    }
    // Check if all characters are letters or spaces
    for (char c : fullName) {
        if (!isalpha(c) && c != ' ' && c != '-') {
            return false;
        }
    }
    // Check if name has at least one letter
    bool hasLetter = false;
    for (char c : fullName) {
        if (isalpha(c)) {
            hasLetter = true;
            break;
        }
    }
    return hasLetter;
}

bool Registration::validateICNumber(const string& icNumber) {
    if (icNumber.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (char c : icNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    // Check if length is exactly 12 digits
    if (icNumber.length() != 12U) {
        return false;
    }
    return true;
}

string Registration::validateAndCorrectGender(string& gender) {
    // Convert to lowercase for comparison
    string genderLower = gender;
    for (char& c : genderLower) {
        c = tolower(c);
    }
    
    // Auto-correct common inputs
    if (genderLower == "m" || genderLower == "male") {
        return "Male";
    } else if (genderLower == "f" || genderLower == "female") {
        return "Female";
    } else {
        // Return original if not valid
        return gender;
    }
}

void Registration::displayTableHeader(const string& title) {
    // Blue theme header matching new GUI style
    const int SEPARATOR_LENGTH = 80;
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Centered title with white text on blue background
    MenuNavigator::displayTitle(title, SEPARATOR_LENGTH);
    
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
}

void Registration::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

string Registration::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

#include "Reports.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Reports::Reports(Database* database) : db(database) {}

void Reports::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "Total Inventory Value",
            "Category Statistics",
            "Monthly Sales Analysis",
            "Profit Margin Analysis",
            "Patient Statistics",
            "Prescription Statistics",
            "Inventory Report",
            "Patient Report",
            "Prescription Report",
            "Financial Report",
            "Low Stock Report",
            "Expiring Items Report",
            "Inventory Value Chart",
            "Category Distribution Chart",
            "Monthly Transaction Chart",
            "Patient Status Chart",
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
        case 16: return;
        default:
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    
    pressEnterToContinue();
}

// Report Generation: Patient Report
void Reports::generatePatientReport() {
    system("cls");
    displayTableHeader("COMPREHENSIVE PATIENT REPORT");
    
    try {
        string query = "SELECT p.patient_id, p.full_name, p.contact_number, p.status, "
                      "p.created_at, "
                      "COUNT(DISTINCT pr.prescription_id) as prescription_count, "
                      "COUNT(DISTINCT mr.record_id) as report_count "
                      "FROM patient p "
                      "LEFT JOIN medical_record mr ON mr.patient_id = p.patient_id "
                      "LEFT JOIN diagnosis d ON d.diagnosis_id = mr.diagnosis_id "
                      "LEFT JOIN prescription pr ON pr.prescription_id = d.prescription_id "
                      "GROUP BY p.patient_id, p.full_name, p.contact_number, p.status, p.created_at "
                      "ORDER BY p.patient_id";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            cout << "\n+-------------+----------------------+----------------------+--------------+----------------------+----------------------+--------------+" << endl;
            cout << "| Patient ID  | Full Name            | Contact Number       | Status       | Registration Date    | Prescription Count   | Report Count |" << endl;
            cout << "+-------------+----------------------+----------------------+--------------+----------------------+----------------------+--------------+" << endl;
            
            while (res->next()) {
                string regDate = res->isNull("created_at") ? "N/A" : res->getString("created_at").substr(0, 10);
                cout << "| " << setw(11) << res->getInt("patient_id")
                     << "| " << setw(20) << left << res->getString("full_name")
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
        cout << "\n[ERROR] Error: " << e.what() << endl;
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
    // Blue theme header with Yellow text on Blue background
    ColorUtils::setColor(BLUE);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title: Yellow text on Blue background
    ColorUtils::setColor(WHITE);
    cout << "|";
    int padding = (60 - title.length()) / 2;
    for (int i = 0; i < padding; i++) cout << " ";
    ColorUtils::printColoredBG(title, YELLOW, BLUE);
    for (int i = 0; i < (60 - title.length() - padding); i++) cout << " ";
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


#include "StaffModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

StaffModule::StaffModule(Database* database, int staffId) : db(database), currentStaffId(staffId) {}

void StaffModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Status",
            "Update Patient Status",
            "Update Patient Prescription",
            "Delete Finished Prescriptions",
            "Update Patient Report",
            "Back to Main Menu"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "STAFF MODULE", true);
        
        if (choice == -1) {
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            viewPatientStatus();
            break;
        case 1:
            updatePatientStatus();
            break;
        case 2:
            updatePrescription();
            break;
        case 3:
            deleteFinishedPrescriptions();
            break;
        case 4:
            updatePatientReport();
            break;
        case 5:
            return;
        default:
            ColorUtils::setColor(YELLOW);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 5);
}

void StaffModule::viewPatientStatus() {
    system("cls");
    displayTableHeader("PATIENT STATUS");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. View All Patients                 |" << endl;
    cout << "|  2. View Active Patients               |" << endl;
    cout << "|  3. View Patients In Treatment         |" << endl;
    cout << "|  4. View Discharged Patients           |" << endl;
    cout << "|  5. Search Patient by ID               |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    string query;
    switch (choice) {
    case 1:
        query = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status, created_at FROM patient ORDER BY patient_id";
        break;
    case 2:
        query = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status, created_at FROM patient WHERE status = 'Active' ORDER BY patient_id";
        break;
    case 3:
        // Note: Schema only has 'Active' and 'Inactive' status, so we'll show Inactive as "In Treatment"
        query = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status, created_at FROM patient WHERE status = 'Inactive' ORDER BY patient_id";
        break;
    case 4:
        // Note: Schema doesn't have 'Discharged' status, showing all patients
        query = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status, created_at FROM patient ORDER BY patient_id";
        break;
    case 5:
        {
            int patientId = getIntInput("Enter Patient ID: ");
            if (patientId <= 0) {
                cout << "\n[ERROR] Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }
            query = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status, created_at FROM patient WHERE patient_id = " + to_string(patientId);
        }
        break;
    case 0:
        return;
    default:
        cout << "\n[ERROR] Invalid choice!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        sql::ResultSet* res = db->executeSelect(query);
        if (res) {
            if (res->rowsCount() == 0) {
                cout << "\n⚠️  No patients found!" << endl;
            } else {
                displayPatientTable(res);
            }
            delete res;
        } else {
            cout << "\n[ERROR] Failed to retrieve patient data!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void StaffModule::updatePatientStatus() {
    system("cls");
    displayTableHeader("UPDATE PATIENT STATUS");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Show current status
    string checkQuery = "SELECT full_name, status FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    
    string currentName = checkRes->getString("full_name");
    string currentStatus = checkRes->isNull("status") ? "N/A" : checkRes->getString("status");
    delete checkRes;

    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|                    PATIENT STATUS INFORMATION                  |" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << "| Patient Name: " << left << setw(44) << currentName << "|" << endl;
    cout << "| Current Status: " << left << setw(42) << currentStatus << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    cout << "\nSelect new status:" << endl;
    cout << "1. Active" << endl;
    cout << "2. Inactive" << endl;
    cout << "Enter choice: ";
    
    int statusChoice;
    cin >> statusChoice;
    cin.ignore();

    string newStatus;
    switch (statusChoice) {
    case 1:
        newStatus = "Active";
        break;
    case 2:
        newStatus = "Inactive";
        break;
    default:
        cout << "\n[ERROR] Invalid choice!" << endl;
        pressEnterToContinue();
        return;
    }

    string query = "UPDATE patient SET status = '" + newStatus + "' WHERE patient_id = " + to_string(patientId);

    if (db->executeUpdate(query)) {
        cout << "\n✅ Patient status updated successfully!" << endl;
        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    STATUS UPDATE DETAILS                       |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient Name: " << left << setw(44) << currentName << "|" << endl;
        cout << "| Previous Status: " << left << setw(41) << currentStatus << "|" << endl;
        cout << "| New Status: " << left << setw(46) << newStatus << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
    } else {
        cout << "\n[ERROR] Failed to update patient status!" << endl;
    }

    pressEnterToContinue();
}

void StaffModule::updatePrescription() {
    system("cls");
    displayTableHeader("PRESCRIPTION MANAGEMENT");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Add New Prescription              |" << endl;
    cout << "|  2. View All Prescriptions            |" << endl;
    cout << "|  3. Update Existing Prescription      |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        {
            system("cls");
            displayTableHeader("ADD NEW PRESCRIPTION");
            
            int patientId = getIntInput("Enter Patient ID: ");
            if (patientId <= 0) {
                cout << "\n[ERROR] Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify patient exists
            string checkQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            if (!checkRes || !checkRes->next()) {
                cout << "\n[ERROR] Patient not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            string patientName = checkRes->getString("full_name");
            delete checkRes;

            // First, show available medications from pharmacy
            string pharmacyQuery = "SELECT pharmacy_id, medicine_name, category_of_meds FROM pharmacy ORDER BY medicine_name";
            sql::ResultSet* pharmacyRes = db->executeSelect(pharmacyQuery);
            if (pharmacyRes && pharmacyRes->rowsCount() > 0) {
                cout << "\nAvailable Medications:\n" << endl;
                cout << "+-------------+----------------------+----------------------+" << endl;
                cout << "| Pharmacy ID | Medicine Name        | Category             |" << endl;
                cout << "+-------------+----------------------+----------------------+" << endl;
                while (pharmacyRes->next()) {
                    cout << "| " << setw(11) << pharmacyRes->getInt("pharmacy_id")
                         << "| " << setw(20) << pharmacyRes->getString("medicine_name")
                         << "| " << setw(20) << pharmacyRes->getString("category_of_meds") << "|" << endl;
                }
                cout << "+-------------+----------------------+----------------------+" << endl;
            }
            if (pharmacyRes) delete pharmacyRes;
            
            int pharmacyId = getIntInput("\nEnter Pharmacy ID (Medication ID): ");
            if (pharmacyId <= 0) {
                cout << "\n[ERROR] Invalid Pharmacy ID!" << endl;
                pressEnterToContinue();
                return;
            }
            
            string dosage = getStringInput("Enter Dosage: ");
            string durationOfMeds = getStringInput("Enter Duration of Medications: ");
            string instructions = getStringInput("Enter Instructions: ");
            string date = getStringInput("Enter Date (YYYY-MM-DD, or press Enter for today): ");
            if (date.empty()) {
                date = "CURDATE()";
            }

            // Get medication name for display
            string getMedQuery = "SELECT medicine_name FROM pharmacy WHERE pharmacy_id = " + to_string(pharmacyId);
            sql::ResultSet* medRes = db->executeSelect(getMedQuery);
            string medicineName = "N/A";
            if (medRes && medRes->next()) {
                medicineName = medRes->getString("medicine_name");
            }
            if (medRes) delete medRes;
            
            string query = "INSERT INTO prescription (pharmacy_id, dosage, duration_of_meds, instructions, date) "
                "VALUES (" + to_string(pharmacyId) + ", '" + dosage + "', '" + durationOfMeds + "', '" + instructions + "', " + (date == "CURDATE()" ? date : "'" + date + "'") + ")";

            if (db->executeUpdate(query)) {
                cout << "\n✅ Prescription added successfully!" << endl;
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    PRESCRIPTION DETAILS                         |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
                cout << "| Medication: " << left << setw(46) << medicineName << "|" << endl;
                cout << "| Dosage: " << left << setw(51) << (dosage.empty() ? "N/A" : dosage) << "|" << endl;
                cout << "| Duration: " << left << setw(49) << (durationOfMeds.empty() ? "N/A" : durationOfMeds) << "|" << endl;
                cout << "| Instructions: " << left << setw(45) << (instructions.empty() ? "N/A" : (instructions.length() > 45 ? instructions.substr(0, 42) + "..." : instructions)) << "|" << endl;
                cout << "| Date: " << left << setw(52) << (date == "CURDATE()" ? "Today" : date) << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            } else {
                cout << "\n[ERROR] Failed to add prescription!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 2:
        {
            system("cls");
            displayTableHeader("ALL PRESCRIPTIONS");
            
            // Join through medical_record -> diagnosis -> prescription to get patient info
            string query = "SELECT p.prescription_id, pt.full_name as patient_name, ph.medicine_name, p.dosage, p.duration_of_meds, p.instructions, p.date "
                "FROM prescription p "
                "JOIN pharmacy ph ON p.pharmacy_id = ph.pharmacy_id "
                "LEFT JOIN diagnosis d ON p.prescription_id = d.prescription_id "
                "LEFT JOIN medical_record mr ON d.diagnosis_id = mr.diagnosis_id "
                "LEFT JOIN patient pt ON mr.patient_id = pt.patient_id "
                "ORDER BY p.prescription_id DESC";
            
            sql::ResultSet* res = db->executeSelect(query);
            if (res) {
                if (res->rowsCount() == 0) {
                    cout << "\n⚠️  No prescriptions found!" << endl;
                } else {
                    displayPrescriptionTable(res);
                }
                delete res;
            }
            pressEnterToContinue();
        }
        break;
    case 3:
        {
            system("cls");
            displayTableHeader("UPDATE PRESCRIPTION");
            
            int prescriptionId = getIntInput("Enter Prescription ID: ");
            if (prescriptionId <= 0) {
                cout << "\n[ERROR] Invalid Prescription ID!" << endl;
                pressEnterToContinue();
                return;
            }

            cout << "\nEnter new details (press Enter to keep current value):" << endl;
            int pharmacyId = getIntInput("Pharmacy ID (Medication ID): ");
            string dosage = getStringInput("Dosage: ");
            string durationOfMeds = getStringInput("Duration of Medications: ");
            string instructions = getStringInput("Instructions: ");
            string date = getStringInput("Date (YYYY-MM-DD): ");

            string query = "UPDATE prescription SET ";
            vector<string> updates;
            
            if (pharmacyId > 0) updates.push_back("pharmacy_id = " + to_string(pharmacyId));
            if (!dosage.empty()) updates.push_back("dosage = '" + dosage + "'");
            if (!durationOfMeds.empty()) updates.push_back("duration_of_meds = '" + durationOfMeds + "'");
            if (!instructions.empty()) updates.push_back("instructions = '" + instructions + "'");
            if (!date.empty()) updates.push_back("date = '" + date + "'");

            if (updates.empty()) {
                cout << "\n[ERROR] No fields to update!" << endl;
                pressEnterToContinue();
                return;
            }

            for (size_t i = 0; i < updates.size(); i++) {
                query += updates[i];
                if (i < updates.size() - 1) query += ", ";
            }
            query += " WHERE prescription_id = " + to_string(prescriptionId);

            if (db->executeUpdate(query)) {
                cout << "\n✅ Prescription updated successfully!" << endl;
            } else {
                cout << "\n[ERROR] Failed to update prescription!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 0:
        return;
    default:
        cout << "\n[ERROR] Invalid choice!" << endl;
        pressEnterToContinue();
    }
}

void StaffModule::deleteFinishedPrescriptions() {
    system("cls");
    displayTableHeader("DELETE FINISHED PRESCRIPTIONS");

    // Note: Schema doesn't have status field in prescription, so we'll show all prescriptions
    // Join through medical_record -> diagnosis -> prescription to get patient info
    string query = "SELECT p.prescription_id, pt.full_name as patient_name, ph.medicine_name, p.dosage, p.date "
        "FROM prescription p "
        "JOIN pharmacy ph ON p.pharmacy_id = ph.pharmacy_id "
        "LEFT JOIN diagnosis d ON p.prescription_id = d.prescription_id "
        "LEFT JOIN medical_record mr ON d.diagnosis_id = mr.diagnosis_id "
        "LEFT JOIN patient pt ON mr.patient_id = pt.patient_id "
        "ORDER BY p.prescription_id";
    
    sql::ResultSet* res = db->executeSelect(query);
    
    if (!res || res->rowsCount() == 0) {
        cout << "\n⚠️  No finished prescriptions found!" << endl;
        if (res) delete res;
        pressEnterToContinue();
        return;
    }

    cout << "\nFinished Prescriptions:" << endl;
    displayPrescriptionTable(res);
    delete res;

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Delete All Finished Prescriptions |" << endl;
    cout << "|  2. Delete Specific Prescription      |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        {
            cout << "\n⚠️  Are you sure you want to delete ALL prescriptions older than 1 year? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            
            if (confirm == "yes" || confirm == "YES") {
                // Delete prescriptions older than 1 year (schema doesn't have status field)
                string deleteQuery = "DELETE FROM prescription WHERE date < DATE_SUB(CURDATE(), INTERVAL 1 YEAR)";
                if (db->executeUpdate(deleteQuery)) {
                    cout << "\n✅ Old prescriptions deleted successfully!" << endl;
                } else {
                    cout << "\n[ERROR] Failed to delete prescriptions!" << endl;
                }
            } else {
                cout << "Deletion cancelled." << endl;
            }
        }
        break;
    case 2:
        {
            int prescriptionId = getIntInput("Enter Prescription ID to delete: ");
            if (prescriptionId <= 0) {
                cout << "\n[ERROR] Invalid Prescription ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify prescription exists
            string checkQuery = "SELECT prescription_id FROM prescription WHERE prescription_id = " + to_string(prescriptionId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            
            if (!checkRes || !checkRes->next()) {
                cout << "\n[ERROR] Prescription not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            delete checkRes;

            cout << "\n⚠️  Are you sure? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            
            if (confirm == "yes" || confirm == "YES") {
                string deleteQuery = "DELETE FROM prescription WHERE prescription_id = " + to_string(prescriptionId);
                if (db->executeUpdate(deleteQuery)) {
                    cout << "\n✅ Prescription deleted successfully!" << endl;
                } else {
                    cout << "\n[ERROR] Failed to delete prescription!" << endl;
                }
            } else {
                cout << "Deletion cancelled." << endl;
            }
        }
        break;
    case 0:
        return;
    default:
        cout << "\n[ERROR] Invalid choice!" << endl;
    }

    pressEnterToContinue();
}

void StaffModule::updatePatientReport() {
    system("cls");
    displayTableHeader("PATIENT REPORT MANAGEMENT");

    int choice;
    cout << "\n+----------------------------------------+" << endl;
    cout << "|  1. Add New Report                    |" << endl;
    cout << "|  2. View All Reports                  |" << endl;
    cout << "|  3. Update Existing Report            |" << endl;
    cout << "|  0. Back                              |" << endl;
    cout << "+----------------------------------------+" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        {
            system("cls");
            displayTableHeader("ADD NEW REPORT");
            
            int patientId = getIntInput("Enter Patient ID: ");
            if (patientId <= 0) {
                cout << "\n[ERROR] Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify patient exists
            string checkQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            if (!checkRes || !checkRes->next()) {
                cout << "\n[ERROR] Patient not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            string patientName = checkRes->getString("full_name");
            delete checkRes;

            cout << "\n+----------------------------------------------------------------+" << endl;
            cout << "|                    PATIENT INFORMATION                         |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
            cout << endl;
            
            string reportType = getStringInput("Report Type: ");
            string reportDate = getStringInput("Report Date (YYYY-MM-DD): ");
            string diagnosis = getStringInput("Diagnosis: ");
            string treatment = getStringInput("Treatment: ");
            string notes = getStringInput("Notes: ");

            if (reportType.empty() || reportDate.empty()) {
                cout << "\n[ERROR] Report type and date are required!" << endl;
                pressEnterToContinue();
                return;
            }

            // Use medical_record table instead (patient_report doesn't exist in schema)
            // Note: medical_record requires doctor_id, so we'll set it to NULL or use a default
            // For staff reports, we'll store the information in notes field
            string reportNotes = "Report Type: " + reportType + " | Diagnosis: " + diagnosis + " | Treatment: " + treatment;
            if (!notes.empty()) {
                reportNotes += " | Notes: " + notes;
            }
            
            // Get first available doctor_id or use NULL (schema allows NULL but NOT NULL constraint exists)
            // Since doctor_id is NOT NULL, we need to get a doctor_id
            string doctorQuery = "SELECT doctor_id FROM doctor WHERE status = 'Active' LIMIT 1";
            sql::ResultSet* doctorRes = db->executeSelect(doctorQuery);
            int doctorId = 1; // Default fallback
            if (doctorRes && doctorRes->next()) {
                doctorId = doctorRes->getInt("doctor_id");
            }
            if (doctorRes) delete doctorRes;
            
            string query = "INSERT INTO medical_record (patient_id, doctor_id, diagnosis_id, date_of_record, notes) "
                "VALUES (" + to_string(patientId) + ", " + to_string(doctorId) + ", NULL, '" + reportDate + "', '" + reportNotes + "')";

            if (db->executeUpdate(query)) {
                cout << "\n✅ Report added successfully!" << endl;
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    REPORT DETAILS                                |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
                cout << "| Report Type: " << left << setw(46) << (reportType.empty() ? "N/A" : reportType) << "|" << endl;
                cout << "| Report Date: " << left << setw(46) << (reportDate.empty() ? "N/A" : reportDate) << "|" << endl;
                cout << "| Diagnosis: " << left << setw(48) << (diagnosis.empty() ? "N/A" : (diagnosis.length() > 48 ? diagnosis.substr(0, 45) + "..." : diagnosis)) << "|" << endl;
                cout << "| Treatment: " << left << setw(48) << (treatment.empty() ? "N/A" : (treatment.length() > 48 ? treatment.substr(0, 45) + "..." : treatment)) << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            } else {
                cout << "\n[ERROR] Failed to add report!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 2:
        {
            system("cls");
            displayTableHeader("ALL PATIENT REPORTS");
            
            // Use medical_record table instead of patient_report
            string query = "SELECT mr.record_id, pt.full_name as patient_name, mr.date_of_record, mr.notes "
                "FROM medical_record mr JOIN patient pt ON mr.patient_id = pt.patient_id ORDER BY mr.record_id DESC";
            
            sql::ResultSet* res = db->executeSelect(query);
            if (res) {
                if (res->rowsCount() == 0) {
                    cout << "\n⚠️  No reports found!" << endl;
                } else {
                    displayReportTable(res);
                }
                delete res;
            }
            pressEnterToContinue();
        }
        break;
    case 3:
        {
            system("cls");
            displayTableHeader("UPDATE REPORT");
            
            int recordId = getIntInput("Enter Record ID: ");
            if (recordId <= 0) {
                cout << "\n[ERROR] Invalid Record ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Get patient name for display
            string getPatientQuery = "SELECT pt.full_name FROM medical_record mr JOIN patient pt ON mr.patient_id = pt.patient_id WHERE mr.record_id = " + to_string(recordId);
            sql::ResultSet* patientRes = db->executeSelect(getPatientQuery);
            string patientName = "N/A";
            if (patientRes && patientRes->next()) {
                patientName = patientRes->getString("full_name");
            }
            if (patientRes) delete patientRes;

            cout << "\nEnter new details (press Enter to keep current value):" << endl;
            string reportType = getStringInput("Report Type: ");
            string reportDate = getStringInput("Report Date (YYYY-MM-DD): ");
            string diagnosis = getStringInput("Diagnosis: ");
            string treatment = getStringInput("Treatment: ");
            string notes = getStringInput("Notes: ");

            // Use medical_record table instead of patient_report
            string query = "UPDATE medical_record SET ";
            vector<string> updates;
            
            if (!reportDate.empty()) updates.push_back("date_of_record = '" + reportDate + "'");
            // Combine all report info into notes field
            if (!reportType.empty() || !diagnosis.empty() || !treatment.empty() || !notes.empty()) {
                string reportNotes = "Report Type: " + reportType + " | Diagnosis: " + diagnosis + " | Treatment: " + treatment;
                if (!notes.empty()) {
                    reportNotes += " | Notes: " + notes;
                }
                updates.push_back("notes = '" + reportNotes + "'");
            }

            if (updates.empty()) {
                cout << "\n[ERROR] No fields to update!" << endl;
                pressEnterToContinue();
                return;
            }

            for (size_t i = 0; i < updates.size(); i++) {
                query += updates[i];
                if (i < updates.size() - 1) query += ", ";
            }
            query += " WHERE record_id = " + to_string(recordId);

            if (db->executeUpdate(query)) {
                cout << "\n✅ Report updated successfully!" << endl;
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    UPDATED REPORT DETAILS                        |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient Name: " << left << setw(44) << patientName << "|" << endl;
                cout << "| Record ID: " << left << setw(48) << recordId << "|" << endl;
                cout << "| Report Type: " << left << setw(46) << (reportType.empty() ? "Not changed" : reportType) << "|" << endl;
                cout << "| Report Date: " << left << setw(46) << (reportDate.empty() ? "Not changed" : reportDate) << "|" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            } else {
                cout << "\n[ERROR] Failed to update report!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 0:
        return;
    default:
        cout << "\n[ERROR] Invalid choice!" << endl;
        pressEnterToContinue();
    }
}

void StaffModule::displayPatientTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+--------+--------------+----------------+----------------+--------------+--------------+" << endl;
    cout << "| Patient ID  | Full Name            | Gender | Date of Birth| Contact Number | Blood Type     | Status       | Created At   |" << endl;
    cout << "+-------------+----------------------+--------+--------------+----------------+----------------+--------------+--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << res->getInt("patient_id")
             << "| " << setw(20) << res->getString("full_name")
             << "| " << setw(6) << res->getString("gender")
             << "| " << setw(12) << res->getString("date_of_birth")
             << "| " << setw(14) << res->getString("contact_number")
             << "| " << setw(14) << (res->isNull("blood_type") ? "N/A" : res->getString("blood_type"))
             << "| " << setw(12) << res->getString("status")
             << "| " << setw(12) << res->getString("created_at").substr(0, 10) << "|" << endl;
    }
    
    cout << "+-------------+----------------------+--------+--------------+----------------+----------------+--------------+--------------+" << endl;
}

void StaffModule::displayPrescriptionTable(sql::ResultSet* res) {
    cout << "\n+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+--------------+" << endl;
    cout << "| Prescription ID | Patient Name          | Medication           | Dosage    | Duration of Meds     | Instructions          | Date         |" << endl;
    cout << "+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+--------------+" << endl;
    
    while (res->next()) {
        string patientName = res->isNull("patient_name") ? "N/A" : res->getString("patient_name");
        cout << "| " << setw(15) << res->getInt("prescription_id")
             << "| " << setw(20) << patientName
             << "| " << setw(20) << res->getString("medicine_name")
             << "| " << setw(9) << (res->isNull("dosage") ? "N/A" : res->getString("dosage"))
             << "| " << setw(20) << (res->isNull("duration_of_meds") ? "N/A" : res->getString("duration_of_meds"))
             << "| " << setw(20) << (res->isNull("instructions") ? "N/A" : res->getString("instructions").substr(0, 18))
             << "| " << setw(12) << res->getString("date").substr(0, 10) << "|" << endl;
    }
    
    cout << "+-----------------+----------------------+----------------------+-----------+----------------------+----------------------+--------------+" << endl;
}

void StaffModule::displayReportTable(sql::ResultSet* res) {
    cout << "\n+-----------+----------------------+--------------+----------------------------------------------------------------------+" << endl;
    cout << "| Record ID | Patient Name          | Date         | Notes                                                                 |" << endl;
    cout << "+-----------+----------------------+--------------+----------------------------------------------------------------------+" << endl;
    
    while (res->next()) {
        string notes = res->getString("notes");
        if (notes.length() > 70) notes = notes.substr(0, 67) + "...";

        cout << "| " << setw(9) << res->getInt("record_id")
             << "| " << setw(20) << res->getString("patient_name")
             << "| " << setw(12) << res->getString("date_of_record").substr(0, 10)
             << "| " << setw(70) << notes << "|" << endl;
    }
    
    cout << "+-----------+----------------------+--------------+----------------------------------------------------------------------+" << endl;
}

void StaffModule::displayTableHeader(const string& title) {
    // Blue theme header with Yellow text on Blue background
    ColorUtils::setColor(BLUE);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title: Yellow text on Blue background
    ColorUtils::setColor(WHITE);
    cout << "|";
    int padding = (60 - title.length()) / 2;
    for (int i = 0; i < padding; i++) cout << " ";
    ColorUtils::printColoredBG(title, YELLOW, BLUE);
    for (int i = 0; i < (60 - title.length() - padding); i++) cout << " ";
    ColorUtils::setColor(WHITE);
    cout << "|" << endl;
    
    ColorUtils::setColor(BLUE);
    cout << "|" << setw(60) << "" << "|" << endl;
    cout << "+----------------------------------------------------------------+" << endl;
    ColorUtils::resetColor();
}

void StaffModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int StaffModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

string StaffModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}


