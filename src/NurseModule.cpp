#include "NurseModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

// Helper: Validate YYYY-MM-DD
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

// Helper: Validate HH:MM
static bool isValidTime(const string& time) {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;
    if (!isdigit(time[0]) || !isdigit(time[1]) || !isdigit(time[3]) || !isdigit(time[4])) return false;
    int h = stoi(time.substr(0, 2));
    int m = stoi(time.substr(3, 2));
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

NurseModule::NurseModule(Database* database, const string& nurseId) : db(database), currentNurseId(nurseId) {}

void NurseModule::showMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "View Patient Record",
            "Generate Next Appointment",
            "Exit"
        };

        choice = MenuNavigator::showMenu(menuOptions, "NURSE MENU", true);

        if (choice == -1) {
            continue;
        }

        switch (choice) {
        case 0: viewPatientRecord(); break;
        case 1: generateNextAppointment(); break;
        case 2: return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (true);
}

void NurseModule::viewPatientRecord() {
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

        // ALIGNMENT FIX: Separator 64 chars. Label+Value+Borders=64.
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
        cout << "| Emergency Contact: " << left << setw(43) << (patientRes->isNull("emergency_contact") ? "N/A" : patientRes->getString("emergency_contact")) << "|" << endl;
        cout << "| Status: " << left << setw(54) << patientRes->getString("status") << "|" << endl;
        cout << "+================================================================+" << endl;
        delete patientRes;

        string recordQuery = "SELECT mr.formatted_id as record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes, doc.full_name as doctor_name "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "LEFT JOIN doctor doc ON mr.doctor_id = doc.formatted_id "
            "WHERE mr.patient_id = '" + patientId + "' "
            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);

        if (recordRes && recordRes->rowsCount() > 0) {
            cout << "\n" << endl;
            cout << "+================================================================+" << endl;
            cout << "|                      MEDICAL RECORDS                           |" << endl;
            cout << "+================================================================+" << endl;

            cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            cout << "| " << left << setw(11) << "Record ID"
                << "| " << left << setw(13) << "Date"
                << "| " << left << setw(21) << "Disease"
                << "| " << left << setw(21) << "Disorder"
                << "| " << left << setw(13) << "Duration"
                << "| " << left << setw(13) << "Severity"
                << "| " << left << setw(21) << "Doctor" << "|" << endl;
            cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;

            while (recordRes->next()) {
                string recordId = string(recordRes->getString("record_formatted_id"));
                string date = recordRes->isNull("date_of_record") ? "N/A" : string(recordRes->getString("date_of_record"));
                string disease = recordRes->isNull("disease") ? "N/A" : string(recordRes->getString("disease"));
                string disorder = recordRes->isNull("disorder") ? "N/A" : string(recordRes->getString("disorder"));
                string duration = recordRes->isNull("duration_of_pain") ? "N/A" : string(recordRes->getString("duration_of_pain"));
                string severity = recordRes->isNull("severity") ? "N/A" : string(recordRes->getString("severity"));
                string doctorName = recordRes->isNull("doctor_name") ? "N/A" : string(recordRes->getString("doctor_name"));

                if (disease.length() > 20) disease = disease.substr(0, 20);
                if (disorder.length() > 20) disorder = disorder.substr(0, 20);
                if (duration.length() > 12) duration = duration.substr(0, 12);
                if (severity.length() > 12) severity = severity.substr(0, 12);
                if (doctorName.length() > 20) doctorName = doctorName.substr(0, 20);

                cout << "| " << left << setw(11) << recordId
                    << "| " << left << setw(13) << date
                    << "| " << left << setw(21) << disease
                    << "| " << left << setw(21) << disorder
                    << "| " << left << setw(13) << duration
                    << "| " << left << setw(13) << severity
                    << "| " << left << setw(21) << doctorName << "|" << endl;
            }
            cout << "+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
        }
        else {
            cout << "\n⚠️  No medical records found for this patient." << endl;
        }
        if (recordRes) delete recordRes;

        string appointmentQuery = "SELECT a.formatted_id as appointment_formatted_id, a.appointment_date, a.appointment_time, a.status, p.full_name as patient_name "
            "FROM appointment a "
            "JOIN patient p ON a.patient_id = p.formatted_id "
            "WHERE a.patient_id = '" + patientId + "' "
            "ORDER BY a.appointment_date DESC";

        sql::ResultSet* appointmentRes = db->executeSelect(appointmentQuery);

        if (appointmentRes && appointmentRes->rowsCount() > 0) {
            cout << "\n" << endl;
            cout << "+================================================================+" << endl;
            cout << "|                      APPOINTMENTS                               |" << endl;
            cout << "+================================================================+" << endl;
            displayAppointmentTable(appointmentRes);
        }
        else {
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

    string patientId = getStringInput("Enter Patient ID (e.g., P001): ");
    if (patientId.empty()) {
        cout << "\n[ERROR] Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    string checkQuery = "SELECT full_name, status FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);

    if (!checkRes || !checkRes->next()) {
        cout << "\n[ERROR] Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }

    string patientName = checkRes->getString("full_name");
    delete checkRes;

    auto redrawContext = [&](const string& selectedDoctorId = "") {
        system("cls");
        displayTableHeader("GENERATE NEXT APPOINTMENT");
        cout << "\n" << endl;
        cout << "+================================================================+" << endl;
        cout << "|                      PATIENT INFORMATION                       |" << endl;
        cout << "+================================================================+" << endl;
        cout << "| Patient Name: " << left << setw(50) << patientName << "|" << endl;
        cout << "+================================================================+" << endl;

        if (!selectedDoctorId.empty()) {
            cout << "\nSelected Doctor ID: " << selectedDoctorId << endl;
        }
        };

    string doctorQuery = "SELECT formatted_id, full_name, specialization FROM doctor WHERE status = 'Active' ORDER BY formatted_id";
    sql::ResultSet* doctorRes = db->executeSelect(doctorQuery);

    if (!doctorRes || doctorRes->rowsCount() == 0) {
        cout << "\n[ERROR] No active doctors available!" << endl;
        if (doctorRes) delete doctorRes;
        pressEnterToContinue();
        return;
    }

    redrawContext("");
    cout << "\nAvailable Doctors:\n" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    cout << "| Doctor ID | Full Name            | Specialization       |" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;

    // ALIGNMENT FIX: Matches header setw exactly
    while (doctorRes->next()) {
        string docId = string(doctorRes->getString("formatted_id"));
        string docName = string(doctorRes->getString("full_name"));
        string spec = string(doctorRes->getString("specialization"));
        if (docName.length() > 20) docName = docName.substr(0, 17) + "...";
        if (spec.length() > 20) spec = spec.substr(0, 17) + "...";
        cout << "| " << left << setw(9) << docId
            << "| " << left << setw(20) << docName
            << "| " << left << setw(20) << spec << "|" << endl;
    }
    cout << "+-----------+----------------------+----------------------+" << endl;
    delete doctorRes;

    string doctorId = getStringInput("\nEnter Doctor ID (e.g., D001): ");
    if (doctorId.empty()) {
        cout << "\n[ERROR] Invalid Doctor ID!" << endl;
        pressEnterToContinue();
        return;
    }

    string verifyDoctorQuery = "SELECT formatted_id FROM doctor WHERE formatted_id = '" + doctorId + "' AND status = 'Active'";
    sql::ResultSet* verifyDoctorRes = db->executeSelect(verifyDoctorQuery);
    if (!verifyDoctorRes || !verifyDoctorRes->next()) {
        cout << "\n[ERROR] Doctor not found or not active!" << endl;
        if (verifyDoctorRes) delete verifyDoctorRes;
        pressEnterToContinue();
        return;
    }
    delete verifyDoctorRes;

    string appointmentDate;
    while (true) {
        appointmentDate = getStringInput("Enter Appointment Date (YYYY-MM-DD): ");
        if (appointmentDate.empty()) {
            cout << "\n[ERROR] Appointment date cannot be empty!" << endl;
            pressEnterToContinue();
            redrawContext(doctorId);
        }
        else if (isValidDate(appointmentDate)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use YYYY-MM-DD (e.g., 2024-05-20)." << endl;
            pressEnterToContinue();
            redrawContext(doctorId);
        }
    }

    string appointmentTime;
    while (true) {
        appointmentTime = getStringInput("Enter Appointment Time (HH:MM): ");
        if (appointmentTime.empty()) {
            appointmentTime = "09:00"; // Default
            break;
        }
        else if (isValidTime(appointmentTime)) {
            break;
        }
        else {
            cout << "\n[ERROR] Invalid format! Please use HH:MM (e.g., 14:30)." << endl;
            pressEnterToContinue();
            redrawContext(doctorId);
            cout << "Enter Appointment Date (YYYY-MM-DD): " << appointmentDate << endl;
        }
    }

    try {
        string clashQuery = "SELECT appointment_time FROM appointment "
            "WHERE doctor_id = '" + doctorId + "' "
            "AND appointment_date = '" + appointmentDate + "' "
            "AND ABS(TIME_TO_SEC(TIMEDIFF(appointment_time, '" + appointmentTime + "'))) < 3600";

        sql::ResultSet* clashRes = db->executeSelect(clashQuery);

        if (clashRes && clashRes->next()) {
            string existingTime = clashRes->getString("appointment_time");
            cout << "\n[ERROR] Appointment Clash Detected!" << endl;
            cout << "Doctor already has an appointment at " << existingTime << "." << endl;
            cout << "Appointments must be at least 1 hour apart." << endl;
            delete clashRes;
            pressEnterToContinue();
            return;
        }
        if (clashRes) delete clashRes;

        string query = "INSERT INTO appointment (formatted_id, patient_id, nurse_id, doctor_id, appointment_date, appointment_time, status) "
            "VALUES (NULL, '" + patientId + "', '" + currentNurseId + "', '" + doctorId + "', '" + appointmentDate + "', '" + appointmentTime + "', 'Scheduled')";

        if (db->executeUpdate(query)) {
            string updateQuery = "UPDATE patient SET status = 'Active' WHERE formatted_id = '" + patientId + "'";
            db->executeUpdate(updateQuery);

            string getDoctorQuery = "SELECT full_name FROM doctor WHERE formatted_id = '" + doctorId + "'";
            sql::ResultSet* doctorResName = db->executeSelect(getDoctorQuery);
            string doctorName = "N/A";
            if (doctorResName && doctorResName->next()) {
                doctorName = doctorResName->getString("full_name");
            }
            if (doctorResName) delete doctorResName;

            string visitQuery = "SELECT COUNT(*) as visit_count FROM appointment WHERE patient_id = '" + patientId + "'";
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
        }
        else {
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
    cout << "| " << left << setw(11) << "Patient ID"
        << "| " << left << setw(20) << "Full Name"
        << "| " << left << setw(8) << "Gender"
        << "| " << left << setw(12) << "Date of Birth"
        << "| " << left << setw(12) << "Status" << "|" << endl;
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;

    // ALIGNMENT FIX: Matches header setw exactly
    while (res->next()) {
        string patientId = string(res->getString("patient_formatted_id"));
        string fullName = string(res->getString("full_name"));
        string gender = string(res->getString("gender"));
        string dob = res->isNull("date_of_birth") ? "N/A" : string(res->getString("date_of_birth"));
        string status = string(res->getString("status"));

        if (fullName.length() > 20) fullName = fullName.substr(0, 17) + "...";

        cout << "| " << left << setw(11) << patientId
            << "| " << left << setw(20) << fullName
            << "| " << left << setw(8) << gender
            << "| " << left << setw(12) << dob
            << "| " << left << setw(12) << status << "|" << endl;
    }
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;
}

void NurseModule::displayAppointmentTable(sql::ResultSet* res) {
    cout << "\n+-----------------+----------------------+--------------+--------------+--------------+" << endl;
    cout << "| " << left << setw(15) << "Appointment ID"
        << "| " << left << setw(20) << "Patient Name"
        << "| " << left << setw(12) << "Date"
        << "| " << left << setw(12) << "Time"
        << "| " << left << setw(12) << "Status" << "|" << endl;
    cout << "+-----------------+----------------------+--------------+--------------+--------------+" << endl;

    // ALIGNMENT FIX: Matches header setw exactly
    while (res->next()) {
        string appointmentId = string(res->getString("appointment_formatted_id"));
        string patientName = string(res->getString("patient_name"));
        string date = res->isNull("appointment_date") ? "N/A" : string(res->getString("appointment_date"));
        string time = res->isNull("appointment_time") ? "N/A" : string(res->getString("appointment_time"));
        string status = string(res->getString("status"));

        if (patientName.length() > 20) patientName = patientName.substr(0, 17) + "...";

        cout << "| " << left << setw(15) << appointmentId
            << "| " << left << setw(20) << patientName
            << "| " << left << setw(12) << date
            << "| " << left << setw(12) << time
            << "| " << left << setw(12) << status << "|" << endl;
    }
    cout << "+-----------------+----------------------+--------------+--------------+--------------+" << endl;
}

void NurseModule::displayTableHeader(const string& title) {
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

void NurseModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int NurseModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try { return stoi(input); }
    catch (...) { return -1; }
}

string NurseModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}
