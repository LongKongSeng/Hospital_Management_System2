-- Migration script to add ic_number column to doctor, nurse, and admin tables
-- Run this script if your tables don't have ic_number column

USE hospital_management_system;

-- Add ic_number column to doctor table
ALTER TABLE doctor 
ADD COLUMN ic_number VARCHAR(20) UNIQUE NULL AFTER contact_number;

-- Add ic_number column to nurse table
ALTER TABLE nurse 
ADD COLUMN ic_number VARCHAR(20) UNIQUE NULL AFTER contact_number;

-- Add ic_number column to admin table
ALTER TABLE admin 
ADD COLUMN ic_number VARCHAR(20) UNIQUE NULL AFTER contact_number;

-- Note: After running this migration, you may want to update existing records
-- with IC numbers and then set the columns to NOT NULL if needed
-- ALTER TABLE doctor MODIFY COLUMN ic_number VARCHAR(20) UNIQUE NOT NULL;
-- ALTER TABLE nurse MODIFY COLUMN ic_number VARCHAR(20) UNIQUE NOT NULL;
-- ALTER TABLE admin MODIFY COLUMN ic_number VARCHAR(20) UNIQUE NOT NULL;






