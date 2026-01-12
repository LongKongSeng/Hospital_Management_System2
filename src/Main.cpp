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
    // Set console to support UTF-8 characters (for box drawing)
    SetConsoleOutputCP(65001);

    Database db;

    if (!db.connect()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n[ERROR] Failed to connect to database!" << endl;
        ColorUtils::setColor(WHITE);
        cout << "Please ensure XAMPP MySQL is running and database exists." << endl;
        cout << "\nPress Enter to exit...";
        cin.get();
        return 1;
    }

    // Initialize Modules
    Registration registration(&db);
    Login login(&db);

    // Pointers for user-specific modules (created upon login)
    AdminModule* adminModule = nullptr;
    DoctorModule* doctorModule = nullptr;
    NurseModule* nurseModule = nullptr;

    int mainChoice = -1;
    bool loggedIn = false;

    do {
        if (!loggedIn) {
            vector<string> mainMenuOptions = {
                "Registration",
                "Login",
                "Exit Application"
            };

            // UPDATED: Using Arrow Key Navigation
            mainChoice = MenuNavigator::showMenu(mainMenuOptions, "HOSPITAL MANAGEMENT SYSTEM", false);

            // Handle ESC key or Exit selection
            if (mainChoice == -1 || mainChoice == 2) {
                mainChoice = 2; // Ensure loop terminates
                ColorUtils::setColor(LIGHT_CYAN);
                cout << "\nThank you for using Hospital Management System!" << endl;
                ColorUtils::resetColor();
                break;
            }

            switch (mainChoice) {
            case 0: // Registration
                registration.showPreRegistrationMenu();
                break;
            case 1: // Login
                login.showLoginMenu();
                if (!login.getCurrentUserId().empty()) {
                    loggedIn = true;
                }
                break;
            }
        }
        else {
            // User is logged in - route to appropriate module
            string role = login.getCurrentRole();
            string userId = login.getCurrentUserId();

            if (role == "Admin") {
                if (!adminModule) adminModule = new AdminModule(&db);
                adminModule->showMenu();

                // Cleanup after logout
                delete adminModule;
                adminModule = nullptr;

            }
            else if (role == "Doctor") {
                if (!doctorModule) doctorModule = new DoctorModule(&db, userId);
                doctorModule->showMenu();

                // Cleanup after logout
                delete doctorModule;
                doctorModule = nullptr;

            }
            else if (role == "Nurse") {
                if (!nurseModule) nurseModule = new NurseModule(&db, userId);
                nurseModule->showMenu();

                // Cleanup after logout
                delete nurseModule;
                nurseModule = nullptr;
            }

            // Perform Logout logic
            login.logout();
            loggedIn = false;
        }
    } while (mainChoice != 2);

    db.disconnect();
    return 0;
}
