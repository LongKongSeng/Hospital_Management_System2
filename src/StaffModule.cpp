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

