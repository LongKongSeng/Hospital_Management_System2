-- Migration Script: Add Formatted IDs to All Tables
-- This script adds formatted_id columns and triggers to auto-generate IDs with prefixes
-- Format: D001, A001, P001, N001, PH001, T001, AP001, PR001, DI001, MR001, F001, L001

USE hospital_management_system;

-- ============================================================================
-- STEP 1: Add formatted_id columns to all tables
-- ============================================================================

-- Doctor Table
ALTER TABLE doctor 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER doctor_id;

-- Admin Table
ALTER TABLE admin 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER admin_id;

-- Patient Table
ALTER TABLE patient 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER patient_id;

-- Nurse Table
ALTER TABLE nurse 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER nurse_id;

-- Pharmacy Table
ALTER TABLE pharmacy 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER pharmacy_id;

-- Treatment Table
ALTER TABLE treatment 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER treatment_id;

-- Appointment Table
ALTER TABLE appointment 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER appointment_id;

-- Prescription Table
ALTER TABLE prescription 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER prescription_id;

-- Diagnosis Table
ALTER TABLE diagnosis 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER diagnosis_id;

-- Medical Record Table
ALTER TABLE medical_record 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER record_id;

-- Finance Table
ALTER TABLE finance 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER finance_id;

-- Login Table
ALTER TABLE login 
ADD COLUMN formatted_id VARCHAR(20) UNIQUE NULL AFTER login_id;

-- ============================================================================
-- STEP 2: Drop existing triggers if they exist
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

-- ============================================================================
-- STEP 3: Create triggers to auto-generate formatted IDs on INSERT
-- Note: Using AFTER INSERT because AUTO_INCREMENT is assigned after insert
-- ============================================================================

-- Doctor Trigger: D001, D002, etc.
DELIMITER //
CREATE TRIGGER trg_doctor_formatted_id
AFTER INSERT ON doctor
FOR EACH ROW
BEGIN
    UPDATE doctor 
    SET formatted_id = CONCAT('D', LPAD(NEW.doctor_id, 3, '0'))
    WHERE doctor_id = NEW.doctor_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Admin Trigger: A001, A002, etc.
DELIMITER //
CREATE TRIGGER trg_admin_formatted_id
AFTER INSERT ON admin
FOR EACH ROW
BEGIN
    UPDATE admin 
    SET formatted_id = CONCAT('A', LPAD(NEW.admin_id, 3, '0'))
    WHERE admin_id = NEW.admin_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Patient Trigger: P001, P002, etc.
DELIMITER //
CREATE TRIGGER trg_patient_formatted_id
AFTER INSERT ON patient
FOR EACH ROW
BEGIN
    UPDATE patient 
    SET formatted_id = CONCAT('P', LPAD(NEW.patient_id, 3, '0'))
    WHERE patient_id = NEW.patient_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Nurse Trigger: N001, N002, etc.
DELIMITER //
CREATE TRIGGER trg_nurse_formatted_id
AFTER INSERT ON nurse
FOR EACH ROW
BEGIN
    UPDATE nurse 
    SET formatted_id = CONCAT('N', LPAD(NEW.nurse_id, 3, '0'))
    WHERE nurse_id = NEW.nurse_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Pharmacy Trigger: PH001, PH002, etc.
DELIMITER //
CREATE TRIGGER trg_pharmacy_formatted_id
AFTER INSERT ON pharmacy
FOR EACH ROW
BEGIN
    UPDATE pharmacy 
    SET formatted_id = CONCAT('PH', LPAD(NEW.pharmacy_id, 3, '0'))
    WHERE pharmacy_id = NEW.pharmacy_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Treatment Trigger: T001, T002, etc.
DELIMITER //
CREATE TRIGGER trg_treatment_formatted_id
AFTER INSERT ON treatment
FOR EACH ROW
BEGIN
    UPDATE treatment 
    SET formatted_id = CONCAT('T', LPAD(NEW.treatment_id, 3, '0'))
    WHERE treatment_id = NEW.treatment_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Appointment Trigger: AP001, AP002, etc.
DELIMITER //
CREATE TRIGGER trg_appointment_formatted_id
AFTER INSERT ON appointment
FOR EACH ROW
BEGIN
    UPDATE appointment 
    SET formatted_id = CONCAT('AP', LPAD(NEW.appointment_id, 3, '0'))
    WHERE appointment_id = NEW.appointment_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Prescription Trigger: PR001, PR002, etc.
DELIMITER //
CREATE TRIGGER trg_prescription_formatted_id
AFTER INSERT ON prescription
FOR EACH ROW
BEGIN
    UPDATE prescription 
    SET formatted_id = CONCAT('PR', LPAD(NEW.prescription_id, 3, '0'))
    WHERE prescription_id = NEW.prescription_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Diagnosis Trigger: DI001, DI002, etc.
