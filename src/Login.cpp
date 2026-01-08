#include "Login.h"
#include "ColorUtils.h"
#include "MenuNavigator.h"

Login::Login(Database* database) : db(database), currentUserId(""), currentUserType(""), currentUsername(""), currentRole("") {}

bool Login::authenticate(const string& username, const string& password) {
    try {
        // Query the login table which has username, password, and role
        string query = "SELECT formatted_id, username, role, doctor_id, nurse_id, admin_id FROM login "
                      "WHERE username = '" + username + "' AND password = '" + password + "'";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res && res->next()) {
            currentUsername = res->getString("username");
            currentRole = res->getString("role");
            
            // Determine user type and ID based on role (now using formatted_id strings)
            if (currentRole == "Doctor") {
                currentUserType = "doctor";
                currentUserId = string(res->getString("doctor_id"));
            } else if (currentRole == "Nurse") {
                currentUserType = "nurse";
                currentUserId = string(res->getString("nurse_id"));
            } else if (currentRole == "Admin") {
                currentUserType = "admin";
                currentUserId = string(res->getString("admin_id"));
            }
            
            delete res;
            return !currentUserId.empty();
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
    
    const int SEPARATOR_LENGTH = 80;
    
    // Top separator line
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;
    
    // Display title (centered, in blue box)
    MenuNavigator::displayTitle("LOGIN", SEPARATOR_LENGTH);
    
    // Separator after title
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl << endl;
    
    string username, password;

    // Enter Username
    ColorUtils::setColor(WHITE);
    cout << "Enter Username: ";
    ColorUtils::resetColor();
    getline(cin, username);
    
    if (username.empty()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n❌ Username cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    // Enter Password
    ColorUtils::setColor(WHITE);
    cout << "Enter Password: ";
    ColorUtils::resetColor();
    getline(cin, password);
    
    if (password.empty()) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n❌ Password cannot be empty!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    // Bottom separator
    cout << endl;
    ColorUtils::setColor(LIGHT_BLUE);
    for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
    ColorUtils::resetColor();
    cout << endl;

    if (authenticate(username, password)) {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n✅ Login successful! Welcome, " << currentUsername << "!\n";
        ColorUtils::resetColor();
        cout << "Role: ";
        ColorUtils::setColor(LIGHT_CYAN);
        cout << currentRole << endl;
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    } else {
        ColorUtils::setColor(LIGHT_CYAN);
        cout << "\n❌ Invalid username or password!\n";
        ColorUtils::resetColor();
        cout << "Press Enter to continue...";
        cin.get();
    }
}

string Login::getCurrentUserId() {
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
    currentUserId = "";
    currentUserType = "";
    currentUsername = "";
    currentRole = "";
}

void Login::displayTableHeader(const string& title) {
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
