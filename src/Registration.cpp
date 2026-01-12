#include "Registration.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"
#include <conio.h> // Required for _getch()
#include <cctype>
#include <algorithm>

Registration::Registration(Database* database) : db(database) {}

// Helper for masking password
string Registration::getMaskedInput() {
    string password;
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) { // Enter
            cout << endl;
            break;
        }
        else if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else {
            password += ch;
            cout << '*';
        }
    }
    return password;
}

// Complexity check
bool Registration::validatePasswordComplexity(const string& password) {
    if (password.length() < 8) return false;

    bool hasUpper = false;
    bool hasLower = false;
    bool hasSpecial = false;

    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        else if (islower(c)) hasLower = true;
        else if (!isalnum(c)) hasSpecial = true;
    }

    return hasUpper && hasLower && hasSpecial;
}

// --- HELPER FUNCTIONS FOR REDRAWING CONTEXT ---
void printContext(const string& title, const string& name, const string& gender = "", const string& ic = "", const string& contact = "", const string& extraLabel = "", const string& extraVal = "") {
    system("cls");
    // Simple header
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
    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

    if (!name.empty()) cout << "Enter Full Name: " << name << endl;
    if (!gender.empty()) cout << "Enter Gender (Male/Female): " << gender << endl;
    if (!ic.empty()) cout << "Enter IC Number (12 digits): " << ic << endl;
    if (!extraLabel.empty() && !extraVal.empty()) cout << extraLabel << ": " << extraVal << endl; // For Specialization or Email
    if (!contact.empty()) cout << "Enter Contact Number: " << contact << endl;
}

void Registration::showPreRegistrationMenu() {
    int choice;
    do {
        vector<string> menuOptions = {
            "Doctor",
            "Nurse",
            "Admin",
            "Back to Main Menu"
        };

        choice = MenuNavigator::showMenu(menuOptions, "PRE-REGISTRATION", true);

        if (choice == -1) {
            return;
        }

        switch (choice) {
        case 0: registerDoctor(); break;
        case 1: registerNurse(); break;
        case 2: registerAdmin(); break;
        case 3: return;
        default:
            ColorUtils::setColor(LIGHT_CYAN);
            cout << "\n[ERROR] Invalid choice! Please try again." << endl;
            ColorUtils::resetColor();
            pressEnterToContinue();
        }
    } while (choice != 3);
}

void Registration::registerDoctor() {
    string fullName, gender, specialization, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    // Helper lambda to easily redraw screen
    auto redraw = [&]() {
        printContext("DOCTOR REGISTRATION", fullName, gender, icNumber, contactNumber, "Enter Specialization", specialization);
        };

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("DOCTOR REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            // Full Name
            while (true) {
                cout << "Enter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") return;
                if (validateFullName(fullName)) break;

                cout << "\n[ERROR] Invalid name format. Use letters only." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("DOCTOR REGISTRATION");
                cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
            }

            // Gender
            while (true) {
                cout << "Enter Gender (Male/Female): ";
                getline(cin, gender);
                if (gender == "0") return;
                string corrected = validateAndCorrectGender(gender);
                if (corrected == "Male" || corrected == "Female") {
                    gender = corrected;
                    break;
                }

                cout << "\n[ERROR] Invalid gender. Enter 'Male', 'Female', 'M', or 'F'." << endl;
                pressEnterToContinue();
                printContext("DOCTOR REGISTRATION", fullName);
            }

            // IC Number
            while (true) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") return;
                if (validateICNumber(icNumber)) break;

                cout << "\n[ERROR] Invalid IC Number. Must be exactly 12 numeric digits." << endl;
                pressEnterToContinue();
                printContext("DOCTOR REGISTRATION", fullName, gender);
            }

            // Specialization
            while (true) {
                cout << "Enter Specialization: ";
                getline(cin, specialization);
                if (specialization == "0") return;
                if (!specialization.empty()) break;

                cout << "\n[ERROR] Cannot be empty." << endl;
                pressEnterToContinue();
                printContext("DOCTOR REGISTRATION", fullName, gender, icNumber);
            }

            // Contact
            while (true) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") return;
                if (validateContactNumber(contactNumber)) break;

                cout << "\n[ERROR] Invalid contact number. Must be 10-11 digits." << endl;
                pressEnterToContinue();
                printContext("DOCTOR REGISTRATION", fullName, gender, icNumber, "", "Enter Specialization", specialization);
            }

            // Username
            while (true) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") return;
                if (!username.empty()) break;

                cout << "\n[ERROR] Username cannot be empty." << endl;
                pressEnterToContinue();
                redraw();
                if (!contactNumber.empty()) cout << "Enter Contact Number: " << contactNumber << endl;
            }

            // Password
            while (true) {
                cout << "Create Password: ";
                password1 = getMaskedInput();
                if (password1 == "0") return;

                if (!validatePasswordComplexity(password1)) {
                    cout << "\n[ERROR] Password too weak!" << endl;
                    cout << "Requirements: At least 8 chars, 1 Uppercase, 1 Lowercase, 1 Special char." << endl;
                    pressEnterToContinue();
                    redraw();
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                    continue;
                }

                cout << "Re-enter Password: ";
                password2 = getMaskedInput();

                if (validatePassword(password1, password2)) break;

                cout << "\n[ERROR] Passwords do not match!" << endl;
                pressEnterToContinue();
                redraw();
                cout << "Enter Contact Number: " << contactNumber << endl;
                cout << "Create Username: " << username << endl;
            }

            // Insert into doctor table
            string query = "INSERT INTO doctor (formatted_id, full_name, gender, specialization, contact_number, ic_number, availability, status, role) "
                "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + specialization + "', '" + contactNumber + "', '" + icNumber + "', 'Available', 'Active', 'Doctor')";

            if (db->executeUpdate(query)) {
                string getIdQuery = "SELECT formatted_id FROM doctor WHERE full_name = '" + fullName + "' AND contact_number = '" + contactNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string doctorId = "";
                if (idRes && idRes->next()) doctorId = idRes->getString("formatted_id");
                if (idRes) delete idRes;

                if (!doctorId.empty()) {
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, doctor_id) VALUES (NULL, '" + username + "', '" + password1 + "', 'Doctor', '" + doctorId + "')";
                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Doctor registered successfully!" << endl;
                        registrationComplete = true;
                    }
                }
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }
    pressEnterToContinue();
}

