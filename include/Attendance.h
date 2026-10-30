#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>

/**
 * @brief Represents an Attendance entry.
 * Demonstrates Encapsulation and Data Validation.
 */
class Attendance {
private:
    std::string m_studentId;
    std::string m_courseId;
    std::string m_date;
    std::string m_status; // "Present" or "Absent"
    std::string m_remarks;

public:
    // Default constructor
    Attendance();

    // Parameterized constructor
    Attendance(std::string studentId, std::string courseId, 
               std::string date, std::string status, std::string remarks);

    // Getters
    std::string getStudentId() const;
    std::string getCourseId() const;
    std::string getDate() const;
    std::string getStatus() const;
    std::string getRemarks() const;

    // Setters with validation
    void setStudentId(const std::string& studentId);
    void setCourseId(const std::string& courseId);
    void setDate(const std::string& date);
    void setStatus(const std::string& status);
    void setRemarks(const std::string& remarks);

    // Display formatted record
    void displayAttendance() const;

    // Helper validation
    static bool isValidStatus(const std::string& status);
    static bool isValidDateFormat(const std::string& date);
};

#endif // ATTENDANCE_H
