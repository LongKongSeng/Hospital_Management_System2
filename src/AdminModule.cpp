#include "../include/AdminModule.h"

AdminModule::AdminModule(Database* database) : db(database) {}

void AdminModule::showMenu() {
    int choice;
    do {
        system("cls");
        displayTableHeader("ADMIN MODULE");
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║  1. Manage Patients                   ║" << endl;
        cout << "║  2. Manage Inventory                  ║" << endl;
        cout << "║  3. View Inventory Reports            ║" << endl;
        cout << "║  0. Back to Main Menu                 ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            managePatients();
            break;
        case 2:
            manageInventory();
            break;
        case 3:
            viewInventoryReports();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice! Please try again." << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::managePatients() {
    int choice;
    do {
        system("cls");
        displayTableHeader("PATIENT MANAGEMENT");
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║  1. Create Patient                    ║" << endl;
        cout << "║  2. View All Patients                 ║" << endl;
        cout << "║  3. Update Patient                    ║" << endl;
        cout << "║  4. Delete Patient                    ║" << endl;
        cout << "║  0. Back                              ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            createPatient();
            break;
        case 2:
            readPatients();
            break;
        case 3:
            updatePatient();
            break;
        case 4:
            deletePatient();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::createPatient() {
    system("cls");
    displayTableHeader("CREATE PATIENT");

    string username, password, fullName, email, phone, dob, gender, address, emergencyContact, emergencyPhone, bloodType;

    try {
        cout << "\nEnter Username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "\n❌ Username cannot be empty!" << endl;
            pressEnterToContinue();
            return;
        }

        cout << "Enter Password: ";
        getline(cin, password);

        cout << "Enter Full Name: ";
        getline(cin, fullName);

        cout << "Enter Email: ";
        getline(cin, email);

        cout << "Enter Phone: ";
        getline(cin, phone);

        cout << "Enter Date of Birth (YYYY-MM-DD): ";
        getline(cin, dob);

        cout << "Enter Gender (Male/Female/Other): ";
        getline(cin, gender);

        cout << "Enter Address: ";
        getline(cin, address);

        cout << "Enter Emergency Contact: ";
        getline(cin, emergencyContact);

        cout << "Enter Emergency Phone: ";
        getline(cin, emergencyPhone);

        cout << "Enter Blood Type: ";
        getline(cin, bloodType);

        string query = "INSERT INTO patient (username, password, full_name, email, phone, date_of_birth, gender, address, emergency_contact, emergency_phone, blood_type) "
            "VALUES ('" + username + "', '" + password + "', '" + fullName + "', '" + email + "', '" + phone + "', ";
        
        query += (dob.empty() ? "NULL" : "'" + dob + "'") + ", ";
        query += (gender.empty() ? "NULL" : "'" + gender + "'") + ", ";
        query += "'" + address + "', '" + emergencyContact + "', '" + emergencyPhone + "', '" + bloodType + "')";

        if (db->executeUpdate(query)) {
            cout << "\n✅ Patient created successfully!" << endl;
        } else {
            cout << "\n❌ Failed to create patient!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::readPatients() {
    system("cls");
    displayTableHeader("ALL PATIENTS");

    try {
        string query = "SELECT patient_id, username, full_name, email, phone, date_of_birth, gender, status FROM patient ORDER BY patient_id";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            displayPatientTable(res);
            delete res;
        } else {
            cout << "\n❌ Failed to retrieve patients!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::updatePatient() {
    system("cls");
    displayTableHeader("UPDATE PATIENT");

    int patientId = getIntInput("Enter Patient ID to update: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // First show current patient details
    string checkQuery = "SELECT * FROM patient WHERE patient_id = " + to_string(patientId);
    sql::ResultSet* checkRes = db->executeSelect(checkQuery);
    
    if (!checkRes || !checkRes->next()) {
        cout << "\n❌ Patient not found!" << endl;
        if (checkRes) delete checkRes;
        pressEnterToContinue();
        return;
    }
    delete checkRes;

    cout << "\nEnter new details (press Enter to keep current value):" << endl;
    
    string fullName = getStringInput("Full Name: ");
    string email = getStringInput("Email: ");
    string phone = getStringInput("Phone: ");
    string status = getStringInput("Status (Active/Discharged/In Treatment): ");

    string query = "UPDATE patient SET ";
    vector<string> updates;
    
    if (!fullName.empty()) updates.push_back("full_name = '" + fullName + "'");
    if (!email.empty()) updates.push_back("email = '" + email + "'");
    if (!phone.empty()) updates.push_back("phone = '" + phone + "'");
    if (!status.empty()) updates.push_back("status = '" + status + "'");

    if (updates.empty()) {
        cout << "\n❌ No fields to update!" << endl;
        pressEnterToContinue();
        return;
    }

    for (size_t i = 0; i < updates.size(); i++) {
        query += updates[i];
        if (i < updates.size() - 1) query += ", ";
    }
    query += " WHERE patient_id = " + to_string(patientId);

    if (db->executeUpdate(query)) {
        cout << "\n✅ Patient updated successfully!" << endl;
    } else {
        cout << "\n❌ Failed to update patient!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::deletePatient() {
    system("cls");
    displayTableHeader("DELETE PATIENT");

    int patientId = getIntInput("Enter Patient ID to delete: ");
    if (patientId <= 0) {
        cout << "\n❌ Invalid Patient ID!" << endl;
        pressEnterToContinue();
        return;
    }

    // Confirm deletion
    cout << "\n⚠️  Are you sure you want to delete this patient? (yes/no): ";
    string confirm;
    getline(cin, confirm);
    
    if (confirm != "yes" && confirm != "YES") {
        cout << "Deletion cancelled." << endl;
        pressEnterToContinue();
        return;
    }

    string query = "DELETE FROM patient WHERE patient_id = " + to_string(patientId);
    
    if (db->executeUpdate(query)) {
        cout << "\n✅ Patient deleted successfully!" << endl;
    } else {
        cout << "\n❌ Failed to delete patient!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::manageInventory() {
    int choice;
    do {
        system("cls");
        displayTableHeader("INVENTORY MANAGEMENT");
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║  1. Add Inventory Item                ║" << endl;
        cout << "║  2. View All Inventory                ║" << endl;
        cout << "║  3. Update Inventory Item             ║" << endl;
        cout << "║  4. Delete Inventory Item             ║" << endl;
        cout << "║  5. Update Inventory Quantity         ║" << endl;
        cout << "║  0. Back                              ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            createInventory();
            break;
        case 2:
            readInventory();
            break;
        case 3:
            updateInventory();
            break;
        case 4:
            deleteInventory();
            break;
        case 5:
            updateInventoryQuantity();
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::createInventory() {
    system("cls");
    displayTableHeader("ADD INVENTORY ITEM");

    string itemName, category, supplier, expiryDate;
    double unitPrice;
    int quantity, reorderLevel;

    try {
        cout << "\nEnter Item Name: ";
        getline(cin, itemName);
        
        cout << "Enter Category: ";
        getline(cin, category);
        
        cout << "Enter Quantity: ";
        cin >> quantity;
        cin.ignore();
        
        cout << "Enter Unit Price: ";
        cin >> unitPrice;
        cin.ignore();
        
        cout << "Enter Supplier: ";
        getline(cin, supplier);
        
        cout << "Enter Expiry Date (YYYY-MM-DD, or press Enter for NULL): ";
        getline(cin, expiryDate);
        
        cout << "Enter Reorder Level: ";
        cin >> reorderLevel;
        cin.ignore();

        string query = "INSERT INTO inventory (item_name, category, quantity, unit_price, supplier, expiry_date, reorder_level) "
            "VALUES ('" + itemName + "', '" + category + "', " + to_string(quantity) + ", " + 
            to_string(unitPrice) + ", '" + supplier + "', ";
        
        query += (expiryDate.empty() ? "NULL" : "'" + expiryDate + "'") + ", " + to_string(reorderLevel) + ")";

        if (db->executeUpdate(query)) {
            // Record transaction
            string transQuery = "INSERT INTO inventory_transaction (inventory_id, transaction_type, quantity_change, notes) "
                "SELECT inventory_id, 'Purchase', " + to_string(quantity) + ", 'Initial stock' FROM inventory WHERE item_name = '" + itemName + "' ORDER BY inventory_id DESC LIMIT 1";
            db->executeUpdate(transQuery);
            
            cout << "\n✅ Inventory item added successfully!" << endl;
        } else {
            cout << "\n❌ Failed to add inventory item!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::readInventory() {
    system("cls");
    displayTableHeader("ALL INVENTORY ITEMS");

    try {
        string query = "SELECT inventory_id, item_name, category, quantity, unit_price, supplier, expiry_date, reorder_level FROM inventory ORDER BY inventory_id";
        sql::ResultSet* res = db->executeSelect(query);
        
        if (res) {
            displayInventoryTable(res);
            delete res;
        } else {
            cout << "\n❌ Failed to retrieve inventory!" << endl;
        }
    }
    catch (exception& e) {
        cout << "\n❌ Error: " << e.what() << endl;
    }

    pressEnterToContinue();
}

void AdminModule::updateInventory() {
    system("cls");
    displayTableHeader("UPDATE INVENTORY ITEM");

    int inventoryId = getIntInput("Enter Inventory ID to update: ");
    if (inventoryId <= 0) {
        cout << "\n❌ Invalid Inventory ID!" << endl;
        pressEnterToContinue();
        return;
    }

    cout << "\nEnter new details (press Enter to keep current value):" << endl;
    
    string itemName = getStringInput("Item Name: ");
    string category = getStringInput("Category: ");
    string supplier = getStringInput("Supplier: ");
    string unitPriceStr = getStringInput("Unit Price: ");

    string query = "UPDATE inventory SET ";
    vector<string> updates;
    
    if (!itemName.empty()) updates.push_back("item_name = '" + itemName + "'");
    if (!category.empty()) updates.push_back("category = '" + category + "'");
    if (!supplier.empty()) updates.push_back("supplier = '" + supplier + "'");
    if (!unitPriceStr.empty()) {
        try {
            double unitPrice = stod(unitPriceStr);
            updates.push_back("unit_price = " + to_string(unitPrice));
        } catch (...) {
            cout << "\n❌ Invalid price format!" << endl;
            pressEnterToContinue();
            return;
        }
    }

    if (updates.empty()) {
        cout << "\n❌ No fields to update!" << endl;
        pressEnterToContinue();
        return;
    }

    for (size_t i = 0; i < updates.size(); i++) {
        query += updates[i];
        if (i < updates.size() - 1) query += ", ";
    }
    query += " WHERE inventory_id = " + to_string(inventoryId);

    if (db->executeUpdate(query)) {
        cout << "\n✅ Inventory item updated successfully!" << endl;
    } else {
        cout << "\n❌ Failed to update inventory item!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::deleteInventory() {
    system("cls");
    displayTableHeader("DELETE INVENTORY ITEM");

    int inventoryId = getIntInput("Enter Inventory ID to delete: ");
    if (inventoryId <= 0) {
        cout << "\n❌ Invalid Inventory ID!" << endl;
        pressEnterToContinue();
        return;
    }

    cout << "\n⚠️  Are you sure? (yes/no): ";
    string confirm;
    getline(cin, confirm);
    
    if (confirm != "yes" && confirm != "YES") {
        cout << "Deletion cancelled." << endl;
        pressEnterToContinue();
        return;
    }

    string query = "DELETE FROM inventory WHERE inventory_id = " + to_string(inventoryId);
    
    if (db->executeUpdate(query)) {
        cout << "\n✅ Inventory item deleted successfully!" << endl;
    } else {
        cout << "\n❌ Failed to delete inventory item!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::updateInventoryQuantity() {
    system("cls");
    displayTableHeader("UPDATE INVENTORY QUANTITY");

    int inventoryId = getIntInput("Enter Inventory ID: ");
    if (inventoryId <= 0) {
        cout << "\n❌ Invalid Inventory ID!" << endl;
        pressEnterToContinue();
        return;
    }

    cout << "\nSelect operation:" << endl;
    cout << "1. Add Quantity" << endl;
    cout << "2. Subtract Quantity" << endl;
    cout << "3. Set Quantity" << endl;
    cout << "Enter choice: ";
    int opChoice;
    cin >> opChoice;
    cin.ignore();

    int quantityChange = getIntInput("Enter Quantity: ");
    if (quantityChange <= 0) {
        cout << "\n❌ Invalid quantity!" << endl;
        pressEnterToContinue();
        return;
    }

    string transType;
    string query;
    
    switch (opChoice) {
    case 1:
        query = "UPDATE inventory SET quantity = quantity + " + to_string(quantityChange) + 
                " WHERE inventory_id = " + to_string(inventoryId);
        transType = "Purchase";
        break;
    case 2:
        query = "UPDATE inventory SET quantity = quantity - " + to_string(quantityChange) + 
                " WHERE inventory_id = " + to_string(inventoryId);
        transType = "Usage";
        break;
    case 3:
        query = "UPDATE inventory SET quantity = " + to_string(quantityChange) + 
                " WHERE inventory_id = " + to_string(inventoryId);
        transType = "Adjustment";
        break;
    default:
        cout << "\n❌ Invalid operation!" << endl;
        pressEnterToContinue();
        return;
    }

    if (db->executeUpdate(query)) {
        // Record transaction
        string transQuery = "INSERT INTO inventory_transaction (inventory_id, transaction_type, quantity_change, notes) "
            "VALUES (" + to_string(inventoryId) + ", '" + transType + "', " + to_string(quantityChange) + ", 'Manual update')";
        db->executeUpdate(transQuery);
        
        cout << "\n✅ Inventory quantity updated successfully!" << endl;
    } else {
        cout << "\n❌ Failed to update inventory quantity!" << endl;
    }

    pressEnterToContinue();
}

void AdminModule::viewInventoryReports() {
    int choice;
    do {
        system("cls");
        displayTableHeader("INVENTORY REPORTS");
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║  1. Total Inventory Value              ║" << endl;
        cout << "║  2. Low Stock Items                    ║" << endl;
        cout << "║  3. Inventory by Category              ║" << endl;
        cout << "║  4. Expiring Items                     ║" << endl;
        cout << "║  5. Monthly Inventory Transactions     ║" << endl;
        cout << "║  6. Inventory Value Chart              ║" << endl;
        cout << "║  0. Back                              ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            {
                system("cls");
                displayTableHeader("TOTAL INVENTORY VALUE");
                string query = "SELECT SUM(quantity * unit_price) as total_value FROM inventory";
                sql::ResultSet* res = db->executeSelect(query);
                if (res && res->next()) {
                    double total = res->getDouble("total_value");
                    cout << "\n╔════════════════════════════════════════╗" << endl;
                    cout << "║  Total Inventory Value: RM " << fixed << setprecision(2) << setw(12) << total << "  ║" << endl;
                    cout << "╚════════════════════════════════════════╝" << endl;
                    delete res;
                }
                pressEnterToContinue();
            }
            break;
        case 2:
            {
                system("cls");
                displayTableHeader("LOW STOCK ITEMS");
                string query = "SELECT inventory_id, item_name, category, quantity, reorder_level FROM inventory WHERE quantity <= reorder_level ORDER BY quantity ASC";
                sql::ResultSet* res = db->executeSelect(query);
                if (res) {
                    cout << "\n┌─────────────┬──────────────────────┬──────────────┬──────────┬──────────────┐" << endl;
                    cout << "│ Inventory ID│ Item Name            │ Category     │ Quantity │ Reorder Level│" << endl;
                    cout << "├─────────────┼──────────────────────┼──────────────┼──────────┼──────────────┤" << endl;
                    while (res->next()) {
                        cout << "│ " << setw(11) << res->getInt("inventory_id") 
                             << "│ " << setw(20) << res->getString("item_name")
                             << "│ " << setw(12) << res->getString("category")
                             << "│ " << setw(8) << res->getInt("quantity")
                             << "│ " << setw(12) << res->getInt("reorder_level") << "│" << endl;
                    }
                    cout << "└─────────────┴──────────────────────┴──────────────┴──────────┴──────────────┘" << endl;
                    delete res;
                }
                pressEnterToContinue();
            }
            break;
        case 3:
            {
                system("cls");
                displayTableHeader("INVENTORY BY CATEGORY");
                string query = "SELECT category, COUNT(*) as item_count, SUM(quantity) as total_quantity, SUM(quantity * unit_price) as category_value FROM inventory GROUP BY category ORDER BY category_value DESC";
                sql::ResultSet* res = db->executeSelect(query);
                if (res) {
                    cout << "\n┌──────────────┬─────────────┬─────────────────┬──────────────────┐" << endl;
                    cout << "│ Category     │ Item Count  │ Total Quantity  │ Category Value   │" << endl;
                    cout << "├──────────────┼─────────────┼─────────────────┼──────────────────┤" << endl;
                    while (res->next()) {
                        cout << "│ " << setw(12) << res->getString("category")
                             << "│ " << setw(11) << res->getInt("item_count")
                             << "│ " << setw(15) << res->getInt("total_quantity")
                             << "│ RM " << setw(12) << fixed << setprecision(2) << res->getDouble("category_value") << "│" << endl;
                    }
                    cout << "└──────────────┴─────────────┴─────────────────┴──────────────────┘" << endl;
                    delete res;
                }
                pressEnterToContinue();
            }
            break;
        case 4:
            {
                system("cls");
                displayTableHeader("EXPIRING ITEMS (Next 30 Days)");
                string query = "SELECT inventory_id, item_name, category, quantity, expiry_date, DATEDIFF(expiry_date, CURDATE()) as days_left FROM inventory WHERE expiry_date IS NOT NULL AND expiry_date BETWEEN CURDATE() AND DATE_ADD(CURDATE(), INTERVAL 30 DAY) ORDER BY expiry_date ASC";
                sql::ResultSet* res = db->executeSelect(query);
                if (res) {
                    cout << "\n┌─────────────┬──────────────────────┬──────────────┬──────────┬──────────────┬───────────┐" << endl;
                    cout << "│ Inventory ID│ Item Name            │ Category     │ Quantity │ Expiry Date  │ Days Left │" << endl;
                    cout << "├─────────────┼──────────────────────┼──────────────┼──────────┼──────────────┼───────────┤" << endl;
                    while (res->next()) {
                        cout << "│ " << setw(11) << res->getInt("inventory_id")
                             << "│ " << setw(20) << res->getString("item_name")
                             << "│ " << setw(12) << res->getString("category")
                             << "│ " << setw(8) << res->getInt("quantity")
                             << "│ " << setw(12) << res->getString("expiry_date")
                             << "│ " << setw(9) << res->getInt("days_left") << "│" << endl;
                    }
                    cout << "└─────────────┴──────────────────────┴──────────────┴──────────┴──────────────┴───────────┘" << endl;
                    delete res;
                }
                pressEnterToContinue();
            }
            break;
        case 5:
            {
                system("cls");
                displayTableHeader("MONTHLY INVENTORY TRANSACTIONS");
                string query = "SELECT DATE_FORMAT(transaction_date, '%Y-%m') as month, transaction_type, SUM(quantity_change) as total_change, COUNT(*) as transaction_count FROM inventory_transaction GROUP BY DATE_FORMAT(transaction_date, '%Y-%m'), transaction_type ORDER BY month DESC, transaction_type";
                sql::ResultSet* res = db->executeSelect(query);
                if (res) {
                    cout << "\n┌───────────┬──────────────────┬───────────────┬──────────────────┐" << endl;
                    cout << "│ Month     │ Transaction Type │ Total Change  │ Transaction Count│" << endl;
                    cout << "├───────────┼──────────────────┼───────────────┼──────────────────┤" << endl;
                    while (res->next()) {
                        cout << "│ " << setw(9) << res->getString("month")
                             << "│ " << setw(16) << res->getString("transaction_type")
                             << "│ " << setw(13) << res->getInt("total_change")
                             << "│ " << setw(16) << res->getInt("transaction_count") << "│" << endl;
                    }
                    cout << "└───────────┴──────────────────┴───────────────┴──────────────────┘" << endl;
                    delete res;
                }
                pressEnterToContinue();
            }
            break;
        case 6:
            {
                system("cls");
                displayTableHeader("INVENTORY VALUE CHART (Text-based)");
                string query = "SELECT item_name, (quantity * unit_price) as value FROM inventory ORDER BY value DESC LIMIT 10";
                sql::ResultSet* res = db->executeSelect(query);
                if (res) {
                    double maxValue = 0;
                    vector<pair<string, double>> items;
                    while (res->next()) {
                        string name = res->getString("item_name");
                        double value = res->getDouble("value");
                        items.push_back({name, value});
                        if (value > maxValue) maxValue = value;
                    }
                    delete res;
                    
                    cout << "\n";
                    for (const auto& item : items) {
                        int barLength = (int)((item.second / maxValue) * 50);
                        cout << left << setw(25) << item.first.substr(0, 24) << " |";
                        for (int i = 0; i < barLength; i++) cout << "█";
                        cout << " RM " << fixed << setprecision(2) << item.second << endl;
                    }
                }
                pressEnterToContinue();
            }
            break;
        case 0:
            return;
        default:
            cout << "\n❌ Invalid choice!" << endl;
            pressEnterToContinue();
        }
    } while (choice != 0);
}

void AdminModule::displayPatientTable(sql::ResultSet* res) {
    cout << "\n┌─────────────┬──────────────┬──────────────────────┬──────────────────────┬──────────────┬──────────────┐" << endl;
    cout << "│ Patient ID  │ Username     │ Full Name            │ Email                │ Phone        │ Status       │" << endl;
    cout << "├─────────────┼──────────────┼──────────────────────┼──────────────────────┼──────────────┼──────────────┤" << endl;
    
    while (res->next()) {
        cout << "│ " << setw(11) << res->getInt("patient_id")
             << "│ " << setw(12) << res->getString("username")
             << "│ " << setw(20) << res->getString("full_name")
             << "│ " << setw(20) << res->getString("email")
             << "│ " << setw(12) << res->getString("phone")
             << "│ " << setw(12) << (res->isNull("status") ? "N/A" : res->getString("status")) << "│" << endl;
    }
    
    cout << "└─────────────┴──────────────┴──────────────────────┴──────────────────────┴──────────────┴──────────────┘" << endl;
}

void AdminModule::displayInventoryTable(sql::ResultSet* res) {
    cout << "\n┌─────────────┬──────────────────────┬──────────────┬──────────┬─────────────┬──────────────────┐" << endl;
    cout << "│ Inventory ID│ Item Name            │ Category     │ Quantity │ Unit Price  │ Supplier         │" << endl;
    cout << "├─────────────┼──────────────────────┼──────────────┼──────────┼─────────────┼──────────────────┤" << endl;
    
    while (res->next()) {
        cout << "│ " << setw(11) << res->getInt("inventory_id")
             << "│ " << setw(20) << res->getString("item_name")
             << "│ " << setw(12) << res->getString("category")
             << "│ " << setw(8) << res->getInt("quantity")
             << "│ RM " << setw(9) << fixed << setprecision(2) << res->getDouble("unit_price")
             << "│ " << setw(16) << res->getString("supplier") << "│" << endl;
    }
    
    cout << "└─────────────┴──────────────────────┴──────────────┴──────────┴─────────────┴──────────────────┘" << endl;
}

void AdminModule::displayTableHeader(const string& title) {
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "║" << setw((60 - title.length()) / 2 + title.length()) << right << title 
         << setw((60 - title.length()) / 2) << "" << "║" << endl;
    cout << "║" << setw(60) << "" << "║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
}

void AdminModule::pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int AdminModule::getIntInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

string AdminModule::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

