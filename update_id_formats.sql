-- Migration script to set different AUTO_INCREMENT starting values for each entity
-- This ensures each entity type has a distinct ID range
-- Run this script to update existing database

USE hospital_management_system;

-- Set AUTO_INCREMENT starting values for distinct ID ranges
-- Admin IDs: 1000-1999 (format: 1xxx)
-- Doctor IDs: 2000-2999 (format: 2xxx)
-- Patient IDs: 3000-99999 (format: 3xxxx)
-- Nurse IDs: 4000-4999 (format: 4xxx)

-- Get current max IDs and set appropriate starting values
SET @max_admin = IFNULL((SELECT MAX(admin_id) FROM admin), 0);
SET @max_doctor = IFNULL((SELECT MAX(doctor_id) FROM doctor), 0);
SET @max_patient = IFNULL((SELECT MAX(patient_id) FROM patient), 0);
SET @max_nurse = IFNULL((SELECT MAX(nurse_id) FROM nurse), 0);

-- Set AUTO_INCREMENT starting values
-- If existing IDs are less than the range start, use range start
-- Otherwise, continue from current max + 1
ALTER TABLE admin AUTO_INCREMENT = GREATEST(1000, @max_admin + 1);
ALTER TABLE doctor AUTO_INCREMENT = GREATEST(2000, @max_doctor + 1);
ALTER TABLE patient AUTO_INCREMENT = GREATEST(3000, @max_patient + 1);
ALTER TABLE nurse AUTO_INCREMENT = GREATEST(4000, @max_nurse + 1);

-- Verify the changes
SELECT 'Admin AUTO_INCREMENT' AS Table_Name, AUTO_INCREMENT 
FROM information_schema.TABLES 
WHERE TABLE_SCHEMA = 'hospital_management_system' AND TABLE_NAME = 'admin'
UNION ALL
SELECT 'Doctor AUTO_INCREMENT', AUTO_INCREMENT 
FROM information_schema.TABLES 
WHERE TABLE_SCHEMA = 'hospital_management_system' AND TABLE_NAME = 'doctor'
UNION ALL
SELECT 'Patient AUTO_INCREMENT', AUTO_INCREMENT 
FROM information_schema.TABLES 
WHERE TABLE_SCHEMA = 'hospital_management_system' AND TABLE_NAME = 'patient'
UNION ALL
SELECT 'Nurse AUTO_INCREMENT', AUTO_INCREMENT 
FROM information_schema.TABLES 
WHERE TABLE_SCHEMA = 'hospital_management_system' AND TABLE_NAME = 'nurse';

