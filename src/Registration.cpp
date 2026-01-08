#include "Registration.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"
#include "IdFormatter.h"

Registration::Registration(Database* database) : db(database) {}

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
            return; // ESC pressed
        }

        switch (choice) {
        case 0:
            registerDoctor();
            break;
        case 1:
            registerNurse();
            break;
        case 2:
            registerAdmin();
            break;
        case 3:
            return;
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

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("DOCTOR REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Gender
            while (!validInput) {
                cout << "Enter Gender (Male/Female or M/F): ";
                getline(cin, gender);
                if (gender == "0") {
                    return;
                }
                if (gender.empty()) {
                    cout << "\n[ERROR] Gender cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    string correctedGender = validateAndCorrectGender(gender);
                    if (correctedGender != "Male" && correctedGender != "Female") {
                        cout << "\n[ERROR] Gender must be Male or Female only! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                    } else {
                        gender = correctedGender; // Use corrected gender
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                } else {
                    // Check if IC number already exists in ANY table (doctor, nurse, admin, patient)
                    string checkICQuery = "SELECT 'Doctor' as user_type, formatted_id, full_name FROM doctor WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Nurse' as user_type, formatted_id, full_name FROM nurse WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Admin' as user_type, formatted_id, full_name FROM admin WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Patient' as user_type, formatted_id, full_name FROM patient WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        string userType = checkICRes->getString("user_type");
                        cout << "\n⚠️  This IC number is already registered!" << endl;
                        cout << "User Type: " << userType << endl;
                        cout << userType << " ID: " << string(checkICRes->getString("formatted_id")) << endl;
                        cout << userType << " Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Specialization
            while (!validInput) {
                cout << "Enter Specialization: ";
                getline(cin, specialization);
                if (specialization == "0") {
                    return;
                }
                if (specialization.empty()) {
                    cout << "\n[ERROR] Specialization cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists in login table
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Specialization: " << specialization << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("DOCTOR REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Specialization: " << specialization << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("DOCTOR REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Specialization: " << specialization << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into doctor table (formatted_id is auto-generated by trigger)
            string query = "INSERT INTO doctor (formatted_id, full_name, gender, specialization, contact_number, ic_number, availability, status, role) "
                "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + specialization + "', '" + contactNumber + "', '" + icNumber + "', 'Available', 'Active', 'Doctor')";

            if (db->executeUpdate(query)) {
                // Get the formatted_id that was just inserted
                string getIdQuery = "SELECT formatted_id FROM doctor WHERE full_name = '" + fullName + "' AND contact_number = '" + contactNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string doctorId = "";
                if (idRes && idRes->next()) {
                    doctorId = string(idRes->getString("formatted_id"));
                }
                if (idRes) delete idRes;

                if (!doctorId.empty()) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, doctor_id) "
                        "VALUES (NULL, '" + username + "', '" + password1 + "', 'Doctor', '" + doctorId + "')";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Doctor registered successfully!" << endl;
                        
                        // Display registered doctor in table format
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED DOCTOR DETAILS                  |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Specialization: " << left << setw(41) << specialization << "|" << endl;
                        cout << "| Contact Number: " << left << setw(41) << contactNumber << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Doctor created but login failed! Please contact admin." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve doctor ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register doctor!" << endl;
                pressEnterToContinue();
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

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("NURSE REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Gender
            while (!validInput) {
                cout << "Enter Gender (Male/Female or M/F): ";
                getline(cin, gender);
                if (gender == "0") {
                    return;
                }
                if (gender.empty()) {
                    cout << "\n[ERROR] Gender cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    string correctedGender = validateAndCorrectGender(gender);
                    if (correctedGender != "Male" && correctedGender != "Female") {
                        cout << "\n[ERROR] Gender must be Male or Female only! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                    } else {
                        gender = correctedGender; // Use corrected gender
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                } else {
                    // Check if IC number already exists in ANY table (doctor, nurse, admin, patient)
                    string checkICQuery = "SELECT 'Doctor' as user_type, formatted_id, full_name FROM doctor WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Nurse' as user_type, formatted_id, full_name FROM nurse WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Admin' as user_type, formatted_id, full_name FROM admin WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Patient' as user_type, formatted_id, full_name FROM patient WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        string userType = checkICRes->getString("user_type");
                        cout << "\n⚠️  This IC number is already registered!" << endl;
                        cout << "User Type: " << userType << endl;
                        cout << userType << " ID: " << string(checkICRes->getString("formatted_id")) << endl;
                        cout << userType << " Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("NURSE REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Gender: " << gender << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("NURSE REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Gender: " << gender << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into nurse table (formatted_id is auto-generated by trigger)
            string query = "INSERT INTO nurse (formatted_id, full_name, gender, contact_number, ic_number, status, role) "
                "VALUES (NULL, '" + fullName + "', '" + gender + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Nurse')";

            if (db->executeUpdate(query)) {
                // Get the formatted_id
                string getIdQuery = "SELECT formatted_id FROM nurse WHERE full_name = '" + fullName + "' AND contact_number = '" + contactNumber + "' ORDER BY formatted_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                string nurseId = "";
                if (idRes && idRes->next()) {
                    nurseId = string(idRes->getString("formatted_id"));
                }
                if (idRes) delete idRes;

                if (!nurseId.empty()) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (formatted_id, username, password, role, nurse_id) "
                        "VALUES (NULL, '" + username + "', '" + password1 + "', 'Nurse', '" + nurseId + "')";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Nurse registered successfully!" << endl;
                        
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED NURSE DETAILS                   |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Contact Number: " << left << setw(41) << contactNumber << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Nurse created but login failed! Please contact admin." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve nurse ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register nurse!" << endl;
                pressEnterToContinue();
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

    while (!registrationComplete) {
        system("cls");
        displayTableHeader("ADMIN REGISTRATION");
        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;

        try {
            bool validInput = false;
            
            // Full Name
            while (!validInput) {
                cout << "\nEnter Full Name: ";
                getline(cin, fullName);
                if (fullName == "0") {
                    return; // User wants to cancel
                }
                if (!validateFullName(fullName)) {
                    if (fullName.empty()) {
                        cout << "\n[ERROR] Full name cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Full name must contain only letters, spaces, and hyphens! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Email
            while (!validInput) {
                cout << "Enter Email: ";
                getline(cin, email);
                if (email == "0") {
                    return;
                }
                if (email.empty() || email.find('@') == string::npos || email.find(".com") == string::npos) {
                    cout << "\n[ERROR] Invalid email format! Email must contain '@' and '.com'. Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // IC Number
            while (!validInput) {
                cout << "Enter IC Number (12 digits): ";
                getline(cin, icNumber);
                if (icNumber == "0") {
                    return;
                }
                if (!validateICNumber(icNumber)) {
                    if (icNumber.empty()) {
                        cout << "\n[ERROR] IC number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] IC number must be exactly 12 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                } else {
                    // Check if IC number already exists in ANY table (doctor, nurse, admin, patient)
                    string checkICQuery = "SELECT 'Doctor' as user_type, formatted_id, full_name FROM doctor WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Nurse' as user_type, formatted_id, full_name FROM nurse WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Admin' as user_type, formatted_id, full_name FROM admin WHERE ic_number = '" + icNumber + "' "
                                         "UNION ALL "
                                         "SELECT 'Patient' as user_type, formatted_id, full_name FROM patient WHERE ic_number = '" + icNumber + "'";
                    sql::ResultSet* checkICRes = db->executeSelect(checkICQuery);
                    if (checkICRes && checkICRes->next()) {
                        string userType = checkICRes->getString("user_type");
                        cout << "\n⚠️  This IC number is already registered!" << endl;
                        cout << "User Type: " << userType << endl;
                        cout << userType << " ID: " << string(checkICRes->getString("formatted_id")) << endl;
                        cout << userType << " Name: " << checkICRes->getString("full_name") << endl;
                        if (checkICRes) delete checkICRes;
                        pressEnterToContinue();
                        return;
                    }
                    if (checkICRes) delete checkICRes;
                    validInput = true;
                }
            }

            validInput = false;
            // Contact Number
            while (!validInput) {
                cout << "Enter Contact Number: ";
                getline(cin, contactNumber);
                if (contactNumber == "0") {
                    return;
                }
                if (!validateContactNumber(contactNumber)) {
                    if (contactNumber.empty()) {
                        cout << "\n[ERROR] Contact number cannot be empty! Please try again." << endl;
                    } else {
                        cout << "\n[ERROR] Contact number must be 10 or 11 digits! Please try again." << endl;
                    }
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                } else {
                    validInput = true;
                }
            }

            validInput = false;
            // Username
            while (!validInput) {
                cout << "Create Username: ";
                getline(cin, username);
                if (username == "0") {
                    return;
                }
                if (username.empty()) {
                    cout << "\n[ERROR] Username cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                } else {
                    // Check if username exists
                    string checkQuery = "SELECT COUNT(*) as count FROM login WHERE username = '" + username + "'";
                    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
                    if (checkRes && checkRes->next() && checkRes->getInt("count") > 0) {
                        cout << "\n[ERROR] Username already exists! Please choose another." << endl;
                        if (checkRes) delete checkRes;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("ADMIN REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Email: " << email << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                    } else {
                        if (checkRes) delete checkRes;
                        validInput = true;
                    }
                }
            }

            validInput = false;
            // Password
            while (!validInput) {
                cout << "Create Password: ";
                getline(cin, password1);
                if (password1 == "0") {
                    return;
                }
                if (password1.empty()) {
                    cout << "\n[ERROR] Password cannot be empty! Please try again." << endl;
                    pressEnterToContinue();
                    system("cls");
                    displayTableHeader("ADMIN REGISTRATION");
                    cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                    cout << "\nEnter Full Name: " << fullName << endl;
                    cout << "Enter Email: " << email << endl;
                    cout << "Enter IC Number: " << icNumber << endl;
                    cout << "Enter Contact Number: " << contactNumber << endl;
                    cout << "Create Username: " << username << endl;
                } else {
                    cout << "Re-enter Password: ";
                    getline(cin, password2);
                    if (password2 == "0") {
                        return;
                    }

                    if (!validatePassword(password1, password2)) {
                        cout << "\n[ERROR] Passwords do not match! Please try again." << endl;
                        pressEnterToContinue();
                        system("cls");
                        displayTableHeader("ADMIN REGISTRATION");
                        cout << "\n⚠️  Note: Enter '0' at any time to cancel registration\n" << endl;
                        cout << "\nEnter Full Name: " << fullName << endl;
                        cout << "Enter Email: " << email << endl;
                        cout << "Enter IC Number: " << icNumber << endl;
                        cout << "Enter Contact Number: " << contactNumber << endl;
                        cout << "Create Username: " << username << endl;
                    } else {
                        validInput = true;
                    }
                }
            }

            // Insert into admin table
            string query = "INSERT INTO admin (full_name, email, contact_number, ic_number, status, role) "
                "VALUES ('" + fullName + "', '" + email + "', '" + contactNumber + "', '" + icNumber + "', 'Active', 'Admin')";

            if (db->executeUpdate(query)) {
                // Get the admin_id
                string getIdQuery = "SELECT admin_id FROM admin WHERE email = '" + email + "' ORDER BY admin_id DESC LIMIT 1";
                sql::ResultSet* idRes = db->executeSelect(getIdQuery);
                int adminId = -1;
                if (idRes && idRes->next()) {
                    adminId = idRes->getInt("admin_id");
                }
                if (idRes) delete idRes;

                if (adminId > 0) {
                    // Insert into login table
                    string loginQuery = "INSERT INTO login (username, password, role, admin_id) "
                        "VALUES ('" + username + "', '" + password1 + "', 'Admin', " + to_string(adminId) + ")";

                    if (db->executeUpdate(loginQuery)) {
                        cout << "\n✅ Admin registered successfully!" << endl;
                        
                        cout << "\n+----------------------------------------------------------------+" << endl;
                        cout << "|                    REGISTERED ADMIN DETAILS                     |" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        cout << "| Username: " << left << setw(47) << username << "|" << endl;
                        cout << "| Full Name: " << left << setw(46) << fullName << "|" << endl;
                        cout << "| Email: " << left << setw(50) << email << "|" << endl;
                        cout << "+----------------------------------------------------------------+" << endl;
                        registrationComplete = true;
                    } else {
                        cout << "\n⚠️  Admin created but login failed! Please contact system administrator." << endl;
                        registrationComplete = true;
                    }
                } else {
                    cout << "\n[ERROR] Failed to retrieve admin ID!" << endl;
                    pressEnterToContinue();
                }
            } else {
                cout << "\n[ERROR] Failed to register admin!" << endl;
                pressEnterToContinue();
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
    if (contactNumber.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (char c : contactNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    // Check if length is 10 or 11 digits
    if (contactNumber.length() != 10U && contactNumber.length() != 11U) {
        return false;
    }
    return true;
}

bool Registration::validateFullName(const string& fullName) {
    if (fullName.empty()) {
        return false;
    }
    // Check if all characters are letters or spaces
    for (char c : fullName) {
        if (!isalpha(c) && c != ' ' && c != '-') {
            return false;
        }
    }
    // Check if name has at least one letter
    bool hasLetter = false;
    for (char c : fullName) {
        if (isalpha(c)) {
            hasLetter = true;
            break;
        }
    }
    return hasLetter;
}

bool Registration::validateICNumber(const string& icNumber) {
    if (icNumber.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (char c : icNumber) {
        if (!isdigit(c)) {
            return false;
        }
    }
    // Check if length is exactly 12 digits
    if (icNumber.length() != 12U) {
        return false;
    }
    return true;
}

string Registration::validateAndCorrectGender(string& gender) {
    // Convert to lowercase for comparison
    string genderLower = gender;
    for (char& c : genderLower) {
        c = tolower(c);
    }
    
    // Auto-correct common inputs
    if (genderLower == "m" || genderLower == "male") {
        return "Male";
    } else if (genderLower == "f" || genderLower == "female") {
        return "Female";
    } else {
        // Return original if not valid
        return gender;
    }
}

void Registration::displayTableHeader(const string& title) {
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
