#include "DoctorModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

DoctorModule::DoctorModule(Database* database, int doctorId) : db(database), currentDoctorId(doctorId) {}

void DoctorModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Record",
            "Generate Next Appointment",
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
            generateNextAppointment();
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
            ColorUtils::setColor(YELLOW);
            cout << "\n❌ Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 4);
}

void DoctorModule::viewPatientRecord() {
    system("cls");
    displayTableHeader("VIEW PATIENT RECORD");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        // Display patient details
        string patientQuery = "SELECT patient_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status "
                             "FROM patient WHERE patient_id = " + to_string(patientId);
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);
        
        if (!patientRes || !patientRes->next()) {
            cout << "\n❌ Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        cout << "\n+----------------------------------------------------------------+" << endl;
        cout << "|                    PATIENT INFORMATION                         |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "| Patient ID: " << left << setw(46) << patientRes->getInt("patient_id") << "|" << endl;
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
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::generateNextAppointment() {
    system("cls");
    displayTableHeader("GENERATE NEXT APPOINTMENT");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify patient exists
    string checkQuery = "SELECT full_name, status FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n❌ Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    
    string patientName = checkRes->getString("full_name");
    delete checkRes;

    // Get available nurses
    string nurseQuery = "SELECT nurse_id, full_name FROM nurse WHERE status = 'Active' ORDER BY nurse_id";
    sql::ResultSet* nurseRes = db->executeSelect(nurseQuery);
    
    if (!nurseRes || nurseRes->rowsCount() == 0) {
        cout << "\n❌ No active nurses available!" << endl;
        if (nurseRes) delete nurseRes;
        pressEnterToContinue();
        return;
    }

    cout << "\nPatient: " << patientName << endl;
    cout << "\nAvailable Nurses:\n" << endl;
    cout << "+-----------+----------------------+" << endl;
    cout << "| Nurse ID  | Full Name            |" << endl;
    cout << "+-----------┼----------------------+" << endl;
    while (nurseRes->next()) {
        cout << "| " << setw(9) << nurseRes->getInt("nurse_id")
             << "| " << setw(20) << nurseRes->getString("full_name") << "|" << endl;
    }
    cout << "+-----------+----------------------+" << endl;
    delete nurseRes;

    int nurseId = getIntInput("\nEnter Nurse ID: ");
    if (nurseId <= 0) {
        cout << "\n❌ Invalid Nurse ID!" << endl;
        pressEnterToContinue();
        return;
    }

    string appointmentDate = getStringInput("Enter Appointment Date (YYYY-MM-DD): ");
    if (appointmentDate.empty()) {
        cout << "\n❌ Appointment date cannot be empty!" << endl;
        pressEnterToContinue();
        return;
    }

    string appointmentTime = getStringInput("Enter Appointment Time (HH:MM:SS): ");
    if (appointmentTime.empty()) {
        appointmentTime = "09:00:00"; // Default time
    }

    try {
        string query = "INSERT INTO appointment (patient_id, nurse_id, appointment_date, appointment_time, status) "
            "VALUES (" + to_string(patientId) + ", " + to_string(nurseId) + ", '" + appointmentDate + "', '" + appointmentTime + "', 'Scheduled')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Next appointment generated successfully!" << endl;
            cout << "Patient: " << patientName << endl;
            cout << "Appointment Date: " << appointmentDate << endl;
            cout << "Appointment Time: " << appointmentTime << endl;
            
            // Update patient status to Active if appointment is scheduled
            string updateQuery = "UPDATE patient SET status = 'Active' WHERE patient_id = " + to_string(patientId);
            db->executeUpdate(updateQuery);
        } else {
            cout << "\n❌ Failed to generate appointment!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::makeDiagnosis() {
    system("cls");
    displayTableHeader("MAKE DIAGNOSIS");

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

    cout << "\nPatient: " << patientName << endl;
    
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
            string dosage = getStringInput("Enter Dosage: ");
            string durationOfMeds = getStringInput("Enter Duration of Medications: ");
            string instructions = getStringInput("Enter Instructions: ");
            
            string presQuery = "INSERT INTO prescription (pharmacy_id, dosage, duration_of_meds, instructions, date) "
                "VALUES (" + to_string(pharmacyId) + ", '" + dosage + "', '" + durationOfMeds + "', '" + instructions + "', '" + date + "')";
            
            if (db->executeUpdate(presQuery)) {
                // Get the prescription_id
                string getIdQuery = "SELECT prescription_id FROM prescription WHERE pharmacy_id = " + to_string(pharmacyId) + " ORDER BY prescription_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                if (idRes && idRes->next()) {
                    prescriptionId = idRes->getInt("prescription_id");
                }
                if (idRes) delete idRes;
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
                    cout << "Medical record created for patient: " << patientName << endl;
                } else {
                    cout << "\n⚠️  Diagnosis created but medical record failed!" << endl;
                }
            } else {
                cout << "\n❌ Failed to retrieve diagnosis ID!" << endl;
            }
        } else {
            cout << "\n❌ Failed to create diagnosis!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::editPatientMedicalRecord() {
    system("cls");
    displayTableHeader("EDIT PATIENT MEDICAL RECORD");

    int patientId = getIntInput("Enter Patient ID: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        // Display current medical record
        string recordQuery = "SELECT mr.record_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
                            "FROM medical_record mr "
                            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.diagnosis_id "
                            "WHERE mr.patient_id = " + to_string(patientId) + " "
                            "ORDER BY mr.date_of_record DESC LIMIT 1";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);
        
        if (!recordRes || !recordRes->next()) {
            cout << "\n⚠️  No medical record found for this patient." << endl;
            cout << "Would you like to create a new record? (yes/no): ";
            string createChoice;
            getline(cin, createChoice);
            
            if (createChoice == "yes" || createChoice == "YES") {
                makeDiagnosis();
            }
            if (recordRes) delete recordRes;
            return;
        }

        cout << "\nCurrent Medical Record:" << endl;
        displayMedicalRecordTable(recordRes);
        delete recordRes;

        // Get diagnosis_id from the latest record
        string getDiagQuery = "SELECT diagnosis_id FROM medical_record WHERE patient_id = " + to_string(patientId) + " ORDER BY date_of_record DESC LIMIT 1";
        sql::ResultSet* diagRes = db->executeSelect(getDiagQuery);
        int diagnosisId = -1;
        if (diagRes && diagRes->next()) {
            diagnosisId = diagRes->getInt("diagnosis_id");
        }
        if (diagRes) delete diagRes;

        if (diagnosisId > 0) {
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
                } else {
                    cout << "\n❌ Failed to update medical record!" << endl;
                }
            } else {
                cout << "\n❌ No fields to update!" << endl;
            }
        } else {
            cout << "\n❌ No diagnosis found to update!" << endl;
        }

    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void DoctorModule::displayPatientRecordTable(sql::ResultSet* res) {
    // Implementation for displaying patient records in table format
    cout << "\n+-------------+----------------------+----------+--------------+--------------+" << endl;
    cout << "| Patient ID  | Full Name            | Gender   | Date of Birth| Status       |" << endl;
    cout << "+-------------┼----------------------┼----------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(11) << res->getInt("patient_id")
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
        cout << "| " << setw(11) << res->getInt("record_id")
             << "| " << setw(12) << res->getString("date_of_record")
             << "| " << setw(20) << (res->isNull("disease") ? "N/A" : res->getString("disease"))
             << "| " << setw(20) << (res->isNull("disorder") ? "N/A" : res->getString("disorder"))
             << "| " << setw(12) << (res->isNull("duration_of_pain") ? "N/A" : res->getString("duration_of_pain"))
             << "| " << setw(12) << (res->isNull("severity") ? "N/A" : res->getString("severity")) << "|" << endl;
    }
    
    cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;
}

void DoctorModule::displayTableHeader(const string& title) {
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



