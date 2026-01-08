-- Migration Script: Replace numeric IDs with formatted_id as primary keys
-- This script converts all tables to use formatted_id (D001, A001, etc.) as the primary key
-- and removes the numeric ID columns (doctor_id, nurse_id, etc.)
-- WARNING: This is a major schema change. Backup your database before running!

USE hospital_management_system;

-- ============================================================================
-- STEP 1: Ensure all formatted_id columns exist and are populated
-- ============================================================================

-- First, run the add_formatted_ids.sql script if you haven't already
-- This ensures all formatted_id columns exist

-- ============================================================================
-- STEP 2: Drop all foreign key constraints that reference numeric IDs
-- ============================================================================

-- Helper procedure to safely drop foreign key if it exists
DELIMITER //
DROP PROCEDURE IF EXISTS SafeDropForeignKey//
CREATE PROCEDURE SafeDropForeignKey(
    IN tableName VARCHAR(64),
    IN constraintName VARCHAR(64)
)
BEGIN
    DECLARE constraintCount INT DEFAULT 0;
    SET @dbname = DATABASE();
    
    SELECT COUNT(*) INTO constraintCount
    FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS
    WHERE TABLE_SCHEMA = @dbname
    AND TABLE_NAME = tableName
    AND CONSTRAINT_NAME = constraintName
    AND CONSTRAINT_TYPE = 'FOREIGN KEY';
    
    IF constraintCount > 0 THEN
        SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP FOREIGN KEY ', constraintName);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END IF;
END//
DELIMITER ;

-- Drop foreign keys from login table (common constraint names)
CALL SafeDropForeignKey('login', 'login_ibfk_1');
CALL SafeDropForeignKey('login', 'login_ibfk_2');
CALL SafeDropForeignKey('login', 'login_ibfk_3');
CALL SafeDropForeignKey('login', 'fk_login_doctor');
CALL SafeDropForeignKey('login', 'fk_login_nurse');
CALL SafeDropForeignKey('login', 'fk_login_admin');

-- Drop foreign keys from appointment table
CALL SafeDropForeignKey('appointment', 'appointment_ibfk_1');
CALL SafeDropForeignKey('appointment', 'appointment_ibfk_2');
CALL SafeDropForeignKey('appointment', 'fk_appointment_doctor');
CALL SafeDropForeignKey('appointment', 'fk_appointment_patient');
CALL SafeDropForeignKey('appointment', 'fk_appointment_nurse');

-- Drop foreign keys from medical_record table
CALL SafeDropForeignKey('medical_record', 'medical_record_ibfk_1');
CALL SafeDropForeignKey('medical_record', 'medical_record_ibfk_2');
CALL SafeDropForeignKey('medical_record', 'medical_record_ibfk_3');
CALL SafeDropForeignKey('medical_record', 'fk_medical_record_patient');
CALL SafeDropForeignKey('medical_record', 'fk_medical_record_doctor');
CALL SafeDropForeignKey('medical_record', 'fk_medical_record_diagnosis');

-- Drop foreign keys from treatment table
CALL SafeDropForeignKey('treatment', 'treatment_ibfk_1');
CALL SafeDropForeignKey('treatment', 'fk_treatment_doctor');

-- Drop foreign keys from finance table
CALL SafeDropForeignKey('finance', 'finance_ibfk_1');
CALL SafeDropForeignKey('finance', 'finance_ibfk_2');
CALL SafeDropForeignKey('finance', 'fk_finance_treatment');
CALL SafeDropForeignKey('finance', 'fk_finance_patient');

-- Drop foreign keys from prescription table
CALL SafeDropForeignKey('prescription', 'prescription_ibfk_1');
CALL SafeDropForeignKey('prescription', 'fk_prescription_pharmacy');

-- Drop foreign keys from diagnosis table
CALL SafeDropForeignKey('diagnosis', 'diagnosis_ibfk_1');
CALL SafeDropForeignKey('diagnosis', 'fk_diagnosis_prescription');

-- Clean up helper
DROP PROCEDURE IF EXISTS SafeDropForeignKey;

