-- Fix script to add missing contact_number column to admin table
-- Run this in phpMyAdmin or MySQL command line

USE hospital_management_system;

-- Check if column exists, if not add it
ALTER TABLE admin 
ADD COLUMN IF NOT EXISTS contact_number VARCHAR(20) NOT NULL DEFAULT '' AFTER email;

-- If the above doesn't work (MySQL version < 8.0), use this instead:
-- ALTER TABLE admin 
-- ADD COLUMN contact_number VARCHAR(20) NOT NULL DEFAULT '' AFTER email;

-- Verify the table structure
DESCRIBE admin;








