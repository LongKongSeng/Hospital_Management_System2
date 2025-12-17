#include "Login.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Login::Login(Database* database) : db(database), currentUserId(-1), currentUserType(""), currentUsername(""), currentRole("") {}

bool Login::authenticate(const string& username, const string& password) {
    try {
        // Query the login table which has username, password, and role
        string query = "SELECT login_id, username, role, doctor_id, nurse_id, admin_id FROM login "
                      "WHERE username = '" + username + "' AND password = '" + password + "'";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res && res->next()) {
            currentUsername = res->getString("username");
            currentRole = res->getString("role");
            
            // Determine user type and ID based on role
            if (currentRole == "Doctor") {
                currentUserType = "doctor";
                currentUserId = res->getInt("doctor_id");
            } else if (currentRole == "Nurse") {
                currentUserType = "nurse";
                currentUserId = res->getInt("nurse_id");
            } else if (currentRole == "Admin") {
                currentUserType = "admin";
                currentUserId = res->getInt("admin_id");
            }
            
            delete res;
            return currentUserId > 0;
        }
        
        if (res) delete res;
        return false;
    }
    catch (exception& e) {
        cout << "\n❌ Authentication Error: " << e.what() << endl;
        return false;
    }
}

void Login::showLoginMenu() {
    system("cls");
    displayTableHeader("LOGIN");
    
    string username, password;

    ColorUtils::setColor(WHITE);
    cout << "\nEnter Username: ";
    ColorUtils::resetColor();
    getline(cin, username);
    
    if (username.empty()) {
        ColorUtils::setColor(YELLOW);
        cout << "\n❌ Username cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    ColorUtils::setColor(WHITE);
    cout << "Enter Password: ";
    ColorUtils::resetColor();
    getline(cin, password);
    
    if (password.empty()) {
        ColorUtils::setColor(YELLOW);
        cout << "\n❌ Password cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    if (authenticate(username, password)) {
        ColorUtils::setColor(YELLOW);
        cout << "\n✅ Login successful! Welcome, " << currentUsername << "!\n";
        ColorUtils::resetColor();
        cout << "Role: ";
        ColorUtils::setColor(YELLOW);
        cout << currentRole << endl;
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    } else {
        ColorUtils::setColor(YELLOW);
        cout << "\n❌ Invalid username or password!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    }
}

int Login::getCurrentUserId() {
    return currentUserId;
}

string Login::getCurrentUserType() {
    return currentUserType;
}

string Login::getCurrentUsername() {
    return currentUsername;
}

string Login::getCurrentRole() {
    return currentRole;
}

void Login::logout() {
    currentUserId = -1;
    currentUserType = "";
    currentUsername = "";
    currentRole = "";
}

void Login::displayTableHeader(const string& title) {
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
