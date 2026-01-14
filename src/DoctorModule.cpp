#define _CRT_SECURE_NO_WARNINGS // prevents errors with time functions in Visual Studio
#include "DoctorModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <ctime> // Required for date generation
#include <limits> // Required for numeric input validation

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

// Helper function to get today's date as a string (YYYY-MM-DD)
static string getSystemDate() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now);
    return string(buffer);
}

// --- NEW HELPER: Force Numeric Input for Fees ---
static double getValidDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            // Check if the next char is a newline (meaning the whole input was a number)
            if (cin.peek() == '\n') {
                cin.ignore((numeric_limits<streamsize>::max)(), '\n'); // Clear buffer
                return value;
            }
            else {
                // Input started with a number but had garbage after (e.g., "12abc")
                cout << "\n[ERROR] Invalid input! Please enter a valid number (e.g. 50.00)." << endl;
                cin.clear();
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            }
        }
        else {
            // Input wasn't a number at all (e.g., "asda")
            cout << "\n[ERROR] Invalid input! Please enter a valid number (e.g. 50.00)." << endl;
            cin.clear(); // Clear error flag
            cin.ignore((numeric_limits<streamsize>::max)(), '\n'); // Discard bad input
        }
    }
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

// ---------------------------------------------------------
// SEARCH HELPER (Fixed Alignment)
// ---------------------------------------------------------
string DoctorModule::searchPatientId() {
    string input = getStringInput("Enter Patient Name, IC Number, or ID: ");
    if (input.empty()) return "";

    string query = "SELECT formatted_id, full_name, ic_number FROM patient WHERE "
        "full_name LIKE '%" + input + "%' OR "
        "ic_number LIKE '%" + input + "%' OR "
        "formatted_id = '" + input + "'";

    sql::ResultSet* res = db->executeSelect(query);

    if (!res || res->rowsCount() == 0) {
        cout << "\n[INFO] No patients found matching \"" << input << "\"." << endl;
        if (res) delete res;
        return "";
    }

    string selectedId = "";

    if (res->rowsCount() == 1) {
        res->next();
        selectedId = res->getString("formatted_id");
    }
    else {
        cout << "\n[INFO] Multiple patients found. Please select one:\n" << endl;
        struct PatientOpt { string id; string name; string ic; };
        vector<PatientOpt> options;

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

            // Strict Truncation: 27 chars max for name
            if (pName.length() > 27) pName = pName.substr(0, 24) + "...";

            cout << "| " << left << setw(4) << index
                << "| " << left << setw(12) << pId
                << "| " << left << setw(28) << pName
                << "| " << left << setw(17) << pIc << "|" << endl;
            index++;
        }
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+-----+-------------+-----------------------------+------------------+" << endl;
        ColorUtils::resetColor();

        int selection = getIntInput("\nEnter selection number: ");
        if (selection > 0 && selection <= (int)options.size()) {
            selectedId = options[selection - 1].id;
        }
        else {
            cout << "\n[ERROR] Invalid selection." << endl;
        }
    }
    delete res;
    return selectedId;
}

