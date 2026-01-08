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

-- Drop foreign keys from login table
ALTER TABLE login DROP FOREIGN KEY IF EXISTS login_ibfk_1;
ALTER TABLE login DROP FOREIGN KEY IF EXISTS login_ibfk_2;
ALTER TABLE login DROP FOREIGN KEY IF EXISTS login_ibfk_3;

-- Drop foreign keys from appointment table
ALTER TABLE appointment DROP FOREIGN KEY IF EXISTS appointment_ibfk_1;
ALTER TABLE appointment DROP FOREIGN KEY IF EXISTS appointment_ibfk_2;
ALTER TABLE appointment DROP FOREIGN KEY IF EXISTS fk_appointment_doctor;

-- Drop foreign keys from medical_record table
ALTER TABLE medical_record DROP FOREIGN KEY IF EXISTS medical_record_ibfk_1;
ALTER TABLE medical_record DROP FOREIGN KEY IF EXISTS medical_record_ibfk_2;
ALTER TABLE medical_record DROP FOREIGN KEY IF EXISTS medical_record_ibfk_3;

-- Drop foreign keys from treatment table
ALTER TABLE treatment DROP FOREIGN KEY IF EXISTS treatment_ibfk_1;

-- Drop foreign keys from finance table
ALTER TABLE finance DROP FOREIGN KEY IF EXISTS finance_ibfk_1;
ALTER TABLE finance DROP FOREIGN KEY IF EXISTS finance_ibfk_2;

-- Drop foreign keys from prescription table
ALTER TABLE prescription DROP FOREIGN KEY IF EXISTS prescription_ibfk_1;

-- Drop foreign keys from diagnosis table
ALTER TABLE diagnosis DROP FOREIGN KEY IF EXISTS diagnosis_ibfk_1;

-- ============================================================================
-- STEP 3: Add new foreign key columns using formatted_id (VARCHAR)
-- ============================================================================

-- Login table: Change doctor_id, nurse_id, admin_id to VARCHAR
ALTER TABLE login 
ADD COLUMN doctor_formatted_id VARCHAR(20) NULL AFTER admin_id,
ADD COLUMN nurse_formatted_id VARCHAR(20) NULL AFTER doctor_formatted_id,
ADD COLUMN admin_formatted_id VARCHAR(20) NULL AFTER nurse_formatted_id;

-- Copy formatted_ids to new columns
UPDATE login l
JOIN doctor d ON l.doctor_id = d.doctor_id
SET l.doctor_formatted_id = d.formatted_id
WHERE l.doctor_id IS NOT NULL;

UPDATE login l
JOIN nurse n ON l.nurse_id = n.nurse_id
SET l.nurse_formatted_id = n.formatted_id
WHERE l.nurse_id IS NOT NULL;

UPDATE login l
JOIN admin a ON l.admin_id = a.admin_id
SET l.admin_formatted_id = a.formatted_id
WHERE l.admin_id IS NOT NULL;

-- Appointment table: Change patient_id, nurse_id, doctor_id to VARCHAR
ALTER TABLE appointment 
ADD COLUMN patient_formatted_id VARCHAR(20) NULL AFTER appointment_id,
ADD COLUMN nurse_formatted_id VARCHAR(20) NULL AFTER patient_formatted_id,
ADD COLUMN doctor_formatted_id VARCHAR(20) NULL AFTER nurse_formatted_id;

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

-- Medical_record table: Change patient_id, doctor_id, diagnosis_id to VARCHAR
ALTER TABLE medical_record 
ADD COLUMN patient_formatted_id VARCHAR(20) NULL AFTER record_id,
ADD COLUMN doctor_formatted_id VARCHAR(20) NULL AFTER patient_formatted_id,
ADD COLUMN diagnosis_formatted_id VARCHAR(20) NULL AFTER doctor_formatted_id;

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

-- Treatment table: Change doctor_id to VARCHAR
ALTER TABLE treatment 
ADD COLUMN doctor_formatted_id VARCHAR(20) NULL AFTER treatment_id;

UPDATE treatment t
JOIN doctor d ON t.doctor_id = d.doctor_id
SET t.doctor_formatted_id = d.formatted_id;

-- Finance table: Change treatment_id, patient_id to VARCHAR
ALTER TABLE finance 
ADD COLUMN treatment_formatted_id VARCHAR(20) NULL AFTER finance_id,
ADD COLUMN patient_formatted_id VARCHAR(20) NULL AFTER treatment_formatted_id;

UPDATE finance f
JOIN treatment t ON f.treatment_id = t.treatment_id
SET f.treatment_formatted_id = t.formatted_id;

UPDATE finance f
JOIN patient p ON f.patient_id = p.patient_id
SET f.patient_formatted_id = p.formatted_id;

-- Prescription table: Change pharmacy_id to VARCHAR
ALTER TABLE prescription 
ADD COLUMN pharmacy_formatted_id VARCHAR(20) NULL AFTER prescription_id;

UPDATE prescription pr
JOIN pharmacy ph ON pr.pharmacy_id = ph.pharmacy_id
SET pr.pharmacy_formatted_id = ph.formatted_id;

-- Diagnosis table: Change prescription_id to VARCHAR
ALTER TABLE diagnosis 
ADD COLUMN prescription_formatted_id VARCHAR(20) NULL AFTER diagnosis_id;

UPDATE diagnosis diag
JOIN prescription pr ON diag.prescription_id = pr.prescription_id
SET diag.prescription_formatted_id = pr.formatted_id
WHERE diag.prescription_id IS NOT NULL;

-- ============================================================================
-- STEP 4: Make formatted_id the primary key for all main tables
-- ============================================================================

