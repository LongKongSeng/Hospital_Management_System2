# Hospital Management System - Setup Instructions

## Prerequisites

1. **Visual Studio 2022** (Community, Professional, or Enterprise)
2. **XAMPP** (Latest version with MySQL)
3. **MySQL Connector/C++ 9.5.0** (or compatible version)

## Step-by-Step Setup

### Step 1: Install XAMPP and Start MySQL

1. Download and install XAMPP from https://www.apachefriends.org/
2. Start XAMPP Control Panel
3. Click "Start" for MySQL service
4. Verify MySQL is running (green indicator)

### Step 2: Create Database

**Option A: Using phpMyAdmin**
1. Open browser and go to: http://localhost/phpmyadmin
2. Click on "SQL" tab
3. Copy and paste the contents of `database_schema.sql`
4. Click "Go" to execute
5. Verify database `hospital_management_system` is created

**Option B: Using MySQL Command Line**
1. Open Command Prompt
2. Navigate to XAMPP MySQL bin directory:
   ```
   cd C:\xampp\mysql\bin
   ```
3. Run MySQL:
   ```
   mysql.exe -u root
   ```
4. Copy and paste the contents of `database_schema.sql`
5. Press Enter to execute

### Step 3: Install MySQL Connector/C++

1. Download MySQL Connector/C++ from:
   https://dev.mysql.com/downloads/connector/cpp/
2. Extract to a location (e.g., `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\`)
3. Note the paths:
   - Include: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include`
   - Library: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64`

### Step 4: Configure Visual Studio Project

1. Open `Workshop 1 - B032410917.sln` in Visual Studio 2022
2. Right-click on the project → Properties
3. Select Configuration: **Release** and Platform: **x64**
4. Go to **C/C++** → **General** → **Additional Include Directories**
   - Add: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include`
   - (Adjust path if your connector is in a different location)
5. Go to **Linker** → **General** → **Additional Library Directories**
   - Add: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64`
6. Go to **Linker** → **Input** → **Additional Dependencies**
   - Add: `mysqlcppconn.lib`
7. Click **OK** to save

**Note**: Also configure Debug|x64 with the same settings if you want to debug.

### Step 5: Copy DLL Files

1. Copy `mysqlcppconn-9-vs143.dll` from the connector's `lib64` folder
2. Paste it into your project's output directory:
   - For Release: `x64\Release\`
   - For Debug: `x64\Debug\`
3. Or copy to Windows System32 folder (requires admin rights)

### Step 6: Build and Run

1. In Visual Studio, select **Release** and **x64** configuration
2. Press **F7** to build the solution
3. If build succeeds, press **F5** to run
4. If you see connection errors, verify:
   - MySQL service is running in XAMPP
   - Database `hospital_management_system` exists
   - Database credentials are correct (default: root, no password)

## Default Login Credentials

After running `database_schema.sql`, you can login with:
- **Username**: admin
- **Password**: admin123

## Testing the System

1. **Test Registration**:
   - Register a new patient
   - Register a new staff member
   - Register a new admin

2. **Test Login**:
   - Login as admin
   - Login as staff (after registering one)
   - Login as patient (after registering one)

3. **Test Admin Functions**:
   - Create a patient
   - View all patients
   - Update patient details
   - Add inventory items
   - View inventory reports
   - Update inventory quantities

4. **Test Staff Functions**:
   - View patient status
   - Update patient status
   - Add prescription
   - View prescriptions
   - Add patient report
   - View reports

## Troubleshooting

### Error: "Failed to connect to database"
- **Solution**: Ensure MySQL is running in XAMPP
- Check database name is exactly `hospital_management_system`
- Verify username is `root` and password is empty (default XAMPP)

### Error: "Cannot open include file: mysql_driver.h"
- **Solution**: Check Additional Include Directories in project properties
- Verify MySQL Connector path is correct
- Ensure you're building for x64 platform

### Error: "Unresolved external symbol"
- **Solution**: Check Additional Library Directories
- Verify `mysqlcppconn.lib` is in Additional Dependencies
- Ensure library path points to `lib64` folder (not `lib`)

### Error: "The program can't start because mysqlcppconn-9-vs143.dll is missing"
- **Solution**: Copy the DLL to the output directory
- Or add the connector's `lib64` folder to system PATH

### Build Error: "Platform toolset v143 not found"
- **Solution**: Install Visual Studio 2022 with C++ desktop development workload
- Or change Platform Toolset to v142 (VS 2019) in project properties

## Database Configuration

If you need to change database connection settings, edit `Database.cpp`:

```cpp
host = "tcp://127.0.0.1:3306";  // Change if MySQL is on different host/port
user = "root";                   // Change if different username
password = "";                    // Change if MySQL has password
database = "hospital_management_system";  // Change if different database name
```

## Project Files Overview

- `Main.cpp` - Application entry point and main menu
- `Database.h/cpp` - Database connection and query execution
- `Registration.h/cpp` - User registration functionality
- `Login.h/cpp` - Authentication system
- `AdminModule.h/cpp` - Admin operations (CRUD, reports)
- `StaffModule.h/cpp` - Staff operations (patient management, prescriptions)
- `database_schema.sql` - Database structure and initial data

## Features Implemented for Grade A

✅ **Database Management (CRUDS)**
- Complete CRUD operations for all entities
- Modular design with separate classes

✅ **Complex Calculations**
- Aggregations (SUM, COUNT)
- Grouping (GROUP BY)
- Joins (INNER JOIN)
- Subqueries
- Unary operations (DATEDIFF)

✅ **Report Generation**
- Formatted tables
- Summary lists
- Text-based charts
- Graph summaries

✅ **Error Handling**
- Database errors
- Input validation
- User feedback

✅ **HCI Elements**
- Table displays
- Menu navigation
- Back options everywhere
- User-friendly messages

## Support

If you encounter issues:
1. Check that all prerequisites are installed
2. Verify database connection settings
3. Ensure all paths are correct
4. Check console output for specific error messages