// ---------------------------------------------------------
// VIEW PATIENT RECORD (Aligned Box & Tables)
// ---------------------------------------------------------
void DoctorModule::viewPatientRecord() {
    system("cls");
    displayTableHeader("VIEW PATIENT RECORD");

    string patientId = searchPatientId();
    if (patientId.empty()) {
        pressEnterToContinue();
        return;
    }

    try {
        string patientQuery = "SELECT formatted_id, full_name, gender, date_of_birth, contact_number, blood_type, emergency_contact, status "
            "FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* patientRes = db->executeSelect(patientQuery);

        if (!patientRes || !patientRes->next()) {
            cout << "\n[ERROR] Error retrieving patient data." << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        // --- PATIENT INFO BOX ---
        string pId = patientRes->getString("formatted_id");
        string pName = patientRes->getString("full_name");
        string pGender = patientRes->getString("gender");
        string pDob = patientRes->getString("date_of_birth");
        string pContact = patientRes->getString("contact_number");
        string pBlood = patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type");
        string pStatus = patientRes->getString("status");
        delete patientRes;

        // Box Width: 65 chars
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n+=================================================================+" << endl;
        cout << "|                       PATIENT INFORMATION                       |" << endl;
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();

        // Label width: 17, Value width: 46
        cout << "| Patient ID      : " << left << setw(46) << pId << "|" << endl;
        cout << "| Full Name       : " << left << setw(46) << pName << "|" << endl;
        cout << "| Gender          : " << left << setw(46) << pGender << "|" << endl;
        cout << "| Date of Birth   : " << left << setw(46) << pDob << "|" << endl;
        cout << "| Contact Number  : " << left << setw(46) << pContact << "|" << endl;
        cout << "| Blood Type      : " << left << setw(46) << pBlood << "|" << endl;
        cout << "| Status          : " << left << setw(46) << pStatus << "|" << endl;

        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();


        string recordQuery = "SELECT mr.formatted_id AS record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "WHERE mr.patient_id = '" + patientId + "' "
            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);

        if (recordRes && recordRes->rowsCount() > 0) {
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n+=================================================================+" << endl;
            cout << "|                         MEDICAL RECORDS                         |" << endl;
            cout << "+=================================================================+" << endl;
            ColorUtils::resetColor();
            displayMedicalRecordTable(recordRes);
        }
        else {
            cout << "\n[INFO] No medical records found for this patient." << endl;
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
            // Widths: ID(12), PatID(10), Name(20), Nurse(20), Date(12), Time(8), Status(11)
            // Separators: 12 | 10 | 20 | 20 | 12 | 8 | 11
            const string SEP = "+------------+----------+--------------------+--------------------+------------+--------+-----------+";

            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n" << SEP << endl;
            cout << "| " << left << setw(11) << "Appt ID"
                << "| " << left << setw(9) << "Pat ID"
                << "| " << left << setw(19) << "Patient Name"
                << "| " << left << setw(19) << "Nurse Name"
                << "| " << left << setw(11) << "Date"
                << "| " << left << setw(7) << "Time"
                << "| " << left << setw(10) << "Status" << "|" << endl;
            cout << SEP << endl;
            ColorUtils::resetColor();

            while (appointmentRes->next()) {
                string appointmentId = string(appointmentRes->getString("appointment_formatted_id"));
                string patientId = string(appointmentRes->getString("patient_formatted_id"));
                string patientName = string(appointmentRes->getString("patient_name"));
                string nurseName = string(appointmentRes->getString("nurse_name"));
                string appointmentDate = appointmentRes->isNull("appointment_date") ? "N/A" : string(appointmentRes->getString("appointment_date"));
                string appointmentTime = appointmentRes->isNull("appointment_time") ? "N/A" : string(appointmentRes->getString("appointment_time"));
                string status = string(appointmentRes->getString("status"));

                // Strict Truncation
                if (patientName.length() > 19) patientName = patientName.substr(0, 16) + "...";
                if (nurseName.length() > 19) nurseName = nurseName.substr(0, 16) + "...";
                if (appointmentTime.length() > 5) appointmentTime = appointmentTime.substr(0, 5); // HH:MM

                cout << "| " << left << setw(11) << appointmentId
                    << "| " << left << setw(9) << patientId
                    << "| " << left << setw(19) << patientName
                    << "| " << left << setw(19) << nurseName
                    << "| " << left << setw(11) << appointmentDate
                    << "| " << left << setw(7) << appointmentTime
                    << "| " << left << setw(10) << status << "|" << endl;
            }
            ColorUtils::setColor(LIGHT_CYAN);
            cout << SEP << endl;
            ColorUtils::resetColor();

            string countQuery = "SELECT COUNT(*) as total_count, "
                "SUM(CASE WHEN status = 'Scheduled' THEN 1 ELSE 0 END) as scheduled_count, "
                "SUM(CASE WHEN status = 'Completed' THEN 1 ELSE 0 END) as completed_count "
                "FROM appointment WHERE doctor_id = '" + currentDoctorId + "'";

            sql::ResultSet* countRes = db->executeSelect(countQuery);
            if (countRes && countRes->next()) {
                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\n+=================================================================+" << endl;
                cout << "|                       APPOINTMENT SUMMARY                       |" << endl;
                cout << "+=================================================================+" << endl;
                ColorUtils::resetColor();

                // Box Width 65. Label ~20, Value ~43
                cout << "| Total Appointments : " << left << setw(43) << countRes->getInt("total_count") << "|" << endl;
                cout << "| Scheduled          : " << left << setw(43) << countRes->getInt("scheduled_count") << "|" << endl;
                cout << "| Completed          : " << left << setw(43) << countRes->getInt("completed_count") << "|" << endl;

                ColorUtils::setColor(LIGHT_CYAN);
                cout << "+=================================================================+" << endl;
                ColorUtils::resetColor();
            }
            if (countRes) delete countRes;
        }
        else {
            cout << "\n[INFO] No appointments found for this doctor." << endl;
        }
        if (appointmentRes) delete appointmentRes;
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// MAKE DIAGNOSIS (With Medication Search Loop & VALIDATION)
// ---------------------------------------------------------
void DoctorModule::makeDiagnosis() {
    system("cls");
    displayTableHeader("MAKE DIAGNOSIS");

    string patientId = searchPatientId();
    if (patientId.empty()) {
        pressEnterToContinue();
        return;
    }

    string checkQuery = "SELECT full_name FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    string patientName = "";
    if (checkRes && checkRes->next()) {
        patientName = checkRes->getString("full_name");
        delete checkRes;
    }

    auto redrawContext = [&]() {
        system("cls");
        displayTableHeader("MAKE DIAGNOSIS");
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n+=================================================================+" << endl;
        cout << "|                       PATIENT INFORMATION                       |" << endl;
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();
        cout << "| Patient Name : " << left << setw(48) << patientName << "|" << endl;
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();
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
            date = getSystemDate();
            break;
        }
        if (isValidDate(date)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD." << endl;
            pressEnterToContinue();
            redrawContext();
            cout << "Enter Disease: " << disease << endl;
            cout << "Enter Disorder: " << disorder << endl;
            cout << "Enter Duration of Pain: " << durationOfPain << endl;
            cout << "Enter Severity: " << severity << endl;
        }
    }

    // --- NEW: MEDICATION SEARCH LOOP ---
    struct PrescribedItem {
        string pharmacyId;
        string medName;
        string dosage;
        string duration;
        string instructions;
    };
    vector<PrescribedItem> prescriptionList;

    cout << "\n--- PRESCRIPTION SECTION ---" << endl;

    while (true) {
        string prompt = (prescriptionList.empty()) ?
            "Enter Medication Name (or Press Enter to skip): " :
            "Enter Next Medication Name (or Press Enter to finish): ";

        string searchName = getStringInput(prompt);

        if (searchName.empty()) break;

        string pharmacyQuery = "SELECT formatted_id, medicine_name, unit_price FROM pharmacy WHERE medicine_name LIKE '%" + searchName + "%' ORDER BY medicine_name";
        sql::ResultSet* pharmRes = db->executeSelect(pharmacyQuery);

        if (!pharmRes || pharmRes->rowsCount() == 0) {
            cout << "\n[INFO] No medication found matching \"" << searchName << "\". Try again." << endl;
            if (pharmRes) delete pharmRes;
            continue;
        }

        string selectedPharmId = "";
        string selectedMedName = "";

        if (pharmRes->rowsCount() == 1) {
            pharmRes->next();
            selectedPharmId = pharmRes->getString("formatted_id");
            selectedMedName = pharmRes->getString("medicine_name");
            cout << ">> Selected: " << selectedMedName << endl;
        }
        else {
            cout << "\nMultiple medications found:" << endl;
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "+-----+-------------+-----------------------------+" << endl;
            cout << "| No. | ID          | Medicine Name               |" << endl;
            cout << "+-----+-------------+-----------------------------+" << endl;
            ColorUtils::resetColor();

            vector<pair<string, string>> options;
            int idx = 1;
            while (pharmRes->next()) {
                string pId = pharmRes->getString("formatted_id");
                string pName = pharmRes->getString("medicine_name");
                options.push_back({ pId, pName });

                if (pName.length() > 27) pName = pName.substr(0, 24) + "...";
                cout << "| " << left << setw(4) << idx
                    << "| " << left << setw(12) << pId
                    << "| " << left << setw(28) << pName << "|" << endl;
                idx++;
            }
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "+-----+-------------+-----------------------------+" << endl;
            ColorUtils::resetColor();

            int selection = getIntInput("Select Number: ");
            if (selection > 0 && selection <= (int)options.size()) {
                selectedPharmId = options[selection - 1].first;
                selectedMedName = options[selection - 1].second;
                cout << ">> Selected: " << selectedMedName << endl;
            }
            else {
                cout << "[ERROR] Invalid selection. Try again." << endl;
                delete pharmRes;
                continue;
            }
        }
        delete pharmRes;

        string dosage = getStringInput("   Enter Dosage (e.g. 2 tabs): ");
        string duration = getStringInput("   Enter Duration (e.g. 5 days): ");
        string instr = getStringInput("   Enter Instructions: ");

        prescriptionList.push_back({ selectedPharmId, selectedMedName, dosage, duration, instr });
        cout << "✅ Added to list.\n" << endl;
    }

    // --- TREATMENT & FINANCIAL SECTION (FIXED) ---
    cout << "\n--- TREATMENT & BILLING ---" << endl;
    string dressing = getStringInput("Treatment/Procedure Description: ");
    if (dressing.empty()) dressing = "Standard Consultation";

    // Replaced getStringInput with getValidDouble to prevent text entry crash
    double consultFee = getValidDouble("Consultation Fee (RM): ");
    double treatFee = getValidDouble("Treatment Fee (RM): ");

    string treatmentQuery = "INSERT INTO treatment (formatted_id, patient_id, doctor_id, dressing_applied, consultation_fee, treatment_fee, treatment_date) "
        "VALUES (NULL, '" + patientId + "', '" + currentDoctorId + "', '" + dressing + "', " + to_string(consultFee) + ", " + to_string(treatFee) + ", '" + date + "')";
    bool treatmentInserted = db->executeUpdate(treatmentQuery);

    try {
        string firstPrescriptionId = "NULL";
        string allMedsSummary = "";

        for (const auto& item : prescriptionList) {
            string presQuery = "INSERT INTO prescription (formatted_id, pharmacy_id, dosage, duration_of_meds, instructions, date) "
                "VALUES (NULL, '" + item.pharmacyId + "', '" + item.dosage + "', '" + item.duration + "', '" + item.instructions + "', '" + date + "')";

            if (db->executeUpdate(presQuery)) {
                string getIdQuery = "SELECT formatted_id FROM prescription WHERE pharmacy_id = '" + item.pharmacyId + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                if (idRes && idRes->next()) {
                    string newId = idRes->getString("formatted_id");
                    if (firstPrescriptionId == "NULL") firstPrescriptionId = "'" + newId + "'";
                }
                if (idRes) delete idRes;

                if (!allMedsSummary.empty()) allMedsSummary += ", ";
                allMedsSummary += item.medName + "(" + item.dosage + ")";
            }
        }

        string diagQuery = "INSERT INTO diagnosis (formatted_id, disease, disorder, duration_of_pain, severity, prescription_id, date) "
            "VALUES (NULL, '" + disease + "', '" + disorder + "', '" + durationOfPain + "', '" + severity + "', " + firstPrescriptionId + ", '" + date + "')";

        if (db->executeUpdate(diagQuery)) {
            string getDiagIdQuery = "SELECT formatted_id FROM diagnosis WHERE disease = '" + disease + "' ORDER BY formatted_id DESC LIMIT 1";
            sql::ResultSet* diagRes = db->executeSelect(getDiagIdQuery);
            string diagnosisId = "";
            if (diagRes && diagRes->next()) {
                diagnosisId = string(diagRes->getString("formatted_id"));
            }
            if (diagRes) delete diagRes;

            if (!diagnosisId.empty()) {
                string finalNotes = "Diagnosis made by doctor.";
                if (!allMedsSummary.empty()) {
                    finalNotes += " Prescribed: " + allMedsSummary;
                }

                string recordQuery = "INSERT INTO medical_record (formatted_id, patient_id, doctor_id, diagnosis_id, date_of_record, notes) "
                    "VALUES (NULL, '" + patientId + "', '" + currentDoctorId + "', '" + diagnosisId + "', '" + date + "', '" + finalNotes + "')";

                if (db->executeUpdate(recordQuery)) {
                    cout << "\n✅ Diagnosis made successfully!" << endl;
                    if (treatmentInserted) cout << "✅ Treatment & Billing recorded." << endl;
                    if (!prescriptionList.empty()) cout << "✅ " << prescriptionList.size() << " medications prescribed." << endl;

                    ColorUtils::setColor(LIGHT_CYAN);
                    cout << "\n+=================================================================+" << endl;
                    cout << "|                        DIAGNOSIS DETAILS                        |" << endl;
                    cout << "+=================================================================+" << endl;
                    ColorUtils::resetColor();

                    cout << "| Patient Name    : " << left << setw(46) << patientName << "|" << endl;
                    cout << "| Disease         : " << left << setw(46) << (disease.empty() ? "N/A" : disease) << "|" << endl;
                    cout << "| Medications     : " << left << setw(46) << (allMedsSummary.empty() ? "None" : (allMedsSummary.length() > 46 ? allMedsSummary.substr(0, 43) + "..." : allMedsSummary)) << "|" << endl;
                    cout << "| Date            : " << left << setw(46) << (date == "CURDATE()" ? "Today" : date) << "|" << endl;
                    cout << "| Total Fee       : RM " << left << setw(43) << fixed << setprecision(2) << (consultFee + treatFee) << "|" << endl;

                    ColorUtils::setColor(LIGHT_CYAN);
                    cout << "+=================================================================+" << endl;
                    ColorUtils::resetColor();
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

// ---------------------------------------------------------
// EDIT RECORD
// ---------------------------------------------------------
void DoctorModule::editPatientMedicalRecord() {
    system("cls");
    displayTableHeader("EDIT PATIENT MEDICAL RECORD");

    string patientId = searchPatientId();
    if (patientId.empty()) {
        pressEnterToContinue();
        return;
    }

    try {
        string checkPatientQuery = "SELECT full_name FROM patient WHERE formatted_id = '" + patientId + "'";
        sql::ResultSet* checkPatientRes = db->executeSelect(checkPatientQuery);
        string patientName = "";
        if (checkPatientRes && checkPatientRes->next()) {
            patientName = checkPatientRes->getString("full_name");
            delete checkPatientRes;
        }

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

        delete recordRes;
        recordRes = db->executeSelect(recordQuery);

        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n+=================================================================+" << endl;
        cout << "|                       PATIENT INFORMATION                       |" << endl;
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();
        cout << "| Patient Name : " << left << setw(48) << patientName << "|" << endl;
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();

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
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n+=================================================================+" << endl;
            cout << "|                    CURRENT DIAGNOSIS DETAILS                    |" << endl;
            cout << "+=================================================================+" << endl;
            ColorUtils::resetColor();
            cout << "| Disease         : " << left << setw(46) << (currentDisease.empty() ? "N/A" : currentDisease) << "|" << endl;
            cout << "| Disorder        : " << left << setw(46) << (currentDisorder.empty() ? "N/A" : currentDisorder) << "|" << endl;
            cout << "| Duration of Pain: " << left << setw(46) << (currentDuration.empty() ? "N/A" : currentDuration) << "|" << endl;
            cout << "| Severity        : " << left << setw(46) << (currentSeverity.empty() ? "N/A" : currentSeverity) << "|" << endl;
            cout << "| Date            : " << left << setw(46) << (currentDate.empty() ? "N/A" : currentDate) << "|" << endl;
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "+=================================================================+" << endl;
            ColorUtils::resetColor();
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

                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\n+=================================================================+" << endl;
                cout << "|                     UPDATED MEDICAL RECORD                      |" << endl;
                cout << "+=================================================================+" << endl;
                ColorUtils::resetColor();

                cout << "| Patient Name    : " << left << setw(46) << patientName << "|" << endl;
                cout << "| Record ID       : " << left << setw(46) << recordId << "|" << endl;
                cout << "| Disease         : " << left << setw(46) << (disease.empty() ? "Not changed" : disease) << "|" << endl;
                cout << "| Disorder        : " << left << setw(46) << (disorder.empty() ? "Not changed" : disorder) << "|" << endl;
                cout << "| Duration of Pain: " << left << setw(46) << (durationOfPain.empty() ? "Not changed" : durationOfPain) << "|" << endl;
                cout << "| Severity        : " << left << setw(46) << (severity.empty() ? "Not changed" : severity) << "|" << endl;
                cout << "| Date            : " << left << setw(46) << (date.empty() ? "Not changed" : date) << "|" << endl;

                ColorUtils::setColor(LIGHT_CYAN);
                cout << "+=================================================================+" << endl;
                ColorUtils::resetColor();
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
    // Deprecated by Box view in viewPatientRecord, kept if needed for lists
}

void DoctorModule::displayMedicalRecordTable(sql::ResultSet* res) {
    // Widths: ID(11), Date(12), Disease(21), Disorder(21), Duration(15), Severity(15)
    // Separators: 11 | 12 | 21 | 21 | 15 | 15
    const string SEP = "+-----------+------------+---------------------+---------------------+---------------+---------------+";

    ColorUtils::setColor(LIGHT_CYAN);
    cout << "\n" << SEP << endl;
    cout << "| " << left << setw(10) << "Record ID"
        << "| " << left << setw(11) << "Date"
        << "| " << left << setw(20) << "Disease"
        << "| " << left << setw(20) << "Disorder"
        << "| " << left << setw(14) << "Duration"
        << "| " << left << setw(14) << "Severity" << "|" << endl;
    cout << SEP << endl;
    ColorUtils::resetColor();

    while (res->next()) {
        string recordId = string(res->getString("record_formatted_id"));
        string date = res->isNull("date_of_record") ? "N/A" : string(res->getString("date_of_record"));
        string disease = res->isNull("disease") ? "N/A" : string(res->getString("disease"));
        string disorder = res->isNull("disorder") ? "N/A" : string(res->getString("disorder"));
        string duration = res->isNull("duration_of_pain") ? "N/A" : string(res->getString("duration_of_pain"));
        string severity = res->isNull("severity") ? "N/A" : string(res->getString("severity"));

        // Strict Truncation Logic to keep table aligned
        if (disease.length() > 20) disease = disease.substr(0, 17) + "...";
        if (disorder.length() > 20) disorder = disorder.substr(0, 17) + "...";
        if (duration.length() > 14) duration = duration.substr(0, 11) + "..";
        if (severity.length() > 14) severity = severity.substr(0, 11) + "..";

        cout << "| " << left << setw(10) << recordId
            << "| " << left << setw(11) << date
            << "| " << left << setw(20) << disease
            << "| " << left << setw(20) << disorder
            << "| " << left << setw(14) << duration
            << "| " << left << setw(14) << severity << "|" << endl;
    }
    ColorUtils::setColor(LIGHT_CYAN);
    cout << SEP << endl;
    ColorUtils::resetColor();
}

void DoctorModule::displayTableHeader(const string& title) {
    ColorUtils::setColor(LIGHT_BLUE);
    cout << "+==============================================================================+" << endl;

    int paddingLeft = (78 - title.length()) / 2;
    int paddingRight = 78 - paddingLeft - title.length();

    cout << "|" << string(paddingLeft, ' ') << title << string(paddingRight, ' ') << "|" << endl;
    cout << "+==============================================================================+" << endl;
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
    try { return stoi(input); }
    catch (...) { return -1; }
}

string DoctorModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}
