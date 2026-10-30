#include "Result.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>

// Default constructor
Result::Result()
    : m_studentId(""), m_courseId(""), m_totalMarks(0.0), 
      m_maxPossibleMarks(0.0), m_percentage(0.0), m_grade("N/A") {
}

// Parameterized constructor
Result::Result(std::string studentId, std::string courseId)
    : m_studentId(std::move(studentId)), m_courseId(std::move(courseId)),
      m_totalMarks(0.0), m_maxPossibleMarks(0.0), m_percentage(0.0), m_grade("N/A") {
    
    if (m_studentId.empty()) {
        throw ValidationException("Student ID cannot be empty in result.");
    }
}

// Getters
std::string Result::getStudentId() const { return m_studentId; }
std::string Result::getCourseId() const { return m_courseId; }
double Result::getTotalMarks() const { return m_totalMarks; }
double Result::getMaxPossibleMarks() const { return m_maxPossibleMarks; }
double Result::getPercentage() const { return m_percentage; }
std::string Result::getGrade() const { return m_grade; }
const std::vector<Result::SubjectEntry>& Result::getSubjects() const { return m_subjects; }

// Setters
void Result::setStudentId(const std::string& studentId) {
    if (studentId.empty()) {
        throw ValidationException("Student ID cannot be empty.");
    }
    m_studentId = studentId;
}

void Result::setCourseId(const std::string& courseId) {
    m_courseId = courseId;
}

// Add subject mark
void Result::addSubjectMark(const std::string& subjectName, double marksObtained, double maxMarks) {
    if (subjectName.empty()) {
        throw ValidationException("Subject name cannot be empty.");
    }
    if (marksObtained < 0.0 || marksObtained > maxMarks) {
        throw ValidationException("Marks obtained (" + std::to_string(marksObtained) + 
                                  ") must be between 0 and max marks (" + std::to_string(maxMarks) + ").");
    }
    if (maxMarks <= 0.0) {
        throw ValidationException("Max marks must be greater than 0.");
    }

    m_subjects.push_back({subjectName, marksObtained, maxMarks});
    computeTotals();
}

// Recompute totals, percentage and grade
void Result::computeTotals() {
    m_totalMarks = 0.0;
    m_maxPossibleMarks = 0.0;

    for (const auto& sub : m_subjects) {
        m_totalMarks += sub.marksObtained;
        m_maxPossibleMarks += sub.maxMarks;
    }

    if (m_maxPossibleMarks > 0.0) {
        m_percentage = (m_totalMarks / m_maxPossibleMarks) * 100.0;
    } else {
        m_percentage = 0.0;
    }

    m_grade = calculateGrade(m_percentage);
}

// Static function to calculate grade from percentage
std::string Result::calculateGrade(double percentage) {
    if (percentage >= 90.0) return "A+";
    if (percentage >= 80.0) return "A";
    if (percentage >= 70.0) return "B";
    if (percentage >= 60.0) return "C";
    if (percentage >= 50.0) return "D";
    return "F"; // Fail
}

// Display formatted result card
void Result::displayResult() const {
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                     STUDENT REPORT CARD                     |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "| Student ID : " << std::left << std::setw(46) << m_studentId << " |\n";
    std::cout << "| Course ID  : " << std::left << std::setw(46) << m_courseId << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "| Subject                    | Marks Obtained | Max Marks     |\n";
    std::cout << "+----------------------------+----------------+---------------+\n";
    
    for (const auto& sub : m_subjects) {
        std::cout << "| " << std::left << std::setw(26) << sub.subjectName
                  << " | " << std::right << std::setw(14) << std::fixed << std::setprecision(1) << sub.marksObtained
                  << " | " << std::setw(13) << sub.maxMarks << " |\n";
    }

    std::cout << "+----------------------------+----------------+---------------+\n";
    std::cout << "| Total Marks: " << std::left << std::setw(14) << (std::to_string(static_cast<int>(m_totalMarks)) + " / " + std::to_string(static_cast<int>(m_maxPossibleMarks)))
              << " | Percentage: " << std::setw(6) << std::fixed << std::setprecision(2) << m_percentage << "%"
              << " | Grade: " << std::setw(4) << m_grade << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
}
