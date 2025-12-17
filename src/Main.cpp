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
#include "MenuNavigator.h"

using namespace std;

int main() {
    // Set console to support UTF-8 characters
    SetConsoleOutputCP(65001);
    
    Database db;
    
    if (!db.connect()) {
        ColorUtils::setColor(YELLOW);
        cout << "\n❌ Failed to connect to database!" << endl;
        ColorUtils::setColor(WHITE);
        cout << "Please ensure:" << endl;
        cout << "1. XAMPP MySQL is running" << endl;
        cout << "2. Database 'hospital_management_system' exists" << endl;
        cout << "3. Run database_schema.sql to create tables" << endl;
        ColorUtils::setColor(YELLOW);
        cout << "\nPress Enter to exit...";
        ColorUtils::resetColor();
        cin.get();
        return 1;
    }

    Registration registration(&db);
    Login login(&db);
    AdminModule* adminModule = nullptr;
    DoctorModule* doctorModule = nullptr;
    NurseModule* nurseModule = nullptr;
    Reports reports(&db);

    int mainChoice = -1;
    bool loggedIn = false;

    do {
        if (!loggedIn) {
            vector<string> mainMenuOptions = {
                "Registration",
                "Login",
                "Exit"
            };
            
            int selected = MenuNavigator::showMenu(mainMenuOptions, "HOSPITAL MANAGEMENT SYSTEM", false);
            
            if (selected == -1) {
                mainChoice = 2; // Exit
            } else {
                mainChoice = selected;
            }

            switch (mainChoice) {
            case 0: // Registration
                registration.showPreRegistrationMenu();
                break;
            case 1: // Login
                login.showLoginMenu();
                if (login.getCurrentUserId() > 0) {
                    loggedIn = true;
                }
                break;
            case 2: // Exit
                ColorUtils::setColor(YELLOW);
                cout << "\nThank you for using Hospital Management System!" << endl;
                ColorUtils::resetColor();
                break;
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
                ColorUtils::setColor(YELLOW);
                cout << "\n❌ Unknown role! Logging out..." << endl;
                ColorUtils::resetColor();
                login.logout();
                loggedIn = false;
            }
        }
    } while (mainChoice != 2); // Exit is now index 2

    // Cleanup
    if (adminModule) delete adminModule;
    if (doctorModule) delete doctorModule;
    if (nurseModule) delete nurseModule;

    db.disconnect();
    return 0;
}
