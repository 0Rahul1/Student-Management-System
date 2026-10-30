#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"
#include <string>

/**
 * @brief Student class derived from Person.
 * 
 * Demonstrates:
 * - Inheritance: Inherits common person traits from Person
 * - Function Overriding: Implements displayInfo()
 * - Static Members: Tracks active student objects in memory
 * - Encapsulation: Validates semester (1-8) and course data
 */
class Student : public Person {
private:
    std::string m_course;
    int m_semester;
    std::string m_enrollmentDate;

    // Static member variable to track in-memory student count
    static int s_activeStudentCount;

public:
    // Default constructor
    Student();

    // Parameterized constructor invoking base class constructor
    Student(std::string id, std::string name, int age, 
            std::string email, std::string phone, std::string gender,
            std::string course, int semester, std::string enrollmentDate);

    // Copy constructor
    Student(const Student& other);

    // Destructor
    ~Student() override;

    // Function Overriding (Runtime Polymorphism)
    void displayInfo() const override;

    // Getters
    std::string getCourse() const;
    int getSemester() const;
    std::string getEnrollmentDate() const;

    // Setters with Encapsulation & Validation
    void setCourse(const std::string& course);
    void setSemester(int semester);
    void setEnrollmentDate(const std::string& date);

    // Static Member Functions
    static int getActiveStudentCount();
    static bool isValidSemester(int sem);
    static bool isValidStudentId(const std::string& id);
};

#endif // STUDENT_H
