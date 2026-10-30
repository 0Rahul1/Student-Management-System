#include "Student.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>
#include <regex>

// Initialize static member variable
int Student::s_activeStudentCount = 0;

// Default constructor
Student::Student()
    : Person(), m_course("General"), m_semester(1), m_enrollmentDate("2026-01-01") {
    ++s_activeStudentCount;
}

// Parameterized constructor invoking base class constructor
Student::Student(std::string id, std::string name, int age, 
                 std::string email, std::string phone, std::string gender,
                 std::string course, int semester, std::string enrollmentDate)
    : Person(std::move(id), std::move(name), age, std::move(email), std::move(phone), std::move(gender)),
      m_course(std::move(course)), m_semester(semester), m_enrollmentDate(std::move(enrollmentDate)) {
    
    if (m_course.empty()) {
        throw ValidationException("Course cannot be empty.");
    }
    if (!isValidSemester(m_semester)) {
        throw ValidationException("Semester must be between 1 and 8.");
    }
    if (m_enrollmentDate.empty()) {
        throw ValidationException("Enrollment date cannot be empty.");
    }

    ++s_activeStudentCount;
}

// Copy constructor
Student::Student(const Student& other)
    : Person(other.m_id, other.m_name, other.m_age, other.m_email, other.m_phone, other.m_gender),
      m_course(other.m_course), m_semester(other.m_semester), m_enrollmentDate(other.m_enrollmentDate) {
    ++s_activeStudentCount;
}

// Destructor
Student::~Student() {
    --s_activeStudentCount;
}

// Function Overriding (Runtime Polymorphism)
void Student::displayInfo() const {
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                      STUDENT PROFILE                        |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "| ID             : " << std::left << std::setw(42) << m_id << " |\n";
    std::cout << "| Full Name      : " << std::left << std::setw(42) << m_name << " |\n";
    std::cout << "| Age            : " << std::left << std::setw(42) << m_age << " |\n";
    std::cout << "| Gender         : " << std::left << std::setw(42) << m_gender << " |\n";
    std::cout << "| Email          : " << std::left << std::setw(42) << m_email << " |\n";
    std::cout << "| Phone          : " << std::left << std::setw(42) << m_phone << " |\n";
    std::cout << "| Course         : " << std::left << std::setw(42) << m_course << " |\n";
    std::cout << "| Semester       : " << std::left << std::setw(42) << m_semester << " |\n";
    std::cout << "| Enrollment Date: " << std::left << std::setw(42) << m_enrollmentDate << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
}

// Getters
std::string Student::getCourse() const { return m_course; }
int Student::getSemester() const { return m_semester; }
std::string Student::getEnrollmentDate() const { return m_enrollmentDate; }

// Setters with Encapsulation & Validation
void Student::setCourse(const std::string& course) {
    if (course.empty()) {
        throw ValidationException("Course name cannot be empty.");
    }
    m_course = course;
}

void Student::setSemester(int semester) {
    if (!isValidSemester(semester)) {
        throw ValidationException("Semester must be between 1 and 8.");
    }
    m_semester = semester;
}

void Student::setEnrollmentDate(const std::string& date) {
    if (date.empty()) {
        throw ValidationException("Enrollment date cannot be empty.");
    }
    m_enrollmentDate = date;
}

// Static Member Functions
int Student::getActiveStudentCount() {
    return s_activeStudentCount;
}

bool Student::isValidSemester(int sem) {
    return (sem >= 1 && sem <= 8);
}

bool Student::isValidStudentId(const std::string& id) {
    if (id.empty() || id.length() > 20) return false;
    // ID contains alphanumeric characters, underscores or hyphens
    const std::regex idPattern(R"(^[A-Za-z0-9_\-]+$)");
    return std::regex_match(id, idPattern);
}
