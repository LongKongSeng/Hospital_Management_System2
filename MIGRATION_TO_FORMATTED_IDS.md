# Migration Guide: Converting to Formatted IDs Only

This document tracks the migration from numeric IDs to formatted_id (D001, A001, etc.) as primary keys.

## SQL Migration Script
Run `migrate_to_formatted_ids_only.sql` first to:
1. Convert all numeric ID columns to formatted_id VARCHAR primary keys
2. Update all foreign key references
3. Create triggers to auto-generate formatted IDs

## Code Changes Required

### 1. All ID variables changed from `int` to `string`
- `currentUserId` in Login: `int` → `string`
- `currentDoctorId` in DoctorModule: `int` → `string`
- `currentNurseId` in NurseModule: `int` → `string`
- All function parameters accepting IDs: `int` → `string`

### 2. All queries need to use formatted_id
- WHERE clauses: `WHERE patient_id = 123` → `WHERE formatted_id = 'P001'`
- JOINs: `JOIN patient p ON a.patient_id = p.patient_id` → `JOIN patient p ON a.patient_id = p.formatted_id`
- INSERT: Remove numeric ID columns, use NULL for formatted_id (trigger generates it)
- SELECT: Remove numeric ID columns from SELECT statements

### 3. Input functions
- Change `getIntInput("Enter Patient ID: ")` → `getStringInput("Enter Patient ID (e.g., P001): ")`
- Remove validation like `if (id <= 0)` → `if (id.empty())`

### 4. ID generation in INSERT
- OLD: Let AUTO_INCREMENT generate numeric ID, then get it
- NEW: Let trigger generate formatted_id, then get the formatted_id

## Status
- ✅ Login.h/cpp - Updated to use string formatted_id
- ✅ DoctorModule.h/cpp - Partially updated
- ✅ NurseModule.h/cpp - Partially updated  
- ✅ Registration.cpp - Partially updated
- ⏳ AdminModule.cpp - Needs updates
- ⏳ All WHERE clauses - Need string formatting
- ⏳ All JOINs - Need to use formatted_id
- ⏳ All INSERT statements - Need updates