DELIMITER //
CREATE TRIGGER trg_diagnosis_formatted_id
AFTER INSERT ON diagnosis
FOR EACH ROW
BEGIN
    UPDATE diagnosis 
    SET formatted_id = CONCAT('DI', LPAD(NEW.diagnosis_id, 3, '0'))
    WHERE diagnosis_id = NEW.diagnosis_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Medical Record Trigger: MR001, MR002, etc.
DELIMITER //
CREATE TRIGGER trg_medical_record_formatted_id
AFTER INSERT ON medical_record
FOR EACH ROW
BEGIN
    UPDATE medical_record 
    SET formatted_id = CONCAT('MR', LPAD(NEW.record_id, 3, '0'))
    WHERE record_id = NEW.record_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Finance Trigger: F001, F002, etc.
DELIMITER //
CREATE TRIGGER trg_finance_formatted_id
AFTER INSERT ON finance
FOR EACH ROW
BEGIN
    UPDATE finance 
    SET formatted_id = CONCAT('F', LPAD(NEW.finance_id, 3, '0'))
    WHERE finance_id = NEW.finance_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- Login Trigger: L001, L002, etc.
DELIMITER //
CREATE TRIGGER trg_login_formatted_id
AFTER INSERT ON login
FOR EACH ROW
BEGIN
    UPDATE login 
    SET formatted_id = CONCAT('L', LPAD(NEW.login_id, 3, '0'))
    WHERE login_id = NEW.login_id 
    AND (formatted_id IS NULL OR formatted_id = '');
END//
DELIMITER ;

-- ============================================================================
-- STEP 4: Backfill existing data with formatted IDs
-- Note: This uses sequential numbering (1, 2, 3...) regardless of AUTO_INCREMENT start value
-- ============================================================================

-- Backfill Doctor formatted IDs (sequential: D001, D002, etc.)
SET @row_num = 0;
UPDATE doctor 
SET formatted_id = CONCAT('D', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY doctor_id ASC;

-- Backfill Admin formatted IDs (sequential: A001, A002, etc.)
SET @row_num = 0;
UPDATE admin 
SET formatted_id = CONCAT('A', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY admin_id ASC;

-- Backfill Patient formatted IDs (sequential: P001, P002, etc.)
SET @row_num = 0;
UPDATE patient 
SET formatted_id = CONCAT('P', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY patient_id ASC;

-- Backfill Nurse formatted IDs (sequential: N001, N002, etc.)
SET @row_num = 0;
UPDATE nurse 
SET formatted_id = CONCAT('N', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY nurse_id ASC;

-- Backfill Pharmacy formatted IDs (sequential: PH001, PH002, etc.)
SET @row_num = 0;
UPDATE pharmacy 
SET formatted_id = CONCAT('PH', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY pharmacy_id ASC;

-- Backfill Treatment formatted IDs (sequential: T001, T002, etc.)
SET @row_num = 0;
UPDATE treatment 
SET formatted_id = CONCAT('T', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY treatment_id ASC;

-- Backfill Appointment formatted IDs (sequential: AP001, AP002, etc.)
SET @row_num = 0;
UPDATE appointment 
SET formatted_id = CONCAT('AP', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY appointment_id ASC;

-- Backfill Prescription formatted IDs (sequential: PR001, PR002, etc.)
SET @row_num = 0;
UPDATE prescription 
SET formatted_id = CONCAT('PR', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY prescription_id ASC;

-- Backfill Diagnosis formatted IDs (sequential: DI001, DI002, etc.)
SET @row_num = 0;
UPDATE diagnosis 
SET formatted_id = CONCAT('DI', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY diagnosis_id ASC;

-- Backfill Medical Record formatted IDs (sequential: MR001, MR002, etc.)
SET @row_num = 0;
UPDATE medical_record 
SET formatted_id = CONCAT('MR', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY record_id ASC;

-- Backfill Finance formatted IDs (sequential: F001, F002, etc.)
SET @row_num = 0;
UPDATE finance 
SET formatted_id = CONCAT('F', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY finance_id ASC;

-- Backfill Login formatted IDs (sequential: L001, L002, etc.)
SET @row_num = 0;
UPDATE login 
SET formatted_id = CONCAT('L', LPAD((@row_num := @row_num + 1), 3, '0'))
ORDER BY login_id ASC;

-- ============================================================================
-- STEP 5: Make formatted_id columns NOT NULL after backfill
-- ============================================================================

ALTER TABLE doctor MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE admin MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE patient MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE nurse MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE pharmacy MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE treatment MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE appointment MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE prescription MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE diagnosis MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE medical_record MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE finance MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;
ALTER TABLE login MODIFY COLUMN formatted_id VARCHAR(20) NOT NULL;

-- ============================================================================
-- Completion Message
-- ============================================================================
SELECT 'Formatted IDs migration completed successfully!' AS Status;

