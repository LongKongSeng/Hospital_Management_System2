-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 14, 2026 at 11:32 AM
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
('A001', 'Ron Weasley', 'ronWeasley@gmail.com', '0192837465', '851225016677', 'Active', 'Admin', '2026-01-13 08:34:46'),
('A002', 'Professor Dumbledore', 'dumbledore@hogwarts.com', '0198765432', '550505105544', 'Active', 'Admin', '2026-01-13 08:58:05');

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
('AP001', 'P001', 'N001', 'D001', '2026-01-10', '09:00:00', 'Completed', '2026-01-13 08:58:05'),
('AP002', 'P002', 'N002', 'D002', '2026-01-12', '10:30:00', 'Completed', '2026-01-13 08:58:05'),
('AP003', 'P003', 'N001', 'D003', '2026-02-15', '14:00:00', 'Completed', '2026-01-13 08:58:05'),
('AP004', 'P004', 'N001', 'D001', '2026-06-20', '09:00:00', 'Scheduled', '2026-01-13 08:58:05'),
('AP005', 'P001', 'N002', 'D002', '2026-06-21', '11:00:00', 'Scheduled', '2026-01-13 08:58:05'),
('AP006', 'P002', 'N002', 'D004', '2026-07-05', '15:30:00', 'Scheduled', '2026-01-13 08:58:05'),
('AP007', 'P001', 'N001', 'D001', '2026-01-16', '08:00:00', 'Scheduled', '2026-01-13 12:47:57'),
('AP008', 'P001', 'N001', 'D001', '2026-01-16', '09:00:00', 'Scheduled', '2026-01-13 12:48:32');

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
('DI001', 'Dehydration', 'General', '2 Days', 'Low', NULL, '2026-01-10', '2026-01-13 08:58:05'),
('DI002', 'Memory Loss', 'Neurological', 'Chronic', 'Moderate', NULL, '2026-01-12', '2026-01-13 08:58:05'),
('DI003', 'Bronchitis', 'Respiratory', '1 Week', 'High', NULL, '2026-02-15', '2026-01-13 08:58:05');

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
('D001', 'Dr.Harry Potter', 'Male', 'Cardiologist', '0123948572', '900101145521', 'Available', 'Active', 'Doctor', '2026-01-13 08:32:19'),
('D002', 'Dr. Strange', 'Male', 'Neurologist', '0142223333', '850520106699', 'Available', 'Active', 'Doctor', '2026-01-13 08:58:05'),
('D003', 'Dr. House', 'Male', 'Diagnostician', '0167778888', '790909087711', 'Busy', 'Active', 'Doctor', '2026-01-13 08:58:05'),
('D004', 'Dr. Meredith Grey', 'Female', 'General Surgeon', '0191122334', '821212143355', 'Off', 'Active', 'Doctor', '2026-01-13 08:58:05'),
('D005', 'Colin Teh Jon Ryan', 'Male', 'Cardiologist', '0197132092', '050705081169', 'Available', 'Active', 'Doctor', '2026-01-13 12:42:04');

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
('L001', 'Harry', 'Harry123!', 'Doctor', 'D001', NULL, NULL, '2026-01-13 08:32:19'),
('L002', 'Hermione', 'Hermione123!', 'Nurse', NULL, 'N001', NULL, '2026-01-13 08:33:34'),
('L003', 'Ron', 'Ron12345!', 'Admin', NULL, NULL, 'A001', '2026-01-13 08:34:46'),
('L004', 'strange', 'magic123', 'Doctor', 'D002', NULL, NULL, '2026-01-13 08:58:05'),
('L005', 'house', 'vicodin123', 'Doctor', 'D003', NULL, NULL, '2026-01-13 08:58:05'),
('L006', 'grey', 'anatomy123', 'Doctor', 'D004', NULL, NULL, '2026-01-13 08:58:05'),
('L007', 'joy', 'pokemon123', 'Nurse', NULL, 'N002', NULL, '2026-01-13 08:58:05'),
('L008', 'dumbledore', 'sherbet123', 'Admin', NULL, NULL, 'A002', '2026-01-13 08:58:05'),
('L009', 'colin', 'Colin123!', 'Doctor', 'D005', NULL, NULL, '2026-01-13 12:42:04');

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
('MR001', 'P001', 'D001', 'DI001', '2026-01-10', 'Patient needs to drink more water.', '2026-01-13 08:58:05'),
('MR002', 'P002', 'D002', 'DI002', '2026-01-12', 'Patient forgot where he lives. Prescribed memory aids.', '2026-01-13 08:58:05'),
('MR003', 'P003', 'D003', 'DI003', '2026-02-15', 'Patient complaining of difficulty breathing underwater.', '2026-01-13 08:58:05'),
('MR004', 'P001', 'D001', NULL, '2026-01-14', 'Diagnosis made by doctor. Prescribed: Panadol Extend(7 weeks)', '2026-01-14 08:15:00'),
('MR005', 'P001', 'D001', NULL, '2026-01-14', 'Diagnosis made by doctor. Prescribed: Panadol Extend(aced)', '2026-01-14 09:54:28'),
('MR006', 'P001', 'D001', NULL, '2026-01-14', 'Diagnosis made by doctor. Prescribed: Panadol Extend(2 tabs), Cough Syrup(2ml)', '2026-01-14 10:08:05'),
('MR007', 'P001', 'D001', NULL, '2026-01-14', 'Diagnosis made by doctor. Prescribed: Panadol Extend(2 tabs), Cough Syrup(2ml)', '2026-01-14 10:15:19'),
('MR008', 'P001', 'D001', NULL, '2026-01-14', 'Diagnosis made by doctor. Prescribed: Antiseptic Cream(2ml)', '2026-01-14 10:19:14');

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
('N001', 'Hermione Granger', 'Female', '0174839201', '020512108842', 'Active', 'Nurse', '2026-01-13 08:33:34'),
('N002', 'Nurse Joy', 'Female', '0173334444', '931111146677', 'Active', 'Nurse', '2026-01-13 08:58:05'),
('N003', 'Nurse Jackie', 'Female', '0112345678', '950101015522', 'Active', 'Nurse', '2026-01-13 08:58:05');

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
('P001', 'Spongebob', 'Male', '2026-01-15', '0162738495', 'A+', '0138765432', '980715071234', 'Active', '2026-01-13 08:46:14'),
('P002', 'Patrick Star', 'Male', '1995-05-20', '0174839201', 'O+', '0138887777 (Spongebob)', '950520108842', 'Active', '2026-01-13 08:58:05'),
('P003', 'Squidward Tentacles', 'Male', '1988-11-11', '0192837465', 'B+', 'None', '881111075599', 'Active', '2026-01-13 08:58:05'),
('P004', 'Sandy Cheeks', 'Female', '1998-03-10', '0162738495', 'AB+', '0122223333 (Mom)', '980310086677', 'Active', '2026-01-13 08:58:05');

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
  `patient_id` varchar(20) NOT NULL,
  `pharmacy_id` varchar(20) NOT NULL,
  `dosage` varchar(50) DEFAULT NULL,
  `duration_of_meds` varchar(50) DEFAULT NULL,
  `instructions` text DEFAULT NULL,
  `date` date NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

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
  `patient_id` varchar(20) DEFAULT NULL,
  `doctor_id` varchar(20) NOT NULL,
  `dressing_applied` varchar(200) DEFAULT NULL,
  `consultation_fee` decimal(10,2) DEFAULT 0.00,
  `treatment_fee` decimal(10,2) DEFAULT 0.00,
  `treatment_date` date NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

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
  ADD KEY `doctor_id` (`doctor_id`),
  ADD KEY `fk_treatment_patient` (`patient_id`);

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
  ADD CONSTRAINT `fk_treatment_patient` FOREIGN KEY (`patient_id`) REFERENCES `patient` (`formatted_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `treatment_ibfk_1` FOREIGN KEY (`doctor_id`) REFERENCES `doctor` (`formatted_id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
