#include "Attendance.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>
#include <regex>

// Default constructor
Attendance::Attendance()
    : m_studentId(""), m_courseId(""), m_date("2026-01-01"), m_status("Present"), m_remarks("Regular") {
}

// Parameterized constructor
Attendance::Attendance(std::string studentId, std::string courseId, 
                       std::string date, std::string status, std::string remarks)
    : m_studentId(std::move(studentId)), m_courseId(std::move(courseId)),
      m_date(std::move(date)), m_status(std::move(status)), m_remarks(std::move(remarks)) {
    
    if (m_studentId.empty()) {
        throw ValidationException("Student ID cannot be empty in attendance.");
    }
    if (m_courseId.empty()) {
        throw ValidationException("Course ID cannot be empty in attendance.");
    }
    if (!isValidStatus(m_status)) {
        throw ValidationException("Attendance status must be 'Present' or 'Absent'.");
    }
    if (!isValidDateFormat(m_date)) {
        throw ValidationException("Date must be in YYYY-MM-DD format.");
    }
}

// Getters
std::string Attendance::getStudentId() const { return m_studentId; }
std::string Attendance::getCourseId() const { return m_courseId; }
std::string Attendance::getDate() const { return m_date; }
std::string Attendance::getStatus() const { return m_status; }
std::string Attendance::getRemarks() const { return m_remarks; }

// Setters with validation
void Attendance::setStudentId(const std::string& studentId) {
    if (studentId.empty()) {
        throw ValidationException("Student ID cannot be empty.");
    }
    m_studentId = studentId;
}

void Attendance::setCourseId(const std::string& courseId) {
    if (courseId.empty()) {
        throw ValidationException("Course ID cannot be empty.");
    }
    m_courseId = courseId;
}

void Attendance::setDate(const std::string& date) {
    if (!isValidDateFormat(date)) {
        throw ValidationException("Date must be in YYYY-MM-DD format.");
    }
    m_date = date;
}

void Attendance::setStatus(const std::string& status) {
    if (!isValidStatus(status)) {
        throw ValidationException("Status must be 'Present' or 'Absent'.");
    }
    m_status = status;
}

void Attendance::setRemarks(const std::string& remarks) {
    m_remarks = remarks;
}

// Display attendance row
void Attendance::displayAttendance() const {
    std::cout << "| " << std::left << std::setw(12) << m_studentId
              << " | " << std::setw(10) << m_courseId
              << " | " << std::setw(12) << m_date
              << " | " << std::setw(8) << m_status
              << " | " << std::setw(16) << m_remarks << " |\n";
}

bool Attendance::isValidStatus(const std::string& status) {
    return (status == "Present" || status == "Absent" || status == "present" || status == "absent");
}

bool Attendance::isValidDateFormat(const std::string& date) {
    const std::regex datePattern(R"(^\d{4}-\d{2}-\d{2}$)");
    return std::regex_match(date, datePattern);
}
