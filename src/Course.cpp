#include "Course.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>
#include <regex>

// Default constructor
Course::Course()
    : m_courseId(""), m_courseName(""), m_department(""), m_credits(3), m_instructor("") {
}

// Parameterized constructor
Course::Course(std::string id, std::string name, std::string dept, 
               int credits, std::string instructor)
    : m_courseId(std::move(id)), m_courseName(std::move(name)), 
      m_department(std::move(dept)), m_credits(credits), m_instructor(std::move(instructor)) {
    
    if (!isValidCourseId(m_courseId)) {
        throw ValidationException("Invalid Course ID format. Example: CS101, BCA201.");
    }
    if (m_courseName.empty()) {
        throw ValidationException("Course name cannot be empty.");
    }
    if (m_credits < 1 || m_credits > 10) {
        throw ValidationException("Course credits must be between 1 and 10.");
    }
}

// Getters
std::string Course::getCourseId() const { return m_courseId; }
std::string Course::getCourseName() const { return m_courseName; }
std::string Course::getDepartment() const { return m_department; }
int Course::getCredits() const { return m_credits; }
std::string Course::getInstructor() const { return m_instructor; }

// Setters with validation
void Course::setCourseId(const std::string& id) {
    if (!isValidCourseId(id)) {
        throw ValidationException("Invalid Course ID format.");
    }
    m_courseId = id;
}

void Course::setCourseName(const std::string& name) {
    if (name.empty()) {
        throw ValidationException("Course name cannot be empty.");
    }
    m_courseName = name;
}

void Course::setDepartment(const std::string& dept) {
    m_department = dept;
}

void Course::setCredits(int credits) {
    if (credits < 1 || credits > 10) {
        throw ValidationException("Course credits must be between 1 and 10.");
    }
    m_credits = credits;
}

void Course::setInstructor(const std::string& instructor) {
    m_instructor = instructor;
}

// Display method
void Course::displayCourse() const {
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                       COURSE DETAILS                        |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "| Course ID      : " << std::left << std::setw(42) << m_courseId << " |\n";
    std::cout << "| Course Name    : " << std::left << std::setw(42) << m_courseName << " |\n";
    std::cout << "| Department     : " << std::left << std::setw(42) << m_department << " |\n";
    std::cout << "| Credits        : " << std::left << std::setw(42) << m_credits << " |\n";
    std::cout << "| Instructor     : " << std::left << std::setw(42) << m_instructor << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
}

bool Course::isValidCourseId(const std::string& id) {
    if (id.empty() || id.length() > 15) return false;
    const std::regex coursePattern(R"(^[A-Za-z0-9_\-]+$)");
    return std::regex_match(id, coursePattern);
}
