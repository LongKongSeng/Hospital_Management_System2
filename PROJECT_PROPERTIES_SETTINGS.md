# Visual Studio Project Properties Settings

## Configuration: Release | x64 (Recommended)

### Step 1: Open Project Properties
1. Right-click on your project in Solution Explorer
2. Select **Properties**
3. Make sure **Configuration** is set to **Release** and **Platform** is set to **x64**

---

## C/C++ → General

### Additional Include Directories
**Location:** `Configuration Properties` → `C/C++` → `General` → `Additional Include Directories`

**Add these paths (one per line or semicolon-separated):**
```
C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include
$(ProjectDir)..\include
```

**Or click the dropdown arrow and click Edit, then add:**
- `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include`
- `$(ProjectDir)..\include`

**Note:** If your MySQL Connector is in a different location, update the first path accordingly.

---

## Linker → General

### Additional Library Directories
**Location:** `Configuration Properties` → `Linker` → `General` → `Additional Library Directories`

**Add this path:**
```
C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64
```

**Or click the dropdown arrow and click Edit, then add:**
- `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64`

**Important:** Use `lib64` for x64 platform, or `lib` for x86/Win32 platform.

---

## Linker → Input

### Additional Dependencies
**Location:** `Configuration Properties` → `Linker` → `Input` → `Additional Dependencies`

**Add this library:**
```
mysqlcppconn.lib
```

**Or click the dropdown arrow and click Edit, then add:**
- `mysqlcppconn.lib`

**Full list should look like:**
```
mysqlcppconn.lib
%(AdditionalDependencies)
```

---

## Configuration: Debug | x64

**Apply the SAME settings as Release | x64:**

### C/C++ → General → Additional Include Directories
```
C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include
$(ProjectDir)..\include
```

### Linker → General → Additional Library Directories
```
C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64
```

### Linker → Input → Additional Dependencies
```
mysqlcppconn.lib
```

---

## Summary Table

| Property Category | Property Name | Value |
|------------------|---------------|-------|
| **C/C++ → General** | Additional Include Directories | `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include`<br>`$(ProjectDir)..\include` |
| **Linker → General** | Additional Library Directories | `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64` |
| **Linker → Input** | Additional Dependencies | `mysqlcppconn.lib` |

---

## Visual Guide (Step by Step)

### 1. Additional Include Directories
```
Right-click Project → Properties
→ Configuration Properties
  → C/C++
    → General
      → Additional Include Directories: [Edit]
        → Add:
          1. C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include
          2. $(ProjectDir)..\include
```

### 2. Additional Library Directories
```
Right-click Project → Properties
→ Configuration Properties
  → Linker
    → General
      → Additional Library Directories: [Edit]
        → Add:
          1. C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64
```

### 3. Additional Dependencies
```
Right-click Project → Properties
→ Configuration Properties
  → Linker
    → Input
      → Additional Dependencies: [Edit]
        → Add:
          1. mysqlcppconn.lib
```

---

## Important Notes

1. **Platform Selection:**
   - For **x64** platform: Use `lib64` folder
   - For **Win32/x86** platform: Use `lib` folder (if available)

2. **Configuration:**
   - Apply settings to **both Debug and Release** configurations
   - Or select "All Configurations" from the Configuration dropdown

3. **Path Variables:**
   - `$(ProjectDir)` = Path to your project folder
   - `$(ProjectDir)..\include` = Points to the `include` folder one level up from project

4. **DLL File:**
   - After building, copy `mysqlcppconn-9-vs143.dll` from the connector's `lib64` folder to your output directory (`x64\Release\` or `x64\Debug\`)
   - Or add the `lib64` folder to your system PATH

5. **If MySQL Connector is in Different Location:**
   - Update all paths that reference `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\`
   - Replace with your actual MySQL Connector installation path

---

## Current Project Settings (From .vcxproj file)

Your project is currently configured with:

**Debug | x64:**
- Include: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include;$(ProjectDir)..\include`
- Library Dir: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64`
- Dependencies: `mysqlcppconn.lib`

**Release | x64:**
- Include: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\include;$(ProjectDir)..\include`
- Library Dir: `C:\Users\tjkca\Desktop\mysql-connector-c++-9.5.0-winx64\mysql-connector-c++-9.5.0-winx64\lib64`
- Dependencies: `mysqlcppconn.lib`

---

## Troubleshooting

### Error: "Cannot open include file: mysql_driver.h"
**Solution:** Check Additional Include Directories path is correct

### Error: "Unresolved external symbol"
**Solution:** 
1. Check Additional Library Directories path
2. Verify Additional Dependencies includes `mysqlcppconn.lib`
3. Ensure you're building for x64 (not Win32)

### Error: "The program can't start because mysqlcppconn-9-vs143.dll is missing"
**Solution:** Copy the DLL from connector's `lib64` folder to your output directory

