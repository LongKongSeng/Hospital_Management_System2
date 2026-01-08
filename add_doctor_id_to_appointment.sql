-- Migration script to add doctor_id column to appointment table
-- Run this script if your appointment table doesn't have doctor_id column

USE hospital_management_system;

-- Add doctor_id column to appointment table
ALTER TABLE appointment 
ADD COLUMN doctor_id INT NULL AFTER nurse_id;

-- Add foreign key constraint
ALTER TABLE appointment 
ADD CONSTRAINT fk_appointment_doctor 
FOREIGN KEY (doctor_id) REFERENCES doctor(doctor_id) ON DELETE CASCADE;

-- Update the column to NOT NULL if needed (after populating existing data)
-- ALTER TABLE appointment MODIFY COLUMN doctor_id INT NOT NULL;






