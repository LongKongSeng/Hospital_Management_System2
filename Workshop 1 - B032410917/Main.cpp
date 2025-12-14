#include <iostream>
#include <mysql.h>
#include <string>

using namespace std;

// Global variables (These should ideally be handled within a class for better design)
MYSQL* conn;
// MYSQL_ROW row;  // Unused here, but kept for context
// MYSQL_RES* res; // Must be a pointer if declared globally for result set

class db_response
{
public:
    static void ConnectionFunction()
    {
        // 1. Initialize the MYSQL handle
        conn = mysql_init(0);

        if (!conn) {
            cerr << "MYSQL Initialization failed: " << mysql_error(conn) << endl;
            return; // Exit if initialization fails
        }

        // 2. Attempt the real connection
        // Parameters: (handle, host, user, password, database, port, unix_socket, client_flags)
        conn = mysql_real_connect(conn, "localhost", "root", "",
            "hospital_managemant_system", 3306, NULL, 0);

        if (conn)
        {
            cout << "✅ Successfully Connected to Database: hospital_managemant_system" << endl;
            // Optionally set character set here: mysql_set_character_set(conn, "utf8");
        }
        else
        {
            // CRITICAL: Print the specific error message for debugging!
            cerr << "❌ Failed To Connect to MySQL!" << endl;
            cerr << "Error: " << mysql_error(conn) << endl;
            // You may want to call mysql_close(conn) here if the connection failed, 
            // though the handle might be invalid if initialization was the failure point.
        }
    }

    // IMPORTANT: Add a function to close the connection when done!
    static void CloseConnection()
    {
        if (conn) {
            mysql_close(conn);
            cout << "Database connection closed." << endl;
            conn = nullptr; // Set to nullptr to avoid dangling pointer issues
        }
    }
};

// Example usage in main (requires you to handle getch/conio.h outside)
int main() {
    db_response::ConnectionFunction();
    // ... Your application logic (CRUD operations) ...
    db_response::CloseConnection();

    // getch() is part of conio.h and non-standard. Use cin.get() instead if possible.
    // cout << "Press any key to exit...";
    // cin.get(); 

    return 0;
}