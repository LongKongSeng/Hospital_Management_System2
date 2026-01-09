-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 09, 2026 at 06:44 AM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `hospital_management_system`
--

-- --------------------------------------------------------

--
-- Table structure for table `admin`
--

CREATE TABLE `admin` (
  `formatted_id` varchar(20) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `email` varchar(100) NOT NULL,
  `contact_number` varchar(20) NOT NULL,
  `ic_number` varchar(20) NOT NULL,
  `status` enum('Active','Inactive') DEFAULT 'Active',
  `role` varchar(20) DEFAULT 'Admin',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `admin`
--

INSERT INTO `admin` (`formatted_id`, `full_name`, `email`, `contact_number`, `ic_number`, `status`, `role`, `created_at`) VALUES
('A001', 'System Administrator', 'admin@hospital.com', '1234567890', 'ADMIN001', 'Active', 'Admin', '2026-01-08 06:20:22'),
('A002', 'Jennifer', 'jen@gmail.com', '01122223333', '711218085957', 'Active', 'Admin', '2026-01-08 07:23:44');

--
-- Triggers `admin`
--
DELIMITER $$
CREATE TRIGGER `trg_admin_formatted_id` BEFORE INSERT ON `admin` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM admin
        WHERE formatted_id LIKE 'A%';
        SET NEW.formatted_id = CONCAT('A', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `appointment`
--

CREATE TABLE `appointment` (
  `formatted_id` varchar(20) NOT NULL,
  `patient_id` varchar(20) NOT NULL,
  `nurse_id` varchar(20) NOT NULL,
  `doctor_id` varchar(20) DEFAULT NULL,
  `appointment_date` date NOT NULL,
  `appointment_time` time NOT NULL,
  `status` enum('Scheduled','Completed','Cancelled') DEFAULT 'Scheduled',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `appointment`
--

INSERT INTO `appointment` (`formatted_id`, `patient_id`, `nurse_id`, `doctor_id`, `appointment_date`, `appointment_time`, `status`, `created_at`) VALUES
('AP001', 'P001', 'N001', 'D001', '2026-01-20', '20:30:00', 'Scheduled', '2026-01-08 07:28:41'),
('AP002', 'P001', 'N001', 'D001', '2026-01-10', '09:00:00', 'Scheduled', '2026-01-08 11:42:59'),
('AP003', 'P002', 'N002', 'D002', '2026-01-10', '10:30:00', 'Scheduled', '2026-01-08 11:42:59'),
('AP004', 'P003', 'N003', 'D003', '2026-01-11', '14:00:00', 'Scheduled', '2026-01-08 11:42:59'),
('AP005', 'P001', 'N001', 'D001', '2026-01-01', '22:30:00', 'Scheduled', '2026-01-08 16:05:55'),
('AP006', 'P003', 'N001', 'D002', '2028-09-30', '15:30:00', 'Scheduled', '2026-01-09 00:56:15');

--
-- Triggers `appointment`
--
DELIMITER $$
CREATE TRIGGER `trg_appointment_formatted_id` BEFORE INSERT ON `appointment` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM appointment
        WHERE formatted_id LIKE 'AP%';
        SET NEW.formatted_id = CONCAT('AP', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `diagnosis`
--

CREATE TABLE `diagnosis` (
  `formatted_id` varchar(20) NOT NULL,
  `disease` varchar(200) DEFAULT NULL,
  `disorder` varchar(200) DEFAULT NULL,
  `duration_of_pain` varchar(50) DEFAULT NULL,
  `severity` varchar(50) DEFAULT NULL,
  `prescription_id` varchar(20) DEFAULT NULL,
  `date` date NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `diagnosis`
--

INSERT INTO `diagnosis` (`formatted_id`, `disease`, `disorder`, `duration_of_pain`, `severity`, `prescription_id`, `date`, `created_at`) VALUES
('DI001', 'alzhimers', 'none', 'few week', 'mid', NULL, '2026-01-08', '2026-01-08 07:27:23'),
('DI002', 'Migraine', 'Neurological', '3 days', 'Moderate', 'PR001', '2026-01-08', '2026-01-08 11:42:59'),
('DI003', '0', 'Respiratory', '1 week', 'Severe', 'PR002', '2026-01-08', '2026-01-08 11:42:59'),
('DI004', 'Skin Infection', 'Dermatological', '4 days', 'Mild', 'PR003', '2026-01-08', '2026-01-08 11:42:59'),
('DI005', '', '', '', '', NULL, '0000-00-00', '2026-01-08 16:51:02'),
('DI006', '', '', '', '', NULL, '0000-00-00', '2026-01-09 00:54:25'),
('DI007', 'Scratch Eye', 'none', 'few days', 'high', NULL, '0000-00-00', '2026-01-09 05:31:32'),
('DI008', '', '', '', '', NULL, '0000-00-00', '2026-01-09 05:34:10');

--
-- Triggers `diagnosis`
--
DELIMITER $$
CREATE TRIGGER `trg_diagnosis_formatted_id` BEFORE INSERT ON `diagnosis` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM diagnosis
        WHERE formatted_id LIKE 'DI%';
        SET NEW.formatted_id = CONCAT('DI', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `doctor`
--

CREATE TABLE `doctor` (
  `formatted_id` varchar(20) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `gender` enum('Male','Female','Other') NOT NULL,
  `specialization` varchar(100) NOT NULL,
  `contact_number` varchar(20) NOT NULL,
  `ic_number` varchar(20) NOT NULL,
  `availability` enum('Available','Busy','Off') DEFAULT 'Available',
  `status` enum('Active','Inactive') DEFAULT 'Active',
  `role` varchar(20) DEFAULT 'Doctor',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `doctor`
--

INSERT INTO `doctor` (`formatted_id`, `full_name`, `gender`, `specialization`, `contact_number`, `ic_number`, `availability`, `status`, `role`, `created_at`) VALUES
('D001', 'COLIN TEH JON RYAN', 'Male', 'Optemetris', '01111922983', '050705081169', 'Available', 'Active', 'Doctor', '2026-01-08 06:39:58'),
('D002', 'Dr. Emily Chen', 'Female', 'Cardiology', '012-1111111', '800101-14-1111', 'Available', 'Active', 'Doctor', '2026-01-08 11:42:59'),
('D003', 'Dr. Raj Kumar', 'Male', 'Neurology', '012-2222222', '750505-10-2222', 'Busy', 'Active', 'Doctor', '2026-01-08 11:42:59'),
('D004', 'Dr. Sarah Ahmad', 'Female', 'General Surgery', '012-3333333', '881212-01-3333', 'Off', 'Active', 'Doctor', '2026-01-08 11:42:59');

--
-- Triggers `doctor`
--
DELIMITER $$
CREATE TRIGGER `trg_doctor_formatted_id` BEFORE INSERT ON `doctor` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM doctor
        WHERE formatted_id LIKE 'D%';
        SET NEW.formatted_id = CONCAT('D', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `finance`
--

CREATE TABLE `finance` (
  `formatted_id` varchar(20) NOT NULL,
  `treatment_id` varchar(20) NOT NULL,
  `date_of_payment` date NOT NULL,
  `payment_status` enum('Paid','Pending','Partial') DEFAULT 'Pending',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `finance`
--

INSERT INTO `finance` (`formatted_id`, `treatment_id`, `date_of_payment`, `payment_status`, `created_at`) VALUES
('F001', 'T001', '2026-01-08', 'Paid', '2026-01-08 11:42:59'),
('F002', 'T002', '2026-01-08', 'Pending', '2026-01-08 11:42:59'),
('F003', 'T003', '2026-01-08', 'Partial', '2026-01-08 11:42:59');

--
-- Triggers `finance`
--
DELIMITER $$
CREATE TRIGGER `trg_finance_formatted_id` BEFORE INSERT ON `finance` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM finance
        WHERE formatted_id LIKE 'F%';
        SET NEW.formatted_id = CONCAT('F', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `login`
--

CREATE TABLE `login` (
  `formatted_id` varchar(20) NOT NULL,
  `username` varchar(50) NOT NULL,
  `password` varchar(255) NOT NULL,
  `role` enum('Doctor','Nurse','Admin') NOT NULL,
  `doctor_id` varchar(20) DEFAULT NULL,
  `nurse_id` varchar(20) DEFAULT NULL,
  `admin_id` varchar(20) DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `login`
--

INSERT INTO `login` (`formatted_id`, `username`, `password`, `role`, `doctor_id`, `nurse_id`, `admin_id`, `created_at`) VALUES
('L001', 'admin', 'admin123', 'Admin', NULL, NULL, 'A001', '2026-01-08 06:20:22'),
('L002', 'colin', 'colin123', 'Doctor', 'D001', NULL, NULL, '2026-01-08 06:39:58'),
('L003', 'cat', 'cat123', 'Nurse', NULL, 'N001', NULL, '2026-01-08 06:41:23'),
('L004', 'jen', 'jen123', 'Admin', NULL, NULL, 'A002', '2026-01-08 07:23:44'),
('L005', 'doc_emily', 'password123', 'Doctor', 'D001', NULL, NULL, '2026-01-08 11:42:59'),
('L006', 'nurse_hafiz', 'password123', 'Nurse', NULL, 'N001', NULL, '2026-01-08 11:42:59'),
('L007', 'doc_raj', 'password123', 'Doctor', 'D002', NULL, NULL, '2026-01-08 11:42:59');

--
-- Triggers `login`
--
DELIMITER $$
CREATE TRIGGER `trg_login_formatted_id` BEFORE INSERT ON `login` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM login
        WHERE formatted_id LIKE 'L%';
        SET NEW.formatted_id = CONCAT('L', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `medical_record`
--

CREATE TABLE `medical_record` (
  `formatted_id` varchar(20) NOT NULL,
  `patient_id` varchar(20) NOT NULL,
  `doctor_id` varchar(20) NOT NULL,
  `diagnosis_id` varchar(20) DEFAULT NULL,
  `date_of_record` date NOT NULL,
  `notes` text DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `medical_record`
--

INSERT INTO `medical_record` (`formatted_id`, `patient_id`, `doctor_id`, `diagnosis_id`, `date_of_record`, `notes`, `created_at`) VALUES
('MR001', 'P001', 'D001', 'DI001', '2026-01-08', 'Diagnosis made by doctor', '2026-01-08 07:27:23'),
('MR002', 'P001', 'D001', 'DI001', '2026-01-08', 'Patient reports sensitivity to light.', '2026-01-08 11:42:59'),
('MR003', 'P002', 'D002', 'DI002', '2026-01-08', 'Breathing difficulty observed.', '2026-01-08 11:42:59'),
('MR004', 'P003', 'D003', 'DI003', '2026-01-08', 'Wound cleaning required.', '2026-01-08 11:42:59'),
('MR005', 'P001', 'D001', 'DI005', '0000-00-00', 'Diagnosis made by doctor', '2026-01-08 16:51:02'),
('MR006', 'P001', 'D001', 'DI006', '0000-00-00', 'Diagnosis made by doctor', '2026-01-09 00:54:25'),
('MR007', 'P001', 'D001', 'DI007', '2026-01-09', 'Diagnosis made by doctor', '2026-01-09 05:31:32'),
('MR008', 'P001', 'D001', 'DI008', '0000-00-00', 'Diagnosis made by doctor', '2026-01-09 05:34:10');

--
-- Triggers `medical_record`
--
DELIMITER $$
CREATE TRIGGER `trg_medical_record_formatted_id` BEFORE INSERT ON `medical_record` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM medical_record
        WHERE formatted_id LIKE 'MR%';
        SET NEW.formatted_id = CONCAT('MR', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `nurse`
--

CREATE TABLE `nurse` (
  `formatted_id` varchar(20) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `gender` enum('Male','Female','Other') NOT NULL,
  `contact_number` varchar(20) NOT NULL,
  `ic_number` varchar(20) NOT NULL,
  `status` enum('Active','Inactive') DEFAULT 'Active',
  `role` varchar(20) DEFAULT 'Nurse',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `nurse`
--

INSERT INTO `nurse` (`formatted_id`, `full_name`, `gender`, `contact_number`, `ic_number`, `status`, `role`, `created_at`) VALUES
('N001', 'Catherine', 'Male', '01111111111', '010101011192', 'Active', 'Nurse', '2026-01-08 06:41:23'),
('N002', 'Nurse Hafiz', 'Male', '019-1110001', '950101-14-5555', 'Active', 'Nurse', '2026-01-08 11:42:59'),
('N003', 'Nurse Jessica Lee', 'Female', '019-2220002', '980202-10-6666', 'Active', 'Nurse', '2026-01-08 11:42:59'),
('N004', 'Nurse Mei Ling', 'Female', '019-3330003', '990303-01-7777', 'Active', 'Nurse', '2026-01-08 11:42:59');

--
-- Triggers `nurse`
--
DELIMITER $$
CREATE TRIGGER `trg_nurse_formatted_id` BEFORE INSERT ON `nurse` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM nurse
        WHERE formatted_id LIKE 'N%';
        SET NEW.formatted_id = CONCAT('N', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `patient`
--

CREATE TABLE `patient` (
  `formatted_id` varchar(20) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `gender` enum('Male','Female','Other') NOT NULL,
  `date_of_birth` date NOT NULL,
  `contact_number` varchar(20) NOT NULL,
  `blood_type` varchar(5) DEFAULT NULL,
  `emergency_contact` varchar(100) DEFAULT NULL,
  `ic_number` varchar(20) NOT NULL,
  `status` enum('Active','Inactive') DEFAULT 'Active',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `patient`
--

INSERT INTO `patient` (`formatted_id`, `full_name`, `gender`, `date_of_birth`, `contact_number`, `blood_type`, `emergency_contact`, `ic_number`, `status`, `created_at`) VALUES
('P001', 'Tobi', 'Male', '2005-05-12', '01133334444', 'A+', '01122225555', '029384729930', 'Active', '2026-01-08 07:24:50'),
('P002', 'Ahmad Albab', 'Male', '1990-05-20', '017-5551234', 'A+', 'Siti: 017-0000001', '900520-14-1234', 'Active', '2026-01-08 11:42:59'),
('P003', 'Muthu Sammy', 'Male', '1985-08-15', '017-5555678', 'O+', 'Devi: 017-0000002', '850815-10-5678', 'Active', '2026-01-08 11:42:59'),
('P004', 'Chong Wei', 'Male', '2001-11-30', '017-5559012', 'B-', 'Mrs. Chong: 017-0000003', '011130-01-9012', 'Active', '2026-01-08 11:42:59');

--
-- Triggers `patient`
--
DELIMITER $$
CREATE TRIGGER `trg_patient_formatted_id` BEFORE INSERT ON `patient` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM patient
        WHERE formatted_id LIKE 'P%';
        SET NEW.formatted_id = CONCAT('P', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `pharmacy`
--

CREATE TABLE `pharmacy` (
  `formatted_id` varchar(20) NOT NULL,
  `medicine_name` varchar(100) NOT NULL,
  `category_of_meds` varchar(50) NOT NULL,
  `unit_price` decimal(10,2) DEFAULT 0.00,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `pharmacy`
--

INSERT INTO `pharmacy` (`formatted_id`, `medicine_name`, `category_of_meds`, `unit_price`, `created_at`) VALUES
('PH001', 'Panadol Extend', 'Painkiller', 1.50, '2026-01-08 11:42:59'),
('PH002', 'Cough Syrup', 'Syrup', 12.00, '2026-01-08 11:42:59'),
('PH003', 'Antibiotic Cream', 'Topical', 15.50, '2026-01-08 11:42:59');

--
-- Triggers `pharmacy`
--
DELIMITER $$
CREATE TRIGGER `trg_pharmacy_formatted_id` BEFORE INSERT ON `pharmacy` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM pharmacy
        WHERE formatted_id LIKE 'PH%';
        SET NEW.formatted_id = CONCAT('PH', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `prescription`
--

CREATE TABLE `prescription` (
  `formatted_id` varchar(20) NOT NULL,
  `pharmacy_id` varchar(20) NOT NULL,
  `dosage` varchar(50) DEFAULT NULL,
  `duration_of_meds` varchar(50) DEFAULT NULL,
  `instructions` text DEFAULT NULL,
  `date` date NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `prescription`
--

INSERT INTO `prescription` (`formatted_id`, `pharmacy_id`, `dosage`, `duration_of_meds`, `instructions`, `date`, `created_at`) VALUES
('PR001', 'PH001', '2 tablets', '3 days', 'Take after food', '2026-01-08', '2026-01-08 11:42:59'),
('PR002', 'PH002', '10ml', '5 days', '3 times a day', '2026-01-08', '2026-01-08 11:42:59'),
('PR003', 'PH003', 'Apply thin layer', '1 week', 'Apply twice daily', '2026-01-08', '2026-01-08 11:42:59');

--
-- Triggers `prescription`
--
DELIMITER $$
CREATE TRIGGER `trg_prescription_formatted_id` BEFORE INSERT ON `prescription` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 3) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM prescription
        WHERE formatted_id LIKE 'PR%';
        SET NEW.formatted_id = CONCAT('PR', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `treatment`
--

CREATE TABLE `treatment` (
  `formatted_id` varchar(20) NOT NULL,
  `doctor_id` varchar(20) NOT NULL,
  `dressing_applied` varchar(200) DEFAULT NULL,
  `consultation_fee` decimal(10,2) DEFAULT 0.00,
  `treatment_fee` decimal(10,2) DEFAULT 0.00,
  `treatment_date` date NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `treatment`
--

INSERT INTO `treatment` (`formatted_id`, `doctor_id`, `dressing_applied`, `consultation_fee`, `treatment_fee`, `treatment_date`, `created_at`) VALUES
('T001', 'D001', 'None', 50.00, 0.00, '2026-01-08', '2026-01-08 11:42:59'),
('T002', 'D002', 'Oxygen Mask', 80.00, 20.00, '2026-01-08', '2026-01-08 11:42:59'),
('T003', 'D003', 'Antiseptic Dressing', 40.00, 15.00, '2026-01-08', '2026-01-08 11:42:59');

--
-- Triggers `treatment`
--
DELIMITER $$
CREATE TRIGGER `trg_treatment_formatted_id` BEFORE INSERT ON `treatment` FOR EACH ROW BEGIN
    DECLARE next_num INT;
    IF NEW.formatted_id IS NULL OR NEW.formatted_id = '' THEN
        SELECT COALESCE(MAX(CAST(SUBSTRING(formatted_id, 2) AS UNSIGNED)), 0) + 1 INTO next_num
        FROM treatment
        WHERE formatted_id LIKE 'T%';
        SET NEW.formatted_id = CONCAT('T', LPAD(next_num, 3, '0'));
    END IF;
END
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `admin`
--
ALTER TABLE `admin`
  ADD PRIMARY KEY (`formatted_id`),
  ADD UNIQUE KEY `email` (`email`),
  ADD UNIQUE KEY `ic_number` (`ic_number`);

--
-- Indexes for table `appointment`
--
ALTER TABLE `appointment`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `patient_id` (`patient_id`),
  ADD KEY `nurse_id` (`nurse_id`),
  ADD KEY `doctor_id` (`doctor_id`),
  ADD KEY `idx_appointment_date` (`appointment_date`);

--
-- Indexes for table `diagnosis`
--
ALTER TABLE `diagnosis`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `prescription_id` (`prescription_id`);

--
-- Indexes for table `doctor`
--
ALTER TABLE `doctor`
  ADD PRIMARY KEY (`formatted_id`),
  ADD UNIQUE KEY `ic_number` (`ic_number`),
  ADD KEY `idx_doctor_status` (`status`);

--
-- Indexes for table `finance`
--
ALTER TABLE `finance`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `treatment_id` (`treatment_id`);

--
-- Indexes for table `login`
--
ALTER TABLE `login`
  ADD PRIMARY KEY (`formatted_id`),
  ADD UNIQUE KEY `username` (`username`),
  ADD KEY `doctor_id` (`doctor_id`),
  ADD KEY `nurse_id` (`nurse_id`),
  ADD KEY `admin_id` (`admin_id`),
  ADD KEY `idx_login_username` (`username`);

--
-- Indexes for table `medical_record`
--
ALTER TABLE `medical_record`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `patient_id` (`patient_id`),
  ADD KEY `doctor_id` (`doctor_id`),
  ADD KEY `diagnosis_id` (`diagnosis_id`);

--
-- Indexes for table `nurse`
--
ALTER TABLE `nurse`
  ADD PRIMARY KEY (`formatted_id`),
  ADD UNIQUE KEY `ic_number` (`ic_number`),
  ADD KEY `idx_nurse_status` (`status`);

--
-- Indexes for table `patient`
--
ALTER TABLE `patient`
  ADD PRIMARY KEY (`formatted_id`),
  ADD UNIQUE KEY `ic_number` (`ic_number`),
  ADD KEY `idx_patient_status` (`status`),
  ADD KEY `idx_patient_ic` (`ic_number`);

--
-- Indexes for table `pharmacy`
--
ALTER TABLE `pharmacy`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `idx_pharmacy_category` (`category_of_meds`);

--
-- Indexes for table `prescription`
--
ALTER TABLE `prescription`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `pharmacy_id` (`pharmacy_id`);

--
-- Indexes for table `treatment`
--
ALTER TABLE `treatment`
  ADD PRIMARY KEY (`formatted_id`),
  ADD KEY `doctor_id` (`doctor_id`);

--
-- Constraints for dumped tables
--

--
-- Constraints for table `appointment`
--
ALTER TABLE `appointment`
  ADD CONSTRAINT `appointment_ibfk_1` FOREIGN KEY (`patient_id`) REFERENCES `patient` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `appointment_ibfk_2` FOREIGN KEY (`nurse_id`) REFERENCES `nurse` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `appointment_ibfk_3` FOREIGN KEY (`doctor_id`) REFERENCES `doctor` (`formatted_id`) ON DELETE SET NULL;

--
-- Constraints for table `diagnosis`
--
ALTER TABLE `diagnosis`
  ADD CONSTRAINT `diagnosis_ibfk_1` FOREIGN KEY (`prescription_id`) REFERENCES `prescription` (`formatted_id`) ON DELETE SET NULL;

--
-- Constraints for table `finance`
--
ALTER TABLE `finance`
  ADD CONSTRAINT `finance_ibfk_1` FOREIGN KEY (`treatment_id`) REFERENCES `treatment` (`formatted_id`) ON DELETE CASCADE;

--
-- Constraints for table `login`
--
ALTER TABLE `login`
  ADD CONSTRAINT `login_ibfk_1` FOREIGN KEY (`doctor_id`) REFERENCES `doctor` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `login_ibfk_2` FOREIGN KEY (`nurse_id`) REFERENCES `nurse` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `login_ibfk_3` FOREIGN KEY (`admin_id`) REFERENCES `admin` (`formatted_id`) ON DELETE CASCADE;

--
-- Constraints for table `medical_record`
--
ALTER TABLE `medical_record`
  ADD CONSTRAINT `medical_record_ibfk_1` FOREIGN KEY (`patient_id`) REFERENCES `patient` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `medical_record_ibfk_2` FOREIGN KEY (`doctor_id`) REFERENCES `doctor` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `medical_record_ibfk_3` FOREIGN KEY (`diagnosis_id`) REFERENCES `diagnosis` (`formatted_id`) ON DELETE SET NULL;

--
-- Constraints for table `prescription`
--
ALTER TABLE `prescription`
  ADD CONSTRAINT `prescription_ibfk_1` FOREIGN KEY (`pharmacy_id`) REFERENCES `pharmacy` (`formatted_id`) ON DELETE CASCADE;

--
-- Constraints for table `treatment`
--
ALTER TABLE `treatment`
  ADD CONSTRAINT `treatment_ibfk_1` FOREIGN KEY (`doctor_id`) REFERENCES `doctor` (`formatted_id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
