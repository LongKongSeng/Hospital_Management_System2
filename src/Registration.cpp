#include "../include/Registration.h"

Registration::Registration(Database* database) : db(database) {}

void Registration::showMenu() {
    int choice;
    do {
        system("cls");
        displayTableHeader("REGISTRATION MODULE");
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║  1. Register Patient                  ║" << endl;
        cout << "║  2. Register Staff                    ║" << endl;
        cout << "║  3. Register Admin                    ║" << endl;
        cout << "║  0. Back to Main Menu                 ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            registerPatient();
            break;
        case 2:
            registerStaff();
            break;
        case 3:
            registerAdmin();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice! Please try again." << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void Registration::registerPatient() {
    system("cls");
    displayTableHeader("PATIENT REGISTRATION");

    string username, password, fullName, email, phone, dob, gender, address, emergencyContact, emergencyPhone, bloodType;

    try {
        cout << "\nEnter Username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "\n❌ Username cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }
        if (usernameExists(username, "patient")) {
            cout << "\n❌ Username already exists! Please choose another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Password: ";
        getline(cin, password);
        if (password.empty()) {
            cout << "\n❌ Password cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Full Name: ";
        getline(cin, fullName);
        if (fullName.empty()) {
            cout << "\n❌ Full name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Email: ";
        getline(cin, email);
        if (!validateEmail(email)) {
            cout << "\n❌ Invalid email format!" << endl;
            pressEnterToContinue();
            return;
        }
        if (emailExists(email, "patient")) {
            cout << "\n❌ Email already exists! Please use another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Phone: ";
        getline(cin, phone);
        if (!validatePhone(phone)) {
            cout << "\n❌ Invalid phone number!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Date of Birth (YYYY-MM-DD): ";
        getline(cin, dob);

        cout << "Enter Gender (Male/Female/Other): ";
        getline(cin, gender);

        cout << "Enter Address: ";
        getline(cin, address);

        cout << "Enter Emergency Contact Name: ";
        getline(cin, emergencyContact);

        cout << "Enter Emergency Contact Phone: ";
        getline(cin, emergencyPhone);

        cout << "Enter Blood Type (e.g., A+, B-, O+): ";
        getline(cin, bloodType);

        string query = "INSERT INTO patient (username, password, full_name, email, phone, date_of_birth, gender, address, emergency_contact, emergency_phone, blood_type) "
            "VALUES ('" + username + "', '" + password + "', '" + fullName + "', '" + email + "', '" + phone + "', ";
        
        if (dob.empty()) {
            query += "NULL, ";
        } else {
            query += "'" + dob + "', ";
        }

        if (gender.empty()) {
            query += "NULL, ";
        } else {
            query += "'" + gender + "', ";
        }

        query += "'" + address + "', '" + emergencyContact + "', '" + emergencyPhone + "', '" + bloodType + "')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Patient registered successfully!" << endl;
            
            // Display registered patient in table format
            cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
            cout << "║                    REGISTERED PATIENT DETAILS                  ║" << endl;
            cout << "╠════════════════════════════════════════════════════════════════╣" << endl;
            cout << "║ Username: " << left << setw(47) << username << "║" << endl;
            cout << "║ Full Name: " << left << setw(46) << fullName << "║" << endl;
            cout << "║ Email: " << left << setw(50) << email << "║" << endl;
            cout << "║ Phone: " << left << setw(50) << phone << "║" << endl;
            cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        } else {
            cout << "\n❌ Failed to register patient!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void Registration::registerStaff() {
    system("cls");
    displayTableHeader("STAFF REGISTRATION");

    string username, password, fullName, email, phone, position, department;

    try {
        cout << "\nEnter Username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "\n❌ Username cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }
        if (usernameExists(username, "staff")) {
            cout << "\n❌ Username already exists! Please choose another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Password: ";
        getline(cin, password);
        if (password.empty()) {
            cout << "\n❌ Password cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Full Name: ";
        getline(cin, fullName);
        if (fullName.empty()) {
            cout << "\n❌ Full name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Email: ";
        getline(cin, email);
        if (!validateEmail(email)) {
            cout << "\n❌ Invalid email format!" << endl;
            pressEnterToContinue();
            return;
        }
        if (emailExists(email, "staff")) {
            cout << "\n❌ Email already exists! Please use another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Phone: ";
        getline(cin, phone);
        if (!validatePhone(phone)) {
            cout << "\n❌ Invalid phone number!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Position: ";
        getline(cin, position);
        if (position.empty()) {
            cout << "\n❌ Position cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Department: ";
        getline(cin, department);

        string query = "INSERT INTO staff (username, password, full_name, email, phone, position, department) "
            "VALUES ('" + username + "', '" + password + "', '" + fullName + "', '" + email + "', '" + phone + "', '" + position + "', '" + department + "')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Staff registered successfully!" << endl;
            
            // Display registered staff in table format
            cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
            cout << "║                     REGISTERED STAFF DETAILS                    ║" << endl;
            cout << "╠════════════════════════════════════════════════════════════════╣" << endl;
            cout << "║ Username: " << left << setw(47) << username << "║" << endl;
            cout << "║ Full Name: " << left << setw(46) << fullName << "║" << endl;
            cout << "║ Email: " << left << setw(50) << email << "║" << endl;
            cout << "║ Position: " << left << setw(47) << position << "║" << endl;
            cout << "║ Department: " << left << setw(45) << department << "║" << endl;
            cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        } else {
            cout << "\n❌ Failed to register staff!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void Registration::registerAdmin() {
    system("cls");
    displayTableHeader("ADMIN REGISTRATION");

    string username, password, fullName, email, phone;

    try {
        cout << "\nEnter Username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "\n❌ Username cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }
        if (usernameExists(username, "admin")) {
            cout << "\n❌ Username already exists! Please choose another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Password: ";
        getline(cin, password);
        if (password.empty()) {
            cout << "\n❌ Password cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Full Name: ";
        getline(cin, fullName);
        if (fullName.empty()) {
            cout << "\n❌ Full name cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Email: ";
        getline(cin, email);
        if (!validateEmail(email)) {
            cout << "\n❌ Invalid email format!" << endl;
            pressEnterToContinue();
            return;
        }
        if (emailExists(email, "admin")) {
            cout << "\n❌ Email already exists! Please use another." << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Phone: ";
        getline(cin, phone);
        if (!validatePhone(phone)) {
            cout << "\n❌ Invalid phone number!" << endl;
            pressEnterToContinue();
            return;
        }

        string query = "INSERT INTO admin (username, password, full_name, email, phone) "
            "VALUES ('" + username + "', '" + password + "', '" + fullName + "', '" + email + "', '" + phone + "')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Admin registered successfully!" << endl;
            
            // Display registered admin in table format
            cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
            cout << "║                    REGISTERED ADMIN DETAILS                     ║" << endl;
            cout << "╠════════════════════════════════════════════════════════════════╣" << endl;
            cout << "║ Username: " << left << setw(47) << username << "║" << endl;
            cout << "║ Full Name: " << left << setw(46) << fullName << "║" << endl;
            cout << "║ Email: " << left << setw(50) << email << "║" << endl;
            cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        } else {
            cout << "\n❌ Failed to register admin!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

bool Registration::validateEmail(const string& email) {
    if (email.empty()) return false;
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return (atPos != string::npos && dotPos != string::npos && dotPos > atPos);
}

bool Registration::validatePhone(const string& phone) {
    if (phone.empty()) return false;
    for (char c : phone) {
        if (!isdigit(c) && c != '-' && c != ' ' && c != '(' && c != ')') {
            return false;
        }
    }
    return true;
}

bool Registration::usernameExists(const string& username, const string& table) {
    string query = "SELECT COUNT(*) as count FROM " + table + " WHERE username = '" + username + "'";
    sql::ResultSet* res = db->executeSelect(query);
    if (res && res->next()) {
        int count = res->getInt("count");
        delete res;
        return count > 0;
    }
    return false;
}

bool Registration::emailExists(const string& email, const string& table) {
    string query = "SELECT COUNT(*) as count FROM " + table + " WHERE email = '" + email + "'";
    sql::ResultSet* res = db->executeSelect(query);
    if (res && res->next()) {
        int count = res->getInt("count");
        delete res;
        return count > 0;
    }
    return false;
}

void Registration::displayTableHeader(const string& title) {
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "║" << setw((60 - title.length()) / 2 + title.length()) << right << title 
         << setw((60 - title.length()) / 2) << "" << "║" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
}

void Registration::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

