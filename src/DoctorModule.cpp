#include "DoctorModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

// Helper function for strict date validation (YYYY-MM-DD)
static bool isValidDate(const string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return false;
    }
    int m = stoi(date.substr(5, 2));
    int d = stoi(date.substr(8, 2));
    return (m >= 1 && m <= 12 && d >= 1 && d <= 31);
}

DoctorModule::DoctorModule(Database* database, const string& doctorId) : db(database), currentDoctorId(doctorId) {}

void DoctorModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Record",
            "View Appointments",
            "Make Diagnosis",
            "Edit Patient Medical Record",
            "Exit"
        };

        choice = MenuNavigator::showMenu(menuOptions, "DOCTOR MENU", true);

        if (choice == -1) {
            continue;
        }

        switch (choice) {
        case 0: viewPatientRecord(); break;
        case 1: viewAppointments(); break;
        case 2: makeDiagnosis(); break;
        case 3: editPatientMedicalRecord(); break;
        case 4: return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (true);
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
        string patientQuery = "SELECT formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status "
            "FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);

        if (!patientRes || !patientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        cout << "\n" << endl;
        cout << "+================================================================+" << endl;
        cout << "|                      PATIENT INFORMATION                       |" << endl;
        cout << "+================================================================+" << endl;
        cout << "| Patient ID: " << left << setw(50) << string(patientRes->getString("formatted_id")) << "|" << endl;
        cout << "| Full Name: " << left << setw(51) << patientRes->getString("full_name") << "|" << endl;
        cout << "| Gender: " << left << setw(54) << patientRes->getString("gender") << "|" << endl;
        cout << "| Date of Birth: " << left << setw(47) << patientRes->getString("date_of_birth") << "|" << endl;
        cout << "| Contact Number: " << left << setw(46) << patientRes->getString("contact_number") << "|" << endl;
        cout << "| Blood Type: " << left << setw(50) << (patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type")) << "|" << endl;
        cout << "| Status: " << left << setw(54) << patientRes->getString("status") << "|" << endl;
        cout << "+================================================================+" << endl;

        string recordQuery = "SELECT mr.formatted_id AS record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "WHERE mr.patient_id = '" + patientId + "' "
            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);

        if (recordRes && recordRes->rowsCount() > 0) {
            cout << "\n" << endl;
            cout << "+================================================================+" << endl;
            cout << "|                      MEDICAL RECORDS                           |" << endl;
            cout << "+================================================================+" << endl;
            displayMedicalRecordTable(recordRes);
        }
        else {
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
            cout << "| " << left << setw(15) << "Appointment ID"
                << "| " << left << setw(9) << "Patient ID"
                << "| " << left << setw(20) << "Patient Name"
                << "| " << left << setw(20) << "Nurse Name"
                << "| " << left << setw(12) << "Date"
                << "| " << left << setw(12) << "Time"
                << "| " << left << setw(20) << "Status" << "|" << endl;
            cout << "+-----------------+-----------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;

            while (appointmentRes->next()) {
                string appointmentId = string(appointmentRes->getString("appointment_formatted_id"));
                string patientId = string(appointmentRes->getString("patient_formatted_id"));
                string patientName = string(appointmentRes->getString("patient_name"));
                string nurseName = string(appointmentRes->getString("nurse_name"));
                string appointmentDate = appointmentRes->isNull("appointment_date") ? "N/A" : string(appointmentRes->getString("appointment_date"));
                string appointmentTime = appointmentRes->isNull("appointment_time") ? "N/A" : string(appointmentRes->getString("appointment_time"));
                string status = string(appointmentRes->getString("status"));

                if (patientName.length() > 20) patientName = patientName.substr(0, 17) + "...";
                if (nurseName.length() > 20) nurseName = nurseName.substr(0, 17) + "...";

                cout << "| " << left << setw(16) << appointmentId
                    << "| " << left << setw(10) << patientId
                    << "| " << left << setw(21) << patientName
                    << "| " << left << setw(21) << nurseName
                    << "| " << left << setw(13) << appointmentDate
                    << "| " << left << setw(13) << appointmentTime
                    << "| " << left << setw(20) << status << " |" << endl;
            }
            cout << "+-----------------+-----------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;

            string countQuery = "SELECT COUNT(*) as total_count, "
                "SUM(CASE WHEN status = 'Scheduled' THEN 1 ELSE 0 END) as scheduled_count, "
                "SUM(CASE WHEN status = 'Completed' THEN 1 ELSE 0 END) as completed_count "
                "FROM appointment WHERE doctor_id = '" + currentDoctorId + "'";

            sql::ResultSet* countRes = db->executeSelect(countQuery);
            if (countRes && countRes->next()) {
                cout << "\n" << endl;
                cout << "+================================================================+" << endl;
                cout << "|                      APPOINTMENT SUMMARY                        |" << endl;
                cout << "+================================================================+" << endl;
                cout << "| Total Appointments: " << left << setw(42) << countRes->getInt("total_count") << "|" << endl;
                cout << "| Scheduled: " << left << setw(51) << countRes->getInt("scheduled_count") << "|" << endl;
                cout << "| Completed: " << left << setw(51) << countRes->getInt("completed_count") << "|" << endl;
                cout << "+================================================================+" << endl;
            }
            if (countRes) delete countRes;
        }
        else {
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

    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    string checkQuery = "SELECT full_name FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);

    if (!checkRes || !checkRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }

    string patientName = checkRes->getString("full_name");
    delete checkRes;

    auto redrawContext = [&]() {
        system("cls");
        displayTableHeader("MAKE DIAGNOSIS");
        cout << "\n" << endl;
        cout << "+================================================================+" << endl;
        cout << "|                      PATIENT INFORMATION                       |" << endl;
        cout << "+================================================================+" << endl;
        cout << "| Patient Name: " << left << setw(50) << patientName << "|" << endl;
        cout << "+================================================================+" << endl;
        cout << endl;
        };

    redrawContext();

    string disease = getStringInput("Enter Disease: ");
    string disorder = getStringInput("Enter Disorder: ");
    string durationOfPain = getStringInput("Enter Duration of Pain: ");
    string severity = getStringInput("Enter Severity: ");

    string date;
    while (true) {
        date = getStringInput("Enter Date (YYYY-MM-DD) or Press Enter for Today: ");
        if (date.empty()) {
            date = "CURDATE()";
            break;
        }
        if (isValidDate(date)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD (e.g., 2024-05-20)." << endl;
            pressEnterToContinue();
            redrawContext();
            cout << "Enter Disease: " << disease << endl;
            cout << "Enter Disorder: " << disorder << endl;
            cout << "Enter Duration of Pain: " << durationOfPain << endl;
            cout << "Enter Severity: " << severity << endl;
        }
    }

    // --- PRESCRIPTION SECTION ---
    string prescriptionId = "";
    string prescriptionChoice = getStringInput("Do you want to add prescription? (yes/no): ");
    if (prescriptionChoice == "yes" || prescriptionChoice == "YES") {
        string pharmacyId = getStringInput("Enter Pharmacy ID (e.g., PH001): ");
        if (!pharmacyId.empty()) {
            string validatePharmacyQuery = "SELECT formatted_id, medicine_name FROM pharmacy WHERE formatted_id = '" + pharmacyId + "'";
            sql::ResultSet* validateRes = db->executeSelect(validatePharmacyQuery);

            if (!validateRes || !validateRes->next()) {
                cout << "\n[ERROR] Pharmacy ID " << pharmacyId << " not found!" << endl;
                cout << "Continuing without prescription..." << endl;
                if (validateRes) delete validateRes;
            }
            else {
                string medicineName = validateRes->getString("medicine_name");
                cout << "Medication: " << medicineName << endl;
                delete validateRes;

                string dosage = getStringInput("Enter Dosage: ");
                string durationOfMeds = getStringInput("Enter Duration of Medications: ");
                string instructions = getStringInput("Enter Instructions: ");

                string presQuery = "INSERT INTO prescription (formatted_id, pharmacy_id, dosage, duration_of_meds, instructions, date) "
                    "VALUES (NULL, '" + pharmacyId + "', '" + dosage + "', '" + durationOfMeds + "', '" + instructions + "', '" + date + "')";

                if (db->executeUpdate(presQuery)) {
                    string getIdQuery = "SELECT formatted_id FROM prescription WHERE pharmacy_id = '" + pharmacyId + "' AND date = '" + date + "' ORDER BY formatted_id DESC LIMIT 1";
                    sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                    if (idRes && idRes->next()) {
                        prescriptionId = string(idRes->getString("formatted_id"));
                    }
                    if (idRes) delete idRes;
                }
                else {
                    cout << "\n⚠️  Failed to create prescription." << endl;
                }
            }
        }
    }

    // --- TREATMENT & FINANCIAL SECTION ---
    cout << "\n--- TREATMENT & BILLING ---" << endl;
    string dressing = getStringInput("Treatment/Procedure Description (e.g. Wound Dressing): ");
    if (dressing.empty()) dressing = "Standard Consultation";

    string consultFeeStr = getStringInput("Consultation Fee (RM): ");
    string treatFeeStr = getStringInput("Treatment Fee (RM): ");

    double consultFee = 0.0;
    double treatFee = 0.0;
    try {
        if (!consultFeeStr.empty()) consultFee = stod(consultFeeStr);
        if (!treatFeeStr.empty()) treatFee = stod(treatFeeStr);
    }
    catch (...) {
        consultFee = 0.0;
        treatFee = 0.0;
    }

    // Insert into TREATMENT table
    string treatmentQuery = "INSERT INTO treatment (formatted_id, patient_id, doctor_id, dressing_applied, consultation_fee, treatment_fee, treatment_date) "
        "VALUES (NULL, '" + patientId + "', '" + currentDoctorId + "', '" + dressing + "', " + to_string(consultFee) + ", " + to_string(treatFee) + ", '" + date + "')";

    bool treatmentInserted = db->executeUpdate(treatmentQuery);

    // --- DIAGNOSIS & MEDICAL RECORD INSERTION ---
    try {
        string diagQuery = "INSERT INTO diagnosis (formatted_id, disease, disorder, duration_of_pain, severity, prescription_id, date) "
            "VALUES (NULL, '" + disease + "', '" + disorder + "', '" + durationOfPain + "', '" + severity + "', ";

        if (!prescriptionId.empty()) {
            diagQuery += "'" + prescriptionId + "', '" + date + "')";
        }
        else {
            diagQuery += "NULL, '" + date + "')";
        }

        if (db->executeUpdate(diagQuery)) {
            string getIdQuery = "SELECT formatted_id FROM diagnosis WHERE disease = '" + disease + "' ORDER BY formatted_id DESC LIMIT 1";
            sql::ResultSet* idRes = db->executeSelect(getIdQuery);
            string diagnosisId = "";
            if (idRes && idRes->next()) {
                diagnosisId = string(idRes->getString("formatted_id"));
            }
            if (idRes) delete idRes;

            if (!diagnosisId.empty()) {
                string recordQuery = "INSERT INTO medical_record (formatted_id, patient_id, doctor_id, diagnosis_id, date_of_record, notes) "
                    "VALUES (NULL, '" + patientId + "', '" + currentDoctorId + "', '" + diagnosisId + "', '" + date + "', 'Diagnosis made by doctor')";

                if (db->executeUpdate(recordQuery)) {
                    cout << "\n✅ Diagnosis made successfully!" << endl;
                    if (treatmentInserted) cout << "✅ Treatment & Billing details recorded." << endl;

                    cout << "\n" << endl;
                    cout << "+================================================================+" << endl;
                    cout << "|                      DIAGNOSIS DETAILS                         |" << endl;
                    cout << "+================================================================+" << endl;
                    // ALIGNMENT FIX: Separator width is 64 characters
                    cout << "| Patient Name: " << left << setw(49) << patientName << "|" << endl;
                    cout << "| Disease: " << left << setw(54) << (disease.empty() ? "N/A" : disease) << "|" << endl;
                    cout << "| Disorder: " << left << setw(53) << (disorder.empty() ? "N/A" : disorder) << "|" << endl;
                    cout << "| Duration of Pain: " << left << setw(45) << (durationOfPain.empty() ? "N/A" : durationOfPain) << "|" << endl;
                    cout << "| Severity: " << left << setw(53) << (severity.empty() ? "N/A" : severity) << "|" << endl;
                    cout << "| Date: " << left << setw(57) << (date == "CURDATE()" ? "Today" : date) << "|" << endl;
                    cout << "| Consultation Fee: RM " << left << setw(42) << fixed << setprecision(2) << consultFee << "|" << endl;
                    cout << "| Treatment Fee: RM " << left << setw(45) << fixed << setprecision(2) << treatFee << "|" << endl;
                    cout << "+================================================================+" << endl;
                }
                else {
                    cout << "\n⚠️  Diagnosis created but medical record failed!" << endl;
                }
            }
        }
        else {
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

    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    try {
        string checkPatientQuery = "SELECT full_name FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* checkPatientRes = db->executeSelect(checkPatientQuery);

        if (!checkPatientRes || !checkPatientRes->next()) {
            cout << "\n[ERROR] Patient not found!" << endl;
            if (checkPatientRes) delete checkPatientRes;
            pressEnterToContinue();
            return;
        }
        string patientName = checkPatientRes->getString("full_name");
        delete checkPatientRes;

        string recordQuery = "SELECT mr.formatted_id as record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "WHERE mr.patient_id = '" + patientId + "' "
            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);

        if (!recordRes || !recordRes->next()) {
            cout << "\n⚠️  No medical record found for this patient." << endl;
            cout << "Would you like to create a new record? (yes/no): ";
            string createChoice;
            getline(cin, createChoice);
            if (createChoice == "yes" || createChoice == "YES") makeDiagnosis();
            if (recordRes) delete recordRes;
            return;
        }

        // Rewind result set
        delete recordRes;
        recordRes = db->executeSelect(recordQuery);

        cout << "\n" << endl;
        cout << "+================================================================+" << endl;
        cout << "|                      PATIENT INFORMATION                       |" << endl;
        cout << "+================================================================+" << endl;
        cout << "| Patient Name: " << left << setw(50) << patientName << "|" << endl;
        cout << "+================================================================+" << endl;
        cout << "\nMedical Record History:\n" << endl;
        displayMedicalRecordTable(recordRes);
        delete recordRes;

        string recordId = getStringInput("\nEnter Record ID to edit (e.g., MR001) or press Enter to cancel: ");
        if (recordId.empty()) {
            cout << "\n[ERROR] Operation cancelled!" << endl;
            pressEnterToContinue();
            return;
        }

        string getDiagQuery = "SELECT mr.diagnosis_id, mr.formatted_id as record_formatted_id, d.disease, d.disorder, d.duration_of_pain, d.severity, d.date "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "WHERE mr.formatted_id = '" + recordId + "' AND mr.patient_id = '" + patientId + "'";

        sql::ResultSet* diagRes = db->executeSelect(getDiagQuery);

        if (!diagRes || !diagRes->next()) {
            cout << "\n[ERROR] Record ID " << recordId << " not found for this patient!" << endl;
            if (diagRes) delete diagRes;
            pressEnterToContinue();
            return;
        }

        string diagnosisId = diagRes->isNull("diagnosis_id") ? "" : string(diagRes->getString("diagnosis_id"));
        string currentDisease = diagRes->isNull("disease") ? "" : diagRes->getString("disease");
        string currentDisorder = diagRes->isNull("disorder") ? "" : diagRes->getString("disorder");
        string currentDuration = diagRes->isNull("duration_of_pain") ? "" : diagRes->getString("duration_of_pain");
        string currentSeverity = diagRes->isNull("severity") ? "" : diagRes->getString("severity");
        string currentDate = diagRes->isNull("date") ? "" : diagRes->getString("date");
        delete diagRes;

        auto redrawEditContext = [&]() {
            system("cls");
            displayTableHeader("EDIT PATIENT MEDICAL RECORD");
            cout << "\n" << endl;
            cout << "+================================================================+" << endl;
            cout << "|                   CURRENT DIAGNOSIS DETAILS                    |" << endl;
            cout << "+================================================================+" << endl;
            cout << "| Disease: " << left << setw(56) << (currentDisease.empty() ? "N/A" : currentDisease) << "|" << endl;
            cout << "| Disorder: " << left << setw(55) << (currentDisorder.empty() ? "N/A" : currentDisorder) << "|" << endl;
            cout << "| Duration of Pain: " << left << setw(47) << (currentDuration.empty() ? "N/A" : currentDuration) << "|" << endl;
            cout << "| Severity: " << left << setw(55) << (currentSeverity.empty() ? "N/A" : currentSeverity) << "|" << endl;
            cout << "| Date: " << left << setw(58) << (currentDate.empty() ? "N/A" : currentDate) << "|" << endl;
            cout << "+================================================================+" << endl;
            cout << "\nEnter new diagnosis details (press Enter to keep current value):" << endl;
            };

        redrawEditContext();

        string disease = getStringInput("Disease: ");
        string disorder = getStringInput("Disorder: ");
        string durationOfPain = getStringInput("Duration of Pain: ");
        string severity = getStringInput("Severity: ");

        string date;
        while (true) {
            date = getStringInput("Date (YYYY-MM-DD): ");
            if (date.empty()) break;
            if (isValidDate(date)) break;

            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD." << endl;
            pressEnterToContinue();
            redrawEditContext();
            cout << "Disease: " << disease << endl;
            cout << "Disorder: " << disorder << endl;
            cout << "Duration of Pain: " << durationOfPain << endl;
            cout << "Severity: " << severity << endl;
        }

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
            updateQuery += " WHERE formatted_id = '" + diagnosisId + "'";

            if (db->executeUpdate(updateQuery)) {
                cout << "\n✅ Medical record updated successfully!" << endl;
                cout << "\n" << endl;
                cout << "+================================================================+" << endl;
                cout << "|                    UPDATED MEDICAL RECORD                      |" << endl;
                cout << "+================================================================+" << endl;
                cout << "| Patient Name: " << left << setw(50) << patientName << "|" << endl;
                cout << "| Record ID: " << left << setw(54) << recordId << "|" << endl;
                cout << "| Disease: " << left << setw(56) << (disease.empty() ? "Not changed" : disease) << "|" << endl;
                cout << "| Disorder: " << left << setw(55) << (disorder.empty() ? "Not changed" : disorder) << "|" << endl;
                cout << "| Duration of Pain: " << left << setw(47) << (durationOfPain.empty() ? "Not changed" : durationOfPain) << "|" << endl;
                cout << "| Severity: " << left << setw(55) << (severity.empty() ? "Not changed" : severity) << "|" << endl;
                cout << "| Date: " << left << setw(58) << (date.empty() ? "Not changed" : date) << "|" << endl;
                cout << "+================================================================+" << endl;
            }
            else {
                cout << "\n[ERROR] Failed to update medical record!" << endl;
            }
        }
        else {
            cout << "\n[ERROR] No fields to update!" << endl;
        }

    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

void DoctorModule::displayPatientRecordTable(sql::ResultSet* res) {
    cout << "\n+-------------+----------------------+----------+--------------+--------------+" << endl;
    cout << "| " << left << setw(11) << "Patient ID"
        << "| " << left << setw(20) << "Full Name"
        << "| " << left << setw(8) << "Gender"
        << "| " << left << setw(12) << "Date of Birth"
        << "| " << left << setw(12) << "Status" << "|" << endl;
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;

    // ALIGNMENT FIX: Matched data row setw to Header-2
    while (res->next()) {
        string patientId = string(res->getString("patient_formatted_id"));
        string fullName = string(res->getString("full_name"));
        string gender = string(res->getString("gender"));
        string dob = res->isNull("date_of_birth") ? "N/A" : string(res->getString("date_of_birth"));
        string status = string(res->getString("status"));

        if (fullName.length() > 20) fullName = fullName.substr(0, 17) + "...";

        cout << "| " << left << setw(12) << patientId
            << "| " << left << setw(21) << fullName
            << "| " << left << setw(9) << gender
            << "| " << left << setw(13) << dob
            << "| " << left << setw(12) << status << " |" << endl;
    }
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;
}

void DoctorModule::displayMedicalRecordTable(sql::ResultSet* res) {
    cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;
    cout << "| " << left << setw(11) << "Record ID"
        << "| " << left << setw(12) << "Date"
        << "| " << left << setw(20) << "Disease"
        << "| " << left << setw(20) << "Disorder"
        << "| " << left << setw(12) << "Duration"
        << "| " << left << setw(12) << "Severity" << "|" << endl;
    cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;

    // ALIGNMENT FIX: Matched data row setw to Header-2
    while (res->next()) {
        string recordId = string(res->getString("record_formatted_id"));
        string date = res->isNull("date_of_record") ? "N/A" : string(res->getString("date_of_record"));
        string disease = res->isNull("disease") ? "N/A" : string(res->getString("disease"));
        string disorder = res->isNull("disorder") ? "N/A" : string(res->getString("disorder"));
        string duration = res->isNull("duration_of_pain") ? "N/A" : string(res->getString("duration_of_pain"));
        string severity = res->isNull("severity") ? "N/A" : string(res->getString("severity"));

        if (disease.length() > 20) disease = disease.substr(0, 17) + "...";
        if (disorder.length() > 20) disorder = disorder.substr(0, 17) + "...";
        if (duration.length() > 12) duration = duration.substr(0, 9) + "...";
        if (severity.length() > 12) severity = severity.substr(0, 9) + "...";

        cout << "| " << left << setw(12) << recordId
            << "| " << left << setw(13) << date
            << "| " << left << setw(21) << disease
            << "| " << left << setw(21) << disorder
            << "| " << left << setw(13) << duration
            << "| " << left << setw(12) << severity << " |" << endl;
    }
    cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+" << endl;
}

void DoctorModule::displayTableHeader(const string& title) {
    const int SEPARATOR_LENGTH = 80;
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
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
    try { return stoi(input); }
    catch (...) { return -1; }
}

string DoctorModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}