-- Drop old primary keys and auto_increment
ALTER TABLE doctor DROP PRIMARY KEY, MODIFY doctor_id INT, DROP COLUMN doctor_id;
ALTER TABLE doctor MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE nurse DROP PRIMARY KEY, MODIFY nurse_id INT, DROP COLUMN nurse_id;
ALTER TABLE nurse MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE admin DROP PRIMARY KEY, MODIFY admin_id INT, DROP COLUMN admin_id;
ALTER TABLE admin MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE patient DROP PRIMARY KEY, MODIFY patient_id INT, DROP COLUMN patient_id;
ALTER TABLE patient MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE pharmacy DROP PRIMARY KEY, MODIFY pharmacy_id INT, DROP COLUMN pharmacy_id;
ALTER TABLE pharmacy MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE prescription DROP PRIMARY KEY, MODIFY prescription_id INT, DROP COLUMN prescription_id;
ALTER TABLE prescription MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE diagnosis DROP PRIMARY KEY, MODIFY diagnosis_id INT, DROP COLUMN diagnosis_id;
ALTER TABLE diagnosis MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE medical_record DROP PRIMARY KEY, MODIFY record_id INT, DROP COLUMN record_id;
ALTER TABLE medical_record MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE appointment DROP PRIMARY KEY, MODIFY appointment_id INT, DROP COLUMN appointment_id;
ALTER TABLE appointment MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE treatment DROP PRIMARY KEY, MODIFY treatment_id INT, DROP COLUMN treatment_id;
ALTER TABLE treatment MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE finance DROP PRIMARY KEY, MODIFY finance_id INT, DROP COLUMN finance_id;
ALTER TABLE finance MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

ALTER TABLE login DROP PRIMARY KEY, MODIFY login_id INT, DROP COLUMN login_id;
ALTER TABLE login MODIFY formatted_id VARCHAR(20) NOT NULL PRIMARY KEY;

-- ============================================================================
-- STEP 5: Update foreign key columns to NOT NULL and rename them
-- ============================================================================

-- Login table
ALTER TABLE login 
DROP COLUMN doctor_id, DROP COLUMN nurse_id, DROP COLUMN admin_id,
MODIFY doctor_formatted_id VARCHAR(20),
MODIFY nurse_formatted_id VARCHAR(20),
MODIFY admin_formatted_id VARCHAR(20);

-- Appointment table
ALTER TABLE appointment 
DROP COLUMN patient_id, DROP COLUMN nurse_id, DROP COLUMN doctor_id,
MODIFY patient_formatted_id VARCHAR(20) NOT NULL,
MODIFY nurse_formatted_id VARCHAR(20) NOT NULL;

-- Medical_record table
ALTER TABLE medical_record 
DROP COLUMN patient_id, DROP COLUMN doctor_id, DROP COLUMN diagnosis_id,
MODIFY patient_formatted_id VARCHAR(20) NOT NULL,
MODIFY doctor_formatted_id VARCHAR(20) NOT NULL,
MODIFY diagnosis_formatted_id VARCHAR(20);

-- Treatment table
ALTER TABLE treatment 
DROP COLUMN doctor_id,
MODIFY doctor_formatted_id VARCHAR(20) NOT NULL;

-- Finance table
ALTER TABLE finance 
DROP COLUMN treatment_id, DROP COLUMN patient_id,
MODIFY treatment_formatted_id VARCHAR(20) NOT NULL,
MODIFY patient_formatted_id VARCHAR(20) NOT NULL;

-- Prescription table
ALTER TABLE prescription 
DROP COLUMN pharmacy_id,
MODIFY pharmacy_formatted_id VARCHAR(20) NOT NULL;

-- Diagnosis table
ALTER TABLE diagnosis 
DROP COLUMN prescription_id,
MODIFY prescription_formatted_id VARCHAR(20);

-- ============================================================================
-- STEP 6: Add new foreign key constraints using formatted_id
-- ============================================================================

ALTER TABLE login 
ADD CONSTRAINT fk_login_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_login_nurse FOREIGN KEY (nurse_formatted_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_login_admin FOREIGN KEY (admin_formatted_id) REFERENCES admin(formatted_id) ON DELETE CASCADE;

ALTER TABLE appointment 
ADD CONSTRAINT fk_appointment_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_appointment_nurse FOREIGN KEY (nurse_formatted_id) REFERENCES nurse(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_appointment_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE;

ALTER TABLE medical_record 
ADD CONSTRAINT fk_medical_record_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_medical_record_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_medical_record_diagnosis FOREIGN KEY (diagnosis_formatted_id) REFERENCES diagnosis(formatted_id) ON DELETE SET NULL;

ALTER TABLE treatment 
ADD CONSTRAINT fk_treatment_doctor FOREIGN KEY (doctor_formatted_id) REFERENCES doctor(formatted_id) ON DELETE CASCADE;

ALTER TABLE finance 
ADD CONSTRAINT fk_finance_treatment FOREIGN KEY (treatment_formatted_id) REFERENCES treatment(formatted_id) ON DELETE CASCADE,
ADD CONSTRAINT fk_finance_patient FOREIGN KEY (patient_formatted_id) REFERENCES patient(formatted_id) ON DELETE CASCADE;

ALTER TABLE prescription 
ADD CONSTRAINT fk_prescription_pharmacy FOREIGN KEY (pharmacy_formatted_id) REFERENCES pharmacy(formatted_id) ON DELETE CASCADE;

ALTER TABLE diagnosis 
ADD CONSTRAINT fk_diagnosis_prescription FOREIGN KEY (prescription_formatted_id) REFERENCES prescription(formatted_id) ON DELETE SET NULL;

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

