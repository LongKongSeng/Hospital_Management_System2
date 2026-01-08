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
            "Exit"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "ADMIN MENU", true);
        
        // ESC key - stay in menu, don't exit
        if (choice == -1) {
            continue;
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
            return; // Exit only when user explicitly chooses "Exit"
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (true); // Loop forever until user chooses Exit (case 4)
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
            
            // Check if IC number already exists in ANY table (doctor, nurse, admin, patient)
            string checkICQuery = "SELECT 'Doctor' as user_type, formatted_id, full_name FROM doctor WHERE ic_number = '" + icNumber + "' "
                                 "UNION ALL "
                                 "SELECT 'Nurse' as user_type, formatted_id, full_name FROM nurse WHERE ic_number = '" + icNumber + "' "
                                 "UNION ALL "
                                 "SELECT 'Admin' as user_type, formatted_id, full_name FROM admin WHERE ic_number = '" + icNumber + "' "
                                 "UNION ALL "
                                 "SELECT 'Patient' as user_type, formatted_id, full_name FROM patient WHERE ic_number = '" + icNumber + "'";
            sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
            
            if (checkICRes && checkICRes->next()) {
                string userType = checkICRes->getString("user_type");
                cout << "\n⚠️  This IC number is already registered!" << endl;
                cout << "User Type: " << userType << endl;
                cout << userType << " ID: " << string(checkICRes->getString("formatted_id")) << endl;
                cout << userType << " Name: " << checkICRes->getString("full_name") << endl;
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

        string query = "INSERT INTO patient (formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, ic_number, status) "
            "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + dob + "', '" + contactNumber + "', '" + bloodType + "', '" + emergencyContact + "', '" + icNumber + "', 'Active')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Patient added successfully!" << endl;
            
            // Display patient details
            string getQuery = "SELECT formatted_id, full_name, gender, date_of_birth, contact_number, status FROM patient WHERE ic_number = '" + icNumber + "' ORDER BY formatted_id DESC LIMIT 1";
            sql::ResultSet* res = db->executeSelect(getQuery);
            if (res && res->next()) {
                cout << "\n+----------------------------------------------------------------+" << endl;
                cout << "|                    PATIENT DETAILS                             |" << endl;
                cout << "+----------------------------------------------------------------+" << endl;
                cout << "| Patient ID: " << left << setw(44) << string(res->getString("formatted_id")) << "|" << endl;
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

    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    calculatePatientReceipt(patientId);
}

void AdminModule::calculatePatientReceipt(const string& patientId) {
    try {
        // First, get patient details
        string patientQuery = "SELECT formatted_id, full_name, contact_number, status FROM patient WHERE formatted_id = '" + patientId + "'";
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
                      "COUNT(t.formatted_id) as treatment_count "
                      "FROM treatment t "
                      "JOIN medical_record mr ON t.doctor_id = mr.doctor_id "
                      "WHERE mr.patient_id = '" + patientId + "'";

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
        string appointmentQuery = "SELECT COUNT(*) as count FROM appointment WHERE patient_id = '" + patientId + "' AND appointment_date >= CURDATE()";
        sql::ResultSet* apptRes = db->executeSelect(appointmentQuery);
        
        string newStatus = "Inactive";
        if (apptRes && apptRes->next() && apptRes->getInt("count") > 0) {
            newStatus = "Active";
        }
        if (apptRes) delete apptRes;

        // Update patient status
        string updateQuery = "UPDATE patient SET status = '" + newStatus + "' WHERE formatted_id = '" + patientId + "'";
        db->executeUpdate(updateQuery);

    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::displayReceipt(const string& patientId, double totalAmount) {
    system("cls");
    displayTableHeader("PATIENT RECEIPT");

    try {
        // Get patient details
        string patientQuery = "SELECT formatted_id, full_name, contact_number, date_of_birth FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            return;
        }

        string patientFormattedId = string(patientRes->getString("formatted_id"));
        string patientName = patientRes->getString("full_name");
        string contactNumber = patientRes->getString("contact_number");
        string dob = patientRes->isNull("date_of_birth") ? "N/A" : patientRes->getString("date_of_birth");
        delete patientRes;

        // Get treatment details
        string treatmentQuery = "SELECT t.formatted_id as treatment_formatted_id, t.consultation_fee, t.treatment_fee, t.treatment_date, d.full_name as doctor_name "
                              "FROM treatment t "
                              "JOIN medical_record mr ON t.doctor_id = mr.doctor_id "
                              "JOIN doctor d ON t.doctor_id = d.formatted_id "
                              "WHERE mr.patient_id = '" + patientId + "' "
                              "ORDER BY t.treatment_date DESC";

        sql::ResultSet* treatmentRes = db->executeSelect(treatmentQuery);

        // Display receipt
        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    HOSPITAL RECEIPT                            |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
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
                cout << "| " << left << setw(5) << string(treatmentRes->getString("treatment_formatted_id"))
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