-- ============================================================================
-- STEP 3: Add new foreign key columns using formatted_id (VARCHAR)
-- (Only if they don't already exist)
-- ============================================================================

-- Helper procedure to add column only if it doesn't exist
DELIMITER //
DROP PROCEDURE IF EXISTS AddColumnIfNotExists//
CREATE PROCEDURE AddColumnIfNotExists(
    IN tableName VARCHAR(64),
    IN columnName VARCHAR(64),
    IN columnDefinition TEXT
)
BEGIN
    DECLARE columnCount INT DEFAULT 0;
    SET @dbname = DATABASE();
    
    SELECT COUNT(*) INTO columnCount
    FROM INFORMATION_SCHEMA.COLUMNS
    WHERE TABLE_SCHEMA = @dbname
    AND TABLE_NAME = tableName
    AND COLUMN_NAME = columnName;
    
    IF columnCount = 0 THEN
        SET @sql = CONCAT('ALTER TABLE ', tableName, ' ADD COLUMN ', columnDefinition);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END IF;
END//
DELIMITER ;

-- Login table: Add new VARCHAR columns for formatted_id foreign keys
CALL AddColumnIfNotExists('login', 'doctor_formatted_id', 'doctor_formatted_id VARCHAR(20) NULL AFTER admin_id');
CALL AddColumnIfNotExists('login', 'nurse_formatted_id', 'nurse_formatted_id VARCHAR(20) NULL AFTER doctor_formatted_id');
CALL AddColumnIfNotExists('login', 'admin_formatted_id', 'admin_formatted_id VARCHAR(20) NULL AFTER nurse_formatted_id');

-- Copy formatted_ids to new columns (only if old numeric columns exist and formatted_id columns exist)
UPDATE login l
JOIN doctor d ON l.doctor_id = d.doctor_id
SET l.doctor_formatted_id = d.formatted_id
WHERE l.doctor_id IS NOT NULL 
AND l.doctor_formatted_id IS NULL
AND EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'login' AND COLUMN_NAME = 'doctor_formatted_id');

UPDATE login l
JOIN nurse n ON l.nurse_id = n.nurse_id
SET l.nurse_formatted_id = n.formatted_id
WHERE l.nurse_id IS NOT NULL 
AND l.nurse_formatted_id IS NULL
AND EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'login' AND COLUMN_NAME = 'nurse_formatted_id');

UPDATE login l
JOIN admin a ON l.admin_id = a.admin_id
SET l.admin_formatted_id = a.formatted_id
WHERE l.admin_id IS NOT NULL 
AND l.admin_formatted_id IS NULL
AND EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'login' AND COLUMN_NAME = 'admin_formatted_id');

-- Appointment table: Add new VARCHAR columns for formatted_id foreign keys
CALL AddColumnIfNotExists('appointment', 'patient_formatted_id', 'patient_formatted_id VARCHAR(20) NULL AFTER appointment_id');
CALL AddColumnIfNotExists('appointment', 'nurse_formatted_id', 'nurse_formatted_id VARCHAR(20) NULL AFTER patient_formatted_id');
CALL AddColumnIfNotExists('appointment', 'doctor_formatted_id', 'doctor_formatted_id VARCHAR(20) NULL AFTER nurse_formatted_id');

UPDATE appointment a
JOIN patient p ON a.patient_id = p.patient_id
SET a.patient_formatted_id = p.formatted_id;

UPDATE appointment a
JOIN nurse n ON a.nurse_id = n.nurse_id
SET a.nurse_formatted_id = n.formatted_id;

UPDATE appointment a
JOIN doctor d ON a.doctor_id = d.doctor_id
SET a.doctor_formatted_id = d.formatted_id
WHERE a.doctor_id IS NOT NULL;

-- Medical_record table: Add new VARCHAR columns for formatted_id foreign keys
CALL AddColumnIfNotExists('medical_record', 'patient_formatted_id', 'patient_formatted_id VARCHAR(20) NULL AFTER record_id');
CALL AddColumnIfNotExists('medical_record', 'doctor_formatted_id', 'doctor_formatted_id VARCHAR(20) NULL AFTER patient_formatted_id');
CALL AddColumnIfNotExists('medical_record', 'diagnosis_formatted_id', 'diagnosis_formatted_id VARCHAR(20) NULL AFTER doctor_formatted_id');

UPDATE medical_record mr
JOIN patient p ON mr.patient_id = p.patient_id
SET mr.patient_formatted_id = p.formatted_id;

