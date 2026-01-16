#include "Database.h"
#include "ColorUtils.h" // Needed for red text

Database::Database() {
    host = "tcp://127.0.0.1:3306";
    user = "root";
    password = "";
    database = "hospital_management_system";
    driver = nullptr;
    con = nullptr;
}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    try {
        driver = get_driver_instance();
        con = driver->connect(host, user, password);
        con->setSchema(database);
        cout << "\n✅ Successfully connected to database: " << database << endl;
        return true;
    }
    catch (sql::SQLException& e) {
        displayError("Connection");
        cout << "SQL Error: " << e.what() << endl;
        cout << "Error Code: " << e.getErrorCode() << endl;
        return false;
    }
    catch (exception& e) {
        displayError("Connection");
        cout << "Error: " << e.what() << endl;
        return false;
    }
}

void Database::disconnect() {
    if (con) {
        delete con;
        con = nullptr;
    }
}

sql::Connection* Database::getConnection() {
    return con;
}

bool Database::executeQuery(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return false;
        }
        sql::Statement* stmt = con->createStatement();
        stmt->execute(query);
        delete stmt;
        return true;
    }
    catch (sql::SQLException& e) {
        displayError("Query Execution");
        cout << "SQL Error: " << e.what() << endl;
        cout << "Query: " << query << endl;
        return false;
    }
}

sql::ResultSet* Database::executeSelect(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return nullptr;
        }
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);
        return res;
    }
    catch (sql::SQLException& e) {
        displayError("Select Query");
        cout << "SQL Error: " << e.what() << endl;
        return nullptr;
    }
}

bool Database::executeUpdate(const string& query) {
    try {
        if (!con || con->isClosed()) {
            if (!connect()) return false;
        }
        sql::Statement* stmt = con->createStatement();
        int result = stmt->executeUpdate(query);
        delete stmt;
        return result > 0;
    }
    catch (sql::SQLException& e) {
        displayError("Update Query");
        cout << "SQL Error: " << e.what() << endl;
        return false;
    }
}

void Database::displayError(const string& operation) {
    cout << "\n❌ Error in " << operation << " operation!" << endl;
}

// ---------------------------------------------------------
// NEW HELPER: CHECK IF PATIENT IS ACTIVE
// ---------------------------------------------------------
bool Database::isPatientActive(const string& patientId) {
    string query = "SELECT status FROM patient WHERE formatted_id = '" + patientId + "'";
    sql::ResultSet* res = executeSelect(query);

    if (res && res->next()) {
        string status = res->getString("status");
        delete res;

        if (status == "Inactive") {
            ColorUtils::setColor(LIGHT_RED);
            cout << "\n[RESTRICTED] This patient is INACTIVE." << endl;
            cout << "Action cannot be performed." << endl;
            ColorUtils::resetColor();

            cout << "\nPress Enter to continue...";
            cin.get();
            return false;
        }
        return true; // Patient is Active
    }

    if (res) delete res;
    return false; // Patient not found
}
