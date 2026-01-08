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
            "Exit"
        };
        
        choice = MenuNavigator::showMenu(menuOptions, "NURSE MENU", true);
        
        // ESC key - stay in menu, don't exit
        if (choice == -1) {
            continue;
        }

        switch (choice) {
        case 0:
            viewPatientRecord();
            break;
        case 1:
            generateNextAppointment();
            break;
        case 2:
            return; // Exit only when user explicitly chooses "Exit"
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (true); // Loop forever until user chooses Exit (case 2)
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

        // Safe truncation variables
        string id = string(patientRes->getString("formatted_id"));
        string name = patientRes->getString("full_name");
        string gender = patientRes->getString("gender");
        string dob = patientRes->getString("date_of_birth");
        string contact = patientRes->getString("contact_number");
        string blood = patientRes->isNull("blood_type") ? "N/A" : patientRes->getString("blood_type");
        string emergency = patientRes->isNull("emergency_contact") ? "N/A" : patientRes->getString("emergency_contact");
        string status = patientRes->getString("status");

        cout << "\n" << endl;
        cout << "+================================================================+" << endl;
        cout << "|                      PATIENT INFORMATION                       |" << endl;
        cout << "+================================================================+" << endl;
        cout << "| Patient ID: " << left << setw(51) << id.substr(0, 51) << "|" << endl;
        cout << "| Full Name: " << left << setw(52) << name.substr(0, 52) << "|" << endl;
        cout << "| Gender: " << left << setw(55) << gender.substr(0, 55) << "|" << endl;
        cout << "| Date of Birth: " << left << setw(48) << dob.substr(0, 48) << "|" << endl;
        cout << "| Contact Number: " << left << setw(47) << contact.substr(0, 47) << "|" << endl;
        cout << "| Blood Type: " << left << setw(51) << blood.substr(0, 51) << "|" << endl;
        cout << "| Emergency Contact: " << left << setw(44) << emergency.substr(0, 44) << "|" << endl;
        cout << "| Status: " << left << setw(55) << status.substr(0, 55) << "|" << endl;
        cout << "+================================================================+" << endl;
        delete patientRes;

        // Display medical records
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

            // Corrected Table Header (Widths increased by 1 to align with separator)
            cout << "\n+-------------+--------------+----------------------+----------------------+--------------+--------------+----------------------+" << endl;
            cout << "| " << left << setw(12) << "Record ID"
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

                // Truncate to fit new widths safely
                if (disease.length() > 20) disease = disease.substr(0, 20);
                if (disorder.length() > 20) disorder = disorder.substr(0, 20);
                if (duration.length() > 12) duration = duration.substr(0, 12);
                if (severity.length() > 12) severity = severity.substr(0, 12);
                if (doctorName.length() > 20) doctorName = doctorName.substr(0, 20);

                // Corrected Data Rows (Widths increased by 1)
                cout << "| " << left << setw(12) << recordId
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

        // Display appointments (reusing existing display function)
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

    // Verify patient exists
    string checkQuery = "SELECT full_name, status FROM patient WHERE formatted_id = '" + patientId + "'";
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
    string doctorQuery = "SELECT formatted_id, full_name, specialization FROM doctor WHERE status = 'Active' ORDER BY formatted_id";
    sql::ResultSet* doctorRes = db->executeSelect(doctorQuery);
    
    if (!doctorRes || doctorRes->rowsCount() == 0) {
        cout << "\n[ERROR] No active doctors available!" << endl;
        if (doctorRes) delete doctorRes;
        pressEnterToContinue();
        return;
    }

    cout << "\n" << endl;
    cout << "+================================================================+" << endl;
    cout << "|                      PATIENT INFORMATION                       |" << endl;
    cout << "+================================================================+" << endl;
    cout << "| Patient Name: " << left << setw(51) << patientName << "|" << endl;
    cout << "+================================================================+" << endl;
    cout << "\nAvailable Doctors:\n" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    cout << "| Doctor ID | Full Name            | Specialization       |" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    while (doctorRes->next()) {
        string doctorId = string(doctorRes->getString("formatted_id"));
        string doctorName = string(doctorRes->getString("full_name"));
        string specialization = string(doctorRes->getString("specialization"));
        
        // Truncate long text to fit column width
        if (doctorName.length() > 20) doctorName = doctorName.substr(0, 17) + "...";
        if (specialization.length() > 20) specialization = specialization.substr(0, 17) + "...";
        
        cout << "| " << left << setw(9) << doctorId
             << "| " << left << setw(20) << doctorName
             << "| " << left << setw(20) << specialization << "|" << endl;
    }
    cout << "+-----------+----------------------+----------------------+" << endl;
    delete doctorRes;

    string doctorId = getStringInput("\nEnter Doctor ID (e.g., D001): ");
    if (doctorId.empty()) {
        cout << "\n[ERROR] Invalid Doctor ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify doctor exists
    string verifyDoctorQuery = "SELECT formatted_id FROM doctor WHERE formatted_id = '" + doctorId + "' AND status = 'Active'";
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
        string query = "INSERT INTO appointment (formatted_id, patient_id, nurse_id, doctor_id, appointment_date, appointment_time, status) "
            "VALUES (NULL, '" + patientId + "', '" + currentNurseId + "', '" + doctorId + "', '" + appointmentDate + "', '" + appointmentTime + "', 'Scheduled')";

        if (db->executeUpdate(query)) {
            // Update patient status: If next appointment exists -> Active, else Inactive
            string updateQuery = "UPDATE patient SET status = 'Active' WHERE formatted_id = '" + patientId + "'";
            db->executeUpdate(updateQuery);
            
            // Get doctor name for display
            string getDoctorQuery = "SELECT full_name FROM doctor WHERE formatted_id = '" + doctorId + "'";
            sql::ResultSet* doctorRes = db->executeSelect(getDoctorQuery);
            string doctorName = "N/A";
            if (doctorRes && doctorRes->next()) {
                doctorName = doctorRes->getString("full_name");
            }
            if (doctorRes) delete doctorRes;
            
            // Show how many times patient has visited
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
    cout << "| " << left << setw(11) << "Patient ID"
         << "| " << left << setw(20) << "Full Name"
         << "| " << left << setw(8) << "Gender"
         << "| " << left << setw(12) << "Date of Birth"
         << "| " << left << setw(12) << "Status" << "|" << endl;
    cout << "+-------------+----------------------+----------+--------------+--------------+" << endl;
    
    while (res->next()) {
        string patientId = string(res->getString("patient_formatted_id"));
        string fullName = string(res->getString("full_name"));
        string gender = string(res->getString("gender"));
        string dob = res->isNull("date_of_birth") ? "N/A" : string(res->getString("date_of_birth"));
        string status = string(res->getString("status"));
        
        // Truncate long names to fit column width
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
    
    while (res->next()) {
        string appointmentId = string(res->getString("appointment_formatted_id"));
        string patientName = string(res->getString("patient_name"));
        string date = res->isNull("appointment_date") ? "N/A" : string(res->getString("appointment_date"));
        string time = res->isNull("appointment_time") ? "N/A" : string(res->getString("appointment_time"));
        string status = string(res->getString("status"));
        
        // Truncate long names to fit column width
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



