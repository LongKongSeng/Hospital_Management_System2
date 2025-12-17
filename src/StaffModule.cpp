#include "StaffModule.h"
#include "ColorUtils.h"

StaffModule::StaffModule(Database* database, int staffId) : db(database), currentStaffId(staffId) {}

void StaffModule::showMenu() {
    int choice;
    do {
        system("cls");
        displayTableHeader("STAFF MODULE");
        
        // Blue color theme for staff menu
        ColorUtils::setColor(BLUE);
        cout << "\n+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(WHITE);
        cout << "|  ";
        ColorUtils::printColored("1. View Patient Status", BLUE);
        ColorUtils::setColor(WHITE);
        cout << "               |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("2. Update Patient Status", BLUE);
        ColorUtils::setColor(WHITE);
        cout << "              |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("3. Update Patient Prescription", BLUE);
        ColorUtils::setColor(WHITE);
        cout << "        |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("4. Delete Finished Prescriptions", BLUE);
        ColorUtils::setColor(WHITE);
        cout << "      |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("5. Update Patient Report", BLUE);
        ColorUtils::setColor(WHITE);
        cout << "              |" << endl;
        
        cout << "|  ";
        ColorUtils::printColored("0. Back to Main Menu", RED);
        ColorUtils::setColor(WHITE);
        cout << "                 |" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::setColor(BLUE);
        cout << "+----------------------------------------+" << endl;
        ColorUtils::resetColor();
        
        ColorUtils::printColored("\nEnter your choice: ", CYAN);
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            viewPatientStatus();
            break;
        case 2:
            updatePatientStatus();
            break;
        case 3:
            updatePrescription();
            break;
        case 4:
            deleteFinishedPrescriptions();
            break;
        case 5:
            updatePatientReport();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice! Please try again." << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
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
        query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status, registration_date FROM patient ORDER BY patient_id";
        break;
    case 2:
        query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status, registration_date FROM patient WHERE status = 'Active' ORDER BY patient_id";
        break;
    case 3:
        query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status, registration_date FROM patient WHERE status = 'In Treatment' ORDER BY patient_id";
        break;
    case 4:
        query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status, registration_date FROM patient WHERE status = 'Discharged' ORDER BY patient_id";
        break;
    case 5:
        {
            int patientId = getIntInput("Enter Patient ID: ");
            if (patientId <= 0) {
                cout << "\n❌ Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }
            query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status, registration_date FROM patient WHERE patient_id = " + to_string(patientId);
        }
        break;
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice!" << endl;
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
            cout << "\n❌ Failed to retrieve patient data!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void StaffModule::updatePatientStatus() {
    system("cls");
    displayTableHeader("UPDATE PATIENT STATUS");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Show current status
    string checkQuery = "SELECT full_name, status FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n❌ Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    
    string currentName = checkRes->getString("full_name");
    string currentStatus = checkRes->isNull("status") ? "N/A" : checkRes->getString("status");
    delete checkRes;

    cout << "\nPatient: " << currentName << endl;
    cout << "Current Status: " << currentStatus << endl;
    cout << "\nSelect new status:" << endl;
    cout << "1. Active" << endl;
    cout << "2. In Treatment" << endl;
    cout << "3. Discharged" << endl;
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
        newStatus = "In Treatment";
        break;
    case 3:
        newStatus = "Discharged";
        break;
    default:
        cout << "\n❌ Invalid choice!" << endl;
        pressEnterToContinue();
        return;
    }

    string query = "UPDATE patient SET status = '" + newStatus + "' WHERE patient_id = " + to_string(patientId);

    if (db->executeUpdate(query)) {
        cout << "\n✅ Patient status updated successfully!" << endl;
        cout << "Status changed from '" << currentStatus << "' to '" << newStatus << "'" << endl;
    } else {
        cout << "\n❌ Failed to update patient status!" << endl;
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
                cout << "\n❌ Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify patient exists
            string checkQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            if (!checkRes || !checkRes->next()) {
                cout << "\n❌ Patient not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            string patientName = checkRes->getString("full_name");
            delete checkRes;

            cout << "Patient: " << patientName << endl;
            
            string medication = getStringInput("Medication Name: ");
            string dosage = getStringInput("Dosage: ");
            string frequency = getStringInput("Frequency: ");
            int duration = getIntInput("Duration (days): ");
            string instructions = getStringInput("Instructions: ");

            if (medication.empty()) {
                cout << "\n❌ Medication name cannot be empty!" << endl;
                pressEnterToContinue();
                return;
            }

            string query = "INSERT INTO prescription (patient_id, staff_id, medication_name, dosage, frequency, duration, instructions, status) "
                "VALUES (" + to_string(patientId) + ", " + to_string(currentStaffId) + ", '" + medication + "', '" + dosage + "', '" + 
                frequency + "', " + to_string(duration) + ", '" + instructions + "', 'Active')";

            if (db->executeUpdate(query)) {
                cout << "\n✅ Prescription added successfully!" << endl;
            } else {
                cout << "\n❌ Failed to add prescription!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 2:
        {
            system("cls");
            displayTableHeader("ALL PRESCRIPTIONS");
            
            string query = "SELECT p.prescription_id, pt.full_name as patient_name, p.medication_name, p.dosage, p.frequency, p.duration, p.status, p.prescribed_date "
                "FROM prescription p JOIN patient pt ON p.patient_id = pt.patient_id ORDER BY p.prescription_id DESC";
            
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
                cout << "\n❌ Invalid Prescription ID!" << endl;
                pressEnterToContinue();
                return;
            }

            cout << "\nEnter new details (press Enter to keep current value):" << endl;
            string medication = getStringInput("Medication Name: ");
            string dosage = getStringInput("Dosage: ");
            string frequency = getStringInput("Frequency: ");
            string durationStr = getStringInput("Duration (days): ");
            string status = getStringInput("Status (Active/Completed/Cancelled): ");

            string query = "UPDATE prescription SET ";
            vector<string> updates;
            
            if (!medication.empty()) updates.push_back("medication_name = '" + medication + "'");
            if (!dosage.empty()) updates.push_back("dosage = '" + dosage + "'");
            if (!frequency.empty()) updates.push_back("frequency = '" + frequency + "'");
            if (!durationStr.empty()) {
                try {
                    int duration = stoi(durationStr);
                    updates.push_back("duration = " + to_string(duration));
                } catch (...) {
                    cout << "\n❌ Invalid duration!" << endl;
                    pressEnterToContinue();
                    return;
                }
            }
            if (!status.empty()) updates.push_back("status = '" + status + "'");

            if (updates.empty()) {
                cout << "\n❌ No fields to update!" << endl;
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
                cout << "\n❌ Failed to update prescription!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice!" << endl;
        pressEnterToContinue();
    }
}

void StaffModule::deleteFinishedPrescriptions() {
    system("cls");
    displayTableHeader("DELETE FINISHED PRESCRIPTIONS");

    // First show finished prescriptions
    string query = "SELECT p.prescription_id, pt.full_name as patient_name, p.medication_name, p.status, p.prescribed_date "
        "FROM prescription p JOIN patient pt ON p.patient_id = pt.patient_id WHERE p.status = 'Completed' ORDER BY p.prescription_id";
    
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
            cout << "\n⚠️  Are you sure you want to delete ALL finished prescriptions? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            
            if (confirm == "yes" || confirm == "YES") {
                string deleteQuery = "DELETE FROM prescription WHERE status = 'Completed'";
                if (db->executeUpdate(deleteQuery)) {
                    cout << "\n✅ All finished prescriptions deleted successfully!" << endl;
                } else {
                    cout << "\n❌ Failed to delete prescriptions!" << endl;
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
                cout << "\n❌ Invalid Prescription ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify it's a completed prescription
            string checkQuery = "SELECT status FROM prescription WHERE prescription_id = " + to_string(prescriptionId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            
            if (!checkRes || !checkRes->next()) {
                cout << "\n❌ Prescription not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }

            string status = checkRes->getString("status");
            delete checkRes;

            if (status != "Completed") {
                cout << "\n❌ Only completed prescriptions can be deleted!" << endl;
                pressEnterToContinue();
                return;
            }

            cout << "\n⚠️  Are you sure? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            
            if (confirm == "yes" || confirm == "YES") {
                string deleteQuery = "DELETE FROM prescription WHERE prescription_id = " + to_string(prescriptionId);
                if (db->executeUpdate(deleteQuery)) {
                    cout << "\n✅ Prescription deleted successfully!" << endl;
                } else {
                    cout << "\n❌ Failed to delete prescription!" << endl;
                }
            } else {
                cout << "Deletion cancelled." << endl;
            }
        }
        break;
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice!" << endl;
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
                cout << "\n❌ Invalid Patient ID!" << endl;
                pressEnterToContinue();
                return;
            }

            // Verify patient exists
            string checkQuery = "SELECT full_name FROM patient WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* checkRes = db->executeSelect(checkQuery);
            if (!checkRes || !checkRes->next()) {
                cout << "\n❌ Patient not found!" << endl;
                if (checkRes) delete checkRes;
                pressEnterToContinue();
                return;
            }
            string patientName = checkRes->getString("full_name");
            delete checkRes;

            cout << "Patient: " << patientName << endl;
            
            string reportType = getStringInput("Report Type: ");
            string reportDate = getStringInput("Report Date (YYYY-MM-DD): ");
            string diagnosis = getStringInput("Diagnosis: ");
            string treatment = getStringInput("Treatment: ");
            string notes = getStringInput("Notes: ");

            if (reportType.empty() || reportDate.empty()) {
                cout << "\n❌ Report type and date are required!" << endl;
                pressEnterToContinue();
                return;
            }

            string query = "INSERT INTO patient_report (patient_id, staff_id, report_type, report_date, diagnosis, treatment, notes) "
                "VALUES (" + to_string(patientId) + ", " + to_string(currentStaffId) + ", '" + reportType + "', '" + reportDate + "', '" + 
                diagnosis + "', '" + treatment + "', '" + notes + "')";

            if (db->executeUpdate(query)) {
                cout << "\n✅ Report added successfully!" << endl;
            } else {
                cout << "\n❌ Failed to add report!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 2:
        {
            system("cls");
            displayTableHeader("ALL PATIENT REPORTS");
            
            string query = "SELECT r.report_id, pt.full_name as patient_name, r.report_type, r.report_date, r.diagnosis, r.treatment, r.created_at "
                "FROM patient_report r JOIN patient pt ON r.patient_id = pt.patient_id ORDER BY r.report_id DESC";
            
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
            
            int reportId = getIntInput("Enter Report ID: ");
            if (reportId <= 0) {
                cout << "\n❌ Invalid Report ID!" << endl;
                pressEnterToContinue();
                return;
            }

            cout << "\nEnter new details (press Enter to keep current value):" << endl;
            string reportType = getStringInput("Report Type: ");
            string reportDate = getStringInput("Report Date (YYYY-MM-DD): ");
            string diagnosis = getStringInput("Diagnosis: ");
            string treatment = getStringInput("Treatment: ");
            string notes = getStringInput("Notes: ");

            string query = "UPDATE patient_report SET ";
            vector<string> updates;
            
            if (!reportType.empty()) updates.push_back("report_type = '" + reportType + "'");
            if (!reportDate.empty()) updates.push_back("report_date = '" + reportDate + "'");
            if (!diagnosis.empty()) updates.push_back("diagnosis = '" + diagnosis + "'");
            if (!treatment.empty()) updates.push_back("treatment = '" + treatment + "'");
            if (!notes.empty()) updates.push_back("notes = '" + notes + "'");

            if (updates.empty()) {
                cout << "\n❌ No fields to update!" << endl;
                pressEnterToContinue();
                return;
            }

            for (size_t i = 0; i < updates.size(); i++) {
                query += updates[i];
                if (i < updates.size() - 1) query += ", ";
            }
            query += " WHERE report_id = " + to_string(reportId);

            if (db->executeUpdate(query)) {
                cout << "\n✅ Report updated successfully!" << endl;
            } else {
                cout << "\n❌ Failed to update report!" << endl;
            }
            pressEnterToContinue();
        }
        break;
    case 0:
        return;
    default:
        cout << "\n❌ Invalid choice!" << endl;
        pressEnterToContinue();
    }
}

void StaffModule::displayPatientTable(sql::ResultSet* res) {
    cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+--------------+" << endl;
    cout << "| Patient ID  | Username     | Full Name            | Email                | Phone        | Status       | Registration |" << endl;
    cout << "+-------------┼--------------┼----------------------┼----------------------┼--------------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << res->getInt("patient_id")
             << "| " << setw(12) << res->getString("username")
             << "| " << setw(20) << res->getString("full_name")
             << "| " << setw(20) << res->getString("email")
             << "| " << setw(12) << res->getString("phone")
             << "| " << setw(12) << (res->isNull("status") ? "N/A" : res->getString("status"))
             << "| " << setw(12) << (res->isNull("registration_date") ? "N/A" : res->getString("registration_date").substr(0, 10)) << "|" << endl;
    }
    
    cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+--------------+" << endl;
}

void StaffModule::displayPrescriptionTable(sql::ResultSet* res) {
    cout << "\n+-----------------+----------------------+----------------------+-----------+-----------+--------------+--------------+" << endl;
    cout << "| Prescription ID | Patient Name          | Medication           | Dosage    | Frequency | Status       | Prescribed   |" << endl;
    cout << "+-----------------┼----------------------┼----------------------┼-----------┼-----------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(15) << res->getInt("prescription_id")
             << "| " << setw(20) << res->getString("patient_name")
             << "| " << setw(20) << res->getString("medication_name")
             << "| " << setw(9) << res->getString("dosage")
             << "| " << setw(9) << res->getString("frequency")
             << "| " << setw(12) << res->getString("status")
             << "| " << setw(12) << res->getString("prescribed_date").substr(0, 10) << "|" << endl;
    }
    
    cout << "+-----------------+----------------------+----------------------+-----------+-----------+--------------+--------------+" << endl;
}

void StaffModule::displayReportTable(sql::ResultSet* res) {
    cout << "\n+-----------+----------------------+--------------+--------------+----------------------+----------------------+" << endl;
    cout << "| Report ID | Patient Name          | Report Type  | Report Date  | Diagnosis             | Treatment            |" << endl;
    cout << "+-----------┼----------------------┼--------------┼--------------┼----------------------┼----------------------+" << endl;
    
    while (res->next()) {
        string diagnosis = res->getString("diagnosis");
        string treatment = res->getString("treatment");
        if (diagnosis.length() > 20) diagnosis = diagnosis.substr(0, 17) + "...";
        if (treatment.length() > 20) treatment = treatment.substr(0, 17) + "...";
        
        cout << "| " << setw(9) << res->getInt("report_id")
             << "| " << setw(20) << res->getString("patient_name")
             << "| " << setw(12) << res->getString("report_type")
             << "| " << setw(12) << res->getString("report_date")
             << "| " << setw(20) << diagnosis
             << "| " << setw(20) << treatment << "|" << endl;
    }
    
    cout << "+-----------+----------------------+--------------+--------------+----------------------+----------------------+" << endl;
}

void StaffModule::displayTableHeader(const string& title) {
    // Blue theme header
    ColorUtils::setColor(BLUE);
    cout << "\n+----------------------------------------------------------------+" << endl;
    cout << "|" << setw(60) << "" << "|" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title
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

