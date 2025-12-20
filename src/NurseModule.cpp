#include "NurseModule.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

NurseModule::NurseModule(Database* database, int nurseId) : db(database), currentNurseId(nurseId) {}

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
            ColorUtils::setColor(YELLOW);
            cout << "\n❌ Invalid choice! Please try again." << endl;
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
        cout << "| Emergency Contact: " << left << setw(39) << (patientRes->isNull("emergency_contact") ? "N/A" : patientRes->getString("emergency_contact")) << "|" << endl;
        cout << "| Status: " << left << setw(49) << patientRes->getString("status") << "|" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        delete patientRes;

        // Display medical records (Nurse can view but not edit)
        string recordQuery = "SELECT mr.record_id, mr.date_of_record, d.disease, d.disorder, d.duration_of_pain, d.severity, mr.notes, doc.full_name as doctor_name "
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
                cout << "| " << setw(11) << recordRes->getInt("record_id")
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
        string appointmentQuery = "SELECT a.appointment_id, a.appointment_date, a.appointment_time, a.status, p.full_name as patient_name "
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
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void NurseModule::generateNextAppointment() {
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
    string patientStatus = checkRes->getString("status");
    delete checkRes;

    // Get available doctors
    string doctorQuery = "SELECT doctor_id, full_name, specialization FROM doctor WHERE status = 'Active' ORDER BY doctor_id";
    sql::ResultSet* doctorRes = db->executeSelect(doctorQuery);
    
    if (!doctorRes || doctorRes->rowsCount() == 0) {
        cout << "\n❌ No active doctors available!" << endl;
        if (doctorRes) delete doctorRes;
        pressEnterToContinue();
        return;
    }

    cout << "\nPatient: " << patientName << endl;
    cout << "\nAvailable Doctors:\n" << endl;
    cout << "+-----------+----------------------+----------------------+" << endl;
    cout << "| Doctor ID | Full Name            | Specialization       |" << endl;
    cout << "+-----------┼----------------------┼----------------------+" << endl;
    while (doctorRes->next()) {
        cout << "| " << setw(9) << doctorRes->getInt("doctor_id")
             << "| " << setw(20) << doctorRes->getString("full_name")
             << "| " << setw(20) << doctorRes->getString("specialization") << "|" << endl;
    }
    cout << "+-----------+----------------------+----------------------+" << endl;
    delete doctorRes;

    int doctorId = getIntInput("\nEnter Doctor ID: ");
    if (doctorId <= 0) {
        cout << "\n❌ Invalid Doctor ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Verify doctor exists
    string verifyDoctorQuery = "SELECT doctor_id FROM doctor WHERE doctor_id = " + to_string(doctorId) + " AND status = 'Active'";
    sql::ResultSet* verifyDoctorRes = db->executeSelect(verifyDoctorQuery);
    if (!verifyDoctorRes || !verifyDoctorRes->next()) {
        cout << "\n❌ Doctor not found or not active!" << endl;
        if (verifyDoctorRes) delete verifyDoctorRes;
        pressEnterToContinue();
        return;
    }
    delete verifyDoctorRes;

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
        string query = "INSERT INTO appointment (patient_id, nurse_id, doctor_id, appointment_date, appointment_time, status) "
            "VALUES (" + to_string(patientId) + ", " + to_string(currentNurseId) + ", " + to_string(doctorId) + ", '" + appointmentDate + "', '" + appointmentTime + "', 'Scheduled')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Next appointment generated successfully!" << endl;
            cout << "Patient: " << patientName << endl;
            cout << "Appointment Date: " << appointmentDate << endl;
            cout << "Appointment Time: " << appointmentTime << endl;
            
            // Update patient status: If next appointment exists -> Active, else Inactive
            string updateQuery = "UPDATE patient SET status = 'Active' WHERE patient_id = " + to_string(patientId);
            db->executeUpdate(updateQuery);
            
            cout << "\nPatient status updated to: Active" << endl;
            
            // Show how many times patient has visited
            string visitQuery = "SELECT COUNT(*) as visit_count FROM appointment WHERE patient_id = " + to_string(patientId);
            sql::ResultSet* visitRes = db->executeSelect(visitQuery);
            if (visitRes && visitRes->next()) {
                cout << "Total visits: " << visitRes->getInt("visit_count") << endl;
            }
            if (visitRes) delete visitRes;
        } else {
            cout << "\n❌ Failed to generate appointment!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void NurseModule::displayPatientRecordTable(sql::ResultSet* res) {
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

void NurseModule::displayAppointmentTable(sql::ResultSet* res) {
    cout << "\n+-----------------+----------------------+--------------+--------------+--------------+" << endl;
    cout << "| Appointment ID  | Patient Name          | Date         | Time         | Status       |" << endl;
    cout << "+-----------------┼----------------------┼--------------┼--------------┼--------------+" << endl;
    
    while (res->next()) {
        cout << "| " << setw(15) << res->getInt("appointment_id")
             << "| " << setw(20) << res->getString("patient_name")
             << "| " << setw(12) << res->getString("appointment_date")
             << "| " << setw(12) << res->getString("appointment_time")
             << "| " << setw(12) << res->getString("status") << "|" << endl;
    }
    
    cout << "+-----------------+----------------------+--------------+--------------+--------------+" << endl;
}

void NurseModule::displayTableHeader(const string& title) {
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



