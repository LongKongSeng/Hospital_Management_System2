-- ============================================================================
-- Hospital Management System - Complete Database Schema
-- All IDs use formatted_id format (D001, A001, P001, etc.) from the start
-- Based on actual code usage in the application
-- ============================================================================

DROP DATABASE IF EXISTS hospital_management_system;
CREATE DATABASE hospital_management_system;
USE hospital_management_system;

-- ============================================================================
-- MAIN TABLES
-- ============================================================================

-- DOCTOR Table
CREATE TABLE doctor (
    formatted_id VARCHAR(20) PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    specialization VARCHAR(100) NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    ic_number VARCHAR(20) UNIQUE NOT NULL,
    availability ENUM('Available', 'Busy', 'Off') DEFAULT 'Available',
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Doctor',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- NURSE Table
CREATE TABLE nurse (
    formatted_id VARCHAR(20) PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    ic_number VARCHAR(20) UNIQUE NOT NULL,
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Nurse',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ADMIN Table
CREATE TABLE admin (
    formatted_id VARCHAR(20) PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    ic_number VARCHAR(20) UNIQUE NOT NULL,
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    role VARCHAR(20) DEFAULT 'Admin',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PATIENT Table
CREATE TABLE patient (
    formatted_id VARCHAR(20) PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    date_of_birth DATE NOT NULL,
    contact_number VARCHAR(20) NOT NULL,
    blood_type VARCHAR(5),
    emergency_contact VARCHAR(100),
    ic_number VARCHAR(20) UNIQUE NOT NULL,
    status ENUM('Active', 'Inactive') DEFAULT 'Active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PHARMACY Table (Medication Inventory)
CREATE TABLE pharmacy (
    formatted_id VARCHAR(20) PRIMARY KEY,
    medicine_name VARCHAR(100) NOT NULL,
    category_of_meds VARCHAR(50) NOT NULL,
    quantity INT DEFAULT 0,
    unit_price DECIMAL(10, 2) DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- PRESCRIPTION Table
CREATE TABLE prescription (
    formatted_id VARCHAR(20) PRIMARY KEY,
    pharmacy_id VARCHAR(20) NOT NULL,
    dosage VARCHAR(50),
    duration_of_meds VARCHAR(50),
    instructions TEXT,
    date DATE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (pharmacy_id) REFERENCES pharmacy(formatted_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- DIAGNOSIS Table
CREATE TABLE diagnosis (
    formatted_id VARCHAR(20) PRIMARY KEY,
    disease VARCHAR(200),
    disorder VARCHAR(200),
    duration_of_pain VARCHAR(50),
    severity VARCHAR(50),
    prescription_id VARCHAR(20) NULL,
    date DATE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (prescription_id) REFERENCES prescription(formatted_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- MEDICAL_RECORD Table
CREATE TABLE medical_record (
    formatted_id VARCHAR(20) PRIMARY KEY,
    patient_id VARCHAR(20) NOT NULL,
    doctor_id VARCHAR(20) NOT NULL,
    diagnosis_id VARCHAR(20) NULL,
    date_of_record DATE NOT NULL,
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (diagnosis_id) REFERENCES diagnosis(formatted_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- APPOINTMENT Table
CREATE TABLE appointment (
    formatted_id VARCHAR(20) PRIMARY KEY,
    patient_id VARCHAR(20) NOT NULL,
    nurse_id VARCHAR(20) NOT NULL,
    doctor_id VARCHAR(20) NULL,
    appointment_date DATE NOT NULL,
    appointment_time TIME NOT NULL,
    status ENUM('Scheduled', 'Completed', 'Cancelled') DEFAULT 'Scheduled',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (nurse_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctor(formatted_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- TREATMENT Table
CREATE TABLE treatment (
    formatted_id VARCHAR(20) PRIMARY KEY,
    doctor_id VARCHAR(20) NOT NULL,
    dressing_applied VARCHAR(200),
    consultation_fee DECIMAL(10, 2) DEFAULT 0.00,
    treatment_fee DECIMAL(10, 2) DEFAULT 0.00,
    treatment_date DATE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (doctor_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- FINANCE Table (Patient Receipt/Payment)
-- Note: ERD shows only Treatment_ID FK, but patient_id is included for easier querying
-- Patient can be accessed through Treatment->Medical_Record->Patient if needed
CREATE TABLE finance (
    formatted_id VARCHAR(20) PRIMARY KEY,
    treatment_id VARCHAR(20) NOT NULL,
    date_of_payment DATE NOT NULL,
    payment_status ENUM('Paid', 'Pending', 'Partial') DEFAULT 'Pending',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (treatment_id) REFERENCES treatment(formatted_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- LOGIN Table (Centralized login system)
CREATE TABLE login (
    formatted_id VARCHAR(20) PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    role ENUM('Doctor', 'Nurse', 'Admin') NOT NULL,
    doctor_id VARCHAR(20) NULL,
    nurse_id VARCHAR(20) NULL,
    admin_id VARCHAR(20) NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (doctor_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (nurse_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
    FOREIGN KEY (admin_id) REFERENCES admin(formatted_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ============================================================================
-- TRIGGERS TO AUTO-GENERATE FORMATTED IDs
-- ============================================================================

DELIMITER //

-- Doctor Trigger: D001, D002, D003, etc.
CREATE TRIGGER trg_doctor_formatted_id
BEFORE INSERT ON doctor
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM doctor
        WHERE formatted_id LIKE 'D%';
        SET NEW.formatted_id = CONCAT('D', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Nurse Trigger: N001, N002, N003, etc.
CREATE TRIGGER trg_nurse_formatted_id
BEFORE INSERT ON nurse
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM nurse
        WHERE formatted_id LIKE 'N%';
        SET NEW.formatted_id = CONCAT('N', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Admin Trigger: A001, A002, A003, etc.
CREATE TRIGGER trg_admin_formatted_id
BEFORE INSERT ON admin
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM admin
        WHERE formatted_id LIKE 'A%';
        SET NEW.formatted_id = CONCAT('A', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Patient Trigger: P001, P002, P003, etc.
CREATE TRIGGER trg_patient_formatted_id
BEFORE INSERT ON patient
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM patient
        WHERE formatted_id LIKE 'P%';
        SET NEW.formatted_id = CONCAT('P', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Pharmacy Trigger: PH001, PH002, PH003, etc.
CREATE TRIGGER trg_pharmacy_formatted_id
BEFORE INSERT ON pharmacy
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM pharmacy
        WHERE formatted_id LIKE 'PH%';
        SET NEW.formatted_id = CONCAT('PH', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Prescription Trigger: PR001, PR002, PR003, etc.
CREATE TRIGGER trg_prescription_formatted_id
BEFORE INSERT ON prescription
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM prescription
        WHERE formatted_id LIKE 'PR%';
        SET NEW.formatted_id = CONCAT('PR', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Diagnosis Trigger: DI001, DI002, DI003, etc.
CREATE TRIGGER trg_diagnosis_formatted_id
BEFORE INSERT ON diagnosis
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM diagnosis
        WHERE formatted_id LIKE 'DI%';
        SET NEW.formatted_id = CONCAT('DI', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Medical Record Trigger: MR001, MR002, MR003, etc.
CREATE TRIGGER trg_medical_record_formatted_id
BEFORE INSERT ON medical_record
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM medical_record
        WHERE formatted_id LIKE 'MR%';
        SET NEW.formatted_id = CONCAT('MR', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Appointment Trigger: AP001, AP002, AP003, etc.
CREATE TRIGGER trg_appointment_formatted_id
BEFORE INSERT ON appointment
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM appointment
        WHERE formatted_id LIKE 'AP%';
        SET NEW.formatted_id = CONCAT('AP', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Treatment Trigger: T001, T002, T003, etc.
CREATE TRIGGER trg_treatment_formatted_id
BEFORE INSERT ON treatment
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM treatment
        WHERE formatted_id LIKE 'T%';
        SET NEW.formatted_id = CONCAT('T', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Finance Trigger: F001, F002, F003, etc.
CREATE TRIGGER trg_finance_formatted_id
BEFORE INSERT ON finance
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM finance
        WHERE formatted_id LIKE 'F%';
        SET NEW.formatted_id = CONCAT('F', LPAD(next_num, 3, '0'));
    END IF;
END//

-- Login Trigger: L001, L002, L003, etc.
CREATE TRIGGER trg_login_formatted_id
BEFORE INSERT ON login
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM login
        WHERE formatted_id LIKE 'L%';
        SET NEW.formatted_id = CONCAT('L', LPAD(next_num, 3, '0'));
    END IF;
END//

DELIMITER ;

-- ============================================================================
-- INDEXES FOR BETTER PERFORMANCE
-- ============================================================================

CREATE INDEX idx_login_username ON login(username);
CREATE INDEX idx_patient_status ON patient(status);
CREATE INDEX idx_patient_ic ON patient(ic_number);
CREATE INDEX idx_appointment_date ON appointment(appointment_date);
CREATE INDEX idx_pharmacy_category ON pharmacy(category_of_meds);
CREATE INDEX idx_doctor_status ON doctor(status);
CREATE INDEX idx_nurse_status ON nurse(status);

-- ============================================================================
-- DEFAULT DATA
-- ============================================================================

-- Insert default admin account (formatted_id will be auto-generated by trigger)
INSERT INTO admin (formatted_id, full_name, email, contact_number, ic_number, status, role) 
VALUES (NULL, 'System Administrator', 'admin@hospital.com', '1234567890', 'ADMIN001', 'Active', 'Admin');

-- Insert default login for admin (username: admin, password: admin123)
-- Note: formatted_id will be auto-generated, and admin_id will reference the admin's formatted_id
INSERT INTO login (formatted_id, username, password, role, admin_id) 
SELECT NULL, 'admin', 'admin123', 'Admin', formatted_id 
FROM admin WHERE email = 'admin@hospital.com';

-- ============================================================================
-- END OF SCHEMA
-- ============================================================================

