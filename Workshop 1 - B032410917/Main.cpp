#include <iostream>
#include <windows.h>
#include "Database.h"
#include "Registration.h"
#include "Login.h"
#include "AdminModule.h"
#include "StaffModule.h"
#include "Reports.h"

using namespace std;

void displayMainMenu() {
    system("cls");
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                                ║" << endl;
    cout << "║              HOSPITAL MANAGEMENT SYSTEM                        ║" << endl;
    cout << "║                                                                ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║  1. Registration                        ║" << endl;
    cout << "║  2. Login                              ║" << endl;
    cout << "║  0. Exit                               ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
}

int main() {
    // Set console to support UTF-8 characters
    SetConsoleOutputCP(65001);
    
    Database db;
    
    if (!db.connect()) {
        cout << "\n❌ Failed to connect to database!" << endl;
        cout << "Please ensure:" << endl;
        cout << "1. XAMPP MySQL is running" << endl;
        cout << "2. Database 'hospital_management_system' exists" << endl;
        cout << "3. Run database_schema.sql to create tables" << endl;
        cout << "\nPress Enter to exit...";
        cin.get();
        return 1;
    }

    Registration registration(&db);
    Login login(&db);
    AdminModule adminModule(&db);
    Reports reports(&db);
    StaffModule* staffModule = nullptr;

    int mainChoice;
    bool loggedIn = false;

    do {
        if (!loggedIn) {
            displayMainMenu();
            cout << "\nEnter your choice: ";
            cin >> mainChoice;
            cin.ignore();

            switch (mainChoice) {
            case 1:
                registration.showMenu();
                break;
            case 2:
                login.showLoginMenu();
                if (login.getCurrentUserId() > 0) {
                    loggedIn = true;
                }
                break;
            case 0:
                cout << "\nThank you for using Hospital Management System!" << endl;
                break;
            default:
                cout << "\n❌ Invalid choice! Please try again." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        } else {
            // User is logged in - show appropriate menu
            string userType = login.getCurrentUserType();
            
            if (userType == "admin") {
                int adminChoice;
                do {
                    system("cls");
                    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
                    cout << "║                    ADMIN DASHBOARD                            ║" << endl;
                    cout << "║                    Welcome, " << setw(30) << left << login.getCurrentUsername() << "║" << endl;
                    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
                    cout << "\n╔════════════════════════════════════════╗" << endl;
                    cout << "║  1. Admin Module                       ║" << endl;
                    cout << "║  2. Reports & Analytics               ║" << endl;
                    cout << "║  0. Logout                             ║" << endl;
                    cout << "╚════════════════════════════════════════╝" << endl;
                    cout << "\nEnter your choice: ";
                    cin >> adminChoice;
                    cin.ignore();
                    
                    switch (adminChoice) {
                    case 1:
                        adminModule.showMenu();
                        break;
                    case 2:
                        reports.showMenu();
                        break;
                    case 0:
                        login.logout();
                        loggedIn = false;
                        break;
                    default:
                        cout << "\n❌ Invalid choice!" << endl;
                        cout << "Press Enter to continue...";
                        cin.get();
                    }
                } while (adminChoice != 0 && loggedIn);
                
                // After admin module returns, ask if they want to logout
                cout << "\nDo you want to logout? (yes/no): ";
                string logoutChoice;
                getline(cin, logoutChoice);
                if (logoutChoice == "yes" || logoutChoice == "YES") {
                    login.logout();
                    loggedIn = false;
                }
            } else if (userType == "staff") {
                if (!staffModule) {
                    staffModule = new StaffModule(&db, login.getCurrentUserId());
                }
                
                system("cls");
                cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
                cout << "║                    STAFF DASHBOARD                            ║" << endl;
                cout << "║                    Welcome, " << setw(30) << left << login.getCurrentUsername() << "║" << endl;
                cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
                staffModule->showMenu();
                
                // After staff module returns, ask if they want to logout
                cout << "\nDo you want to logout? (yes/no): ";
                string logoutChoice;
                getline(cin, logoutChoice);
                if (logoutChoice == "yes" || logoutChoice == "YES") {
                    login.logout();
                    loggedIn = false;
                    delete staffModule;
                    staffModule = nullptr;
                }
            } else if (userType == "patient") {
                system("cls");
                cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
                cout << "║                    PATIENT DASHBOARD                           ║" << endl;
                cout << "║                    Welcome, " << setw(30) << left << login.getCurrentUsername() << "║" << endl;
                cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
                cout << "\nPatient module features coming soon!" << endl;
                cout << "\nDo you want to logout? (yes/no): ";
                string logoutChoice;
                getline(cin, logoutChoice);
                if (logoutChoice == "yes" || logoutChoice == "YES") {
                    login.logout();
                    loggedIn = false;
                }
            }
        }
    } while (mainChoice != 0);

    if (staffModule) {
        delete staffModule;
    }

    db.disconnect();
    return 0;
}
