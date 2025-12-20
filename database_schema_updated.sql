-- Hospital Management System Database Schema
-- Updated to match all C++ interface queries
-- Run this script in phpMyAdmin or MySQL command line

CREATE DATABASE IF NOT EXISTS hospital_management_system;
USE hospital_management_system;

-- DOCTOR Table
-- Note: AUTO_INCREMENT starts at 2000 for Doctor IDs (format: 2xxx)
CREATE TABLE IF NOT EXISTS doctor (
    doctor_id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    specialization VARCHAR(100) NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    availability ENUM('Available', 'Busy', 'Off') DEFAULT 'Available',
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Doctor',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- NURSE Table
-- Note: AUTO_INCREMENT starts at 4000 for Nurse IDs (format: 4xxx)
CREATE TABLE IF NOT EXISTS nurse (
    nurse_id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Nurse',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ADMIN Table
-- Note: AUTO_INCREMENT starts at 1000 for Admin IDs (format: 1xxx)
CREATE TABLE IF NOT EXISTS admin (
    admin_id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Admin',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- LOGIN Table (Centralized login system)
CREATE TABLE IF NOT EXISTS login (
    login_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    role ENUM('Doctor', 'Nurse', 'Admin') NOT NULL,
    doctor_id INT NULL,
    nurse_id INT NULL,
    admin_id INT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (doctor_id) REFERENCES doctor(doctor_id) ON DELETE CASCADE,
    FOREIGN KEY (nurse_id) REFERENCES nurse(nurse_id) ON DELETE CASCADE,
    FOREIGN KEY (admin_id) REFERENCES admin(admin_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PATIENT Table
-- Note: AUTO_INCREMENT starts at 3000 for Patient IDs (format: 3xxxx)
CREATE TABLE IF NOT EXISTS patient (
    patient_id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    date_of_birth DATE NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    blood_type VARCHAR(5),
    emergency_contact VARCHAR(100),
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PHARMACY Table (Medication Inventory)
CREATE TABLE IF NOT EXISTS pharmacy (
    pharmacy_id INT AUTO_INCREMENT PRIMARY KEY,
    medicine_name VARCHAR(100) NOT NULL,
    category_of_meds VARCHAR(50) NOT NULL,
    quantity INT DEFAULT 0,
    unit_price DECIMAL(10, 2) DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- INVENTORY Table (General Hospital Inventory - for Reports module)
CREATE TABLE IF NOT EXISTS inventory (
    inventory_id INT AUTO_INCREMENT PRIMARY KEY,
    item_name VARCHAR(100) NOT NULL,
    category VARCHAR(50) NOT NULL,
    quantity INT DEFAULT 0,
    reorder_level INT DEFAULT 10,
    unit_price DECIMAL(10, 2) DEFAULT 0.00,
    expiry_date DATE NULL,
    supplier VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- INVENTORY_TRANSACTION Table (Track inventory changes)
CREATE TABLE IF NOT EXISTS inventory_transaction (
    transaction_id INT AUTO_INCREMENT PRIMARY KEY,
    inventory_id INT NOT NULL,
    transaction_type ENUM('Purchase', 'Usage', 'Adjustment') NOT NULL,
    quantity_change INT NOT NULL,
    transaction_date DATE NOT NULL,
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (inventory_id) REFERENCES inventory(inventory_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PRESCRIPTION Table
CREATE TABLE IF NOT EXISTS prescription (
    prescription_id INT AUTO_INCREMENT PRIMARY KEY,
    pharmacy_id INT NOT NULL,
    dosage VARCHAR(50),
    duration_of_meds VARCHAR(50),
    instructions TEXT,
    date DATE NOT NULL,
    FOREIGN KEY (pharmacy_id) REFERENCES pharmacy(pharmacy_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- DIAGNOSIS Table
CREATE TABLE IF NOT EXISTS diagnosis (
    diagnosis_id INT AUTO_INCREMENT PRIMARY KEY,
    disease VARCHAR(200),
    disorder VARCHAR(200),
    duration_of_pain VARCHAR(50),
    severity VARCHAR(50),
    prescription_id INT NULL,
    date DATE NOT NULL,
    FOREIGN KEY (prescription_id) REFERENCES prescription(prescription_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- MEDICAL_RECORD Table
CREATE TABLE IF NOT EXISTS medical_record (
    record_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    doctor_id INT NOT NULL,
    diagnosis_id INT NULL,
    date_of_record DATE NOT NULL,
    notes TEXT,
    FOREIGN KEY (patient_id) REFERENCES patient(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctor(doctor_id) ON DELETE CASCADE,
    FOREIGN KEY (diagnosis_id) REFERENCES diagnosis(diagnosis_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- APPOINTMENT Table
CREATE TABLE IF NOT EXISTS appointment (
    appointment_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    nurse_id INT NOT NULL,
    appointment_date DATE NOT NULL,
    appointment_time TIME NOT NULL,
    status ENUM('Scheduled', 'Completed', 'Cancelled') DEFAULT 'Scheduled',
    FOREIGN KEY (patient_id) REFERENCES patient(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (nurse_id) REFERENCES nurse(nurse_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- TREATMENT Table
CREATE TABLE IF NOT EXISTS treatment (
    treatment_id INT AUTO_INCREMENT PRIMARY KEY,
    doctor_id INT NOT NULL,
    dressing_applied VARCHAR(200),
    consultation_fee DECIMAL(10, 2) DEFAULT 0.00,
    treatment_fee DECIMAL(10, 2) DEFAULT 0.00,
    treatment_date DATE NOT NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctor(doctor_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- FINANCE Table (Patient Receipt/Payment)
CREATE TABLE IF NOT EXISTS finance (
    finance_id INT AUTO_INCREMENT PRIMARY KEY,
    treatment_id INT NOT NULL,
    patient_id INT NOT NULL,
    date_of_payment DATE NOT NULL,
    payment_status ENUM('Paid', 'Pending', 'Partial') DEFAULT 'Pending',
    total_amount DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (treatment_id) REFERENCES treatment(treatment_id) ON DELETE CASCADE,
    FOREIGN KEY (patient_id) REFERENCES patient(patient_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create indexes for better performance
CREATE INDEX idx_login_username ON login(username);
CREATE INDEX idx_patient_status ON patient(status);
CREATE INDEX idx_appointment_date ON appointment(appointment_date);
CREATE INDEX idx_pharmacy_category ON pharmacy(category_of_meds);
CREATE INDEX idx_inventory_category ON inventory(category);
CREATE INDEX idx_inventory_transaction_date ON inventory_transaction(transaction_date);
CREATE INDEX idx_inventory_transaction_type ON inventory_transaction(transaction_type);

-- Set AUTO_INCREMENT starting values for distinct ID ranges
-- Admin IDs: 1000-1999 (format: 1xxx)
-- Doctor IDs: 2000-2999 (format: 2xxx)
-- Patient IDs: 3000-99999 (format: 3xxxx)
-- Nurse IDs: 4000-4999 (format: 4xxx)
ALTER TABLE admin AUTO_INCREMENT = 1000;
ALTER TABLE doctor AUTO_INCREMENT = 2000;
ALTER TABLE patient AUTO_INCREMENT = 3000;
ALTER TABLE nurse AUTO_INCREMENT = 4000;

-- Insert default admin account
INSERT INTO admin (full_name, email, contact_number, status, role) 
VALUES ('System Administrator', 'admin@hospital.com', '1234567890', 'Active', 'Admin')
ON DUPLICATE KEY UPDATE full_name=full_name;

-- Insert default login for admin (username: admin, password: admin123)
INSERT INTO login (username, password, role, admin_id) 
SELECT 'admin', 'admin123', 'Admin', admin_id 
FROM admin WHERE email = 'admin@hospital.com'
ON DUPLICATE KEY UPDATE username=username;



