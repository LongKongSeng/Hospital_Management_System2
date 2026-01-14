#include "NurseModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <iomanip>
#include <iostream>
#include <string>

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

// Helper: Display a centered title box of a specific width
static void displayBoxHeader(const string& title, int width) {
    ColorUtils::setColor(LIGHT_CYAN);
    cout << "\n+";
    for (int i = 0; i < width - 2; ++i) cout << "=";
    cout << "+" << endl;

    int paddingLeft = (width - 2 - title.length()) / 2;
    int paddingRight = width - 2 - paddingLeft - title.length();

    cout << "| " << string(paddingLeft - 1, ' ') << title << string(paddingRight - 1, ' ') << " |" << endl;

    cout << "+";
    for (int i = 0; i < width - 2; ++i) cout << "=";
    cout << "+" << endl;
    ColorUtils::resetColor();
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

// ---------------------------------------------------------
// SEARCH HELPERS (Fixed Alignment: Dashes = setw + 2)
// ---------------------------------------------------------
string NurseModule::searchPatientId() {
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

        // FIXED: Dashes match (setw + 2)
        // No(4)->6, ID(10)->12, Name(26)->28, IC(15)->17
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+------+------------+----------------------------+-----------------+" << endl;
        cout << "| No.  | ID         | Name                       | IC Number       |" << endl;
        cout << "+------+------------+----------------------------+-----------------+" << endl;
        ColorUtils::resetColor();

        int index = 1;
        while (res->next()) {
            string pId = res->getString("formatted_id");
            string pName = res->getString("full_name");
            string pIc = res->getString("ic_number");
            options.push_back({ pId, pName, pIc });

            if (pName.length() > 27) pName = pName.substr(0, 24) + "...";

            cout << "| " << left << setw(4) << index
                << " | " << left << setw(10) << pId
                << " | " << left << setw(26) << pName
                << " | " << left << setw(15) << pIc << " |" << endl;
            index++;
        }
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+------+------------+----------------------------+-----------------+" << endl;
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

string NurseModule::searchDoctorId() {
    string input = getStringInput("Enter Doctor Name, IC Number, or ID: ");
    if (input.empty()) return "";

    string query = "SELECT formatted_id, full_name, specialization, ic_number FROM doctor WHERE "
        "status = 'Active' AND ("
        "full_name LIKE '%" + input + "%' OR "
        "ic_number LIKE '%" + input + "%' OR "
        "formatted_id = '" + input + "')";

    sql::ResultSet* res = db->executeSelect(query);

    if (!res || res->rowsCount() == 0) {
        cout << "\n[INFO] No active doctors found matching \"" << input << "\"." << endl;
        if (res) delete res;
        return "";
    }

    string selectedId = "";

    if (res->rowsCount() == 1) {
        res->next();
        selectedId = res->getString("formatted_id");
    }
    else {
        cout << "\n[INFO] Multiple doctors found. Please select one:\n" << endl;
        struct DocOpt { string id; string name; string spec; };
        vector<DocOpt> options;

        // FIXED: Dashes match (setw + 2)
        // No(4)->6, ID(10)->12, Name(26)->28, Spec(21)->23
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+------+------------+----------------------------+-----------------------+" << endl;
        cout << "| No.  | ID         | Name                       | Specialization        |" << endl;
        cout << "+------+------------+----------------------------+-----------------------+" << endl;
        ColorUtils::resetColor();

        int index = 1;
        while (res->next()) {
            string dId = res->getString("formatted_id");
            string dName = res->getString("full_name");
            string dSpec = res->getString("specialization");
            options.push_back({ dId, dName, dSpec });

            if (dName.length() > 27) dName = dName.substr(0, 24) + "...";
            if (dSpec.length() > 20) dSpec = dSpec.substr(0, 17) + "...";

            cout << "| " << left << setw(4) << index
                << " | " << left << setw(10) << dId
                << " | " << left << setw(26) << dName
                << " | " << left << setw(21) << dSpec << " |" << endl;
            index++;
        }
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+------+------------+----------------------------+-----------------------+" << endl;
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
// VIEW PATIENT RECORD
// ---------------------------------------------------------
void NurseModule::viewPatientRecord() {
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
            cout << "\n[ERROR] Patient not found!" << endl;
            if (patientRes) delete patientRes;
            pressEnterToContinue();
            return;
        }

        // --- PATIENT INFO BOX ---
        string pId = string(patientRes->getString("formatted_id"));
        string pName = patientRes->getString("full_name");
        string pGender = patientRes->getString("gender");
        string pDob = patientRes->getString("date_of_birth");
        string pContact = patientRes->getString("contact_number");
        string pBlood = patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type");
        string pEmerg = patientRes->isNull("emergency_contact") ? "N/A" : patientRes->getString("emergency_contact");
        string pStatus = patientRes->getString("status");
        delete patientRes;

        // FIXED: Box Width = 65 chars
        displayBoxHeader("PATIENT INFORMATION", 65);

        cout << "| Patient ID      : " << left << setw(43) << pId << " |" << endl;
        cout << "| Full Name       : " << left << setw(43) << pName << " |" << endl;
        cout << "| Gender          : " << left << setw(43) << pGender << " |" << endl;
        cout << "| Date of Birth   : " << left << setw(43) << pDob << " |" << endl;
        cout << "| Contact Number  : " << left << setw(43) << pContact << " |" << endl;
        cout << "| Blood Type      : " << left << setw(43) << pBlood << " |" << endl;
        cout << "| Emergency Cont. : " << left << setw(43) << pEmerg << " |" << endl;
        cout << "| Status          : " << left << setw(43) << pStatus << " |" << endl;

        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();


        string recordQuery = "SELECT mr.formatted_id as record_formatted_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes, doc.full_name as doctor_name "
            "FROM medical_record mr "
            "LEFT JOIN diagnosis d ON mr.diagnosis_id = d.formatted_id "
            "LEFT JOIN doctor doc ON mr.doctor_id = doc.formatted_id "
            "WHERE mr.patient_id = '" + patientId + "' "
            "ORDER BY mr.date_of_record DESC";

        sql::ResultSet* recordRes = db->executeSelect(recordQuery);

        if (recordRes && recordRes->rowsCount() > 0) {
            // FIXED: Box Width = 132 chars (Matches table width)
            // Table Width: 1+13+1+15+1+22+1+22+1+15+1+15+1+22+1 = 132
            displayBoxHeader("MEDICAL RECORDS", 132);

            // Widths: ID(11), Date(13), Disease(20), Disorder(20), Duration(13), Severity(13), Doctor(20)
            const string SEP = "+-------------+---------------+----------------------+----------------------+---------------+---------------+----------------------+";

            ColorUtils::setColor(LIGHT_CYAN);
            // No leading endl, flush with box
            cout << SEP << endl;
            cout << "| " << left << setw(11) << "Record ID"
                << " | " << left << setw(13) << "Date"
                << " | " << left << setw(20) << "Disease"
                << " | " << left << setw(20) << "Disorder"
                << " | " << left << setw(13) << "Duration"
                << " | " << left << setw(13) << "Severity"
                << " | " << left << setw(20) << "Doctor" << " |" << endl;
            cout << SEP << endl;
            ColorUtils::resetColor();

            while (recordRes->next()) {
                string recordId = string(recordRes->getString("record_formatted_id"));
                string date = recordRes->isNull("date_of_record") ? "N/A" : string(recordRes->getString("date_of_record"));
                string disease = recordRes->isNull("disease") ? "N/A" : string(recordRes->getString("disease"));
                string disorder = recordRes->isNull("disorder") ? "N/A" : string(recordRes->getString("disorder"));
                string duration = recordRes->isNull("duration_of_pain") ? "N/A" : string(recordRes->getString("duration_of_pain"));
                string severity = recordRes->isNull("severity") ? "N/A" : string(recordRes->getString("severity"));
                string doctorName = recordRes->isNull("doctor_name") ? "N/A" : string(recordRes->getString("doctor_name"));

                if (disease.length() > 20) disease = disease.substr(0, 17) + "..";
                if (disorder.length() > 20) disorder = disorder.substr(0, 17) + "..";
                if (duration.length() > 13) duration = duration.substr(0, 10) + "..";
                if (severity.length() > 13) severity = severity.substr(0, 10) + "..";
                if (doctorName.length() > 20) doctorName = doctorName.substr(0, 17) + "..";

                cout << "| " << left << setw(11) << recordId
                    << " | " << left << setw(13) << date
                    << " | " << left << setw(20) << disease
                    << " | " << left << setw(20) << disorder
                    << " | " << left << setw(13) << duration
                    << " | " << left << setw(13) << severity
                    << " | " << left << setw(20) << doctorName << " |" << endl;
            }
            ColorUtils::setColor(LIGHT_CYAN);
            cout << SEP << endl;
            ColorUtils::resetColor();
        }
        else {
            cout << "\n[INFO] No medical records found for this patient." << endl;
        }
        if (recordRes) delete recordRes;

        string appointmentQuery = "SELECT a.formatted_id as appointment_formatted_id, a.appointment_date, a.appointment_time, a.status, p.full_name as patient_name "
            "FROM appointment a "
            "JOIN patient p ON a.patient_id = p.formatted_id "
            "WHERE a.patient_id = '" + patientId + "' "
            "ORDER BY a.appointment_date DESC";

        sql::ResultSet* appointmentRes = db->executeSelect(appointmentQuery);

        if (appointmentRes && appointmentRes->rowsCount() > 0) {
            // FIXED: Box Width = 87 chars (Matches table width)
            // Table Width: 1+17+1+22+1+14+1+14+1+14+1 = 86? Let's check.
            // ID(15)+2=17. Name(20)+2=22. Date(12)+2=14. Time(12)+2=14. Status(12)+2=14.
            // 17+22+14+14+14 = 81. + 6 pipes = 87 chars.
            displayBoxHeader("APPOINTMENTS", 87);

            displayAppointmentTable(appointmentRes);
        }
        else {
            cout << "\n[INFO] No appointments found for this patient." << endl;
        }
        if (appointmentRes) delete appointmentRes;
    }
    catch (exception& e) {
        cout << "\n[ERROR] Error: " << e.what() << endl;
    }
    pressEnterToContinue();
}

// ---------------------------------------------------------
// GENERATE APPOINTMENT
// ---------------------------------------------------------
void NurseModule::generateNextAppointment() {
    system("cls");
    displayTableHeader("GENERATE NEXT APPOINTMENT");

    // 1. Search Patient
    string patientId = searchPatientId();

    if (patientId.empty()) {
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

        displayBoxHeader("PATIENT INFORMATION", 65);
        cout << "| Patient Name : " << left << setw(46) << patientName << " |" << endl;
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "+=================================================================+" << endl;
        ColorUtils::resetColor();

        if (!selectedDoctorId.empty()) {
            cout << "\nSelected Doctor ID: " << selectedDoctorId << endl;
        }
        };

    redrawContext("");

    // 2. Search Doctor
    cout << "\n[SELECT DOCTOR]" << endl;
    string doctorId = searchDoctorId();

    if (doctorId.empty()) {
        pressEnterToContinue();
        return;
    }

    redrawContext(doctorId);

    // 3. Enter Date
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

    // 4. Enter Time (Loop until valid & no clash)
    string appointmentTime;

    try {
        while (true) {
            appointmentTime = getStringInput("Enter Appointment Time (HH:MM): ");

            if (appointmentTime.empty()) {
                appointmentTime = "09:00";
            }

            if (!isValidTime(appointmentTime)) {
                cout << "\n[ERROR] Invalid format! Please use HH:MM (e.g., 14:30)." << endl;
                pressEnterToContinue();
                redrawContext(doctorId);
                cout << "Enter Appointment Date (YYYY-MM-DD): " << appointmentDate << endl;
                continue;
            }

            // Clash Check
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
                redrawContext(doctorId);
                cout << "Enter Appointment Date (YYYY-MM-DD): " << appointmentDate << endl;
                continue;
            }

            if (clashRes) delete clashRes;
            break;
        }

        // 5. Insert Appointment
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

            displayBoxHeader("APPOINTMENT DETAILS", 65);
            cout << "| Patient Name    : " << left << setw(44) << patientName << " |" << endl;
            cout << "| Doctor Name     : " << left << setw(44) << doctorName << " |" << endl;
            cout << "| Appointment Date: " << left << setw(44) << appointmentDate << " |" << endl;
            cout << "| Appointment Time: " << left << setw(44) << appointmentTime << " |" << endl;
            cout << "| Patient Status  : " << left << setw(44) << "Active" << " |" << endl;
            cout << "| Total Visits    : " << left << setw(44) << visitCount << " |" << endl;

            ColorUtils::setColor(LIGHT_CYAN);
            cout << "+=================================================================+" << endl;
            ColorUtils::resetColor();
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
    // Deprecated by Box View
}

void NurseModule::displayAppointmentTable(sql::ResultSet* res) {
    // Widths: ID(15), Name(20), Date(12), Time(12), Status(12)
    // Dashes: 17, 22, 14, 14, 14
    const string SEP = "+-----------------+----------------------+--------------+--------------+--------------+";

    ColorUtils::setColor(LIGHT_CYAN);
    // No top newline, flush with box
    cout << SEP << endl;
    cout << "| " << left << setw(15) << "Appointment ID"
        << " | " << left << setw(20) << "Patient Name"
        << " | " << left << setw(12) << "Date"
        << " | " << left << setw(12) << "Time"
        << " | " << left << setw(12) << "Status" << " |" << endl;
    cout << SEP << endl;
    ColorUtils::resetColor();

    while (res->next()) {
        string appointmentId = string(res->getString("appointment_formatted_id"));
        string patientName = string(res->getString("patient_name"));
        string date = res->isNull("appointment_date") ? "N/A" : string(res->getString("appointment_date"));
        string time = res->isNull("appointment_time") ? "N/A" : string(res->getString("appointment_time"));
        string status = string(res->getString("status"));

        if (patientName.length() > 20) patientName = patientName.substr(0, 17) + "...";
        if (time.length() > 5) time = time.substr(0, 5);

        cout << "| " << left << setw(15) << appointmentId
            << " | " << left << setw(20) << patientName
            << " | " << left << setw(12) << date
            << " | " << left << setw(12) << time
            << " | " << left << setw(12) << status << " |" << endl;
    }

    ColorUtils::setColor(LIGHT_CYAN);
    cout << SEP << endl;
    ColorUtils::resetColor();
}

void NurseModule::displayTableHeader(const string& title) {
    ColorUtils::setColor(LIGHT_BLUE);
    cout << "+==============================================================================+" << endl;
    int paddingLeft = (78 - title.length()) / 2;
    int paddingRight = 78 - paddingLeft - title.length();
    cout << "|" << string(paddingLeft, ' ') << title << string(paddingRight, ' ') << "|" << endl;
    cout << "+==============================================================================+" << endl;
    ColorUtils::resetColor();
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