UPDATE medical_record mr
JOIN doctor d ON mr.doctor_id = d.doctor_id
SET mr.doctor_formatted_id = d.formatted_id;

UPDATE medical_record mr
JOIN diagnosis diag ON mr.diagnosis_id = diag.diagnosis_id
SET mr.diagnosis_formatted_id = diag.formatted_id
WHERE mr.diagnosis_id IS NOT NULL;

-- Treatment table: Add new VARCHAR column for formatted_id foreign key
CALL AddColumnIfNotExists('treatment', 'doctor_formatted_id', 'doctor_formatted_id VARCHAR(20) NULL AFTER treatment_id');

UPDATE treatment t
JOIN doctor d ON t.doctor_id = d.doctor_id
SET t.doctor_formatted_id = d.formatted_id;

-- Finance table: Add new VARCHAR columns for formatted_id foreign keys
CALL AddColumnIfNotExists('finance', 'treatment_formatted_id', 'treatment_formatted_id VARCHAR(20) NULL AFTER finance_id');
CALL AddColumnIfNotExists('finance', 'patient_formatted_id', 'patient_formatted_id VARCHAR(20) NULL AFTER treatment_formatted_id');

UPDATE finance f
JOIN treatment t ON f.treatment_id = t.treatment_id
SET f.treatment_formatted_id = t.formatted_id;

UPDATE finance f
JOIN patient p ON f.patient_id = p.patient_id
SET f.patient_formatted_id = p.formatted_id;

-- Prescription table: Add new VARCHAR column for formatted_id foreign key
CALL AddColumnIfNotExists('prescription', 'pharmacy_formatted_id', 'pharmacy_formatted_id VARCHAR(20) NULL AFTER prescription_id');

UPDATE prescription pr
JOIN pharmacy ph ON pr.pharmacy_id = ph.pharmacy_id
SET pr.pharmacy_formatted_id = ph.formatted_id;

-- Diagnosis table: Add new VARCHAR column for formatted_id foreign key
CALL AddColumnIfNotExists('diagnosis', 'prescription_formatted_id', 'prescription_formatted_id VARCHAR(20) NULL AFTER diagnosis_id');

UPDATE diagnosis diag
JOIN prescription pr ON diag.prescription_id = pr.prescription_id
SET diag.prescription_formatted_id = pr.formatted_id
WHERE diag.prescription_id IS NOT NULL;

-- ============================================================================
-- STEP 4: Make formatted_id the primary key for all main tables
-- (Safely drop old numeric ID columns if they exist)
-- ============================================================================

