-- Migration script to add ic_number column to patient table
-- Run this script if your patient table doesn't have ic_number column

USE hospital_management_system;

-- Add ic_number column to patient table
ALTER TABLE patient 
ADD COLUMN ic_number VARCHAR(20) UNIQUE NULL AFTER date_of_birth;

-- Note: After running this migration, you may want to update existing records
-- with IC numbers and then set the column to NOT NULL if needed
-- ALTER TABLE patient MODIFY COLUMN ic_number VARCHAR(20) UNIQUE NOT NULL;






