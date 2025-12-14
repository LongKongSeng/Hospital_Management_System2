#include "../include/Login.h"

Login::Login(Database* database) : db(database), currentUserId(-1), currentUserType(""), currentUsername("") {}

bool Login::authenticate(const string& username, const string& password, const string& userType) {
    try {
        string table = userType;
        string idField = userType + "_id";
        string query = "SELECT " + idField + ", username, full_name FROM " + table + 
                      " WHERE username = '" + username + "' AND password = '" + password + "'";
        
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res && res->next()) {
            currentUserId = res->getInt(idField);
            currentUsername = res->getString("username");
            currentUserType = userType;
            delete res;
            return true;
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
    displayTableHeader("LOGIN MODULE");
    
    string username, password, userType;
    int choice;

    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║  Select User Type:                     ║" << endl;
    cout << "║  1. Patient                            ║" << endl;
    cout << "║  2. Staff                              ║" << endl;
    cout << "║  3. Admin                              ║" << endl;
    cout << "║  0. Back to Main Menu                  ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 0) {
        return;
    }

    switch (choice) {
    case 1:
        userType = "patient";
        break;
    case 2:
        userType = "staff";
        break;
    case 3:
        userType = "admin";
        break;
    default:
        cout << "\n❌ Invalid choice!" << endl;
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    cout << "\nEnter Username: ";
    getline(cin, username);
    
    if (username.empty()) {
        cout << "\n❌ Username cannot be empty!" << endl;
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    cout << "Enter Password: ";
    getline(cin, password);
    
    if (password.empty()) {
        cout << "\n❌ Password cannot be empty!" << endl;
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }

    if (authenticate(username, password, userType)) {
        cout << "\n✅ Login successful! Welcome, " << currentUsername << "!" << endl;
        cout << "Press Enter to continue...";
        cin.get();
    } else {
        cout << "\n❌ Invalid username or password!" << endl;
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

void Login::logout() {
    currentUserId = -1;
    currentUserType = "";
    currentUsername = "";
}

void Login::displayTableHeader(const string& title) {
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "║" << setw((60 - title.length()) / 2 + title.length()) << right << title 
         << setw((60 - title.length()) / 2) << "" << "║" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
}