void Registration::registerNurse() {
    string fullName, gender, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    auto redraw = [&]() {
        printContext("NURSE REGISTRATION", fullName, gender, icNumber, contactNumber);
        };

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("NURSE REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            // Full Name
            while (true) {
                cout << "Enter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") return;
                if (validateFullName(fullName)) break;

                cout << "\n[ERROR] Invalid name format. Use letters only." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("NURSE REGISTRATION");
                cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
            }

            // Gender
            while (true) {
                cout << "Enter Gender (Male/Female): ";
                getline(cin, gender);
                if (gender == "0") return;
                string corrected = validateAndCorrectGender(gender);
                if (corrected == "Male" || corrected == "Female") {
                    gender = corrected;
                    break;
                }

                cout << "\n[ERROR] Invalid gender. Enter 'Male', 'Female', 'M', or 'F'." << endl;
                pressEnterToContinue();
                printContext("NURSE REGISTRATION", fullName);
            }

            // IC Number
            while (true) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") return;
                if (validateICNumber(icNumber)) break;

                cout << "\n[ERROR] Invalid IC Number. Must be exactly 12 numeric digits." << endl;
                pressEnterToContinue();
                printContext("NURSE REGISTRATION", fullName, gender);
            }

            // Contact
            while (true) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") return;
                if (validateContactNumber(contactNumber)) break;

                cout << "\n[ERROR] Invalid contact number. Must be 10-11 digits." << endl;
                pressEnterToContinue();
                printContext("NURSE REGISTRATION", fullName, gender, icNumber);
            }

            // Username
            while (true) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") return;
                if (!username.empty()) break;

                cout << "\n[ERROR] Username cannot be empty." << endl;
                pressEnterToContinue();
                redraw();
            }

            // Password
            while (true) {
                cout << "Create Password: ";
                password1 = getMaskedInput();
                if (password1 == "0") return;

                if (!validatePasswordComplexity(password1)) {
                    cout << "\n[ERROR] Password too weak!" << endl;
                    cout << "Requirements: At least 8 chars, 1 Uppercase, 1 Lowercase, 1 Special char." << endl;
                    pressEnterToContinue();
                    redraw();
                    cout << "Create Username: " << username << endl;
                    continue;
                }

                cout << "Re-enter Password: ";
                password2 = getMaskedInput();

                if (validatePassword(password1, password2)) break;

                cout << "\n[ERROR] Passwords do not match!" << endl;
                pressEnterToContinue();
                redraw();
                cout << "Create Username: " << username << endl;
            }

            string query = "INSERT INTO nurse (formatted_id, full_name, gender, contact_number, ic_number, status, role) VALUES (NULL, '" + fullName + "', '" + gender + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Nurse')";

            if (db->executeUpdate(query)) {
                string getIdQuery = "SELECT formatted_id FROM nurse WHERE ic_number = '" + icNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string nurseId = "";
                if (idRes && idRes->next()) nurseId = idRes->getString("formatted_id");
                if (idRes) delete idRes;

                if (!nurseId.empty()) {
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, nurse_id) VALUES (NULL, '" + username + "', '" + password1 + "', 'Nurse', '" + nurseId + "')";
                    db->executeUpdate(loginQuery);
                    cout << "\n✅ Nurse registered successfully!" << endl;
                    registrationComplete = true;
                }
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }
    pressEnterToContinue();
}