-- Helper to safely convert to formatted_id primary key
DELIMITER //
DROP PROCEDURE IF EXISTS SafeConvertToFormattedId//
CREATE PROCEDURE SafeConvertToFormattedId(
    IN tableName VARCHAR(64),
    IN numericIdColumn VARCHAR(64)
)
BEGIN
    DECLARE columnExists INT DEFAULT 0;
    DECLARE hasPK INT DEFAULT 0;
    DECLARE pkColumn VARCHAR(64);
    DECLARE isAutoIncrement INT DEFAULT 0;
    DECLARE idxExists INT DEFAULT 0;
    SET @dbname = DATABASE();
    
    -- Check if numeric ID column exists
    SELECT COUNT(*) INTO columnExists
    FROM INFORMATION_SCHEMA.COLUMNS
    WHERE TABLE_SCHEMA = @dbname
    AND TABLE_NAME = tableName
    AND COLUMN_NAME = numericIdColumn;
    
    IF columnExists > 0 THEN
        -- Check if the column has AUTO_INCREMENT
        SELECT COUNT(*) INTO isAutoIncrement
        FROM INFORMATION_SCHEMA.COLUMNS
        WHERE TABLE_SCHEMA = @dbname
        AND TABLE_NAME = tableName
        AND COLUMN_NAME = numericIdColumn
        AND EXTRA LIKE '%auto_increment%';
        
        -- Check if table has primary key and what column it's on
        SELECT COUNT(*), COALESCE(KCU.COLUMN_NAME, '') INTO hasPK, pkColumn
        FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS TC
        LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE KCU 
            ON TC.CONSTRAINT_SCHEMA = KCU.CONSTRAINT_SCHEMA
            AND TC.CONSTRAINT_NAME = KCU.CONSTRAINT_NAME
            AND TC.TABLE_NAME = KCU.TABLE_NAME
        WHERE TC.TABLE_SCHEMA = @dbname
        AND TC.TABLE_NAME = tableName
        AND TC.CONSTRAINT_TYPE = 'PRIMARY KEY'
        LIMIT 1;
        
        -- If primary key exists and is on the numeric ID column
        IF hasPK > 0 AND pkColumn = numericIdColumn THEN
            -- If column has AUTO_INCREMENT, remove it first
            IF isAutoIncrement > 0 THEN
                SET @sql = CONCAT('ALTER TABLE ', tableName, ' MODIFY ', numericIdColumn, ' INT NOT NULL');
                PREPARE stmt FROM @sql;
                EXECUTE stmt;
                DEALLOCATE PREPARE stmt;
            END IF;
            
            -- Now drop the primary key
            SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP PRIMARY KEY');
            PREPARE stmt FROM @sql;
            EXECUTE stmt;
            DEALLOCATE PREPARE stmt;
        END IF;
        
        -- Drop the numeric ID column
        SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP COLUMN ', numericIdColumn);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END IF;
    
    -- Check if formatted_id column exists before trying to make it primary key
    SELECT COUNT(*) INTO columnExists
    FROM INFORMATION_SCHEMA.COLUMNS
    WHERE TABLE_SCHEMA = @dbname
    AND TABLE_NAME = tableName
    AND COLUMN_NAME = 'formatted_id';
    
    IF columnExists > 0 THEN
        -- Check if formatted_id is already the primary key
        SELECT COUNT(*) INTO hasPK
        FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS TC
        JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE KCU 
            ON TC.CONSTRAINT_SCHEMA = KCU.CONSTRAINT_SCHEMA
            AND TC.CONSTRAINT_NAME = KCU.CONSTRAINT_NAME
            AND TC.TABLE_NAME = KCU.TABLE_NAME
        WHERE TC.TABLE_SCHEMA = @dbname
        AND TC.TABLE_NAME = tableName
        AND TC.CONSTRAINT_TYPE = 'PRIMARY KEY'
        AND KCU.COLUMN_NAME = 'formatted_id';
        
        IF hasPK = 0 THEN
            -- Drop existing primary key if it's on a different column
            SELECT COUNT(*) INTO hasPK
            FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS
            WHERE TABLE_SCHEMA = @dbname
            AND TABLE_NAME = tableName
            AND CONSTRAINT_TYPE = 'PRIMARY KEY';
            
            IF hasPK > 0 THEN
                SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP PRIMARY KEY');
                PREPARE stmt FROM @sql;
                EXECUTE stmt;
                DEALLOCATE PREPARE stmt;
            END IF;
            
            -- Check if there's an index named 'formatted_id' (non-primary key index)
            SELECT COUNT(*) INTO idxExists
            FROM INFORMATION_SCHEMA.STATISTICS
            WHERE TABLE_SCHEMA = @dbname
            AND TABLE_NAME = tableName
            AND INDEX_NAME = 'formatted_id'
            AND INDEX_NAME != 'PRIMARY';
            
            IF idxExists > 0 THEN
                SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP INDEX formatted_id');
                PREPARE stmt FROM @sql;
                EXECUTE stmt;
                DEALLOCATE PREPARE stmt;
            END IF;
            
            -- Ensure formatted_id is NOT NULL first
            SET @sql = CONCAT('ALTER TABLE ', tableName, ' MODIFY formatted_id VARCHAR(20) NOT NULL');
            PREPARE stmt FROM @sql;
            EXECUTE stmt;
            DEALLOCATE PREPARE stmt;
            
            -- Make formatted_id the primary key (in separate statement)
            -- Use a try-catch approach by checking if primary key already exists
            SET @sql = CONCAT('ALTER TABLE ', tableName, ' ADD PRIMARY KEY (formatted_id)');
            PREPARE stmt FROM @sql;
            BEGIN
                DECLARE CONTINUE HANDLER FOR 1061 -- Error 1061: Duplicate key name
                BEGIN
                    -- Primary key already exists, ignore the error
                END;
                EXECUTE stmt;
            END;
            DEALLOCATE PREPARE stmt;
        END IF;
    END IF;
END//
DELIMITER ;

