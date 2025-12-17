#include <iostream>
#include <windows.h>
#include <iomanip>
#include "Database.h"
#include "Registration.h"
#include "Login.h"
#include "AdminModule.h"
#include "DoctorModule.h"
#include "NurseModule.h"
#include "Reports.h"
#include "ColorUtils.h"

using namespace std;

void displayMainMenu() {
    system("cls");
    
    // Beautiful colorful header with highlighted title
    ColorUtils::setColor(CYAN);
    cout << "\n+----------------------------------------------------------------------+" << endl;
    cout << "|                                                                |" << endl;
    ColorUtils::resetColor();
    
    // Highlighted title with bright cyan background
    ColorUtils::printColoredBG("|              ", CYAN, BLACK);
    ColorUtils::printColoredBG("HOSPITAL MANAGEMENT SYSTEM", YELLOW, CYAN);
    ColorUtils::printColoredBG("                        |\n", CYAN, BLACK);
    
    ColorUtils::setColor(CYAN);
    cout << "|                                                                |" << endl;
    cout << "+----------------------------------------------------------------------+" << endl;
    ColorUtils::resetColor();
    
    // Colorful menu options
    ColorUtils::setColor(BLUE);
    cout << "\n+----------------------------------------+" << endl;
    ColorUtils::resetColor();
    
    ColorUtils::setColor(WHITE);
    cout << "|  ";
    ColorUtils::printColored("1. Registration", CYAN);
    ColorUtils::setColor(WHITE);
    cout << "                        |" << endl;
    
    cout << "|  ";
    ColorUtils::printColored("2. Login", CYAN);
    ColorUtils::setColor(WHITE);
    cout << "                              |" << endl;
    
    cout << "|  ";
    ColorUtils::printColored("0. Exit", RED);
    ColorUtils::setColor(WHITE);
    cout << "                               |" << endl;
    ColorUtils::resetColor();
    
    ColorUtils::setColor(BLUE);
    cout << "+----------------------------------------+" << endl;
    ColorUtils::resetColor();
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
    AdminModule* adminModule = nullptr;
    DoctorModule* doctorModule = nullptr;
    NurseModule* nurseModule = nullptr;
    Reports reports(&db);

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
                registration.showPreRegistrationMenu();
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
            // User is logged in - route to appropriate module based on role
            string role = login.getCurrentRole();
            int userId = login.getCurrentUserId();
            
            if (role == "Admin") {
                if (!adminModule) {
                    adminModule = new AdminModule(&db);
                }
                adminModule->showMenu();
                
                // After admin module returns, logout
                login.logout();
                loggedIn = false;
                delete adminModule;
                adminModule = nullptr;
                
            } else if (role == "Doctor") {
                if (!doctorModule) {
                    doctorModule = new DoctorModule(&db, userId);
                }
                doctorModule->showMenu();
                
                // After doctor module returns, logout
                login.logout();
                loggedIn = false;
                delete doctorModule;
                doctorModule = nullptr;
                
            } else if (role == "Nurse") {
                if (!nurseModule) {
                    nurseModule = new NurseModule(&db, userId);
                }
                nurseModule->showMenu();
                
                // After nurse module returns, logout
                login.logout();
                loggedIn = false;
                delete nurseModule;
                nurseModule = nullptr;
            } else {
                cout << "\n❌ Unknown role! Logging out..." << endl;
                login.logout();
                loggedIn = false;
            }
        }
    } while (mainChoice != 0);

    // Cleanup
    if (adminModule) delete adminModule;
    if (doctorModule) delete doctorModule;
    if (nurseModule) delete nurseModule;

    db.disconnect();
    return 0;
}