void Registration::registerAdmin() {
    string fullName, email, contactNumber, password1, password2, username, icNumber;
    bool registrationComplete = false;

    auto redraw = [&]() {
        printContext("ADMIN REGISTRATION", fullName, "", icNumber, contactNumber, "Enter Email", email);
        };

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("ADMIN REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            // Full Name
            while (true) {
                cout << "Enter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") return;
                if (validateFullName(fullName)) break;

                cout << "\n[ERROR] Invalid name format. Use letters only." << endl;
                pressEnterToContinue();
                system("cls");
                displayTableHeader("ADMIN REGISTRATION");
                cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
            }

            // Email (Unique to Admin)
            while (true) {
                cout << "Enter Email: ";
                getline(cin, email);
                if (email == "0") return;
                if (!email.empty()) break; // Basic check

                cout << "\n[ERROR] Email cannot be empty." << endl;
                pressEnterToContinue();
                printContext("ADMIN REGISTRATION", fullName);
            }

            // IC Number
            while (true) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") return;
                if (validateICNumber(icNumber)) break;

                cout << "\n[ERROR] Invalid IC Number. Must be exactly 12 numeric digits." << endl;
                pressEnterToContinue();
                printContext("ADMIN REGISTRATION", fullName, "", "", "", "Enter Email", email);
            }

            // Contact
            while (true) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") return;
                if (validateContactNumber(contactNumber)) break;

                cout << "\n[ERROR] Invalid contact number. Must be 10-11 digits." << endl;
                pressEnterToContinue();
                printContext("ADMIN REGISTRATION", fullName, "", icNumber, "", "Enter Email", email);
            }

            // Username
            while (true) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") return;
                if (!username.empty()) break;

                cout << "\n[ERROR] Username cannot be empty." << endl;
                pressEnterToContinue();
                redraw();
            }

            // Password
            while (true) {
                cout << "Create Password: ";
                password1 = getMaskedInput();
                if (password1 == "0") return;

                if (!validatePasswordComplexity(password1)) {
                    cout << "\n[ERROR] Password too weak!" << endl;
                    cout << "Requirements: At least 8 chars, 1 Uppercase, 1 Lowercase, 1 Special char." << endl;
                    pressEnterToContinue();
                    redraw();
                    cout << "Create Username: " << username << endl;
                    continue;
                }

                cout << "Re-enter Password: ";
                password2 = getMaskedInput();

                if (validatePassword(password1, password2)) break;

                cout << "\n[ERROR] Passwords do not match!" << endl;
                pressEnterToContinue();
                redraw();
                cout << "Create Username: " << username << endl;
            }

            string query = "INSERT INTO admin (formatted_id, full_name, email, contact_number, ic_number, status, role) VALUES (NULL, '" + fullName + "', '" + email + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Admin')";

            if (db->executeUpdate(query)) {
                string getIdQuery = "SELECT formatted_id FROM admin WHERE ic_number = '" + icNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string adminId = "";
                if (idRes && idRes->next()) adminId = idRes->getString("formatted_id");
                if (idRes) delete idRes;

                if (!adminId.empty()) {
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, admin_id) VALUES (NULL, '" + username + "', '" + password1 + "', 'Admin', '" + adminId + "')";
                    db->executeUpdate(loginQuery);
                    cout << "\n✅ Admin registered successfully!" << endl;
                    registrationComplete = true;
                }
            }
        }
        catch (exception& e) {
            cout << "\n[ERROR] Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }
    pressEnterToContinue();
}

bool Registration::validatePassword(const string& password1, const string& password2) {
    return password1 == password2;
}

bool Registration::validateContactNumber(const string& contactNumber) {
    if (contactNumber.empty()) return false;
    for (char c : contactNumber) {
        if (!isdigit(c)) return false;
    }
    return (contactNumber.length() == 10 || contactNumber.length() == 11);
}

bool Registration::validateFullName(const string& fullName) {
    if (fullName.empty()) return false;
    for (char c : fullName) {
        if (!isalpha(c) && c != ' ' && c != '-') return false;
    }
    return true;
}

bool Registration::validateICNumber(const string& icNumber) {
    if (icNumber.empty()) return false;
    for (char c : icNumber) if (!isdigit(c)) return false;
    return icNumber.length() == 12;
}

string Registration::validateAndCorrectGender(string& gender) {
    string g = gender;
    // Lowercase for comparison
    transform(g.begin(), g.end(), g.begin(), ::tolower);

    if (g == "m" || g == "male") return "Male";
    if (g == "f" || g == "female") return "Female";
    return ""; // Invalid
}

void Registration::displayTableHeader(const string& title) {
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

void Registration::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

string Registration::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}