-- Convert all tables to use formatted_id as primary key
CALL SafeConvertToFormattedId('doctor', 'doctor_id');
CALL SafeConvertToFormattedId('nurse', 'nurse_id');
CALL SafeConvertToFormattedId('admin', 'admin_id');
CALL SafeConvertToFormattedId('patient', 'patient_id');
CALL SafeConvertToFormattedId('pharmacy', 'pharmacy_id');
CALL SafeConvertToFormattedId('prescription', 'prescription_id');
CALL SafeConvertToFormattedId('diagnosis', 'diagnosis_id');
CALL SafeConvertToFormattedId('medical_record', 'record_id');
CALL SafeConvertToFormattedId('appointment', 'appointment_id');
CALL SafeConvertToFormattedId('treatment', 'treatment_id');
CALL SafeConvertToFormattedId('finance', 'finance_id');
CALL SafeConvertToFormattedId('login', 'login_id');

-- Clean up procedures
DROP PROCEDURE IF EXISTS SafeConvertToFormattedId;
DROP PROCEDURE IF EXISTS AddColumnIfNotExists;

-- ============================================================================
-- STEP 5: Update foreign key columns to NOT NULL and rename them
-- (Only drop old numeric columns if they exist)
-- ============================================================================

-- Helper to safely drop column if it exists
DELIMITER //
DROP PROCEDURE IF EXISTS DropColumnIfExists//
CREATE PROCEDURE DropColumnIfExists(
    IN tableName VARCHAR(64),
    IN columnName VARCHAR(64)
)
BEGIN
    DECLARE columnCount INT DEFAULT 0;
    SET @dbname = DATABASE();
    
    SELECT COUNT(*) INTO columnCount
    FROM INFORMATION_SCHEMA.COLUMNS
    WHERE TABLE_SCHEMA = @dbname
    AND TABLE_NAME = tableName
    AND COLUMN_NAME = columnName;
    
    IF columnCount > 0 THEN
        SET @sql = CONCAT('ALTER TABLE ', tableName, ' DROP COLUMN ', columnName);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END IF;
END//
DELIMITER ;

-- Login table
CALL DropColumnIfExists('login', 'doctor_id');
CALL DropColumnIfExists('login', 'nurse_id');
CALL DropColumnIfExists('login', 'admin_id');
ALTER TABLE login 
MODIFY doctor_formatted_id VARCHAR(20),
MODIFY nurse_formatted_id VARCHAR(20),
MODIFY admin_formatted_id VARCHAR(20);

-- Appointment table
CALL DropColumnIfExists('appointment', 'patient_id');
CALL DropColumnIfExists('appointment', 'nurse_id');
CALL DropColumnIfExists('appointment', 'doctor_id');
ALTER TABLE appointment 
MODIFY patient_formatted_id VARCHAR(20) NOT NULL,
MODIFY nurse_formatted_id VARCHAR(20) NOT NULL;

-- Medical_record table
CALL DropColumnIfExists('medical_record', 'patient_id');
CALL DropColumnIfExists('medical_record', 'doctor_id');
CALL DropColumnIfExists('medical_record', 'diagnosis_id');
ALTER TABLE medical_record 
MODIFY patient_formatted_id VARCHAR(20) NOT NULL,
MODIFY doctor_formatted_id VARCHAR(20) NOT NULL,
MODIFY diagnosis_formatted_id VARCHAR(20);

-- Treatment table
CALL DropColumnIfExists('treatment', 'doctor_id');
ALTER TABLE treatment 
MODIFY doctor_formatted_id VARCHAR(20) NOT NULL;

-- Finance table
CALL DropColumnIfExists('finance', 'treatment_id');
CALL DropColumnIfExists('finance', 'patient_id');
ALTER TABLE finance 
MODIFY treatment_formatted_id VARCHAR(20) NOT NULL,
MODIFY patient_formatted_id VARCHAR(20) NOT NULL;

-- Prescription table
CALL DropColumnIfExists('prescription', 'pharmacy_id');
ALTER TABLE prescription 
MODIFY pharmacy_formatted_id VARCHAR(20) NOT NULL;

-- Diagnosis table
CALL DropColumnIfExists('diagnosis', 'prescription_id');
ALTER TABLE diagnosis 
MODIFY prescription_formatted_id VARCHAR(20);

-- ============================================================================
-- STEP 6: Add new foreign key constraints using formatted_id
-- (Drop existing constraints first if they exist)
-- ============================================================================

-- Login table
CALL DropForeignKeyIfExists('login', 'fk_login_doctor');
CALL DropForeignKeyIfExists('login', 'fk_login_nurse');
CALL DropForeignKeyIfExists('login', 'fk_login_admin');
ALTER TABLE login 
ADD CONSTRAINT fk_login_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_login_nurse FOREIGN KEY (nurse_formatted_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_login_admin FOREIGN KEY (admin_formatted_id) REFERENCES admin(formatted_id) ON DELETE CASCADE;

-- Appointment table
CALL DropForeignKeyIfExists('appointment', 'fk_appointment_patient');
CALL DropForeignKeyIfExists('appointment', 'fk_appointment_nurse');
CALL DropForeignKeyIfExists('appointment', 'fk_appointment_doctor');
ALTER TABLE appointment 
ADD CONSTRAINT fk_appointment_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_appointment_nurse FOREIGN KEY (nurse_formatted_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_appointment_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE;

-- Medical_record table
CALL DropForeignKeyIfExists('medical_record', 'fk_medical_record_patient');
CALL DropForeignKeyIfExists('medical_record', 'fk_medical_record_doctor');
CALL DropForeignKeyIfExists('medical_record', 'fk_medical_record_diagnosis');
ALTER TABLE medical_record 
ADD CONSTRAINT fk_medical_record_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_medical_record_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_medical_record_diagnosis FOREIGN KEY (diagnosis_formatted_id) REFERENCES diagnosis(formatted_id) ON DELETE SET NULL;

-- Treatment table
CALL DropForeignKeyIfExists('treatment', 'fk_treatment_doctor');
ALTER TABLE treatment 
ADD CONSTRAINT fk_treatment_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE;

-- Finance table
CALL DropForeignKeyIfExists('finance', 'fk_finance_treatment');
CALL DropForeignKeyIfExists('finance', 'fk_finance_patient');
ALTER TABLE finance 
ADD CONSTRAINT fk_finance_treatment FOREIGN KEY (treatment_formatted_id) REFERENCES treatment(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_finance_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE;

-- Prescription table
CALL DropForeignKeyIfExists('prescription', 'fk_prescription_pharmacy');
ALTER TABLE prescription 
ADD CONSTRAINT fk_prescription_pharmacy FOREIGN KEY (pharmacy_formatted_id) REFERENCES pharmacy(formatted_id) ON DELETE CASCADE;

-- Diagnosis table
CALL DropForeignKeyIfExists('diagnosis', 'fk_diagnosis_prescription');
ALTER TABLE diagnosis 
ADD CONSTRAINT fk_diagnosis_prescription FOREIGN KEY (prescription_formatted_id) REFERENCES prescription(formatted_id) ON DELETE SET NULL;

-- Clean up procedure
DROP PROCEDURE IF EXISTS DropForeignKeyIfExists;

-- ============================================================================
-- STEP 7: Update triggers to generate formatted_id before insert
-- ============================================================================

DROP TRIGGER IF EXISTS trg_doctor_formatted_id;
DROP TRIGGER IF EXISTS trg_admin_formatted_id;
DROP TRIGGER IF EXISTS trg_patient_formatted_id;
DROP TRIGGER IF EXISTS trg_nurse_formatted_id;
DROP TRIGGER IF EXISTS trg_pharmacy_formatted_id;
DROP TRIGGER IF EXISTS trg_treatment_formatted_id;
DROP TRIGGER IF EXISTS trg_appointment_formatted_id;
DROP TRIGGER IF EXISTS trg_prescription_formatted_id;
DROP TRIGGER IF EXISTS trg_diagnosis_formatted_id;
DROP TRIGGER IF EXISTS trg_medical_record_formatted_id;
DROP TRIGGER IF EXISTS trg_finance_formatted_id;
DROP TRIGGER IF EXISTS trg_login_formatted_id;

-- Doctor Trigger: D001, D002, etc. (generates before insert)
DELIMITER //
CREATE TRIGGER trg_doctor_formatted_id
BEFORE INSERT ON doctor
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM doctor;
        SET NEW.formatted_id = CONCAT('D', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Admin Trigger: A001, A002, etc.
DELIMITER //
CREATE TRIGGER trg_admin_formatted_id
BEFORE INSERT ON admin
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM admin;
        SET NEW.formatted_id = CONCAT('A', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Patient Trigger: P001, P002, etc.
DELIMITER //
CREATE TRIGGER trg_patient_formatted_id
BEFORE INSERT ON patient
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM patient;
        SET NEW.formatted_id = CONCAT('P', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Nurse Trigger: N001, N002, etc.
DELIMITER //
CREATE TRIGGER trg_nurse_formatted_id
BEFORE INSERT ON nurse
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM nurse;
        SET NEW.formatted_id = CONCAT('N', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Pharmacy Trigger: PH001, PH002, etc.
DELIMITER //
CREATE TRIGGER trg_pharmacy_formatted_id
BEFORE INSERT ON pharmacy
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num FROM pharmacy;
        SET NEW.formatted_id = CONCAT('PH', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Treatment Trigger: T001, T002, etc.
DELIMITER //
CREATE TRIGGER trg_treatment_formatted_id
BEFORE INSERT ON treatment
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM treatment;
        SET NEW.formatted_id = CONCAT('T', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Appointment Trigger: AP001, AP002, etc.
DELIMITER //
CREATE TRIGGER trg_appointment_formatted_id
BEFORE INSERT ON appointment
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num FROM appointment;
        SET NEW.formatted_id = CONCAT('AP', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Prescription Trigger: PR001, PR002, etc.
DELIMITER //
CREATE TRIGGER trg_prescription_formatted_id
BEFORE INSERT ON prescription
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num FROM prescription;
        SET NEW.formatted_id = CONCAT('PR', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Diagnosis Trigger: DI001, DI002, etc.
DELIMITER //
CREATE TRIGGER trg_diagnosis_formatted_id
BEFORE INSERT ON diagnosis
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num FROM diagnosis;
        SET NEW.formatted_id = CONCAT('DI', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Medical Record Trigger: MR001, MR002, etc.
DELIMITER //
CREATE TRIGGER trg_medical_record_formatted_id
BEFORE INSERT ON medical_record
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num FROM medical_record;
        SET NEW.formatted_id = CONCAT('MR', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Finance Trigger: F001, F002, etc.
DELIMITER //
CREATE TRIGGER trg_finance_formatted_id
BEFORE INSERT ON finance
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM finance;
        SET NEW.formatted_id = CONCAT('F', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- Login Trigger: L001, L002, etc.
DELIMITER //
CREATE TRIGGER trg_login_formatted_id
BEFORE INSERT ON login
FOR EACH ROW
BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num FROM login;
        SET NEW.formatted_id = CONCAT('L', LPAD(next_num, 3, '0'));
    END IF;
END//
DELIMITER ;

-- ============================================================================
-- STEP 8: Rename foreign key columns to simpler names
-- ============================================================================

ALTER TABLE login 
CHANGE COLUMN doctor_formatted_id doctor_id VARCHAR(20),
CHANGE COLUMN nurse_formatted_id nurse_id VARCHAR(20),
CHANGE COLUMN admin_formatted_id admin_id VARCHAR(20);

ALTER TABLE appointment 
CHANGE COLUMN patient_formatted_id patient_id VARCHAR(20),
CHANGE COLUMN nurse_formatted_id nurse_id VARCHAR(20),
CHANGE COLUMN doctor_formatted_id doctor_id VARCHAR(20);

ALTER TABLE medical_record 
CHANGE COLUMN patient_formatted_id patient_id VARCHAR(20),
CHANGE COLUMN doctor_formatted_id doctor_id VARCHAR(20),
CHANGE COLUMN diagnosis_formatted_id diagnosis_id VARCHAR(20);

ALTER TABLE treatment 
CHANGE COLUMN doctor_formatted_id doctor_id VARCHAR(20);

ALTER TABLE finance 
CHANGE COLUMN treatment_formatted_id treatment_id VARCHAR(20),
CHANGE COLUMN patient_formatted_id patient_id VARCHAR(20);

ALTER TABLE prescription 
CHANGE COLUMN pharmacy_formatted_id pharmacy_id VARCHAR(20);

ALTER TABLE diagnosis 
CHANGE COLUMN prescription_formatted_id prescription_id VARCHAR(20);

-- ============================================================================
-- Completion Message
-- ============================================================================
SELECT 'Migration to formatted_id primary keys completed successfully!' AS Status;

